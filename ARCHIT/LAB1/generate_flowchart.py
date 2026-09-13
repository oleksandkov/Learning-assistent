"""
generate_flowchart.py
Скрипт для генерації векторної блок-схеми алгоритму у форматі SVG
та виклику рендерера PNG.
"""

import os
import subprocess

SVG_CONTENT = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1200 1750" width="1200" height="1750">
  <defs>
    <style>
      .title { font-family: 'Segoe UI', Arial, sans-serif; font-size: 26px; font-weight: bold; fill: #0f172a; text-anchor: middle; }
      .subtitle { font-family: 'Segoe UI', Arial, sans-serif; font-size: 15px; fill: #64748b; text-anchor: middle; }
      .node-text { font-family: 'Segoe UI', Arial, sans-serif; font-size: 14px; fill: #1e293b; text-anchor: middle; dominant-baseline: middle; }
      .node-bold { font-family: 'Segoe UI', Arial, sans-serif; font-size: 15px; font-weight: bold; fill: #0f172a; text-anchor: middle; dominant-baseline: middle; }
      .label-yes { font-family: 'Segoe UI', Arial, sans-serif; font-size: 14px; font-weight: bold; fill: #16a34a; }
      .label-no { font-family: 'Segoe UI', Arial, sans-serif; font-size: 14px; font-weight: bold; fill: #dc2626; }
      .arrow { stroke: #475569; stroke-width: 2.5; fill: none; marker-end: url(#arrowhead); }
      .line { stroke: #475569; stroke-width: 2.5; fill: none; }
      .border { stroke: #334155; stroke-width: 2; }
    </style>
    <marker id="arrowhead" markerWidth="8" markerHeight="6" refX="7" refY="3" orient="auto">
      <polygon points="0 0, 8 3, 0 6" fill="#475569" />
    </marker>
  </defs>

  <!-- Background -->
  <rect width="1200" height="1750" fill="#f8fafc" />

  <!-- Header -->
  <text x="600" y="50" class="title">БЛОК-СХЕМА АЛГОРИТМУ ПЕРЕВЕДЕННЯ</text>
  <text x="600" y="80" class="subtitle">двійкового числа (цілого або дробового) у десяткову систему числення</text>

  <!-- 1. Start -->
  <rect x="480" y="130" width="240" height="50" rx="25" ry="25" fill="#dbeafe" class="border" />
  <text x="600" y="155" class="node-bold">ПОЧАТОК</text>
  <path d="M 600 180 L 600 215" class="arrow" />

  <!-- 2. Input -->
  <polygon points="415 215, 810 215, 785 270, 390 270" fill="#fef3c7" class="border" />
  <text x="600" y="235" class="node-text">Введення двійкового числа S</text>
  <text x="600" y="255" class="node-text">(цілого або дробового)</text>
  <path d="M 600 270 L 600 305" class="arrow" />

  <!-- 3. Normalization -->
  <rect x="370" y="305" width="460" height="65" rx="8" ry="8" fill="#f1f5f9" class="border" />
  <text x="600" y="325" class="node-text">Видалення пробілів, заміна ',' на '.'</text>
  <text x="600" y="343" class="node-text">Визначення знаку (+/-) та очищення від нього</text>
  <text x="600" y="361" class="node-text">Розбиття рядка на S_int та S_frac</text>
  <path d="M 600 370 L 600 435" class="arrow" />

  <!-- 4. Validation Decision -->
  <polygon points="600 435, 860 480, 600 525, 340 480" fill="#fee2e2" class="border" />
  <text x="600" y="468" class="node-text">Валідація: Рядок містить тільки '0' і '1',</text>
  <text x="600" y="490" class="node-text">та не більше однієї крапки?</text>

  <!-- Error branch -->
  <path d="M 860 480 L 960 480" class="arrow" />
  <text x="870" y="465" class="label-no">Ні (Помилка)</text>

  <!-- Error output block -->
  <polygon points="975 450, 1160 450, 1145 510, 960 510" fill="#fecaca" class="border" />
  <text x="1060" y="473" class="node-text">Виведення помилки:</text>
  <text x="1060" y="493" class="node-text">«Некоректне число»</text>
  <path d="M 1060 510 L 1060 1635 L 720 1635" class="arrow" />

  <!-- Valid branch -->
  <path d="M 600 525 L 600 580" class="arrow" />
  <text x="615" y="555" class="label-yes">Так</text>

  <!-- 5. Init -->
  <rect x="370" y="580" width="460" height="70" rx="8" ry="8" fill="#f1f5f9" class="border" />
  <text x="600" y="605" class="node-text">Ініціалізація: D_int = 0;   D_frac = 0</text>
  <text x="600" y="628" class="node-text">N = довжина(S_int);   M = довжина(S_frac)</text>
  <path d="M 600 650 L 600 690" class="arrow" />

  <!-- 6. Integer Calculation -->
  <rect x="350" y="690" width="500" height="90" rx="8" ry="8" fill="#f1f5f9" class="border" />
  <text x="600" y="715" class="node-text">Обчислення цілої частини (i від 0 до N-1):</text>
  <text x="600" y="735" class="node-text">power = N - 1 - i;   bit = S_int[i]</text>
  <text x="600" y="755" class="node-text">D_int = D_int + bit · 2^power;   запис у формулу</text>
  <path d="M 600 780 L 600 847" class="arrow" />

  <!-- 7. Has Fraction? -->
  <polygon points="600 847, 830 890, 600 933, 370 890" fill="#fee2e2" class="border" />
  <text x="600" y="883" class="node-text">Чи задано дробову частину?</text>
  <text x="600" y="903" class="node-text">(довжина S_frac &gt; 0)</text>

  <path d="M 600 933 L 600 995" class="arrow" />
  <text x="615" y="960" class="label-yes">Так</text>

  <!-- Bypass fraction -->
  <path d="M 370 890 L 270 890 L 270 1130 L 370 1130" class="arrow" />
  <text x="310" y="875" class="label-no">Ні</text>

  <!-- 8. Fraction Calculation -->
  <rect x="350" y="995" width="500" height="90" rx="8" ry="8" fill="#f1f5f9" class="border" />
  <text x="600" y="1020" class="node-text">Обчислення дробової частини (j від 1 до M):</text>
  <text x="600" y="1040" class="node-text">power = -j;   bit = S_frac[j-1]</text>
  <text x="600" y="1060" class="node-text">D_frac = D_frac + bit · 2^(-j);   запис у формулу</text>
  <path d="M 600 1085 L 600 1130" class="arrow" />

  <!-- 9. Total Calculation -->
  <rect x="370" y="1130" width="460" height="75" rx="8" ry="8" fill="#f1f5f9" class="border" />
  <text x="600" y="1152" class="node-text">Підсумовування та врахування знаку:</text>
  <text x="600" y="1172" class="node-text">D = D_int + D_frac;   Якщо sign == '-' то D = -D</text>
  <text x="600" y="1192" class="node-text">Формування підсумкового рядка результату</text>
  <path d="M 600 1205 L 600 1245" class="arrow" />

  <!-- 10. Table formatting -->
  <rect x="360" y="1245" width="480" height="70" rx="8" ry="8" fill="#f1f5f9" class="border" />
  <text x="600" y="1270" class="node-text">Формування покрокової таблиці:</text>
  <text x="600" y="1292" class="node-text">Розряд (степінь), біт b_i, вага 2^i, внесок розряду</text>
  <path d="M 600 1315 L 600 1355" class="arrow" />

  <!-- 11. Output -->
  <polygon points="365 1355, 885 1355, 860 1440, 340 1440" fill="#dcfce7" class="border" />
  <text x="600" y="1380" class="node-bold">Виведення на екран:</text>
  <text x="600" y="1402" class="node-text">1) Десяткового результату D_10</text>
  <text x="600" y="1420" class="node-text">2) Полінома розкладу N_10 = ∑ (b_i · 2^i) та таблиці розрядів</text>
  <path d="M 600 1440 L 600 1610" class="arrow" />

  <!-- 12. End -->
  <rect x="480" y="1610" width="240" height="50" rx="25" ry="25" fill="#dbeafe" class="border" />
  <text x="600" y="1635" class="node-bold">КІНЕЦЬ</text>
</svg>
"""


def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    svg_path = os.path.join(base_dir, "algorithm_flowchart.svg")
    with open(svg_path, "w", encoding="utf-8") as f:
        f.write(SVG_CONTENT.strip())
    print(f"SVG flowchart saved to {svg_path}")

    # Запуск рендерингу PNG через PowerShell
    ps_script = os.path.join(base_dir, "render_flowchart.ps1")
    if os.path.exists(ps_script):
        subprocess.run(["pwsh", "-File", ps_script], check=True)


if __name__ == "__main__":
    main()

