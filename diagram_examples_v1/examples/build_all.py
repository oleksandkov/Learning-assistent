"""Regenerates every SVG in this folder. Run: python examples/build_all.py"""

from __future__ import annotations

import pathlib

import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "scripts"))

from gost import Chart  # noqa: E402
from uml_class import Diagram  # noqa: E402

OUT = HERE.parent


def write(name: str, svg: str) -> None:
    (OUT / name).write_text(svg, encoding="utf-8")
    print(f"{name}  {len(svg)} bytes")


# --------------------------------------------------------------------- 01
def branching() -> None:
    c = Chart()
    s = c.start()
    i = c.io("Введення: a, b", below=s)
    p = c.process("c = a * b + 2", below=i)
    d = c.decision("a > b", below=p)
    t = c.process("res = a", below=d)
    f = c.process("res = b", below=d, row=t, col=1)
    o = c.io("Виведення: res, c", below=t, dy=76)
    e = c.end(below=o)

    c.drop(s, i)
    c.drop(i, p)
    c.drop(p, d)
    c.drop(d, t)
    c.tag(t, "Так", "top", dx=9, dy=-16)
    c.branch(d, f, "right", "Ні")
    c.drop(f, o)
    c.drop(o, e)
    write("01_gost_branching.svg", c.render())


# --------------------------------------------------------------------- 02
def loop_while() -> None:
    c = Chart()
    s = c.start()
    i = c.io("Введення: n, arr", below=s)
    a = c.process("сума = 0\ni = 0", below=i)
    d = c.decision("i < n", below=a)
    b = c.process("сума = сума + arr[i]", below=d)
    inc = c.process("i = i + 1", below=b)
    o = c.io("Виведення: сума", below=d, dy=200)
    e = c.end(below=o)

    c.drop(s, i)
    c.drop(i, a)
    c.drop(a, d)
    c.drop(d, b)
    c.drop(b, inc)
    c.tag(b, "Так", "top", dy=-16)
    c.loop(inc, d, "left", at=c.lane("left", 40))
    c.branch(d, o, "right", "Ні", out=140)
    c.drop(o, e)
    write("02_gost_loop_while.svg", c.render())


# --------------------------------------------------------------------- 03
def loop_for() -> None:
    c = Chart()
    s = c.start()
    i = c.io("Введення: n, arr", below=s)
    pr = c.preparation("i = 0, n − 1, 1", below=i)
    d = c.decision("сума > 0", below=pr)
    b = c.process("сума = сума + arr[i]", below=d)
    inc = c.process("i = i + 1", below=b)
    o = c.io("Виведення: сума", below=d, dy=200)
    e = c.end(below=o)

    c.drop(s, i)
    c.drop(i, pr)
    c.drop(pr, d)
    c.drop(d, b)
    c.drop(b, inc)
    c.tag(b, "Так", "top", dy=-16)
    c.loop(inc, pr, "left", lane=132)
    c.branch(d, o, "right", "Ні", out=140)
    c.drop(o, e)
    write("03_gost_loop_for.svg", c.render())


# --------------------------------------------------------------------- 04
def subroutine() -> None:
    c = Chart()
    s = c.start()
    i = c.io("Введення: n, arr", below=s)
    sub = c.subroutine("Сортування(arr, n)\nСума(arr, n)", below=i)
    r = c.process("res = Сума(arr, n)", below=sub)
    o = c.io("Виведення: res", below=r)
    k = c.connector("А", below=o)
    k2 = c.connector("Б", row=k, col=1)
    m = c.process("Продовження на аркуші Б", below=k2, col=1, dy=70)
    e = c.end(below=m)

    c.drop(s, i)
    c.drop(i, sub)
    c.drop(sub, r)
    c.drop(r, o)
    c.drop(o, k)
    c.branch(k, m, "right", "")
    c.drop(m, e)
    write("04_gost_subroutine.svg", c.render())


