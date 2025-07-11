import asyncio
import websockets
import logging
import base64
from io import BytesIO
from PIL import Image
import os
import traceback

# Configure logging
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)

# Create a directory to save received images
UPLOAD_FOLDER = "received_images"
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

# Store connected clients
connected_clients = set()


async def broadcast(message):
    """
    Broadcast a message to all connected clients.

    :param message: Message to broadcast
    """
    if connected_clients:
        await asyncio.gather(
            *[client.send(message) for client in connected_clients],
            return_exceptions=True,  # Prevents one failed send from stopping others
        )


# Function to handle and display image if received
async def handle_image(websocket, image_data):
    print("Image received")
    try:
        # Convert the received binary data into an image
        image = Image.open(BytesIO(image_data))
        # logger.info(f"Received and saved image: {filename}")
        logger.info(f"Image details:")
        logger.info(f"  - Format: {image.format}")
        logger.info(f"  - Size: {image.size}")
        logger.info(f"  - Mode: {image.mode}")
        # # Generate a unique filename
        # filename = os.path.join(
        #     UPLOAD_FOLDER,
        #     f"received_image_{len(os.listdir(UPLOAD_FOLDER)) + 1}{file_ext}",
        # )
        # # Open and display image details using Pillow
        # with Image.open(filename) as img:
        # # Save the image
        # with open(filename, "wb") as f:
        #     f.write(image_data)

        # Display the image using Pillow
        image.show()
    except Exception as e:
        print(f"Error displaying image: {e}")


async def handle_client(websocket):
    """
    Handle individual client connections.

    :param websocket: WebSocket connection
    """
    try:
        # Register client
        connected_clients.add(websocket)
        logger.info(f"New client connected. Total clients: {len(connected_clients)}")

        # Handle incoming messages
        async for message in websocket:
            try:
                # Check if the message is binary (image data)
                if isinstance(message, bytes):
                    logger.info(f"Received Image")
                    await handle_image(websocket, message)

                else:
                    # Handle plain text message
                    logger.info(f"Received message: {message}")

                    # Broadcast the message to all connected clients
                    await broadcast(message)

            except Exception as e:
                logger.error(f"Error processing message: {e}")
                logger.error(traceback.format_exc())
                await asyncio.gather(
                    websocket.send(f"Error processing your message: {str(e)}"),
                    return_exceptions=True,
                )

    except websockets.exceptions.ConnectionClosed:
        logger.info("Client connection closed")
    except Exception as e:
        logger.error(f"Unexpected error in handle_client: {e}")
        logger.error(traceback.format_exc())
    finally:
        # Unregister client
        connected_clients.discard(websocket)
        logger.info(f"Client disconnected. Remaining clients: {len(connected_clients)}")


async def input_handler():
    """
    Non-blocking user input handler.
    """
    loop = asyncio.get_event_loop()
    while True:
        # Use a thread-safe way to handle blocking input
        message = await loop.run_in_executor(
            None, input, "Enter message to send to clients (or 'exit' to stop): "
        )
        if message.lower() == "exit":
            print("Exiting server...")
            break
        # Broadcast the message to all connected clients
        await broadcast(message)


async def main():
    """
    Start the WebSocket server and user input handler.
    """
    # Create and start the server
    server = await websockets.serve(
        handle_client,
        "0.0.0.0",  # Listen on all available interfaces
        8000,  # Port number
        ping_interval=20,  # Ping every 20 seconds
        ping_timeout=20,  # Timeout after 20 seconds
    )

    logger.info("WebSocket server started on ws://0.0.0.0:8000")

    # Run the server and input handler concurrently
    await asyncio.gather(
        server.wait_closed(),  # Keep the WebSocket server running
        input_handler(),  # Handle user input
    )


# Run the server
if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Server stopped by user")
