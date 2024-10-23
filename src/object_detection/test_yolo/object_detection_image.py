from ultralytics import YOLO
import cv2


# Load a COCO-pretrained YOLOv8n model
model = YOLO("yolov8n.pt")

# Display model information (optional)
print(model.info())
# Run inference with the YOLOv8n model on the 'bus.jpg' image
results = model("./objects.jpg")

annotated_image = results[0].plot()  # Results[0] holds the detection results for the first image

# Display the image using OpenCV
cv2.imshow("Object Detection", annotated_image)
cv2.waitKey(0)  # Press any key to close the window
cv2.destroyAllWindows()