# --------------------------------------------------------------------- 05
def routing_branching() -> None:
    """Reference image 1: branch right for the error path, left for the trunk."""
    c = Chart(wrap_w=470)
    s = c.start()
    i = c.io(
        "Введення зв’язків мережі,\nсерверів A і B, режиму пошуку", below=s
    )
    p = c.process("Розбір рядків таблиці,\nпобудова графа", below=i)
    d = c.decision("Дані коректні,\nA і B є в мережі?", below=p)
    s1 = c.subroutine(
        "Пошук маршруту алгоритмом\nДейкстри, вимірювання часу t₁", below=d
    )
    s2 = c.subroutine(
        "Пошук маршруту алгоритмом\nБеллмана-Форда, вимірювання часу t₂",
        below=s1,
    )
    cmp_ = c.process("Порівняння якості маршрутів", below=s2)
    o = c.io(
        "Виведення маршрутів, затримки,\nпропускної здатності, t₁ і t₂",
        below=cmp_,
    )
    tbl = c.process("Виділення зв’язків маршруту\nв таблиці", below=o)
    e = c.end(below=tbl)

    err = c.io("Виведення повідомлення\nпро помилку", row=s1, col=1)

    c.drop(s, i)
    c.drop(i, p)
    c.drop(p, d)
    c.drop(d, s1)
    c.tag(s1, "Так", "top", dx=9, dy=-18)
    c.drop(s1, s2)
    c.drop(s2, cmp_)
    c.drop(cmp_, o)
    c.drop(o, tbl)
    c.drop(tbl, e)
    c.branch(d, err, "right", "Ні", out=170)
    rx = c.rail_x(0, 44)
    my = tbl.bottom + c.gap * 0.5
    c.wire([(rx, err.bottom), (rx, my)])
    c.feed(rx, my, e)
    write("05_gost_routing_branch.svg", c.render())


# --------------------------------------------------------------------- 06
def bellman_ford() -> None:
    """Reference image 3: nested loops with shared left/right buses."""
    c = Chart(wrap_w=470)
    s = c.start()
    init = c.process(
        "Нумерація серверів (HashMap),\ndist[i] = ∞, dist[A] = 0, k = 0", below=s
    )
    d1 = c.decision("k < V − 1 ?", below=init)
    upd = c.process("updated = false", below=d1)
    d2 = c.decision("Є неопрацьоване\nребро (u, v)?", below=upd)
    d3 = c.decision("b(u, v) ≥ β\nі dist[u] < ∞?", below=d2)
    d4 = c.decision("dist[u] + c(u, v)\n< dist[v]?", below=d3)
    relax = c.process(
        "dist[v] = dist[u] + c(u, v),\nprev[v] = (u, v), updated = true", below=d4
    )
    d5 = c.decision("updated?", below=relax)
    k1 = c.process("k = k + 1", below=d5)
    restore = c.process("Відновлення маршруту\nвід B до A за prev[] (дек)", below=k1, dy=76)
    e = c.end(below=restore)

    c.drop(s, init)
    c.drop(init, d1)
    c.drop(d1, upd)
    c.drop(upd, d2)
    c.drop(d2, d3)
    c.drop(d3, d4)
    c.drop(d4, relax)
    c.drop(relax, d5)
    c.drop(d5, k1)
    c.tag(k1, "Так", "top", dx=9, dy=-18)

    # inner loop: back edge relax -> d2, lane just left of the block column
    left1 = c.lane("left", 40)
    left2 = left1 - 56
    left3 = left2 - 56
    c.loop(relax, d2, "left", at=left1)
    # outer loop: back edge k1 -> d1, outermost lane
    c.loop(k1, d1, "left", at=left3)
    # "Ні" out of the unprocessed-edge test: rail down and into the trunk
    c.branch(d2, restore, "left", "Ні", at=left2)
    # right bus collects every other exit
    rx = c.rail_x(0, 40)
    c.rail(rx, d1.cy, restore.top - 38)
    c.join(d1, "right", rx, "Ні")
    c.join(d3, "right", rx, "Ні")
    c.join(d4, "right", rx, "Ні")
    c.join(d5, "right", rx, "Ні")
    c.feed(rx, restore.top - 38, restore)
    c.drop(restore, e)
    write("06_gost_bellman_ford.svg", c.render())


