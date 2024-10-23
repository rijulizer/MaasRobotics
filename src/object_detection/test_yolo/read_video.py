import cv2

# HTTP stream URL (replace this with your actual stream URL)
stream_url = 'http://192.168.178.125:81/stream'

# Initialize VideoCapture with the HTTP stream
cap = cv2.VideoCapture(stream_url)

if not cap.isOpened():
    print("Error: Couldn't open the video stream.")
else:
    while True:
        # Capture frame-by-frame
        ret, frame = cap.read()

        if not ret:
            print("Failed to grab frame.")
            break

        # Display the resulting frame
        cv2.imshow('Stream', frame)

        # Press 'q' to exit the video stream
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

# Release the capture and close the window
cap.release()
cv2.destroyAllWindows()
