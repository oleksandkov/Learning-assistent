#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <cstdlib>

using namespace std;

struct Edge {
    int u;
    int v;
    int w;
};

struct DSU {
    vector<int> parent;
    vector<int> rnk;

    DSU(int n) : parent(n + 1), rnk(n + 1, 0) {
        for (int i = 0; i <= n; i++) parent[i] = i;
    }

    int find(int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }

    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rnk[a] < rnk[b]) swap(a, b);
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;
        return true;
    }
};

void generateRandomGraph(int n, int m) {
    if (n < 1) n = 1;
    if (m < n - 1) m = n - 1;

    int maxEdges = (n > 1) ? n * (n - 1) / 2 : 0;
    if (m > maxEdges) m = maxEdges;

    mt19937 gen(12345);
    uniform_int_distribution<int> wdist(1, 30000);

    set<pair<int,int>> used;
    vector<Edge> edges;
    edges.reserve(m);

    for (int i = 2; i <= n; i++) {
        uniform_int_distribution<int> vdist(1, i - 1);
        int u = vdist(gen);
        int a = min(u, i), b = max(u, i);
        used.insert({a, b});
        edges.push_back({u, i, wdist(gen)});
    }

    uniform_int_distribution<int> vdist(1, n);
    while ((int)edges.size() < m && (int)used.size() < maxEdges) {
        int u = vdist(gen);
        int v = vdist(gen);
        if (u == v) continue;
        int a = min(u, v), b = max(u, v);
        if (used.count({a, b})) continue;
        used.insert({a, b});
        edges.push_back({u, v, wdist(gen)});
    }

    ofstream out("input.txt");
    if (!out) {
        cerr << "Не вдалося створити input.txt\n";
        return;
    }
    out << n << " " << edges.size() << "\n";
    for (const auto& e : edges) {
        out << e.u << " " << e.v << " " << e.w << "\n";
    }
    out.close();
}

int main(int argc, char* argv[]) {
    if (argc >= 3) {
        int n = atoi(argv[1]);
        int m = atoi(argv[2]);
        generateRandomGraph(n, m);
        cout << "input.txt: N=" << n << ", M=" << m << "\n";
        return 0;
    }

    ifstream in("input.txt");
    if (!in) {
        cout << "Файл input.txt не знайдено.\n";
        generateRandomGraph(8, 15);
        in.open("input.txt");
        if (!in) {
            cerr << "Не вдалося створити input.txt\n";
            return 1;
        }
    }

    int n, m;
    if (!(in >> n >> m)) {
        cerr << "Помилка читання N, M\n";
        return 1;
    }

    vector<Edge> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        Edge e;
        if (!(in >> e.u >> e.v >> e.w)) {
            cerr << "Помилка читання ребра " << i + 1 << "\n";
            return 1;
        }
        edges.push_back(e);
    }
    in.close();

    sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.w < b.w;
    });

    DSU dsu(n);

    long long total = 0;
    int used = 0;

    for (const auto& e : edges) {
        if (dsu.unite(e.u, e.v)) {
            total += e.w;
            if (++used == n - 1)
                break;
        }
    }

    ofstream out("output.txt");
    if (!out) {
        cerr << "Не вдалося створити output.txt\n";
        return 1;
    }
    out << total << "\n";
    out.close();

    cout << "Вага мінімального кістяка: " << total << "\n";
    return 0;
}
