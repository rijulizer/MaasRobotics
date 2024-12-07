import asyncio
import websockets
import threading
from PIL import Image
from io import BytesIO


# Function to send a message to the client
async def send_message(websocket, message):
    await websocket.send(message)
    print(f"Sent message to ESP32: {message}")


# Function to receive a message from the client
async def receive_message(websocket):
    message = await websocket.recv()
    print(f"Received message from ESP32: {message}")
    return message


# Function to handle and display image if received
async def handle_image(websocket, image_data):
    print("Image received")
    try:
        # Convert the received binary data into an image
        image = Image.open(BytesIO(image_data))

        # Display the image using Pillow
        image.show()
    except Exception as e:
        print(f"Error displaying image: {e}")


# Define the WebSocket server handler
async def echo(websocket):
    print("Client connected")
    try:
        # Start a task for receiving messages
        async def listen_to_client():
            f = open(".demofile.txt", "a")
            f.write("Now the file has more content!")
            f.close()
            while True:
                try:
                    message = await receive_message(websocket)
                    if isinstance(message, bytes):
                        print("Received binary data (image or file).")
                    else:
                        print(f"Message: {message}")
                except websockets.exceptions.ConnectionClosed:
                    print("Client disconnected.")
                    break

        # Run the listener task
        listen_task = asyncio.create_task(listen_to_client())

        # Handle sending messages interactively
        while True:
            try:
                message = input("Enter a message to send (or 'exit' to quit): ")
                if message.lower() == "exit":
                    print("Closing connection.")
                    break
                await send_message(websocket, message)
            except websockets.exceptions.ConnectionClosed:
                print("Client disconnected.")
                break

        # Cleanup
        listen_task.cancel()
        await websocket.close()

    except websockets.exceptions.ConnectionClosed:
        print("Connection closed.")


# Start the WebSocket server
async def main():
    server = await websockets.serve(echo, "0.0.0.0", 8000)
    print(f"WebSocket server running on ws://{server.sockets[0].getsockname()}")
    await server.wait_closed()


# Run the server
asyncio.run(main())
