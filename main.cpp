#include<bits/stdc++.h>
using namespace std;

#define INF 1000000000

struct Timer {
    time_t start;
    Timer() {
        start = clock();
    }

    double now() {
        return (double)(clock() - start) / CLOCKS_PER_SEC;
    }
} timer;

// https://www.slideshare.net/yosupo/ss-46612984
// https://github.com/yosupo06/Algorithm/blob/55622ec4a96c2fc1e11f810cb981eb4a86eee8fc/src/datastructure/radixheap.hpp
int bsr(uint x) { return 31 - __builtin_clz(x); }
template <class D> struct RadixHeap {
    using C = typename make_unsigned<decltype(D().key)>::type;
    static int bsr1(C x) { return (x == 0) ? 0 : bsr(x) + 1; }
    size_t sz = 0, cnt = 0;
    C last = 0;
    vector<vector<D>> v = vector<vector<D>>(sizeof(C) * 8 + 1);
    RadixHeap() {}
    void push(D x) {
        assert(decltype(x.key)(last) <= x.key);
        sz++;
        v[bsr1(x.key ^ last)].push_back(x);
    }
    void pull() {
        if (cnt < v[0].size()) return;
        int i = 1;
        while (v[i].empty()) i++;
        last = min_element(v[i].begin(), v[i].end(), [&](D l, D r) {
                   return l.key < r.key;
               })->key;
        for (D x : v[i]) {
            v[bsr1(x.key ^ last)].push_back(x);
        }
        v[i].clear();
    }
    D top() {
        pull();
        return v[0][cnt];
    }
    void pop() {
        pull();
        sz--;
        cnt++;
    }
    size_t size() const { return sz; }
    bool empty() const { return sz == 0; }
};

struct edge {
    int v, w, id;
};

struct uv { int u, v, w, id; };

struct IO {
    int N, M, D, K;
    vector<vector<edge>> G;
    vector<uv> E;
    vector<vector<int>> dist;
    vector<int> base;

    IO () {
        cin >> N >> M >> D >> K;
        G.resize(N);
        E.resize(M);
        for (int i = 0; i < M; i++) {
            int u, v, w;
            cin >> u >> v >> w;
            u--; v--;
            G[u].push_back({v, w, i});
            G[v].push_back({u, w, i});
            E[i] = {u, v, w, i};
        }
        struct P { int x, y, id; };
        P ul = {INF, -INF, -1}, ur = {-INF, -INF, -1}, dl = {INF, INF, -1}, dr = {-INF, INF, -1};
        for (int i = 0; i < N; i++) {
            int x, y;
            cin >> x >> y;
            if (x < ul.x && y > ul.y) ul = {x, y, i};
            if (x > ur.x && y > ur.y) ur = {x, y, i};
            if (x < dl.x && y < dl.y) dl = {x, y, i};
            if (x > dr.x && y < dr.y) dr = {x, y, i};
        }
        base = {ul.id, ur.id, dl.id, dr.id};
        calc_dist();
    }


    void Dijkstra(vector<int> &dist, int st, const bitset<3000> &flag = 0) {
        dist.resize(N, INF);
        struct P { uint key; int v; };
        RadixHeap<P> que;
        // priority_queue<P, vector<P>, function<bool(P, P)>> que([](P a, P b) { return a.key > b.key; });
        que.push({0, st});
        dist[st] = 0;
        while (!que.empty()) {
            P p = que.top(); que.pop();
            auto [d, v] = tie(p.key, p.v);
            if (dist[v] < (int)d) continue;
            for (auto e : G[v]) if (!flag[e.id]) {
                if (dist[e.v] > (int)d + e.w) {
                    dist[e.v] = d + e.w;
                    que.push({(uint)dist[e.v], e.v});
                }
            }
        }
    }

    void calc_dist() {
        dist.resize(N, vector<int> (N, INF));
        vector<int> tmp;
        for (int i = 0; i < N; i++) {
            Dijkstra(tmp, i);
            dist[i] = tmp;
        }
    }
} io;

struct UnionFind {
    int n, cnt;
    vector<int> parent;
    UnionFind() : n(0), cnt(0) {}
    UnionFind(int n) : n(n), cnt(n), parent(n, -1) {}

    int merge(int a, int b) {
        assert(0 <= a && a < n && 0 <= b && b < n);
        int x = leader(a), y = leader(b);
        if (x == y) return x;
        if (-parent[x] < -parent[y]) swap(x, y);
        parent[x] += parent[y];
        parent[y] = x;
        cnt--;
        return x;
    } 

    int leader(int a) {
        assert(0 <= a && a < n);
        if (parent[a] < 0) return a;
        return parent[a] = leader(parent[a]);
    }

    bool same(int a, int b) {
        assert(0 <= a && a < n && 0 <= b && b < n);
        return leader(a) == leader(b);
    }

    int size(int a) {
        assert(0 <= a && a < n);
        return -parent[leader(a)];
    }

    int count() { return cnt; }

