#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <deque>
using namespace std;

const int NMAX = 100005;

class Disjcnt {
 public:
  void solve() {
    read_input();
    print_output(get_result());
  }

 private:
  int n;
  int m;
  deque<int> S;
  vector<int> adj[NMAX];
  vector<int> idx[NMAX];
  vector<int> lowlink[NMAX];
  vector<bool> isMember;
  unsigned long sol;

  void read_input() {
    ifstream fin("disjcnt.in");
    fin >> n >> m;
    for (int i = 1, x, y; i <= m; i++) {
      fin >> x >> y;
      adj[x].push_back(y);
    }
    fin.close();
  }

  void Tarjan(int v, int &index) {
    idx[v] = index;
    lowlink[v] = index;
    index++;
    S.push_back(v);
    isMember[v] = true;

    for (auto u : adj[v]) {
      auto remIndex = std::find(adj[u].begin(), adj[u].end(), v);
      adj[u].pop_back(remIndex);

      if (idx[u] == -1) {
        Tarjan(u, index);
        lowlink[v] = min(lowlink[v], lowlink[u]);
      } else if (isMember[u] == true) {
        lowlink[v] = min(lowlink[v], idx[u]);
      }
    }

    if (lowlink[v] == idx[v]) {
      long no = 0;
      int u = 0;
      while (u != v) {
        u = S.back();
        no++;
        isMember[u] = false;
        S.pop_back();
      }
      sol += no * (no - 1) / 2;
    }
  }

  unsigned long get_result() {

    vector<bool> isMember(n + 1);
    for (auto v = 1; v <= n; v++) {
      idx[v] = -1;
    }

    Tarjan(1, 0);

    return sol;
  }

  void print_output(unsigned long result) {
    ofstream fout("disjcnt.out");
    fout << result;
    fout.close();
  }
};

int main() {
  Disjcnt *task = new Disjcnt();
  task->solve();
  delete task;
  return 0;
}
