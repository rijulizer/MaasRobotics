from ultralytics import YOLO
import cv2
import numpy as np

# Load YOLO model
model = YOLO("yolov8l.pt")

# Assume known parameters
baseline = 3.5  # Baseline distance in cm
focal_length = 700  # Example focal length in pixels, can be calibrated

# Load two images using OpenCV
cup_left = cv2.imread("./cup_left.jpg")
cup_right = cv2.imread("./cup_mid.jpg")

# Check if images were loaded correctly
if cup_left is None or cup_right is None:
    print("Error: One or both images could not be loaded.")
else:
    # Run YOLO on each image
    results1 = model(cup_left)
    results2 = model(cup_right)

    # Annotate the images with bounding boxes and labels
    annotated_image1 = results1[0].plot()  # First image with YOLO results
    annotated_image2 = results2[0].plot()  # Second image with YOLO results

    # Display the annotated images
    cv2.imshow("YOLO Detection on Image 1", annotated_image1)
    cv2.imshow("YOLO Detection on Image 2", annotated_image2)

    # Assuming the object we are interested in is at the first position in results
    # Get the bounding box center for the object in both images
    x1, y1 = results1[0].boxes.xywh[0][:2]  # Get x, y from the first detection in img1
    x2, y2 = results2[0].boxes.xywh[0][:2]  # Get x, y from the first detection in img2

    # Calculate disparity
    disparity = x1 - x2
    print(results1[0].boxes)
    print("disparity", disparity)

    # Calculate depth (Z) using triangulation
    if disparity != 0:
        Z = (baseline * focal_length) / disparity
        print(f"Estimated Depth: {Z} cm")
        z_cal = 18
        f_cal = z_cal * disparity / baseline
        print(f"for depth == {z_cal} cm, f should be {f_cal}")
    else:
        print("No disparity, cannot calculate depth")

    # Wait for a key press and close the image windows
    cv2.waitKey(0)
    cv2.destroyAllWindows()


