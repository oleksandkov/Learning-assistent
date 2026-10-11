# Академічні схеми: ГОСТ 19.701-90 + UML 2.5

Схеми для курсових, дипломних і лабораторних робіт: суворо чорно-білі,
українською термінологією, без заголовків на самому зображенні, з ортогональною
геометрією. Генератори рахують ширину тексту й підганяють розміри блоків автоматично.

Відкрити галерею: [`index.html`](index.html) — фільтри «ГОСТ / UML», перемикач
підкладки (білий / шахівниця / темний) для перевірки прозорості, зум, завантаження SVG.

---

## Структура

```
diagram_examples_v1/
├── scripts/
│   ├── gost.py           # генератор блок-схем ГОСТ (Chart, Node)
│   └── uml_class.py      # генератор діаграм класів UML (Klass, Diagram)
├── examples/
│   ├── build_all.py      # перегенерувати всі 7 прикладів у SVG
│   ├── check_orthogonal.py  # перевірка геометрії (exit != 0 при помилці)
│   └── export_raster.py  # рендер SVG → PNG + JPEG
├── 01..07_*.svg/.png/.jpg  # згенеровані приклади
├── comparison/           # «скрипт vs draw.io» для блок-схеми Беллмана–Форда
├── drawio/               # draw.io-експорт схеми маршрутизації
├── SKILL.md              # довідка скіла
└── README.md             # цей файл
```

## Команди

```bash
cd diagram_examples_v1
python examples/build_all.py                                   # → SVG
python examples/check_orthogonal.py                            # перевірка
uv run --with cairosvg --with pillow python examples/export_raster.py   # → PNG/JPEG
```

`check_orthogonal.py` — обов'язковий крок перед рендером: він ловить діагональні
дроти у блок-схемі, вершини дротів усередині блоку та перекриття блоків.

## Приклади

| Файл | Опис | Стандарт |
| :--- | :--- | :--- |
| `01_gost_branching` | Розгалуження if-else (пошук максимуму) | ГОСТ 19.701-90 |
| `02_gost_loop_while` | Цикл while із сумами та зворотним зв'язком | ГОСТ 19.701-90 |
| `03_gost_loop_for` | Цикл for із шестикутником підготовки | ГОСТ 19.701-90 |
| `04_gost_subroutine` | Виклик підпрограми та з'єднувач | ГОСТ 19.701-90 |
| `05_gost_routing_branch` | Вкладені розгалуження (маршрутизація) | ГОСТ 19.701-90 |
| `06_gost_bellman_ford` | Алгоритм Беллмана–Форда | ГОСТ 19.701-90 |
| `07_uml_class` | Класи, наслідування, інтерфейс | UML 2.5 |

## Швидке використання генератора

```python
import sys, pathlib
sys.path.insert(0, str(pathlib.Path("scripts").resolve()))
from gost import Chart

c = Chart(width=520)
c.terminator("Початок", 160, 20)
c.io("Введення: a, b", 160, 96)
c.condition("a > b", 190, 280, right=True)
c.down(205, 318, 355)
pathlib.Path("out.svg").write_text(c.svg(), encoding="utf-8")
```

Детальніше — у [`SKILL.md`](SKILL.md).

## draw.io

Фінальні рисунки для курсової збираються в [diagrams.net](https://app.diagrams.net/?locale=uk).
Експорт PNG — через `convert.diagrams.net/export3.html` (`format=png&scale=3&transparent=true`),
щоб задати масштаб і прозорий фон. На паралелограмах draw.io обов'язково `perimeter=none`
(інакше биті діагоналі). Порівняння двох підходів — у [`comparison/index.html`](comparison/index.html).