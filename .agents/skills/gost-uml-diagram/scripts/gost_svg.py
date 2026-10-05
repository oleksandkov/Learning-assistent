"""
ГОСТ 19.701-90 SVG Generator Helper
Модуль для програмної генерації академічних блок-схем без зовнішніх залежностей.
Дотримується:
1. Прозорий фон (background: transparent).
2. Біла заливка фігур (fill: #ffffff).
3. Чорні лінії 1.5px (stroke: #000000).
4. Суворо українська термінологія.
5. Жодних заголовків на самому малюнку.
6. Прямокутні злами ліній зв'язку 90 градусів.
"""

from typing import List, Dict, Tuple, Optional
import xml.etree.ElementTree as ET


class GostFlowchartBuilder:
    def __init__(self, width: int = 460, start_x: int = 230):
        self.width = width
        self.center_x = start_x
        self.current_y = 20
        self.nodes = []
        self.lines = []
        self.labels = []

    def add_terminator(self, text: str = "Початок", w: int = 150, h: int = 38):
        y = self.current_y
        x = self.center_x - w // 2
        rx = h // 2
        svg_code = f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{rx}" ry="{rx}" class="block" />'
        txt_code = f'<text x="{self.center_x}" y="{y + h // 2}" class="txt">{text}</text>'
        self.nodes.append(svg_code)
        self.nodes.append(txt_code)
        prev_bottom = y + h
        self.current_y += h + 30
        return y, prev_bottom

    def add_io(self, text: str = "Введення: x", w: int = 180, h: int = 40, slant: int = 20):
        y = self.current_y
        cx = self.center_x
        # Паралелограм зі скосом
        x1, y1 = cx - w // 2 + slant // 2, y
        x2, y2 = cx + w // 2 + slant // 2, y
        x3, y3 = cx + w // 2 - slant // 2, y + h
        x4, y4 = cx - w // 2 - slant // 2, y + h
        pts = f"{x1},{y1} {x2},{y2} {x3},{y3} {x4},{y4}"
        svg_code = f'<polygon points="{pts}" class="block" />'
        txt_code = f'<text x="{cx}" y="{y + h // 2}" class="txt">{text}</text>'
        self.nodes.append(svg_code)
        self.nodes.append(txt_code)
        prev_bottom = y + h
        self.current_y += h + 30
        return y, prev_bottom

    def add_process(self, text: str, w: int = 160, h: int = 40):
        y = self.current_y
        x = self.center_x - w // 2
        svg_code = f'<rect x="{x}" y="{y}" width="{w}" height="{h}" class="block" />'
        txt_code = f'<text x="{self.center_x}" y="{y + h // 2}" class="txt">{text}</text>'
        self.nodes.append(svg_code)
        self.nodes.append(txt_code)
        prev_bottom = y + h
        self.current_y += h + 30
        return y, prev_bottom

    def add_subroutine(self, text: str, w: int = 220, h: int = 44, margin: int = 16):
        y = self.current_y
        x = self.center_x - w // 2
        svg_code = f'<rect x="{x}" y="{y}" width="{w}" height="{h}" class="block" />'
        l1 = f'<line x1="{x + margin}" y1="{y}" x2="{x + margin}" y2="{y + h}" class="line" />'
        l2 = f'<line x1="{x + w - margin}" y1="{y}" x2="{x + w - margin}" y2="{y + h}" class="line" />'
        txt_code = f'<text x="{self.center_x}" y="{y + h // 2}" class="txt">{text}</text>'
        self.nodes.extend([svg_code, l1, l2, txt_code])
        prev_bottom = y + h
        self.current_y += h + 30
        return y, prev_bottom

    def add_decision(self, text: str, w: int = 180, h: int = 76):
        y = self.current_y
        cx = self.center_x
        top = f"{cx},{y}"
        right = f"{cx + w // 2},{y + h // 2}"
        bottom = f"{cx},{y + h}"
        left = f"{cx - w // 2},{y + h // 2}"
        pts = f"{top} {right} {bottom} {left}"
        svg_code = f'<polygon points="{pts}" class="block" />'
        txt_code = f'<text x="{cx}" y="{y + h // 2}" class="txt">{text}</text>'
        self.nodes.append(svg_code)
        self.nodes.append(txt_code)
        prev_bottom = y + h
        self.current_y += h + 30
        return y, prev_bottom

    def add_preparation(self, text: str, w: int = 180, h: int = 44, cut: int = 24):
        y = self.current_y
        cx = self.center_x
        # Шестикутник
        pts = (f"{cx - w // 2 + cut},{y} {cx + w // 2 - cut},{y} "
               f"{cx + w // 2},{y + h // 2} "
               f"{cx + w // 2 - cut},{y + h} {cx - w // 2 + cut},{y + h} "
               f"{cx - w // 2},{y + h // 2}")
        svg_code = f'<polygon points="{pts}" class="block" />'
        txt_code = f'<text x="{cx}" y="{y + h // 2}" class="txt">{text}</text>'
        self.nodes.append(svg_code)
        self.nodes.append(txt_code)
        prev_bottom = y + h
        self.current_y += h + 30
        return y, prev_bottom

    def add_connector_line(self, y1: int, y2: int, with_arrow: bool = True):
        marker = ' marker-end="url(#gost-arr)"' if with_arrow else ''
        code = f'<line x1="{self.center_x}" y1="{y1}" x2="{self.center_x}" y2="{y2}" class="line"{marker} />'
        self.lines.append(code)

    def render(self) -> str:
        total_height = self.current_y + 20
        header = f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {self.width} {total_height}" width="100%" height="100%" style="background: transparent; font-family: 'Segoe UI', Arial, sans-serif;">
  <defs>
    <marker id="gost-arr" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 1.5 L 10 5 L 0 8.5 z" fill="#000000" />
    </marker>
    <style>
      .line {{ stroke: #000000; stroke-width: 1.5; fill: none; stroke-linecap: square; stroke-linejoin: miter; }}
      .block {{ stroke: #000000; stroke-width: 1.5; fill: #ffffff; }}
      .txt {{ font-size: 13px; font-weight: 500; fill: #000000; text-anchor: middle; dominant-baseline: central; }}
      .lbl {{ font-size: 12px; font-weight: 600; fill: #000000; }}
    </style>
  </defs>'''
        content = "\n  ".join(self.lines + self.nodes)
        footer = "\n</svg>"
        return f"{header}\n  {content}{footer}"


if __name__ == "__main__":
    builder = GostFlowchartBuilder()
    _, b1 = builder.add_terminator("Початок")
    y2, b2 = builder.add_io("Введення: a, b")
    builder.add_connector_line(b1, y2)
    y3, b3 = builder.add_process("c = a + b")
    builder.add_connector_line(b2, y3)
    y4, b4 = builder.add_io("Виведення: c")
    builder.add_connector_line(b3, y4)
    y5, b5 = builder.add_terminator("Кінець")
    builder.add_connector_line(b4, y5)
    print("SVG generated successfully, length:", len(builder.render()))
