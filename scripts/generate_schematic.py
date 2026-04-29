from PIL import Image, ImageDraw, ImageFont
import os

# Load the composite image we just created
base_dir = os.path.join(os.path.dirname(__file__), '..', 'OOP', 'OOP LABA#4', 'LABA_MAIN')
resources_dir = os.path.join(base_dir, 'src', 'main', 'resources')
composite_path = os.path.normpath(os.path.join(resources_dir, 'warrior_composite.png'))
schematic_path = os.path.normpath(os.path.join(resources_dir, 'warrior_schematic.png'))

# Load composite to get dimensions
composite = Image.open(composite_path).convert('RGBA')
comp_width, comp_height = composite.size

# Create schematic canvas with extra margin for labels
margin = 80
canvas_width = comp_width + 2 * margin
canvas_height = comp_height + 2 * margin
schematic = Image.new('RGB', (canvas_width, canvas_height), (240, 240, 245))
draw = ImageDraw.Draw(schematic)

# Load a font
try:
    font_large = ImageFont.truetype('arial.ttf', 14)
    font_small = ImageFont.truetype('arial.ttf', 11)
except:
    try:
        font_large = ImageFont.truetype('DejaVuSans.ttf', 14)
        font_small = ImageFont.truetype('DejaVuSans.ttf', 11)
    except:
        font_large = font_small = ImageFont.load_default()

# Paste composite image in center
schematic.paste(composite, (margin, margin), composite)

# Draw canvas boundary
draw.rectangle(
    [(margin, margin), (margin + comp_width - 1, margin + comp_height - 1)],
    outline=(50, 50, 100),
    width=2
)

# Draw axes
draw.line([(margin - 10, margin), (margin + comp_width + 10, margin)], fill=(100, 100, 100), width=1)
draw.line([(margin, margin - 10), (margin, margin + comp_height + 10)], fill=(100, 100, 100), width=1)

# Label origin
draw.text((margin - 35, margin - 25), 'Origin (0,0)', font=font_small, fill=(0, 0, 0))

# Draw dimension arrows and labels
# Canvas width
draw.line([(margin, margin - 50), (margin + comp_width, margin - 50)], fill=(200, 50, 50), width=2)
draw.text((margin + comp_width // 2 - 30, margin - 65), f'Canvas: {comp_width}px', font=font_large, fill=(200, 50, 50))

# Canvas height
draw.line([(margin - 50, margin), (margin - 50, margin + comp_height)], fill=(50, 200, 50), width=2)
draw.text((margin - 75, margin + comp_height // 2 - 10), f'{comp_height}px', font=font_large, fill=(50, 200, 50))

# Add annotations for key elements
# Top padding region
draw.rectangle(
    [(margin, margin), (margin + comp_width, margin + 125)],
    outline=(255, 165, 0),
    width=2
)
draw.text((margin + 10, margin + 50), 'Top Padding: 125px', font=font_large, fill=(255, 100, 0))

# Health bar region
bar_box = [(margin + 75, margin + 18), (margin + 425, margin + 48)]
draw.rectangle(bar_box, outline=(100, 200, 100), width=2)
draw.text((margin + 80, margin + 48 + 5), 'Health Bar (350x30)', font=font_small, fill=(100, 200, 100))
draw.text((margin + 80, margin + 18 - 18), f'@(75,18)', font=font_small, fill=(100, 150, 100))

# Name text region (approximately)
draw.rectangle(
    [(margin + 199, margin - 12), (margin + 300, margin + 10)],
    outline=(100, 100, 200),
    width=2
)
draw.text((margin + 205, margin - 12 - 18), f'Name (101x22)', font=font_small, fill=(100, 100, 200))
draw.text((margin + 205, margin - 12 - 33), f'@(199,-12)', font=font_small, fill=(100, 100, 200))

# Base image region
draw.rectangle(
    [(margin, margin + 125), (margin + 500, margin + 625)],
    outline=(200, 50, 200),
    width=2
)
draw.text((margin + 10, margin + 125 + 10), 'Unit Image (500x500)', font=font_small, fill=(200, 50, 200))
draw.text((margin + 10, margin + 125 + 28), '@(0,125)', font=font_small, fill=(200, 50, 200))

# Add coordinate grid points
points = [
    (75, 18, 'Bar Start'),
    (199, -12, 'Text Start'),
    (0, 125, 'Unit Start'),
]
for px, py, label in points:
    draw.ellipse([(margin + px - 3, margin + py - 3), (margin + px + 3, margin + py + 3)], fill=(255, 0, 0), outline=(100, 0, 0))

# Save schematic
schematic.save(schematic_path, format='PNG')
print(f'Schematic saved to: {schematic_path}')
print(f'Schematic dimensions: {canvas_width}x{canvas_height}')
