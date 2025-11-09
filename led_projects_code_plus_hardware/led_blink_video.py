import cv2 as cv
import sys

video_path = "videos/led_blinking.mp4"
cap = cv.VideoCapture(video_path)

while True:
    ret, frame = cap.read()
    if not ret:
        break 
    
    frame_resized = cv.resize(frame, (640, 360))
    cv.imshow('LED Blinking Video', frame_resized)
    if cv.waitKey(25) == ord('q'):
        break
cap.release()
cv.destroyAllWindows()
sys.exit(0)

