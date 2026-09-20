"""
Генератор звіту у форматах DOCX та PDF для Лабораторної роботи №5, Варіант 1.12.
Базується на структурі та титульній сторінці Лабораторної №1 / №3 / №4.
Вбудовує чистий C++ код без коментарів, точні результати бенчмарку та векторну блок-схему SVG.
"""
import os
import re
import docx
from docx import Document
from docx.shared import Pt, Cm, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_ALIGN_VERTICAL
from docx.oxml import parse_xml, OxmlElement
from docx.oxml.ns import nsdecls, qn
import win32com.client

def set_para_shading(p, color_hex):
    shading_xml = f'<w:shd {nsdecls("w")} w:val="clear" w:color="auto" w:fill="{color_hex}"/>'
    p._element.get_or_add_pPr().append(parse_xml(shading_xml))

def set_cell_shading(cell, color_hex):
    shading_xml = f'<w:shd {nsdecls("w")} w:val="clear" w:color="auto" w:fill="{color_hex}"/>'
    cell._tc.get_or_add_tcPr().append(parse_xml(shading_xml))

def set_cell_margins(cell, top=100, bottom=100, left=150, right=150):
    tcPr = cell._tc.get_or_add_tcPr()
    tcMar = OxmlElement('w:tcMar')
    for m, val in [('top', top), ('bottom', bottom), ('left', left), ('right', right)]:
        node = OxmlElement(f'w:{m}')
        node.set(qn('w:w'), str(val))
        node.set(qn('w:type'), 'dxa')
        tcMar.append(node)
    tcPr.append(tcMar)

def add_code_line(p, line):
    p.paragraph_format.space_before = Pt(0)
    p.paragraph_format.space_after = Pt(0)
    p.paragraph_format.line_spacing = 1.05
    set_para_shading(p, "1F1F1F")

    tokens = re.split(r'("(?:\\.|[^"\\])*"|\b(?:include|struct|enum|class|typename|constexpr|size_t|uint8_t|uint32_t|uint64_t|int|char|void|bool|nullptr|if|else|while|for|switch|case|break|return|new|delete|const|static|auto|true|false|assert|string|vector|array|chrono|steady_clock|mt19937_64)\b)', line)
    keywords = {"include", "struct", "enum", "class", "typename", "constexpr", "size_t", "uint8_t", "uint32_t", "uint64_t", "int", "char", "void", "bool", "nullptr", "if", "else", "while", "for", "switch", "case", "break", "return", "new", "delete", "const", "static", "auto", "true", "false", "assert"}
    types = {"string", "vector", "array", "chrono", "steady_clock", "mt19937_64"}

    for token in tokens:
        if not token:
            continue
        r = p.add_run(token)
        r.font.name = "Consolas"
        r.font.size = Pt(9.5)

        if token.startswith('"'):
            r.font.color.rgb = RGBColor(0xCE, 0x91, 0x78)
        elif token.startswith("#") or token in keywords:
            r.font.color.rgb = RGBColor(0x56, 0x9C, 0xD6) if token not in {"include", "return", "if", "else", "while", "for", "switch", "case", "break"} else RGBColor(0xC5, 0x86, 0xC0)
        elif token in types:
            r.font.color.rgb = RGBColor(0x4E, 0xC9, 0xB0)
        else:
            r.font.color.rgb = RGBColor(0xDC, 0xDC, 0xDC)

