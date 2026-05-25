from __future__ import annotations

import re
import sys
from pathlib import Path

from reportlab.lib.enums import TA_CENTER
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import ListFlowable, ListItem, Paragraph, SimpleDocTemplate, Spacer


def register_fonts() -> tuple[str, str]:
    font_regular = Path(r"C:\Windows\Fonts\arial.ttf")
    font_bold = Path(r"C:\Windows\Fonts\arialbd.ttf")

    if not font_regular.exists() or not font_bold.exists():
        raise FileNotFoundError("Windows Arial fonts were not found.")

    pdfmetrics.registerFont(TTFont("UARegular", str(font_regular)))
    pdfmetrics.registerFont(TTFont("UABold", str(font_bold)))
    return "UARegular", "UABold"


def markdown_to_flowables(markdown_text: str, styles: dict[str, ParagraphStyle]):
    story = []
    bullet_buffer: list[str] = []

    def flush_bullets() -> None:
        nonlocal bullet_buffer
        if not bullet_buffer:
            return
        items = [ListItem(Paragraph(item, styles["bullet"])) for item in bullet_buffer]
        story.append(ListFlowable(items, bulletType="bullet", start="-"))
        story.append(Spacer(1, 4))
        bullet_buffer = []

    for raw_line in markdown_text.splitlines():
        line = raw_line.strip()

        if not line:
            flush_bullets()
            story.append(Spacer(1, 3))
            continue

        if line.startswith("# "):
            flush_bullets()
            story.append(Paragraph(line[2:].strip(), styles["title"]))
            continue

        if line.startswith("## "):
            flush_bullets()
            story.append(Paragraph(line[3:].strip(), styles["heading"]))
            continue

        if line.startswith("- "):
            bullet_buffer.append(line[2:].strip())
            continue

        flush_bullets()
        cleaned = re.sub(r"\*\*(.+?)\*\*", r"<b>\1</b>", line)
        cleaned = re.sub(r"\*(.+?)\*", r"<i>\1</i>", cleaned)
        story.append(Paragraph(cleaned, styles["body"]))

    flush_bullets()
    return story


def main() -> int:
    if len(sys.argv) < 2:
        print("Usage: convert_md_to_pdf.py <input.md> [output.pdf]", file=sys.stderr)
        return 1

    input_path = Path(sys.argv[1]).expanduser().resolve()
    if not input_path.exists():
        print(f"Input file not found: {input_path}", file=sys.stderr)
        return 1

    output_path = Path(sys.argv[2]).expanduser().resolve() if len(sys.argv) > 2 else input_path.with_suffix(".pdf")

    regular_font, bold_font = register_fonts()

    stylesheet = getSampleStyleSheet()
    styles: dict[str, ParagraphStyle] = {
        "title": ParagraphStyle(
            "TitleUA",
            parent=stylesheet["Title"],
            fontName=bold_font,
            fontSize=18,
            leading=22,
            alignment=TA_CENTER,
            spaceAfter=10,
        ),
        "heading": ParagraphStyle(
            "HeadingUA",
            parent=stylesheet["Heading2"],
            fontName=bold_font,
            fontSize=13,
            leading=16,
            spaceBefore=8,
            spaceAfter=4,
        ),
        "body": ParagraphStyle(
            "BodyUA",
            parent=stylesheet["BodyText"],
            fontName=regular_font,
            fontSize=11,
            leading=15,
            spaceAfter=4,
        ),
        "bullet": ParagraphStyle(
            "BulletUA",
            parent=stylesheet["BodyText"],
            fontName=regular_font,
            fontSize=11,
            leading=15,
            leftIndent=12,
            firstLineIndent=0,
            spaceAfter=2,
        ),
    }

    document = SimpleDocTemplate(
        str(output_path),
        pagesize=A4,
        rightMargin=18 * mm,
        leftMargin=18 * mm,
        topMargin=16 * mm,
        bottomMargin=16 * mm,
    )

    story = markdown_to_flowables(input_path.read_text(encoding="utf-8"), styles)
    document.build(story)
    print(f"PDF saved: {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())