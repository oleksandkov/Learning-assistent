from PIL import Image, ImageDraw, ImageFont
import os

# Paths
base_dir = os.path.join(os.path.dirname(__file__), '..', 'OOP', 'OOP LABA#4', 'LABA_MAIN')
resources_dir = os.path.join(base_dir, 'src', 'main', 'resources')
input_name = 'warrior_backup_before_transparency-removebg-preview.png'
output_name = 'warrior_composite.png'
input_path = os.path.normpath(os.path.join(resources_dir, input_name))
output_path = os.path.normpath(os.path.join(resources_dir, output_name))

if not os.path.exists(input_path):
    raise FileNotFoundError(f'Base image not found: {input_path}')

# Open base image
base = Image.open(input_path).convert('RGBA')
width, height = base.size

# Create canvas a bit taller to fit name and health bar above
top_padding = int(0.25 * height)
canvas = Image.new('RGBA', (width, height + top_padding), (255, 255, 255, 0))
canvas.paste(base, (0, top_padding), base)

draw = ImageDraw.Draw(canvas)

# Health bar parameters
bar_width = int(width * 0.7)
bar_height = int(0.06 * height)
bar_x = (width - bar_width) // 2
bar_y = int(top_padding * 0.15)

# Draw background of health bar (light gray)
draw.rounded_rectangle([bar_x, bar_y, bar_x + bar_width, bar_y + bar_height], radius=6, fill=(200, 200, 200, 180))
# Draw current health (green fill) - full for this example (100%)
health_pct = 1.0
fill_width = int(bar_width * health_pct)
draw.rounded_rectangle([bar_x, bar_y, bar_x + fill_width, bar_y + bar_height], radius=6, fill=(120, 230, 140, 220))

# Draw name text above bar
name = 'Warrior'
# Try loading a common TTF font; fallback to default
font = None
try:
    font = ImageFont.truetype('arial.ttf', max(24, top_padding // 4))
except Exception:
    try:
        font = ImageFont.truetype('DejaVuSans.ttf', max(24, top_padding // 4))
    except Exception:
        font = ImageFont.load_default()

# Calculate text size using textbbox (more reliable across Pillow versions)
bbox = draw.textbbox((0, 0), name, font=font)
text_width = bbox[2] - bbox[0]
text_height = bbox[3] - bbox[1]
text_x = (width - text_width) // 2
text_y = bar_y - text_height - 8
# Draw subtle shadow
draw.text((text_x+1, text_y+1), name, font=font, fill=(0,0,0,120))
# Draw main text
draw.text((text_x, text_y), name, font=font, fill=(30,30,100,255))

# Save
canvas.convert('RGB').save(output_path, format='PNG')
print(f'Composite saved to: {output_path}')
