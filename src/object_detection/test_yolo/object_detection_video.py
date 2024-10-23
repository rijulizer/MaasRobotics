import cv2
from ultralytics import YOLO

def setup_front_camera():

    # Open the MacBook front camera (usually device 0)
    cap = cv2.VideoCapture(0)
    # Check if the camera opened correctly
    if not cap.isOpened():
        print("Error: Could not open video stream from the front camera.")
        exit()
    # Set the width and height of the capture (optional)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 800)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 600)
    return cap

def setup_ip_camera(stream_url):
    # HTTP stream URL (replace this with your actual stream URL)
    cap = cv2.VideoCapture(stream_url)
    # Check if the camera opened correctly
    if not cap.isOpened():
        print("Error: Could not open video stream from the IP camera.")
        exit()
    # Set the width and height of the capture (optional)
    # cap.set(cv2.CAP_PROP_FRAME_WIDTH, 800)
    # cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 600)
    return cap

stream_url = 'http://192.168.178.125:81/stream'
cap = setup_ip_camera(stream_url)
# Load the YOLOv8 model
model = YOLO("yolov8n.pt")

while True:
    ret, frame = cap.read()  # Capture frame-by-frame
    if not ret:
        print("Error: Failed to capture image")
        break

    # Run YOLOv8 object detection on the frame
    results = model(frame)
    # Get annotated frame (with bounding boxes, labels, and confidence scores)
    annotated_frame = results[0].plot()
    # Display the resulting frame with predictions
    cv2.imshow("YOLOv8 Real-Time Object Detection", annotated_frame)

    # Exit the loop if 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the video capture object and close windows
cap.release()
cv2.destroyAllWindows()
