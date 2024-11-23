import os
from ultralytics import YOLO

# load model
model = YOLO("yolov8n.pt")

# source=0
# source = '../depth_measure/cup_left.jpeg'
# run inference in source
 #,'http://192.168.178.125:81/stream', 
results = model.track(
    source="http://192.168.1.1:81/stream",
    show=True, 
    persist=True,
    stream=False,
    )

