import cv2
import numpy as np

def generate_contour(image_path, color='red'):
    # Load the image
    img = cv2.imread(image_path)
    if img is None:
        print("Error: Could not find image.")
        return

    # Convert to grayscale
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Thresholding: Assuming a black background like in the tower/temple images
    # This creates a binary mask of the non-black pixels
    _, thresh = cv2.threshold(gray, 10, 255, cv2.THRESH_BINARY)

    # Find the contours
    contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Create a blank white canvas (matching original dimensions)
    # 255 for white background
    canvas = np.ones_like(img) * 255 

    # Set color (OpenCV uses BGR)
    contour_color = (0, 0, 255) if color == 'red' else (0, 255, 0)

    # Draw the contours on the canvas
    # -1 draws all contours, thickness of 1 pixel
    cv2.drawContours(canvas, contours, -1, contour_color, 1)

    # Save the result
    output_name = f"contour_{color}.png"
    cv2.imwrite(output_name, canvas)
    print(f"Saved {output_name}")

# Example usage for source2.jpg
generate_contour('source2.jpg', color='red')