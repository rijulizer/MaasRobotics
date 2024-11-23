from ultralytics import YOLO
import cv2
import numpy as np
import time

# Assume known parameters
BASELINE = 5  # Baseline distance in cm between the two frames
FOCAL_LEN = 700  # Example focal length in pixels, to be calibrated

def setup_ip_camera(stream_url):
    # HTTP stream URL (replace this with your actual stream URL)
    cap = cv2.VideoCapture(stream_url)
    # Check if the camera opened correctly
    if not cap.isOpened():
        print("Error: Could not open video stream from the IP camera.")
        exit()
    # Set the width and height of the capture (optional)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 800)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 600)
    return cap

stream_url = 'http://192.168.178.125:81/stream'
cap = setup_ip_camera(stream_url)

# Load YOLO model
model = YOLO("yolov8n.pt")
# Initialize variables
frame1, frame2 = None, None
tracking_data1, tracking_data2 = None, None

while True:
    ret, frame = cap.read()
    if not ret:
        break
    # Use YOLO to track objects
    tracking_results = model.track(source=frame, stream=True)
    # Save two frames for disparity calculation
    if frame1 is None:
        frame1 = frame.copy()
        tracking_data1 = tracking_results[0]  # Store first frame's tracking data
    elif frame2 is None and time.time() > 2:  # Wait for some time to get baseline separation
        frame2 = frame.copy()
        tracking_data2 = tracking_results[0]  # Store second frame's tracking data
        break  # Exit loop after capturing two frames

# Ensure the same object is tracked in both frames
object_id1 = tracking_data1.boxes[0].id if tracking_data1.boxes else None
object_id2 = tracking_data2.boxes[0].id if tracking_data2.boxes else None

# Check that the same object is identified in both frames
if object_id1 == object_id2 and object_id1 is not None:
    # Get bounding box centers for the tracked object in both frames
    x1, y1 = tracking_data1.boxes[0].xywh[0][:2]  # Center of the object in frame1
    x2, y2 = tracking_data2.boxes[0].xywh[0][:2]  # Center of the object in frame2

    # Calculate disparity
    disparity = x1 - x2
    print("Disparity:", disparity)

    # Calculate depth (Z) using triangulation
    if disparity != 0:
        Z = (baseline * focal_length) / disparity
        print(f"Estimated Depth: {Z} cm")
        z_cal = 21
        f_cal = z_cal * disparity / baseline
        print(f"for depth == 21 cm, f should be {f_cal}")
    else:
        print("No disparity; cannot calculate depth")

else:
    print("Tracking failed to find the same object in both frames.")
# Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break
# Release resources
cap.release()
cv2.destroyAllWindows()