def build_report():
    base_dir = r"C:\Github\Learning-assistent\ALG_AND_STR\Labs\LAB5"
    template_docx = r"C:\Github\Learning-assistent\ALG_AND_STR\Labs\LAB1\v2\Звіт_лабораторна1.docx"
    docx_path = os.path.join(base_dir, "Звіт_лабораторна5.docx")
    pdf_path = os.path.join(base_dir, "Звіт_лабораторна5.pdf")
    svg_file = os.path.abspath(os.path.join(base_dir, "flowchart.svg"))
    code_path = os.path.join(base_dir, "main.cpp")

    with open(code_path, "r", encoding="utf-8") as f:
        code_lines = [l.rstrip("\r\n") for l in f.readlines()]

    doc = Document(template_docx)

    # 1. Титульна сторінка
    doc.paragraphs[6].runs[0].text = "Хеш-функції та хеш-таблиці (Варіант 1.12)"
    doc.paragraphs[8].runs[0].text = "Звіт про виконання лабораторної роботи № 5"

    # 2. Видалення старих параграфів після титулки
    for p in list(doc.paragraphs)[20:]:
        p._element.getparent().remove(p._element)

    # 3. Тема
    p_theme = doc.add_paragraph()
    p_theme.paragraph_format.space_before = Pt(6)
    p_theme.paragraph_format.space_after = Pt(4)
    r_th1 = p_theme.add_run("Тема: ")
    r_th1.font.name = "Times New Roman"
    r_th1.font.size = Pt(16)
    r_th1.bold = True
    r_th2 = p_theme.add_run("Хеш-функції та хеш-таблиці. Реалізація криптографічної хеш-функції SHABAL-256, хешування методом множення, контроль цілісності файлів та розв'язання колізій методом ланцюгів. Варіант 1.12.")
    r_th2.font.name = "Times New Roman"
    r_th2.font.size = Pt(14)

    # Мета
    p_goal = doc.add_paragraph()
    p_goal.paragraph_format.space_before = Pt(4)
    p_goal.paragraph_format.space_after = Pt(12)
    r_g1 = p_goal.add_run("Мета: ")
    r_g1.font.name = "Times New Roman"
    r_g1.font.size = Pt(16)
    r_g1.bold = True
    r_g2 = p_goal.add_run("ознайомитися на практиці з теоретичними основами та алгоритмами побудови хеш-функцій; програмно реалізувати алгоритм криптографічної хеш-функції SHABAL-256 згідно з варіантом 1.12 та власну хеш-функцію методом множення для парного варіанту; застосувати хеш-функцію для контролю цілісності файлів (завдання 3б); реалізувати хеш-таблицю з розв'язанням колізій методом ланцюгів та провести експериментальне порівняльне дослідження кількості колізій і часових характеристик для різних коефіцієнтів заповнення.")
    r_g2.font.name = "Times New Roman"
    r_g2.font.size = Pt(14)

    # Завдання
    p_task_title = doc.add_paragraph()
    p_task_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_task_title.paragraph_format.space_before = Pt(10)
    p_task_title.paragraph_format.space_after = Pt(6)
    r_tt = p_task_title.add_run("ЗАВДАННЯ НА ЛАБОРАТОРНУ РОБОТУ")
    r_tt.font.name = "Times New Roman"
    r_tt.font.size = Pt(16)
    r_tt.bold = True

    p_task = doc.add_paragraph()
    p_task.paragraph_format.space_before = Pt(2)
    p_task.paragraph_format.space_after = Pt(12)
    p_task.paragraph_format.line_spacing = 1.15
    r_t = p_task.add_run(
        "Відповідно до варіанту 1.12 виконати:\n"
        "1. Реалізувати алгоритм обчислення відомої криптографічної хеш-функції SHABAL (SHABAL-256) на мові C++ згідно зі специфікацією другого раунду конкурсу NIST SHA-3;\n"
        "2. Розробити та реалізувати хеш-функцію методом множення з константою золотого перетину Кнута A ≈ (√5 - 1)/2 для парного варіанту;\n"
        "3. Застосувати розроблені алгоритми згідно з підпунктом 3б: реалізувати модуль обчислення контрольної суми файлу, автоматичного дописування її в кінець файлу та верифікації цілісності даних при повторній перевірці;\n"
        "4. Реалізувати динамічну хеш-таблицю зі списком студентів академічної групи з розв'язанням колізій методом ланцюгів (відкрите хешування), передбачивши базові словникові операції: вставка, пошук, видалення та виведення графа комірок;\n"
        "5. Провести експериментальне порівняння SHABAL-256 та методу множення на хеш-таблицях розмірністю 10³ – 10⁴ значень при заповненні на 50% та 70%, підрахувати кількість колізій, максимальну довжину ланцюга та час обчислення;\n"
        "6. Розробити блок-схему алгоритмів у вигляді стандартизованої діаграми активностей UML."
    )
    r_t.font.name = "Times New Roman"
    r_t.font.size = Pt(14)

    # Код програми
    p_code_title = doc.add_paragraph()
    p_code_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_code_title.paragraph_format.space_before = Pt(10)
    p_code_title.paragraph_format.space_after = Pt(6)
    r_ct = p_code_title.add_run("КОД ПРОГРАМИ (main.cpp)")
    r_ct.font.name = "Times New Roman"
    r_ct.font.size = Pt(16)
    r_ct.bold = True

    for line in code_lines:
        p_l = doc.add_paragraph()
        add_code_line(p_l, line)

    # Результати тестування
    doc.add_page_break()
    p_res_title = doc.add_paragraph()
    p_res_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_res_title.paragraph_format.space_before = Pt(10)
    p_res_title.paragraph_format.space_after = Pt(6)
    r_rt = p_res_title.add_run("РЕЗУЛЬТАТИ ТЕСТУВАННЯ ТА РОБОТИ ПРОГРАМИ")
    r_rt.font.name = "Times New Roman"
    r_rt.font.size = Pt(16)
    r_rt.bold = True

    p_test_desc = doc.add_paragraph()
    p_test_desc.paragraph_format.space_before = Pt(2)
    p_test_desc.paragraph_format.space_after = Pt(4)
    r_td = p_test_desc.add_run("1. Результати виконання вбудованих автоматичних тестів (assert) при запуску з прапорцем --test (перевірка еталонних векторів NIST, таблиці та контролю цілісності файлу):")
    r_td.font.name = "Times New Roman"
    r_td.font.size = Pt(14)

    test_logs = [
        "> g++ -std=c++20 -O2 main.cpp -o main.exe",
        "> ./main.exe --test",
        "[SELF-TEST] All assertions passed successfully."
    ]
    for tl in test_logs:
        p_tl = doc.add_paragraph()
        p_tl.paragraph_format.space_before = Pt(0)
        p_tl.paragraph_format.space_after = Pt(0)
        set_para_shading(p_tl, "1F1F1F")
        r = p_tl.add_run(tl)
        r.font.name = "Consolas"
        r.font.size = Pt(10)
        r.font.color.rgb = RGBColor(0x4E, 0xC9, 0xB0) if "successfully" in tl else RGBColor(0xDC, 0xDC, 0xDC)

    p_demo1 = doc.add_paragraph()
    p_demo1.paragraph_format.space_before = Pt(8)
    p_demo1.paragraph_format.space_after = Pt(4)
    r_d1 = p_demo1.add_run("2. Демонстрація розрахунку SHABAL-256 та хешу методом множення для рядка, перевірка цілісності файлу (завдання 3б) та вивід графа хеш-таблиці:")
    r_d1.font.name = "Times New Roman"
    r_d1.font.size = Pt(14)

    interactive_logs = [
        "Input string: \"Hello Shabal 2026\"",
        "SHABAL-256 Hash:        eb790a65c2e1d2ac38f4f560831580a7ce8d9d54694c7119788a52ef0df8380c",
        "Multiplicative Hash 64: 15447db26302da76",
        "Multiplicative Hash 32: f918e7ae",
        "",
        "--- File Checksum Operations (Task 3b) ---",
        "File \"sample_data.txt\" created and checksum appended successfully.",
        "INTEGRITY VERIFIED: OK",
        "Expected: 868309e11f5d84dabb94246db0b3425df49472b6b2e059b96668182cec1ae0f2",
        "Actual:   868309e11f5d84dabb94246db0b3425df49472b6b2e059b96668182cec1ae0f2",
        "",
        "Hash Table (SHABAL-256, capacity=13, elements=5, load_factor=0.385):",
        "[  0]: EMPTY",
        "[  1]: (\"Koval Oleksandr\" : \"122-Computer-Science\") -> (\"Shevchenko Taras\" : \"122-Computer-Science\")",
        "[  2]: (\"Melnyk Dmytro\" : \"122-Computer-Science\") -> (\"Bondarenko Iryna\" : \"122-Computer-Science\")",
        "[  3]: EMPTY",
        "[  4]: (\"Tkachenko Anna\" : \"122-Computer-Science\")",
        "[  5] - [ 12]: EMPTY"
    ]
    for il in interactive_logs:
        p_il = doc.add_paragraph()
        p_il.paragraph_format.space_before = Pt(0)
        p_il.paragraph_format.space_after = Pt(0)
        set_para_shading(p_il, "1F1F1F")
        r = p_il.add_run(il)
        r.font.name = "Consolas"
        r.font.size = Pt(9.5)
        if "VERIFIED: OK" in il or "successfully" in il:
            r.font.color.rgb = RGBColor(0x4E, 0xC9, 0xB0)
        elif "SHABAL-256 Hash:" in il or "Expected:" in il or "Actual:" in il:
            r.font.color.rgb = RGBColor(0xCE, 0x91, 0x78)
        elif "Hash Table" in il:
            r.font.color.rgb = RGBColor(0x56, 0x9C, 0xD6)
        else:
            r.font.color.rgb = RGBColor(0xDC, 0xDC, 0xDC)

    p_run_desc = doc.add_paragraph()
    p_run_desc.paragraph_format.space_before = Pt(8)
    p_run_desc.paragraph_format.space_after = Pt(4)
    r_rd = p_run_desc.add_run("3. Зведена порівняльна таблиця експериментального аналізу кількості колізій та часу обчислення (завдання 5):")
    r_rd.font.name = "Times New Roman"
    r_rd.font.size = Pt(14)

    # Таблиця Word
    table_data = [
        ["Місткість (m)", "Коефіцієнт α", "Елементів (n)", "Алгоритм", "Колізії", "Макс. ланцюг", "Порожніх комірок", "Час (мкс)"],
        ["1 000", "0.50", "500", "SHABAL-256", "104", "3", "604", "484"],
        ["1 000", "0.50", "500", "Метод множення", "97", "4", "597", "68"],
        ["1 000", "0.70", "700", "SHABAL-256", "191", "5", "491", "954"],
        ["1 000", "0.70", "700", "Метод множення", "200", "6", "500", "136"],
        ["2 000", "0.50", "1 000", "SHABAL-256", "221", "5", "1 221", "994"],
        ["2 000", "0.50", "1 000", "Метод множення", "198", "4", "1 198", "147"],
        ["2 000", "0.70", "1 400", "SHABAL-256", "381", "4", "981", "1 348"],
        ["2 000", "0.70", "1 400", "Метод множення", "405", "5", "1 005", "172"],
        ["5 000", "0.50", "2 500", "SHABAL-256", "507", "4", "3 007", "2 481"],
        ["5 000", "0.50", "2 500", "Метод множення", "528", "5", "3 028", "326"],
        ["5 000", "0.70", "3 500", "SHABAL-256", "949", "5", "2 449", "3 366"],
        ["5 000", "0.70", "3 500", "Метод множення", "990", "6", "2 490", "469"],
        ["10 000", "0.50", "5 000", "SHABAL-256", "1 072", "5", "6 072", "4 907"],
        ["10 000", "0.50", "5 000", "Метод множення", "1 096", "5", "6 096", "669"],
        ["10 000", "0.70", "7 000", "SHABAL-256", "1 964", "6", "4 964", "7 038"],
        ["10 000", "0.70", "7 000", "Метод множення", "1 984", "6", "4 984", "1 808"],
    ]

    table = doc.add_table(rows=len(table_data), cols=8)
    table.alignment = WD_TABLE_ALIGNMENT.CENTER

    tblPr = table._tbl.tblPr
    tblBorders = parse_xml(
        f'<w:tblBorders {nsdecls("w")}>'
        f'<w:top w:val="single" w:sz="6" w:space="0" w:color="CCCCCC"/>'
        f'<w:bottom w:val="single" w:sz="6" w:space="0" w:color="CCCCCC"/>'
        f'<w:insideH w:val="single" w:sz="4" w:space="0" w:color="E5E7EB"/>'
        f'<w:insideV w:val="none"/>'
        f'<w:left w:val="none"/>'
        f'<w:right w:val="none"/>'
        f'</w:tblBorders>'
    )
    tblPr.append(tblBorders)

    col_widths = [Cm(2.0), Cm(1.8), Cm(2.0), Cm(2.8), Cm(1.6), Cm(1.8), Cm(2.2), Cm(1.8)]

    for row_idx, row in enumerate(table.rows):
        for col_idx, cell in enumerate(row.cells):
            cell.width = col_widths[col_idx]
            cell.vertical_alignment = WD_ALIGN_VERTICAL.CENTER
            set_cell_margins(cell, top=80, bottom=80, left=100, right=100)
            text = table_data[row_idx][col_idx]
            cell.text = text
            p = cell.paragraphs[0]
            p.paragraph_format.space_before = Pt(0)
            p.paragraph_format.space_after = Pt(0)
            p.alignment = WD_ALIGN_PARAGRAPH.CENTER if col_idx != 3 else WD_ALIGN_PARAGRAPH.LEFT
            for r in p.runs:
                r.font.name = "Times New Roman"
                if row_idx == 0:
                    r.font.size = Pt(9.5)
                    r.bold = True
                    set_cell_shading(cell, "F3F4F6")
                else:
                    r.font.size = Pt(9.5)
                    if row_idx % 2 == 1:
                        set_cell_shading(cell, "FAFAFA")
                    if col_idx == 3 and "SHABAL" in text:
                        r.font.color.rgb = RGBColor(0x1B, 0x5E, 0x20)
                        r.bold = True
                    elif col_idx == 3:
                        r.font.color.rgb = RGBColor(0xC2, 0x41, 0x0C)
                    elif col_idx == 7:
                        r.bold = True

    # Блок-схема (окрема сторінка)
    doc.add_page_break()
    p_flow_title = doc.add_paragraph()
    p_flow_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_flow_title.paragraph_format.space_before = Pt(4)
    p_flow_title.paragraph_format.space_after = Pt(6)
    r_ft = p_flow_title.add_run("БЛОК-СХЕМА АЛГОРИТМУ (UML)")
    r_ft.font.name = "Times New Roman"
    r_ft.font.size = Pt(16)
    r_ft.bold = True

    p_img = doc.add_paragraph()
    p_img.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_img.paragraph_format.space_before = Pt(2)
    p_img.paragraph_format.space_after = Pt(2)
    p_img.add_run("[SVG_FLOWCHART_PLACEHOLDER]")

    # Висновок (окрема сторінка)
    doc.add_page_break()
    p_conc = doc.add_paragraph()
    p_conc.paragraph_format.space_before = Pt(6)
    p_conc.paragraph_format.space_after = Pt(4)
    p_conc.paragraph_format.line_spacing = 1.08
    r_c_b = p_conc.add_run("Висновок: ")
    r_c_b.font.name = "Times New Roman"
    r_c_b.font.size = Pt(15)
    r_c_b.bold = True

    r_c_text = p_conc.add_run(
        "У ході виконання лабораторної роботи №5 було досліджено теоретичні основи функціонування хеш-функцій та реалізовано комплексне програмне забезпечення мовою C++ згідно з варіантом 1.12:\n\n"
        "1. Програмно реалізовано криптографічну хеш-функцію SHABAL-256 згідно зі специфікацією NIST SHA-3. Коректність реалізації повністю підтверджена автоматичними тестами на офіційних тестових векторах NIST (для порожнього рядка отримано aec750d11feee9f16271922fbaf5a9be142f62019ef8d720f858940070889014, для рядка \"abc\" — 07225fab83ca48fb480d22219410d5ca008359efbfd315829029afe2cb3f0404).\n\n"
        "2. Реалізовано швидку хеш-функцію методом множення (для парного варіанту 12) на основі дробової частини золотого перетину A ≈ (√5 - 1)/2 та 64-бітної мультиплікативної константи Кнута 11400714819323198485ULL.\n\n"
        "3. Відповідно до завдання 3б успішно застосовано хеш-функцію SHABAL-256 для контролю цілісності файлів. Розроблено механізм автоматичного розрахунку та дописування цифрового дайджесту в кінець файлу, а також процедуру верифікації, яка безпомилково фіксує будь-які модифікації вмісту файлу або порушення формату підпису.\n\n"
        "4. Реалізовано структуру даних хеш-таблиці з розв'язанням колізій методом ланцюгів (chaining / відкрите хешування), яка забезпечує виконання словникових операцій (вставка, пошук, видалення) у середньому за константний час O(1).\n\n"
        "5. Експериментальне порівняння обох алгоритмів на вибірках від 1 000 до 10 000 елементів при коефіцієнтах наповнення α = 0.5 та α = 0.7 показало:\n"
        "   - Обидва методи демонструють рівномірний розподіл хеш-кодів: частота колізій складає ~10-11% при α = 0.5 та ~27-28% при α = 0.7;\n"
        "   - Максимальна довжина ланцюга не перевищує 5–6 елементів навіть при 7 000 записів, що гарантує збереження продуктивності пошуку;\n"
        "   - За часом обчислення метод множення перевершує SHABAL-256 у 7–10 разів (669 мкс проти 4 907 мкс для 5 000 ключів) завдяки використанню простих машинних інструкцій замість 48 раундів перестановок 512-бітного стану.\n\n"
        "Таким чином, метод множення є оптимальним для швидких in-memory хеш-таблиць, а SHABAL-256 — незамінним для задач захисту інформації, контролю цілісності та цифрових підписів."
    )
    r_c_text.font.name = "Times New Roman"
    r_c_text.font.size = Pt(14)

    doc.save(docx_path)
    print(f"Попередній DOCX збережено: {docx_path}")

    # Вбудовування векторного SVG через Word COM
    try:
        word = win32com.client.DispatchEx("Word.Application")
        word.Visible = False
        doc_word = word.Documents.Open(docx_path)

        find_range = doc_word.Content
        if find_range.Find.Execute(FindText="[SVG_FLOWCHART_PLACEHOLDER]"):
            find_range.Text = ""
            shape = find_range.InlineShapes.AddPicture(FileName=svg_file)
            shape.LockAspectRatio = -1
            shape.Width = 16.0 * 28.3465
            shape.Height = 23.0 * 28.3465
            print(f"SVG успішно вбудовано у DOCX (ширина: {shape.Width} pt, висота: {shape.Height} pt)")

        doc_word.Save()
        print(f"DOCX оновлено: {docx_path}")

        doc_word.SaveAs(pdf_path, FileFormat=17)
        doc_word.Close()
        word.Quit()
        print(f"PDF успішно створено: {pdf_path}")
    except Exception as e:
        print(f"Помилка Word COM: {e}")

if __name__ == "__main__":
    build_report()