    vector<vector<int>> groups() {
        vector<int> leader_buf(n), group_size(n);
        for (int i = 0; i < n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        vector<vector<int>> result(n);
        for (int i = 0; i < n; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < n; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(
            remove_if(result.begin(), result.end(),
                      [&](const vector<int> &v) { return v.empty(); }),
            result.end());
        return result;
    }
};

unsigned long long rnd() {
    static unsigned long long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    unsigned long long t = (x ^ (x << 11));
    x = y; y = z; z = w;
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

unordered_map<bitset<3000>, long long> memo_score;

long long bitset_score(const bitset<3000> &flag) {
    if (memo_score.count(flag)) return memo_score[flag];
    long long res = 0;
    vector<int> dist(io.N, INF);
    for (auto st : io.base) {
        io.Dijkstra(dist, st, flag);
        for (int i = 0; i < io.N; i++) {
            res += dist[i] - io.dist[st][i];
        }
    }
    return memo_score[flag] = res;
}

struct state {
    vector<bitset<3000>> data;
    vector<int> cnt;
    vector<int> ans;
    state() {
        data.resize(io.D);
        cnt.resize(io.D, 0);
        ans.resize(io.M, -1);
    }

    bool on(int d, int i) {
        if (data[d][i]) return false;
        if (cnt[d] >= io.K) return false;
        if (ans[i] != -1) {
            data[ans[i]][i] = false;
            cnt[ans[i]]--;
        }
        data[d][i] = true;
        cnt[d]++;
        ans[i] = d;
        return true;
    }


    long long score () {
        long long ret = 0;
        for (int day = 0; day < io.D; day++) {
            ret += bitset_score(data[day]);
        }
        return ret;
    }

    void out() {
        for (int i = 0; i < (int)ans.size(); i++) {
            cout << ans[i] + 1 << (i == (int)ans.size() - 1 ? "" : " ");
        }
        cout << endl;
    }
};

struct Solver {

    state initial_answer(bool random = false) {
        state ret;

        // // 長い順に交互に割り振っていく？
        vector<uv> sortE = io.E;
        if (random) {
            shuffle(sortE.begin(), sortE.end(), mt19937(rnd()));
        } else {
            sort(sortE.begin(), sortE.end(), [](uv a, uv b) { return a.w > b.w; });
        }
        vector<vector<int>> cnt(io.D, vector<int>(io.N, 0));
        for (auto [u, v, w, i] : sortE) {
            vector<pair<int, int>> ind;
            for (int d = 0; d < io.D; d++) {
                ind.push_back({(cnt[d][u] + cnt[d][v]) * 10 + ret.cnt[d], d});
            }
            sort(ind.begin(), ind.end());
            for (auto [_, d] : ind) {
                if (ret.cnt[d] <= io.M / io.D && ret.on(d, i)) {
                    cnt[d][u]++;
                    cnt[d][v]++;
                    break;
                }
            }
        }


        // 連結？
        vector<bool> connect(io.D, false);
        vector<uv> E = io.E;
        mt19937 mt(chrono::steady_clock::now().time_since_epoch().count());
        for (int cnt = 0; cnt < 10; cnt++) {
            if (all_of(connect.begin(), connect.end(), [](bool b) { return b; })) break;
            shuffle(E.begin(), E.end(), mt);
            for (int d = 0; d < io.D; d++) if (!connect[d]) {
                UnionFind uf(io.N);
                for (auto [u, v, w, i] : io.E) if (!ret.data[d][i]) {
                    uf.merge(u, v);
                }
                for (auto [u, v, w, i] : io.E) if (ret.data[d][i] && !uf.same(u, v)) {
                    int nxt_d = (d + rnd() % (io.D - 1) + 1) % io.D;
                    if (!ret.on(nxt_d, i)) continue;
                    connect[nxt_d] = false;
                    uf.merge(u, v);
                }
                if (uf.count() == 1) connect[d] = true;
            }
        }

        return ret;
    }

    void solve() {

        state ans = initial_answer();
        long long best_score = ans.score();
        for (int i = 0; i < 10; i++) {
            state cur = initial_answer(true);
            long long cur_score = cur.score();
            if (cur_score < best_score) {
                best_score = cur_score;
                ans = cur;
            }
        }

        // cerr << "initial score: " << best_score << endl;

        int cnt = 0;
        int update = 0;
        double start_temp = 100000, end_temp = 100;
        double TL = 5.0;

        while (timer.now() <= TL) {
            cnt++;    
            state cur = ans;
            int change = rnd() % 10 + 1;
            for (int x = 0; x < change; x++) {
                int new_d = rnd() % io.D;
                int i = rnd() % io.M;
                cur.on(new_d, i);
            }


            long long cur_score = cur.score();
            double temp = start_temp + (end_temp - start_temp) * timer.now() / TL;
            double prob = exp((double)(best_score - cur_score) / temp);
            // if (cur_score < best_score) {
            if (prob > (double)(rnd() % 100000) / (double)100000) {
                best_score = cur_score;
                ans = cur;
                update++;
            }
        }

        // cerr << "cnt = " << cnt << endl;
        // cerr << "update = " << update << endl;

        ans.out();
    }
};

int main() {
    Solver solver;
    solver.solve();
}

