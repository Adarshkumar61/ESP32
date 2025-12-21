#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#define SERVO_PIN 18
#define SERVO_CHANNEL LEDC_CHANNEL_0
#define SERVO_TIMER LEDC_TIMER_0
#define SERVO_MODE LEDC_LOW_SPEED_MODE
#define SERVO_FREQ 50
#define SERVO_RESOLUTION LEDC_TIMER_16_BIT

// Servo pulse width range (microseconds)
#define SERVO_MIN_PULSE 1000    // 0 degrees
#define SERVO_MAX_PULSE 2000    // 180 degrees

void servo_init() {
    ledc_timer_config_t timer_conf = {
        .speed_mode = SERVO_MODE,
        .timer_num = SERVO_TIMER,
        .duty_resolution = SERVO_RESOLUTION,
        .freq_hz = SERVO_FREQ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .speed_mode = SERVO_MODE,
        .channel = SERVO_CHANNEL,
        .timer_sel = SERVO_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = SERVO_PIN,
        .duty = 0,
    };
    ledc_channel_config(&channel_conf);
}

void servo_set_angle(int angle) {
    // Clamp angle between 0 and 180 degrees
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    // Calculate pulse width in microseconds
    int pulse_width = SERVO_MIN_PULSE + (angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE)) / 180;
    
    // PWM period = 1/frequency in microseconds (50 Hz = 20000 µs)
    // Duty cycle = (pulse_width / period) * max_duty_value
    uint32_t max_duty = (1 << SERVO_RESOLUTION) - 1;
    uint32_t duty = (pulse_width * max_duty) / 20000;
    
    ledc_set_duty(SERVO_MODE, SERVO_CHANNEL, duty);
    ledc_update_duty(SERVO_MODE, SERVO_CHANNEL);
}

void app_main() {
    servo_init();
    servo_set_angle(90);  // Move servo to 90 degrees
}