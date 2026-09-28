"""
Допоміжний модуль для генераторів звітів v3.
Відкриває DOCX-звіт v2, оновлює заголовок до v3 та вставляє розділ
UML-діаграм (діаграма класів, послідовності, станів) у стилі,
уніфікованому з курсовою роботою «Сокобан» (Додаток Б).
"""
import os
import docx
from docx import Document
from docx.shared import Pt, Cm
from docx.enum.text import WD_ALIGN_PARAGRAPH
import win32com.client


def _set_font(run, size=14, bold=False):
    run.font.name = "Times New Roman"
    run.font.size = Pt(size)
    run.bold = bold


def update_title_to_v3(doc):
    """Замінює '(v2)' на '(v3)' у заголовку титульної сторінки."""
    for p in doc.paragraphs:
        for r in p.runs:
            if "(v2)" in r.text:
                r.text = r.text.replace("(v2)", "(v3)")
                return True
    return False


def _find_paragraph(doc, text):
    for p in doc.paragraphs:
        if text in p.text:
            return p
    return None


def _insert_uml_section(doc, ref_para, uml_dir, lab_no):
    """Вставляє розділ UML-діаграм перед параграфом ref_para."""
    diagrams = [
        ("uml_class_v3.png", "Діаграма класів"),
        ("uml_sequence_v3.png", "Діаграма послідовності"),
        ("uml_state_v3.png", "Діаграма станів"),
    ]

    # розрив сторінки перед розділом
    p_break = ref_para.insert_paragraph_before()
    p_break.paragraph_format.space_before = Pt(0)
    p_break.paragraph_format.space_after = Pt(0)
    p_break.add_run().add_page_break()

    # заголовок розділу
    p_title = ref_para.insert_paragraph_before()
    p_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_title.paragraph_format.space_before = Pt(10)
    p_title.paragraph_format.space_after = Pt(8)
    r = p_title.add_run(f"UML-ДІАГРАМИ ПРОГРАМНОЇ СИСТЕМИ")
    _set_font(r, 16, bold=True)

    p_desc = ref_para.insert_paragraph_before()
    p_desc.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_desc.paragraph_format.space_before = Pt(2)
    p_desc.paragraph_format.space_after = Pt(10)
    r = p_desc.add_run(
        f"На рисунках наведено UML-діаграми програмної системи лабораторної роботи №{lab_no} "
        "у стилі, уніфікованому з курсовою роботою: діаграму класів, діаграму послідовності "
        "та діаграму станів."
    )
    _set_font(r, 12)

    for img_name, caption in diagrams:
        img_path = os.path.join(uml_dir, img_name)
        # підпис
        p_cap = ref_para.insert_paragraph_before()
        p_cap.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p_cap.paragraph_format.space_before = Pt(6)
        p_cap.paragraph_format.space_after = Pt(2)
        r = p_cap.add_run(f"Рисунок — {caption} програмної системи")
        _set_font(r, 11, bold=True)

        # зображення
        p_img = ref_para.insert_paragraph_before()
        p_img.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p_img.paragraph_format.space_before = Pt(2)
        p_img.paragraph_format.space_after = Pt(6)
        run = p_img.add_run()
        run.add_picture(img_path, width=Cm(14.5))


def _replace_flowchart(doc, new_flowchart_path):
    """Замінює вбудовану блок-схему на нову (v3) з автопідбором ширини."""
    from docx.oxml.ns import qn
    from PIL import Image
    # визначаємо розміри зображення та підбираємо ширину, щоб влізло на сторінку
    with Image.open(new_flowchart_path) as img:
        w_px, h_px = img.size
    max_h_cm = 24.0  # максимальна висота на сторінці A4 з полями
    width_cm = min(15.5, max_h_cm * w_px / h_px)
    for p in doc.paragraphs:
        drawings = p._element.findall('.//' + qn('w:drawing'))
        if drawings:
            for run in list(p.runs):
                run._element.getparent().remove(run._element)
            run = p.add_run()
            run.add_picture(new_flowchart_path, width=Cm(width_cm))
            print(f"Блок-схему замінено на v3 (ширина {width_cm:.1f} см)")
            return True
    print("Попередження: вбудовану блок-схему не знайдено")
    return False


def build_v3_report(lab_no, base_dir, v2_docx_name, flowchart_title_text="БЛОК-СХЕМА АЛГОРИТМУ (UML)"):
    """
    Будує звіт v3 на основі звіту v2:
    base_dir: директорія лабораторної (LAB1, LAB3, ...)
    v2_docx_name: ім'я DOCX-файлу v2
    """
    # v2-файл може бути у підпапці v2 або безпосередньо в папці лабораторної
    v2_docx = os.path.join(base_dir, "v2", v2_docx_name)
    if not os.path.exists(v2_docx):
        v2_docx = os.path.join(base_dir, v2_docx_name)
    docx_path = os.path.join(base_dir, v2_docx_name.replace("_v2", "_v3"))
    pdf_path = docx_path.replace(".docx", ".pdf")
    v3_dir = os.path.join(base_dir, "v3")
    new_flowchart = os.path.join(v3_dir, "flowchart_v3.png")

    doc = Document(v2_docx)

    # 1. оновлення заголовка
    if not update_title_to_v3(doc):
        print(f"Попередження: не знайдено '(v2)' у заголовку {v2_docx}")

    # 2. заміна блок-схеми на v3
    _replace_flowchart(doc, new_flowchart)

    doc.save(docx_path)
    print(f"DOCX v3 збережено: {docx_path}")

    # 3. конвертація в PDF через Word COM
    try:
        word = win32com.client.DispatchEx("Word.Application")
        word.Visible = False
        doc_word = word.Documents.Open(docx_path)
        doc_word.SaveAs(pdf_path, FileFormat=17)
        doc_word.Close()
        word.Quit()
        print(f"PDF v3 створено: {pdf_path}")
    except Exception as e:
        print(f"Помилка Word COM: {e}")
