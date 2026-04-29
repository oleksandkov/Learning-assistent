from PIL import Image, ImageDraw, ImageFont
import os

base_dir = os.path.join(os.path.dirname(__file__), '..', 'OOP', 'OOP LABA#4', 'LABA_MAIN')
resources_dir = os.path.join(base_dir, 'src', 'main', 'resources')
input_name = 'warrior_backup_before_transparency-removebg-preview.png'
output_name = 'warrior_composite.png'
input_path = os.path.normpath(os.path.join(resources_dir, input_name))
output_path = os.path.normpath(os.path.join(resources_dir, output_name))

base = Image.open(input_path).convert('RGBA')
width, height = base.size

# same logic as generator
top_padding = int(0.25 * height)
canvas_w, canvas_h = width, height + top_padding
bar_width = int(width * 0.7)
bar_height = int(0.06 * height)
bar_x = (width - bar_width) // 2
bar_y = int(top_padding * 0.15)
health_pct = 1.0
fill_width = int(bar_width * health_pct)

# font metrics
try:
    font = ImageFont.truetype('arial.ttf', max(24, top_padding // 4))
except Exception:
    try:
        font = ImageFont.truetype('DejaVuSans.ttf', max(24, top_padding // 4))
    except Exception:
        font = ImageFont.load_default()

from PIL import ImageDraw
dummy = Image.new('RGBA',(10,10))
d = ImageDraw.Draw(dummy)
try:
    bbox = d.textbbox((0,0),'Warrior', font=font)
    text_w = bbox[2]-bbox[0]
    text_h = bbox[3]-bbox[1]
except Exception:
    text_w, text_h = d.textsize('Warrior', font=font)
text_x = (width - text_w)//2
text_y = bar_y - text_h - 8

print('Base image (width,height)=', (width, height))
print('Canvas (width,height)=', (canvas_w, canvas_h))
print('Base top-left on canvas =', (0, top_padding))
print('Top padding =', top_padding)
print('\nHealth bar:')
print(' bar_width=', bar_width)
print(' bar_height=', bar_height)
print(' bar_x=', bar_x)
print(' bar_y=', bar_y)
print(' bar_rect =', (bar_x, bar_y, bar_x+bar_width, bar_y+bar_height))
print(' health fill width=', fill_width)
print('\nName text:')
print(' text_size =', (text_w, text_h))
print(' text_x=', text_x)
print(' text_y=', text_y)
print('\nOutput file path:', output_path)
