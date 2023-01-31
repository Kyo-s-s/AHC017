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

struct edge {
    int v, w, id;
};

struct IO {
    int N, M, D, K;
    vector<vector<edge>> G;
    vector<vector<int>> dist;
    IO () {
        cin >> N >> M >> D >> K;
        G.resize(N);
        for (int i = 0; i < M; i++) {
            int u, v, w;
            cin >> u >> v >> w;
            u--; v--;
            G[u].push_back({v, w, i});
            G[v].push_back({u, w, i});
        }
        for (int i = 0; i < N; i++) {
            int x, y;
            cin >> x >> y;
        }
        calc_dist();
    }

    void calc_dist() {
        dist.resize(N, vector<int> (N, INF));
        for (int st = 0; st < N; st++) {
            priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> que;
            que.push({0, st});
            dist[st][st] = 0;
            while (!que.empty()) {
                auto p = que.top(); que.pop();
                int d = p.first;
                int v = p.second;
                if (dist[st][v] < d) continue;
                for (auto e : G[v]) {
                    if (dist[st][e.v] > d + e.w) {
                        dist[st][e.v] = d + e.w;
                        que.push({dist[st][e.v], e.v});
                    }
                }
            }
        }
    }
} io;

unordered_map<bitset<3030>, long long> memo_score;

struct state {
    vector<bitset<3030>> data;
    vector<int> cnt;
    state() {
        data.resize(io.D);
        cnt.resize(io.D, 0);
    }

    void on(int d, int i) {
        if (data[d][i]) return;
        if (cnt[d] >= io.K) return;
        for (int j = 0; j < io.D; j++) {
            if (data[j][i]) {
                data[j][i] = false;
                cnt[j]--;
            }
        }
        data[d][i] = true;
        cnt[d]++;
    }


    long long score () {
        if (memo_score.count(data[0])) return memo_score[data[0]];
        long long ret = 0;
        bool connect = true;
        for (int day = 0; day < io.D; day++) {
            priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> que;
            que.push({0, 0});
            vector<int> dist(io.N, INF);
            dist[0] = 0;
            while (!que.empty()) {
                auto p = que.top(); que.pop();
                int d = p.first;
                int v = p.second;
                if (dist[v] < d) continue;
                for (auto e : io.G[v]) {
                    if (dist[e.v] > d + e.w && !data[day][e.id]) {
                        dist[e.v] = d + e.w;
                        que.push({dist[e.v], e.v});
                    }
                }
            }
            for (int i = 0; i < io.N; i++) {
                if (dist[i] == INF) connect = false;
                ret += dist[i] - io.dist[0][i];
            } 
        }
        return memo_score[data[0]] = ret;
    }

    void out() {
        vector<int> ans(io.M);
        for (int d = 0; d < io.D; d++) {
            for (int i = 0; i < io.M; i++) if (data[d][i]) {
                ans[i] = d + 1; 
            }
        }
        for (int i = 0; i < ans.size(); i++) {
            cout << ans[i] << (i == ans.size() - 1 ? "" : " ");
        }
        cout << endl;
    }
};


struct Solver {

    state initial_answer() {
        state ret;
        int d = 0;
        int cnt = 0;
        for (int i = 0; i < io.M; i++) {
            ret.on(d, i);
            cnt++;
            if (cnt > min(io.K, io.M / io.D)) {
                d++;
                cnt = 0;
            }
        } 
        return ret;
    }

    void solve() {
        state ans = initial_answer();
        long long best_score = ans.score();
        int cnt = 0;
        int update = 0;
        // while (timer.now() <= 5.0) {
        while (timer.now() <= 20.0) {
            cnt++;    
            state cur = ans;
            for (int x = 0; x < 10; x++) {
                int new_d = rand() % io.D;
                int i = rand() % io.M;
                cur.on(new_d, i);
            }

            long long cur_score = cur.score();
            if (cur_score < best_score) {
                best_score = cur_score;
                ans = cur;
                update++;
            }
        }

        ans.out();
    }
};

int main() {
    Solver solver;
    solver.solve();
}