# --------------------------------------------------------------------- 07
def uml_class() -> None:
    d = Diagram()
    main = d.klass(
        "Main",
        attrs=[
            "-rows: ObservableList<List<ConnectionRow>>",
            "-connections: TableView<ConnectionRow>",
        ],
        methods=[
            "-calculateRoute()",
            "-markRoute(r1, r2): boolean",
            "-deleteRouteRows()",
            "-deleteSelectedRows()",
            "-runBenchmark()",
        ],
        x=330,
        y=20,
    )
    server = d.klass("Server", attrs=["-name: String"], x=-10, y=840)
    gen = d.klass(
        "GraphGenerator",
        attrs=[],
        methods=["+generate(n, deg, rnd): List<Connection>"],
        stereotype="utility",
        x=20,
        y=260,
    )
    row = d.klass(
        "ConnectionRow",
        attrs=[
            "-server1, server2: StringProperty",
            "-bandwidth, delay: StringProperty",
            "-mark: ObjectProperty<Mark>",
            "-step: StringProperty",
        ],
        methods=["+isValid(): boolean", "+fromLine(line): ConnectionRow"],
        x=330,
        y=250,
    )
    mark = d.klass(
        "Mark", attrs=["NONE", "BOTH", "DIJKSTRA_ONLY", "BELLMAN_ONLY", "INVALID"],
        x=380,
        y=460,
    )
    iface = d.klass(
        "ShortestPathAlgorithm",
        stereotype="interface",
        methods=["+findShortestPath(g, a, b, s): PathResult", "+getName(): String"],
        x=660,
        y=250,
    )
    abstract = d.klass(
        "AbstractShortestPathAlgorithm",
        attrs=[],
        methods=[
            "+findShortestPath(g, a, b, s): PathResult",
            "#findWidestBottleneck(...): double",
            "#findCheapestPath(...): List<Connection>",
            "-buildResult(...): PathResult",
        ],
        abstract=True,
        x=660,
        y=430,
    )
    net = d.klass(
        "NetworkGraph",
        attrs=[
            "-servers: LinkedHashSet<String>",
            "-adjacencyList: HashMap<String, List<Connection>>",
            "-allConnections: ArrayList<Connection>",
        ],
        methods=[
            "+addConnection(s1, s2, bw, delay)",
            "+getNeighbors(s): List<Connection>",
            "+getAllConnections(): List<Connection>",
        ],
        x=180,
        y=600,
    )
    res = d.klass(
        "PathResult",
        attrs=[
            "-path: List<String>",
            "-edges: List<Connection>",
            "-totalDelay: double",
            "-bottleneckBandwidth: double",
            "-cost: double",
            "-executionTimeNanos: long",
        ],
        x=560,
        y=600,
    )
    dij = d.klass(
        "DijkstraAlgorithm",
        methods=["-reconstructPath(...): List<Connection>"],
        x=790,
        y=620,
    )
    bell = d.klass(
        "BellmanFordAlgorithm",
        methods=[
            "-buildIndex(...): Map<String, Integer>",
            "-reconstructPath(...): List<Connection>",
        ],
        x=1090,
        y=620,
    )
    nd = d.klass(
        "NodeDistance",
        attrs=["-server: String", "-distance: double"],
        methods=["+compareTo(o): int"],
        x=840,
        y=770,
    )
    setg = d.klass(
        "RoutingSettings",
        attrs=["-mode: RoutingMode", "-parameter: double"],
        methods=[
            "+getMinBandwidth(): double",
            "+getBalanceWeight(): double",
            "+edgeCost(e, alpha): double",
        ],
        x=1310,
        y=800,
    )
    mode = d.klass(
        "RoutingMode",
        attrs=["MIN_DELAY", "MIN_DELAY_WITH_LIMIT", "MAX_BANDWIDTH", "BALANCED"],
        x=1330,
        y=960,
    )
    conn = d.klass(
        "Connection",
        attrs=["-from, to: String", "-bandwidth: double", "-delay: double", "-id: int"],
        x=140,
        y=790,
    )

    d.inherits(row, main, source_anchor="top", target_anchor="bottom")
    d.depends(main, gen, label="", source_anchor="left", target_anchor="top")
    d.aggregates(row, mark, filled=True, source_anchor="bottom", target_anchor="top")
    d.implements(abstract, iface, source_anchor="top", target_anchor="bottom")
    d.inherits(dij, abstract, source_anchor="top", target_anchor="bottom",
               routing="straight")
    d.inherits(bell, abstract, source_anchor="top", target_anchor="bottom",
               routing="straight")
    d.depends(main, iface, source_anchor="right", target_anchor="top")
    d.aggregates(net, conn, filled=True, source_anchor="left", target_anchor="top")
    d.depends(net, res, source_anchor="right", target_anchor="left",
              routing="straight", multiplicity=("1", "*"))
    d.depends(net, server, source_anchor="left", target_anchor="bottom",
              routing="ortho", multiplicity=("1", "*"))
    d.depends(dij, nd, source_anchor="bottom", target_anchor="top")
    d.depends(setg, mode, source_anchor="bottom", target_anchor="top")
    d.depends(abstract, setg, source_anchor="right", target_anchor="top")
    d.depends(main, net, source_anchor="bottom", target_anchor="top",
              bends=[(600, 300), (360, 300)])
    d.depends(abstract, res, source_anchor="left", target_anchor="top",
              bends=[(830, 520), (700, 520)])
    d.note(640, 555, "створює")
    write("07_uml_class.svg", d.render())


if __name__ == "__main__":
    branching()
    loop_while()
    loop_for()
    subroutine()
    routing_branching()
    bellman_ford()
    uml_class()