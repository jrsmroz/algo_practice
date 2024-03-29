templates/barcodes.cpp                                                                              0000666 0001751 0001752 00000005620 12562612334 015453  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>

// 0001101(0) -> 3:2:1:1
// 0011001(1) -> 2:2:2:1
// 0010011(2) -> 2:1:2:2 
// 0111101(3) -> 1:4:1:1
// 0100011(4) -> 1:1:3:2
// 0110001(5) -> 1:2:3:1
// 0101111(6) -> 1:1:1:4
// 0111011(7) -> 1:3:1:2
// 0110111(8) -> 1:2:1:3
// 0001011(9) -> 3:1:1:2

int barcode_digit_old(const char *buff, unsigned size, unsigned width) {
	static unsigned codes[10][4] = {
		{3, 2, 1, 1},
		{2, 2, 2, 1},
		{2, 1, 2, 2},
		{1, 4, 1, 1},
		{1, 1, 3, 2},
		{1, 2, 3, 1},
		{1, 1, 1, 4},
		{1, 3, 1, 2},
		{1, 2, 1, 3},
		{3, 1, 1, 2}
	};
	// find the code
	unsigned code[4] = {0, 0, 0, 0};
	// char bit = 0;
	char bit = '0';
	unsigned pos = 0;
	for (unsigned i = 0; i < size && pos < 4; ++i) {
		if (buff[i] == bit) {
			code[pos]++;
		} else if (++pos < 4) {
			// switch between 0 and 1
			bit =  '0' + ((bit+1) & ('\1')); // bit = '0' + (bit + 1)%2;
			code[pos] = 1;
		}
	}

	// normalize the code
	for (unsigned i = 0; i < 4; ++i) {
		code[i] /= width;
	}

	for (unsigned i = 0; i < 10; ++i) {
		if (codes[i][0] == code[0] && codes[i][1] == code[1] &&
			codes[i][2] == code[2] && codes[i][3] == code[3])
			return i;
	}

	return -1;
}

int barcode_digit(const char *buff, unsigned size, unsigned width) {
	unsigned code_buff[4] = {0, 0, 0, 0};
	char bit = '1';
	for (unsigned pos = 0, i = 0; pos < 4; ++pos) {
		bit =  '0' + ((bit+1) & 0x1); // this will do bit = '0' + (bit + 1)%2;
		for ( ; i < size && buff[i] == bit; ++i)
			code_buff[pos]++;
	}

	unsigned code = ((code_buff[0]/width) << 12) + 
					((code_buff[1]/width) << 8)  + 
					((code_buff[2]/width) << 4)  + 
					(code_buff[3]/width);

	switch (code) {
		case 0x3211: return 0;
		case 0x2221: return 1;
		case 0x2122: return 2;
		case 0x1411: return 3;
		case 0x1132: return 4;
		case 0x1231: return 5;
		case 0x1114: return 6;
		case 0x1312: return 7;
		case 0x1213: return 8;
		case 0x3112: return 9;
	}

	return -1;
}

int barcode(const char *buff, unsigned size, unsigned width, unsigned &sum) {
	int digits[8];
	int result = 0;
	sum = 0;

	for (int i = 0, pos = 0; i < size; i += 7) {
		int digit = barcode_digit(buff+i, 7, width);
		if (digit < 0)
			return -1;
		result = result*10 + digit;
		digits[pos++] = digit;
		sum += digit;
	}

	unsigned hash = (digits[0] + digits[2] + digits[4] + digits[6])*3 + 
		   			(digits[1] + digits[3] + digits[5]);
	if ((hash + digits[7])%10 != 0)
		return -1;

	return result;
}

int main() {

	const char* test_codes[10] = {
		"0001101",
		"0011001",
		"0010011",
		"0111101",
		"0100011",
		"0110001",
		"0101111",
		"0111011",
		"0110111",
		"0001011"
	};
	for (int i = 0; i < 10; ++i)
		std::cout << barcode_digit(test_codes[i], 7, 1) << std::endl;
	std::cout << barcode_digit("00000011001111", 14, 2) << std::endl;

	const char* bin_code = "01110110110001011101101100010110001000110100100110111011";
	unsigned sum;
	int res = barcode(bin_code, 56, 1, sum);
	std::cout << res << std::endl;
	std::cout << sum << std::endl;

	return 0;
}                                                                                                                templates/bisection.cpp                                                                             0000666 0001751 0001752 00000003343 12562060012 015636  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <cassert>

// inclusive
template <typename T>
bool bsearch_first(const T &key, T *array, unsigned b, unsigned e, unsigned &pos) {

	while (b < e) {
		unsigned mid = (b+e)/2;
		if (key <= array[mid]) {
			e = mid;
		} else {
			b = mid+1;
		}
	}

	if (array[b] == key) {
		pos = b;
		return true;
	}
	return false;
}

// inclusive upper bound
template <typename T>
bool bsearch_last(const T &key, T *array, unsigned b, unsigned e, unsigned &pos) {

	while (b < e) {
		unsigned mid = (b+e+1)/2;
		if (key < array[mid]) {
			e = mid-1;
		} else {
			b = mid;
		}
	}

	if (array[e] == key) {
		pos = e;
		return true;
	}
	return false;
}

// find first bigger them
template <typename T>
bool bsearch_greater(const T &key, T *array, unsigned b, unsigned e, unsigned &pos) {
	e++;
	while (b < e) {
		unsigned mid = (b+e)/2;
		if (key < array[mid]) {
			e = mid;
		} else {
			b = mid+1;
		}
	}

	pos = e;
	if (array[e] == key) {
		return true;
	}
	return false;
}

int main() {
	{
		int array[12] = {0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9};
		unsigned lower = 0;
		unsigned upper = 0;
		for (int i = 0; i < 10; ++i) {
			bool found = bsearch_first(i, array, 0, 11-1, lower);
			bsearch_last(i, array, 0, 11-1, upper);
			std::cout << i << " l:" << lower << " u:" << upper << "g:" << std::endl;
		}
	}


	// {
	// 	int array[4] = {1, 1, 1, 1};
	// 	unsigned lower = 0;
	// 	unsigned upper = 0;
	// 	bool found = 0;
	// 	// found = bsearch_first(2, array, 0, 3, lower);
	// 	// found = found && bsearch_last(2, array, 0, 3, upper);
	// 	// std::cout << "f:" << found << " l:" << lower << " u:" << upper << std::endl;
	// 	// upper = bsearch_greater(2, array, 0, 3, lower);
	// 	std::cout << " l:" << lower << " u:" << upper << std::endl;
	// }

}                                                                                                                                                                                                                                                                                             templates/combinatorics/                                                                            0000777 0001751 0001752 00000000000 12647417747 016035  5                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             templates/combinatorics/permutation.cpp                                                             0000666 0001751 0001752 00000003160 12562633121 021066  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <cassert>
#include <iostream>


// bool next_permutation_(int *seq, unsigned size, int perm_num) {
// 	assert(size);

// 	static max_perm = 1;
// 	if (perm_num == 0) {
// 		for (int i = 1; i <= size; ++i)
// 			max_perm *= i;
// 	}
// 	acc++;

// 	static unsigned i = 0;
// 	swap(seq[i], seq[i+1]);
// 	++i;
// 	if (i == size) {
// 		++i;

// 	}


// 	// if (acc == 0) {
// 	// 	i = 0, j = 1;
// 	// 	return false;
// 	// }

// 	return true;
// }

template<typename T>
void swap(T &a, T&b) {
	T t = a; 
	a = b; b = t;
}

bool next_permutation_down(int *seq, unsigned size) {
	static int pos = 0;
	if (pos == size-1) {
		pos = 0;
		swap(seq[pos], seq[pos+1]);
		return false;
	}
	swap(seq[pos], seq[pos+1]);
	pos++;
	return true;
}


bool next_permutation_up(int *seq, unsigned size) {
	static int pos = size-1;
	if (pos == 0) {
		pos = size-1;
		swap(seq[pos], seq[pos-1]);
		return false;
	}
	swap(seq[pos], seq[pos-1]);
	pos--;
	return true;
}

bool next_permutation(int *seq, unsigned size) {
	enum Direction {down, up};
	static Direction dir = down;

	static unsigned long long perm_num = 0;
	static unsigned long long max_perm = 1;
	if (perm_num == max_perm) {
		perm_num = 0;
		max_perm = 1;
		return false;
	}

	if (perm_num == 0) {
		for (int i = 1; i <= size; ++i)
			max_perm *= i;
	}
	++perm_num;

	if (dir == down && !next_permutation_down(seq, size))
		dir = up;
	else if (dir == up && !next_permutation_up(seq, size))
		dir = down;

	return true;
}

int main() {
	int V[4] = {1, 2, 3, 4};

	while (next_permutation(V, 4)) {
		for (int i = 0; i < 4; ++i)
			std::cout << V[i] << " ";
		std::cout << std::endl;
	}

	return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                templates/graph/                                                                                    0000755 0001751 0001752 00000000000 12647417776 014300  5                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             templates/graph/belman_ford_shortest_path.cpp                                                       0000644 0001751 0001752 00000003206 12337317176 022211  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

static const int Inf =  1<<30;
static const int None =  -1;

struct Edge : public pair<int, int> {
	Edge(int weight, int dst) : pair(weight, dst) { }
	inline int dst() {
		return this->second;
	}
	inline int weight() {
		return this->first;
	}
};

struct Algo {
	void readInput() {
		int vertices, edges;
		cin >> vertices >> edges;
		adjlist_.resize(vertices);
		while (edges--) {
			int u, v, weight;
			cin >> u >> v >> weight;
			adjlist_[u].push_back(Edge(weight, v));
		}
		prev_.resize(vertices);
		dist_.resize(vertices);
	}

	void run() {
		belmanFordShort(0);
	}

	void belmanFordShort(int start) {
		fill(prev_.begin(), prev_.end(), None);
		fill(dist_.begin(), dist_.end(), Inf);
		dist_[start] = 0;

		int n = adjlist_.size();
		for (int i = 1; i <= n-1; ++i)
			relaxAll();
	}

	void relaxAll() {
		for (int u = 0; u < adjlist_.size(); ++u) {
			for (int vidx = 0; vidx < adjlist_[u].size(); ++vidx) {
				int v = adjlist_[u][vidx].dst();
				int uv_weiht = adjlist_[u][vidx].weight();
				relax(u, v, uv_weiht);
			}
		}
	}

	void relax(int u, int v, int uv_weight) {
		if (dist_[u] + uv_weight < dist_[v]) {
			dist_[v] = dist_[u] + uv_weight;
			prev_[v] = u;
		}
	}

	void printOutput() {
		for (int i = 0; i < dist_.size(); ++i) {
			cout << "0->" << i << ": " << dist_[i] << endl;
		}
	}

	vector<vector<Edge> > adjlist_;
	vector<int> prev_;
	vector<int> dist_;

};

int main(int argc, char* argv[]) {
	int tests_count;
	cin >> tests_count;
	while (tests_count--) {
		Algo algo;
		algo.readInput();
		algo.run();
		algo.printOutput();
	}
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                          templates/graph/floydWarshal.cpp                                                                    0000644 0001751 0001752 00000003032 12625561653 017427  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
using namespace std;

static const int INF = 1 << 30;

struct Algo {
	void readInput() {
		cin >> N_;
		for (int i = 0; i < N_; ++i)
			for (int j = 0; j < N_; ++j)
				cin >> adjmatrix_[i][j];
	}

	void run() {
		findDc();
		floydWarshall();
		findCc();
	}

	void findDc() {
		// Find highest DC
		dc_ = 0;
		for (int i = 0; i < N_; ++i) {
			int dci = 0;
			for (int j = 0; j < N_; ++j)
				dci += adjmatrix_[i][j];
			if (dci > dc_)
				dc_ = dci;
		}
	}

	void findCc() {
		// Find lowest CC
		cc_ = 1<<30;
		for (int i = 0; i < N_; ++i) {
			int cci = 0;
			for (int j = 0; j < N_; ++j)
				cci += shortest_[i][j];
			if (cci < cc_)
				cc_ = cci;
		}
	}

	void floydWarshall() {
		for (int i = 0; i < N_; ++i) {
			for (int j = 0; j < N_; ++j) {
				if (adjmatrix_[i][j] != 0)
					shortest_[i][j] = adjmatrix_[i][j];
				else if (i == j)
					shortest_[i][j] = 0;
				else
					shortest_[i][j] = INF;
			}
		}

		for (int x = 0; x < N_; ++x) {
			for (int u = 0; u < N_; ++u) {
				for (int v = 0; v < N_; ++v) {
					if (shortest_[u][v] > shortest_[u][x] + shortest_[x][v]) {
						shortest_[u][v] = shortest_[u][x] + shortest_[x][v];
					}
				}
			}
		}
	}

	void printOutput() {
		cout << dc_ << " " << cc_ << endl;
	}

	int N_;
	int dc_;
	int cc_;
	int adjmatrix_[1000][1000];
	int shortest_[1000][1000];
} algo;

int main(int argc, char* argv[]) {
	int tests_count;
	cin >> tests_count;
	for (int i = 1; i <= tests_count; ++i) {
		algo.readInput();
		algo.run();
		cout << "#" << i << " ";
		algo.printOutput();
	}
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      templates/graph/dijkstra_shortest_path.cpp                                                          0000644 0001751 0001752 00000003141 12343021641 021534  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

#define INF (1<<30)

struct Edge : public pair<int, int> {
	Edge(int weight, int dst) : pair(weight, dst) { }
	inline int dst() {
		return this->second;
	}
	inline int weight() {
		return this->first;
	}
};

struct Algo {
	void readInput() {
		int vertices, edges;
		cin >> vertices >> edges;
		adjlist_.resize(vertices);
		while (edges--) {
			int u, v, weight;
			cin >> u >> v >> weight;
			adjlist_[u].push_back(Edge(weight, v));
		}
		prev_.resize(vertices);
		dist_.resize(vertices);
	}

	void run() {
		dijkstraShort(0);
	}

	void dijkstraShort(int start) {
		fill(prev_.begin(), prev_.end(), -1);
		fill(dist_.begin(), dist_.end(), INF);
		dist_[start] = 0;
		priority_queue<Edge> edge_queue;
		edge_queue.push(Edge(dist_[start], start));

		while (!edge_queue.empty()) {
			Edge e = edge_queue.top(); edge_queue.pop();
			int u = e.dst();

			for (int i = 0; i < adjlist_[u].size(); ++i) {
				Edge uv = adjlist_[u][i];
				int v = uv.dst();

				// Find best distance from starting vertex
				if (dist_[v] > dist_[u] + uv.weight()) {
					prev_[v] = u;
					dist_[v] = dist_[u] + uv.weight();
					edge_queue.push(uv);
				}
			}
		}
	}

	void printOutput() {
		for (int i = 0; i < dist_.size(); ++i) {
			cout << "0->" << i << ": " << dist_[i] << endl;
		}
	}

	vector<vector<Edge> > adjlist_;
	vector<int> prev_;
	vector<int> dist_;

};

int main(int argc, char* argv[]) {
	int tests_count;
	cin >> tests_count;
	while (tests_count--) {
		Algo algo;
		algo.readInput();
		algo.run();
		algo.printOutput();
	}
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                               templates/graph/prim_mst.cpp                                                                        0000644 0001751 0001752 00000003105 12336636132 016615  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

#define INF (1<<30)

struct Edge : public pair<int, int> {
	Edge(int weight, int dst) : pair(weight, dst) {}
	inline int dst() {
		return this->second;
	}
	inline int weight() {
		return this->first;
	}
};

struct Algo {
	void readInput() {
		int vertices, edges;
		cin >> vertices >> edges;
		adjlist_.resize(vertices);
		while (edges--) {
			int u, v, weight;
			cin >> u >> v >> weight;
			adjlist_[u].push_back(Edge(weight, v));
		}
		prev_.resize(vertices);
		dist_.resize(vertices);
	}

	void run() {
		primMst(0);
	}

	void primMst(int start) {
		fill(prev_.begin(), prev_.end(), -1);
		fill(dist_.begin(), dist_.end(), INF);
		dist_[start] = 0;
		priority_queue<Edge> edge_queue;
		edge_queue.push(Edge(dist_[start], start));

		while (!edge_queue.empty()) {
			Edge e = edge_queue.top();
			edge_queue.pop();
			int u = e.dst();

			for (int i = 0; i < adjlist_[u].size(); ++i) {
				Edge uv = adjlist_[u][i];
				int v = uv.dst();

				// Find best distance from current tree
				if (dist_[v] > uv.weight()) {
					dist_[v] = uv.weight();
					prev_[v] = u;
					edge_queue.push(uv);
				}
			}
		}
	}

	void printOutput() {
		int sum = 0;
		for (int i = 0; i < dist_.size(); ++i) {
			sum += dist_[i];
		}
		cout << sum << endl;
	}

	vector<vector<Edge> > adjlist_;
	vector<int> prev_;
	vector<int> dist_;

};

int main(int argc, char* argv[]) {
	int tests_count;
	cin >> tests_count;
	while (tests_count--) {
		Algo algo;
		algo.readInput();
		algo.run();
		algo.printOutput();
	}
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                           templates/graph/DFS-pseudo.py                                                                       0000644 0001751 0001752 00000000743 12312525027 016541  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             T = 0
def time():
    global T
    T += 1
    return T

def dfs(G):
    for v in vertices_of(G):
        set_vertex(v, color=WHITE, parrent=None)

    for v in vertices_of(G):
        dfs_visit(v)


def dfs_visit(v):
    global time
    time  += 1
    set_vertex(v, color=GRAY, visit_time=time)

    for u in adjacent_to(u):
        if color_of(u) is WHITE:
            set_vertex(u, parrent=v)
            dfs_visit(u)
    time  += 1
    set_vertex(u, color=BLACK, finish_time=time)                             templates/graph/graph.cpp                                                                           0000644 0001751 0001752 00000002755 12314560406 016072  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <vector>
#include <list>
#include <queue>

using std::vector;
using std::list;
using std::queue;

/**
 * Edge representation
 */
struct Edge {
    int source;
    int destination;
};


/**
 * Vertex representation
 */
struct Vertex {
    list<Edge> edges;
};
typedef list<Edge>::iterator EdgeIterator;

/**
 * Graph representation
 */
class Graph {
    vector<Vertex> vertices_;

public:
    explicit Graph(int vertices_count = 0);
    void BroadFirstSearch(int start_vertex);
    void DeepFirstSearch();
    void StronglyConnectedComponentsGraph();
    void TopologicalSort();
    void MinimalSpaningTree();
};

/**
 * Constructor of graph
 */
Graph::Graph(int vertices_count) {
    vertices_.resize(vertices_count+1);
    vertices_[0] = Vertex(); // A guard, not really need as it would be constructed anyway

}

/**
 * BFS
 */
void Graph::BroadFirstSearch(int start_vertex) {

    // Init visit queue
    vector<bool> visited_vertices(vertices_.size(), false);
    queue<int> visit_queue;
    visit_queue.push(start_vertex);

    // Visit nodes
    while (!visit_queue.empty()) {
        // Get vertex from queue head
        int vertex_number = visit_queue.front();
        visit_queue.pop();

        // For each neigbouring edge
        EdgeIterator edge_iterator = vertices_[vertex_number].edges.begin();
        EdgeIterator edges_end = vertices_[vertex_number].edges.end();
        for ( ; edge_iterator != edges_end; ++edge_iterator) {

        }

    }
}

int main() {
    Graph g;
    return 0;
}                   templates/graph/TopoSort.cpp                                                                        0000644 0001751 0001752 00000003104 12336635116 016554  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Algo {
	void readInput() {
		int vertices, edges;
		cin >> vertices >> edges;
		adjlist_.resize(vertices);
		while (edges--) {
			int u, v;
			cin >> u >> v;
			adjlist_[u].push_back(v);
		}
	}

	void run() {
		topoSort();
	}

	void topoSort() {
		vector<bool> visited(adjlist_.size(), false);
		vector<int> finish(adjlist_.size(), -1);
		vector<int> sorted;
		sorted.reserve(adjlist_.size());

		finish_timer_ = 0;
		for (int u = 0; u < adjlist_.size(); ++u) {
			if (!visited[u])
				topoDfs(u, visited, finish, sorted);
		}
		reverse(sorted.begin(), sorted.end());
		have_cycle_ = haveCycle(finish);
	}

	void topoDfs(int u, vector<bool>& visited, vector<int>& finish, vector<int>& sorted) {
		visited[u] = true;
		for (int i = 0; i < adjlist_.size(); ++i) {
			int v = adjlist_[u][i];
			if (!visited[v])
				topoDfs(v, visited, finish, sorted);
		}
		finish[u] = ++finish_timer_;
		sorted.push_back(u);
	}

	bool haveCycle(vector<int>& finish) {
		for (int u = 0; u < adjlist_.size(); ++u) {
			for (int i = 0; adjlist_.size(); ++i) {
				int v = adjlist_[u][i];
	            if (finish[v] <= finish[u]) {
	            	return true;
	            }
			}
		}
		return false;
	}

	void printOutput() {
		cout << have_cycle_ << endl;
	}

	bool have_cycle_;
	vector<vector<int> > adjlist_;
	int finish_timer_;
};

int main(int argc, char* argv[]) {
	int tests_count;
	cin >> tests_count;
	while (tests_count--) {
		Algo algo;
		algo.readInput();
		algo.run();
		algo.printOutput();
	}
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                            templates/graph/adjacency_list.cpp                                                                  0000755 0001751 0001752 00000005011 12315000617 017726  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <vector>
#include <list>
#include <queue>

using std::vector;
using std::list;
using std::queue;

/**
 * Optional template parameter
 */
struct Optional {
};

/**
 * Tuple, up to thre elements only
 */
template <typename First, typename Second, typename Third  = Optional>
struct Tuple {
    First first;
    Second second;
    Third third;
};
template <typename First, typename Second>
struct Tuple<First, Second, Optional> {
    int first;
    int second;
};

/**
 * Edge representation
 */
template <typename Data = Optional>
struct StoredEdge {

    StoredEdge(int source, int adjacent) { 
        this->source() = source;
        this->adjacent() = adjacent;
    }

    StoredEdge(int source, int adjacent, Data data) { 
        this->source() = source;
        this->adjacent() = adjacent;
        this->data() = data;
    }

    inline int& source() {
        return self.first;
    }

    inline int& adjacent() {
        return self.second;
    }

    inline Data& data() {
        return self.third;
    }

    Tuple<int, int, Data> self;
};





/**
 * Vertex representation
 */
// struct StoredVertex {
//     list<Edge> edges;
// };
// typedef list<Edge>::iterator EdgeIterator;

/**
 * Adjacency list
 */
template<typename VertexData = std::size_t, typename EdgeData = Optional>
class AdjacencyList {
public:
    typedef ::StoredEdge<EdgeData> StoredEdge;
    typedef list<StoredEdge> EdgeList;

    vector<EdgeList> vertices_edges_;
    vector<VertexData> vertices_;

public:
    std::size_t AddVertex(const VertexData& data) {
        vertices_.push_back(data);
        vertices_edges_.push_back( EdgeList() );
        return vertices_.size()-1;
    }
    void AddEdge(std::size_t u, std::size_t v) {
        AdjacencyListOf(u).push_back( MakeEdge(u, v) );
    }
    void AddEdge(std::size_t u, std::size_t v, const EdgeData& data) {
        AdjacencyListOf(u).push_back( MakeEdge(u, v, data) );
    }
    inline EdgeList& AdjacencyListOf(std::size_t u) {
        return vertices_edges_.at(u); // Safty
        // return vertices_edges_[u]; // Performance
    }
    inline StoredEdge MakeEdge(std::size_t u, std::size_t v) {
        return StoredEdge(u, v);
    }
    inline StoredEdge MakeEdge(std::size_t u, std::size_t v, const EdgeData& data) {
        return StoredEdge(u, v, data);
    }
};



int main() {
    
    typedef AdjacencyList<int> Graph;
    Graph graph;
    std::size_t u = graph.AddVertex(8);;
    std::size_t v = graph.AddVertex(1);

    graph.AddEdge(u, v);
    graph.AddEdge(555, 15);

    return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       templates/graph/ford-fulkerson.py                                                                   0000666 0001751 0001752 00000003411 12600725335 017573  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             # G = [
#     [(1, 16), (2, 13)],     # s
#     [(3, 12), (2, 10)],     # v1
#     [(1, 4), (4, 14)],      # v2
#     [(2, 9), (-1, 20)],     # v3
#     [(3, 7),],              # v4
#     [(4, 4),],              # t
# ]

# G = [
#     [(1, 1), (2, 1), (3, 1)],   # s
#     [(4, 1)],                   # v1
#     [(5, 1)],      # v2
#     [(5, 1)],     # v3
#     [(-1, 1),],              # v4
#     [(-1, 1),],              # t
#     [(6,1)]
# ]

G = [
    [(1, 1), (2, 1), (3, 1), (4, 1), (5, 1)],   # s

    [(5+2, 1), (5+1, 1)],
    [(5+2, 1), (5+3, 1), (5+4, 1)],
    [(5+1, 1), (5+5, 1)],
    [(5+1, 1), (5+2, 1), (5+5, 1)],
    [(5+2, 1)],

    [(-1, 1)],
    [(-1, 1)],
    [(-1, 1)],
    [(-1, 1)],
    [(-1, 1)],

    [(11, 1)]
]


def residual_paths(G, F, s, t):

    def adj_residual(u):
        for (v, w) in G[u]:
            if not visited[v] and F[(u, v)] < w:
                yield (v, w)

    def dfs(u, p=[]):

        for v, w in adj_residual(u):
            visited[v] = True
            ret = dfs(v, p + [(u, v, w)])
            if ret:
                return ret
        return p if (u == t) else None

    while True:
        visited = [False for v in G]
        p = dfs(s)
        if not p:
            break
        yield p


def ford_fulkerson(G):
    flows = dict()
    edges = list((u, v, c) for (u, u_edges) in enumerate(G) for (v, c) in u_edges)
    print edges
    for (u, v, _) in edges:
        flows[(u, v)] = 0
        flows[(v, u)] = 0

    for path in residual_paths(G, flows, 0, -1):
        path_capacity =  min(w for _, _ , w in path)
        for u, v, _ in path:
            flows[(u, v)] += path_capacity
            flows[(v, u)] = -flows[(u, v)]
    return sum(flows[(0, v)] for v, w in G[0])
        

if __name__ == "__main__":
    print ford_fulkerson(G)                                                                                                                                                                                                                                                       templates/graph/kruskal_mst.cpp                                                                     0000644 0001751 0001752 00000003543 12376604067 017336  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

#define NONE (-1)

typedef pair<int, int> vertex_pair_t;

struct Edge : public pair<int, vertex_pair_t> {
	Edge(int weight, int src, int dst) : pair<int, vertex_pair_t>(weight, vertex_pair_t(src, dst)) {}
	inline int src() {
		return this->second.first;
	}
	inline int dst() {
		return this->second.second;
	}
	inline int weight() {
		return this->first;
	}
};

struct Algo {
	void readInput() {
		int vertices, edges;
		cin >> vertices >> edges;
		edges_.reserve(edges);
		while (edges--) {
			int u, v, weight;
			cin >> u >> v >> weight;
			edges_.push_back(Edge(weight, u, v));
		}
		sets_.resize(vertices);
		mst_adjlist_.resize(vertices);
	}

	void run() {
		kruskalMst(0);
	}

	void kruskalMst(int start) {
		fill(sets_.begin(), sets_.end(), NONE); //Bug? not NONE?
		sort(edges_.begin(), edges_.end());

		for (int i = 0; i < edges_.size(); ++i) {
			int u = edges_[i].src();
			int v = edges_[i].dst();
			if (findSet(u) != findSet(v)) {
				addMstEdge(edges_[i]);
				makeUnion(u, v);
			}
		}
	}

	void addMstEdge(Edge& e) {
		mst_adjlist_[e.src()].push_back(e);
	}

	int findSet(int u) {
		if (sets_[u] == NONE)
			return u;
		sets_[u] = findSet(sets_[u]);
		return sets_[u];
	}

	void makeUnion(int u, int v) {
		int xset = findSet(u);
		int vset = findSet(v);
		if (xset != vset)
			sets_[xset] = vset;
	}

	void printOutput() {
		int sum = 0;
		for (int i = 0; i < mst_adjlist_.size(); ++i)
			for (int j = 0; j < mst_adjlist_[i].size(); ++j)
				sum += mst_adjlist_[i][j].weight();
		cout << sum << endl;
	}

	vector<vector<Edge> > mst_adjlist_;
	vector<int> sets_;
	vector<Edge> edges_;
};

int main(int argc, char* argv[]) {
	int tests_count;
	cin >> tests_count;
	while (tests_count--) {
		Algo algo;
		algo.readInput();
		algo.run();
		algo.printOutput();
	}
	return 0;
}
                                                                                                                                                             templates/graph/BFS-pseudo.py                                                                       0000644 0001751 0001752 00000001202 12312525522 016526  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             # G - graf
# s - wierzcholek startowy
# WHITE - wierzcholek nieodwiedzony
# GRAY - wierzcholek zakolejkowany
# BLACK - wierzchołek przetworzonu

def bfs(G, start):

    for v in vertices_of(G):
        set_vertex(v, color=WHITE, distance=INFINITY, parent=None)
    set_vertex(start, color=GRAY, distance=0, parent=None)

    # While queue is not empty
    queue = [ start ]
    while queue:
        u = queue.pop(0)
        for v in adjacent_to(u):
            if color_of(v) is WHITE:
                set_vertex(v, color=GRAY, distance=distance_of(u)+1, parent=u)
                queue.append(v)
        set_vertex(u, color=BLACK)









                                                                                                                                                                                                                                                                                                                                                                                              templates/sort/                                                                                     0000755 0001751 0001752 00000000000 12562151322 014140  5                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             templates/sort/sorting.cpp                                                                          0000664 0001751 0001752 00000011622 12562615101 016335  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <utility>
#include <vector>
#include <limits>
#include <iterator>
#include <cassert>
#include <list>
#include <type_traits>
using namespace std;


template<typename I> 
void ins_sort(I first, I last) {
	for (auto pos = first+1; pos != last; ++pos) {
		for (auto k = pos; k != first && *k < *(k-1); --k) {
			swap(*k, *(k-1));
		}
	}
}

template<typename I> 
void ins_sort2(I first, I last) {
	for (auto pos = first+1; pos != last; ++pos) {
		auto k = pos;
		auto el = *pos;
		for (; k != first && el < *(k-1); --k) {
			*k = *(k-1);
		}
		*k = el;
	}
}

template<typename I> 
void sel_sort(I first, I last) {
	for ( ; first != last; ++first) {
		auto min = first;
		for (auto el = first+1; el != last; ++el) {
			if (*el < *min) {
				min = el;
			}
		}
		swap(*first, *min);
	}
}

template<typename I> 
void sel_sort_r(I first, I last) {
	if (first == last)
		return;
	auto min = first;
	for (auto el = first+1; el != last; ++el)
		if (*el < *min)
			min = el;
	sel_sort_r(++first, last);
}



template <typename T, T min, T max, typename I>
void count_sort(I first, I last) {
	static_assert(numeric_limits<T>::is_integer, "requires integers");

	vector<T> cnt(max-min+1, 0);
	for (auto it = first; it != last; ++it)
		++cnt[(*it)-min];
	for (size_t i = 0; i < cnt.size(); ++i) {
		while (cnt[i]-- > 0 && first != last) {
			*(first++) = (T)(i+min);
		}
	}
}

template <typename I, typename T>
void _merge_sort(I first, I last, vector<T> &buff) {
	size_t n =  distance(first, last);
	auto pivot = first;
	advance(pivot, n/2);

	if (n > 1) {
		_merge_sort(first, pivot, buff);
		_merge_sort(pivot, last, buff);
	}

	auto left = first;
	auto right = pivot;

	while (n--) {
		if ((left != pivot && *left <= *right) || right == last) {
			buff.push_back(move(*(left++)));
		} else if (right != last && *left > *right){
			buff.push_back(move(*(right++)));
		} else {
			break;
		}
	}
	move(left, pivot, back_inserter(buff));
	move(right, last, back_inserter(buff));
	move(buff.begin(), buff.end(), first);
	buff.clear();
}

template <typename I>
void merge_sort(I first, I last) {
	using T = typename remove_reference<decltype(*first)>::type;
	vector<T> buff;
	buff.reserve(distance(first, last));
	_merge_sort(first, last, buff);
}


template <typename T> 
struct ListNode {
	T val;
	ListNode *next;
};


template <typename T>
void _list_merge(ListNode<T> **edge, ListNode<T> *left, ListNode<T> *right) {
	if (left && (!right || left->val < right->val)) {
		*edge = left;
		_list_merge(&left->next, left->next, right);
	} else if (right) {
		*edge = right;
		_list_merge(&right->next, left, right->next);
	}
}

template <typename T>
ListNode<T>* _list_merge_sort(ListNode<T> *first, ListNode<T> *last, size_t size) {
	assert(first != nullptr);

	if (first->next == last || size <= 1) {
		first->next = nullptr;
		return first;
	}

	ListNode<T> *pivot = first;
	for (size_t dist = size/2; pivot->next && dist > 0; --dist)
		pivot = pivot->next;

	ListNode<T> *left =  _list_merge_sort(first, pivot, size/2);
	ListNode<T> *right = _list_merge_sort(pivot, last, size-size/2);

	ListNode<T> *result;
	ListNode<T> **edge = &result;
	// _list_merge(&result, left, right); //backup if below not working
	while (size--) {
		if (left && (!right || left->val < right->val)) {
			*edge = left;
			left = left->next;
		} else if (right) {
			*edge = right;
			right = right->next;
		}
		edge = &(*edge)->next;
	}

	return result;
}


template <typename T>
ListNode<T>* list_merge_sort(ListNode<T> *first) {
	assert(first != nullptr);

	size_t size = 1;
	for (ListNode<T>* node = first; node->next != nullptr; node = node->next)
		++size;

	return _list_merge_sort(first, (ListNode<T>*)nullptr, size);
}


template <typename T>
int partition(T *array, int p, int b, int e) {
	while (b < e) {
		while (array[p] < array[e]) // pivot < pivot will stop it
			--e;
		while (b < e && array[b] <= array[p])
			++b;
		if (b < e)
			swap(array[b], array[e]);
	}
	swap(array[b], array[p]);
	return e;
}

template <typename T>
void quick_sort(T *array, int b, int e) {
	if (e-b <= 0)
		return;
	int p = partition(array, b, b, e);
	quick_sort(array, b, p-1);
	quick_sort(array, p+1, e);
}

int main() {
	int tab[] = {1, 4, 2, 3, -1, 100, 88, 0, 0};
	// int tab[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	
	// ins_sort2(tab, tab+9);
	// merge_sort(tab, tab+9);
	// quick_sort(tab, 0, 8);
	// for (auto e: tab)
	// 	cout << e << " ";
	// cout << endl;

	ListNode<int> *list;
	ListNode<int> **edge = &list;
	for (auto& v : tab) {
		ListNode<int> *node = new ListNode<int>;
		node->val = v;
		*edge = node;
		edge = &node->next;
	}
	list = list_merge_sort(list);
	size_t size = 9;
	for (auto *node = list; node != nullptr && size-- > 0; node = node->next) {
		cout << node->val << " -> ";
	}
	cout << endl;

	// list<int> l{99, 0, 1, -10, 100, -10, 3, 8};
	// // count_sort<int, -10, 100>(l.begin(), l.end());
	// merge_sort(l.begin(), l.end());

	// for (auto e: l)
	// 	cout << e << " ";
	// cout << endl;
	return 0;
}
                                                                                                              templates/sort/a.out                                                                                0000755 0001751 0001752 00000033313 12562151322 015117  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             ELF          >    P	@     @        "          @ 8 	 @         @       @ @     @ @     �      �                   8      8@     8@                                          @       @     �      �                    �      �`     �`     �      �                          `     `     �      �                   T      T@     T@     D       D              P�td   �      �@     �@     T       T              Q�td                                                  R�td   �      �`     �`                        /lib64/ld-linux-x86-64.so.2          GNU                       GNU �D�	 ���K�S��(�h               !          !��	(E�L�CyIk�                            �                                                                    s                     4                     �                     �                     3                       �                      O                       #                     �                      Z                     �      	@             i      �@             �     � `           �       	@              libstdc++.so.6 __gmon_start__ _Jv_RegisterClasses _ITM_deregisterTMCloneTable _ITM_registerTMCloneTable _ZNSt8ios_base4InitD1Ev __gxx_personality_v0 _ZSt4cout _ZNSolsEi _Znwm _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc _ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_ _ZNSolsEPFRSoS_E _ZNSt8ios_base4InitC1Ev libgcc_s.so.1 _Unwind_Resume libc.so.6 __assert_fail __cxa_atexit __libc_start_main GCC_3.0 GLIBC_2.2.5 CXXABI_1.3 GLIBCXX_3.4                           L         P&y   �        i         ui	   �                  ӯk   �     t)�   �      �`                   � `                    `                     `                   ( `                   0 `                   8 `                   @ `                   H `                   P `        	           X `                   ` `                   h `                   p `                   x `                   H��H��  H��t�3   H���              �5�  �%�  @ �%�  h    ������%�  h   ������%�  h   ������%z  h   �����%r  h   �����%j  h   �����%b  h   �����%Z  h   �p����%R  h   �`����%J  h	   �P����%B  h
   �@����%:  h   �0����%2  h   � ���1�I��^H��H���PTI��`@ H���@ H��=
@ �G����fD  �� ` UH-� ` H��H��w]ø    H��t�]�� ` ���    �� ` UH-� ` H��H��H��H��?H�H��u]ú    H��t�]H�ƿ� ` ���    �=�   uUH���~���]��  ��@ H�=�   t�    H��tU� ` H����]�{��� �s���UH��H�Ā�E�   �E�   �E�   �E�   �E������E�d   �E�X   �E�    �E�    H�E�H�E�H�E�H�E�H�E�H�E�H�E�H��$H�E��>H�E�H�E��   �t���H�E�H�E��H�EȉH�E�H�U�H�H�E�H��H�E�H�E�H�E�H;E�u�H�E�H����   H�E�H�E�	   H�E�H�E��+H�E�� �ƿ� ` �Z�����@ H������H�E�H�@H�E�H�}� tH�E�H�P�H�U�H��t�   ��    ��u��	@ �� ` �����    �H��������UH��H���}��u��}�u'�}���  u��!` ������ ` ��!` ��@ ������UH����  �   ����]�UH��H�� H�}�H�}� u��@ ��   ��@ ��@ ����H�E�   H�E�H�E��H�E�H�E�H�@H�E�H�E�H�@H��u�H�U�H�E�    H���   ��UH��H��PH�}�H�u�H�U�H�}� u� @ �x   ��@ ��@ � ���H�E�H�@H;E�tH�}�wH�E�H�@    H�E��  H�E�H�E�H�E�H��H�E��H�E�H�@H�E�H�m�H�E�H�@H��tH�}� u�H�E�H��H��H�M�H�E�H��H���K���H�E�H�E�H��H��H�E�H)�H��H�M�H�E�H��H��� ���H�E�H�E�H�E��dH�}� t0H�}� tH�E�H�E�� 9�}H�E�H�U�H�H�E�H�@H�E��H�}� tH�E�H�U�H�H�E�H�@H�E�H�E�H� H��H�E�H�E�H�P�H�U�H������u�H�E��H���~�����f.�     f�H�l$�L�d$�H�-  L�%   H�\$�L�l$�L�t$�L�|$�H��8L)�A��I��H��I��1��1���H��t@ L��L��D��A��H��H9�u�H�\$H�l$L�d$L�l$ L�t$(L�|$0H��8��    ��  H��H���                                                   ->  sorting.cpp first != nullptr                              ListNode<T>* list_merge_sort(ListNode<T>*) [with T = int]       ListNode<T>* _list_merge_sort(ListNode<T>*, ListNode<T>*, size_t) [with T = int; size_t = long unsigned int]    ;T   	   �����   ����p   �����   ����X  4���x  I���  ����0  @����  �����             zR x�      H���*                  zR x�  $      8����    FJw� ?;*3$"           zPLR x 	@ �  $   $   ����J  l@ A�CE        �   1���s    A�Cn  $   l   ����x  {@ A�Cs        �   ����=    A�Cx      �   ����    A�CP   $     �����    J��f@����X      <  ���               ��z�� �  ���0� �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
@     �@     �	@                                  L             i             H@            d@            �`                          �`                   ���o    �@            �@            �@     
       �                                            `            8                           @            �@            0       	              ���o    p@     ���o           ���o    L@                                                                                                             `                     �@     �@     �@     �@     �@     �@     �@     �@     	@     	@     &	@     6	@     F	@                     GCC: (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1 GCC: (Ubuntu/Linaro 4.7.3-7ubuntu3) 4.7.3  .symtab .strtab .shstrtab .interp .note.ABI-tag .note.gnu.build-id .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt .init .text .fini .rodata .eh_frame_hdr .eh_frame .gcc_except_table .init_array .fini_array .jcr .dynamic .got .got.plt .data .bss .comment                                                                               8@     8                                    #             T@     T                                     1             t@     t      $                              D   ���o       �@     �      4                             N             �@     �      �                          V             �@     �      �                             ^   ���o       L@     L      $                            k   ���o       p@     p      p                            z             �@     �      0                            �             @           8                          �             H@     H                                    �             p@     p      �                             �             P	@     P	                                   �             d@     d      	                              �             �@     �                                    �             �@     �      T                              �             �@     �      �                             �             l@     l                                    �             �`     �                                    �             �`     �                                    �              `                                          �             `           �                           �             �`     �                                   �               `             �                                         � `     �                                                 � `     �                                          0               �       U                                                   �                                                          �)      �         4                 	                      �1                                                                 8@                   T@                   t@                   �@                   �@                   �@                   L@                   p@                  	 �@                  
 @                   H@                   p@                   P	@                   d@                   �@                   �@                   �@                   l@                   �`                   �`                    `                   `                   �`                     `                   � `                   � `                                       ��                      `                  �	@             .     �	@             A     �	@             W     �!`            f     �`             �     
@             �     �`             �    ��                �     �@            �     �!`            �     �@     :       -     @     m       q    �@     =       �    �@                ��                �    h@             �     `                  ��                �      `             �     �`             �     �`             	    `                  � `                                  4    `@            D    P	@             K                      Z                      n                     �    d@             �                     �                     �                     �     �@                                   .                     s    �@            �                      �    � `             �  "  L@     x      �   � `             �    � `           �   � `                 �@     �           � `             "    �!`             '                     E     	@             �    � `             �      	@             �                     �                     �  "  �@     s           =
@     J          H@              crtstuff.c __JCR_LIST__ deregister_tm_clones register_tm_clones __do_global_dtors_aux completed.6992 __do_global_dtors_aux_fini_array_entry frame_dummy __frame_dummy_init_array_entry sorting.cpp _ZStL19piecewise_construct _ZStL8__ioinit _ZZ15list_merge_sortIiEP8ListNodeIT_ES3_E19__PRETTY_FUNCTION__ _ZZ16_list_merge_sortIiEP8ListNodeIT_ES3_S3_mE19__PRETTY_FUNCTION__ _Z41__static_initialization_and_destruction_0ii _GLOBAL__sub_I_main __FRAME_END__ __JCR_END__ _GLOBAL_OFFSET_TABLE_ __init_array_end __init_array_start _DYNAMIC data_start _ZNSolsEi@@GLIBCXX_3.4 __libc_csu_fini _start __gmon_start__ _Jv_RegisterClasses __assert_fail@@GLIBC_2.2.5 _fini _ZNSt8ios_base4InitC1Ev@@GLIBCXX_3.4 __libc_start_main@@GLIBC_2.2.5 __cxa_atexit@@GLIBC_2.2.5 _ZNSt8ios_base4InitD1Ev@@GLIBCXX_3.4 _ITM_deregisterTMCloneTable _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@@GLIBCXX_3.4 _IO_stdin_used _ITM_registerTMCloneTable __data_start _Z16_list_merge_sortIiEP8ListNodeIT_ES3_S3_m __TMC_END__ _ZSt4cout@@GLIBCXX_3.4 __dso_handle __libc_csu_init __bss_start _end _ZNSolsEPFRSoS_E@@GLIBCXX_3.4 _ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_@@GLIBCXX_3.4 _edata __gxx_personality_v0@@CXXABI_1.3 _Znwm@@GLIBCXX_3.4 _Unwind_Resume@@GCC_3.0 _Z15list_merge_sortIiEP8ListNodeIT_ES3_ main _init                                                                                                                                                                                                                                                                                                                      templates/text/                                                                                     0000755 0001751 0001752 00000000000 12647420007 014140  5                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             templates/text/suffix_tree_static.cpp                                                               0000666 0001751 0001752 00000004353 12607210213 020537  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <cassert>
#include <iostream>
#include <string>
#include <map>

using namespace std;

template<unsigned S>
struct SufixTree {

	struct Node;

	void build(const char *T, unsigned s) {
		assert(s < S);

		size = s;
		for (unsigned i = 0; i < s; ++i)
			text[i] = T[i];
		text[s] = 0;

		nodes[0] = Node();
		next_node = 1;

		for (unsigned i = s; i > 0; --i)
			add_sufix(i-1);
	}

	void add_sufix(unsigned pos) {
		// node 0 is root
		nodes[0].add_sufix(text, pos, pos, nodes[next_node++]);
	}

	void print() {
		in_order_print(nodes[0]);
	}

	unsigned suffix(unsigned i) {
		unsigned res;
		suffix(nodes[0], res, i);
		return res;
	}

	bool suffix(Node &n, unsigned &pos, unsigned &remaining) {
		if (remaining == 0) {
			pos = n.start_idx != S ? n.start_idx : size;
			return false;
		}

		Node *child = n.first_child;
		while (child != 0 && suffix(*child, pos, --remaining))
			child = child->sibling;

		return true;
	}	

	void in_order_print(Node &n) {
		Node *child;
		for (child = n.first_child; child != 0 &&  text[n.start_idx] > text[child->start_idx]; child = child->sibling)
			in_order_print(*child);

		if (n.start_idx != S)
			std::cout << text + n.start_idx << endl;

		for (; child != 0; child = child->sibling)
			in_order_print(*child);

	}

	unsigned size;
	char text[S];
	Node nodes[S];
	unsigned next_node;
};


template<unsigned S>
struct SufixTree<S>::Node {

	Node() : start_idx(S), first_child(0), sibling(0) { }

	Node(unsigned s, Node *sib = 0) : start_idx(s), first_child(0), sibling(sib) { }

	void add_sufix(char *text, unsigned pos, unsigned acc, Node& memory) {
		assert(text);

		if (!text[acc])
			return;

		Node **edge = &first_child;
		while (edge != 0 && *edge != 0 && text[(*edge)->start_idx] <= text[pos])
			edge = &(*edge)->sibling;

		if (*edge && text[(*edge)->start_idx] == text[pos])
			(*edge)->add_sufix(text, pos, acc+1, memory);
		else {
			memory = Node(pos, *edge);
			*edge = &memory;
		}
	}

	unsigned start_idx;
	Node *first_child;
	Node *sibling;
};


int main() {
	static SufixTree<1000> tree;
	string b = "monster";
	// string b = "bananaasdxeaabbcctasd";
	tree.build(b.c_str(), b.length());
	tree.print();

	cout << endl;

	for (unsigned i = 0; i <= b.length(); ++i) 
		cout <<  tree.suffix(i) << endl;

	return 0;
}                                                                                                                                                                                                                                                                                     templates/text/palindrome.cpp                                                                       0000644 0001751 0001752 00000004070 12312320563 016772  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using std::vector;
using std::string;
using std::min;
using std::max;

void palindrome_rads(string input, vector<int>& rads, bool even) {
    rads.clear();
    rads.resize(input.length(), 0);

    int pivot = 1;
    int radius = 0;
    int k;

    while (pivot < input.length() ) {

        while (pivot+radius+even <= input.length() && pivot-radius > 0 && input[pivot-radius-1] == input[pivot+radius+even]) {
            radius++;
        }
        rads[pivot] = radius;
        k = 1;

        while (rads[pivot-radius] != radius-k && k < radius) {
            rads[pivot+k] = max(rads[pivot-k], radius-k++);
        }
        radius = min(0, radius-k);
        pivot += k;
    }
}

void palindrom_odd(string input, vector<int>& p, bool is_even = false) {
    p.clear();
    p.resize(input.size(), 1);
    p[0] = 1;

    int step = 0;

    
    // Start from position 1 and increment the pivot after each itteration
    for (int pivot = 1; pivot < input.size(); ++pivot) {
        // Start with radius 1
        int radius = 1;

        if (step + p[step] > pivot) 
            radius = std::min(p[step+step-pivot], p[step]+step-pivot);

        // Calculate radius for position input[pivot], span to two sides an check if identical
        if (!is_even)
            while (radius <= pivot && pivot+radius < input.size() && input[pivot+radius] == input[pivot-radius])
                ++radius;
        else
            while (radius <= pivot && pivot+radius+1 < input.size() && input[pivot+radius+1] == input[pivot-radius])
                ++radius;

        // Optymized step incrementing
        if (radius + pivot > step + p[step])
            step = pivot;

        p[pivot] = max(radius, p[pivot]);
        if (is_even && pivot+1 < p.size())
            p[pivot+1] = radius;
    }

}


int main() {
    vector<int> rads;
    int r = palindrom_odd("a1dfABBAx", rads, true);
    for (vector<int>::iterator i = rads.begin(); i != rads.end(); ++i)
        std::cout << *i << ' ';
    std::cout << std::endl;
    return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                        templates/text/ukkonen_suffix_tree.cpp                                                              0000666 0001751 0001752 00000011002 12563171325 020722  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <unordered_map>
#include <cassert>
#include <string>

static const unsigned Inf = (unsigned)-1;


struct Node {
	Node(unsigned b, unsigned e, char *t, Node *p) 
		: begin(b), end(e), text(t), parent(p), suffix_link(0) {}

	void add_child(char c, Node *n) {
		assert(children.count(c) == 0);
		children[c] = n;
	}

	bool is_leaf() {
		return !children.size();
	}

	unsigned begin;
	unsigned end;

	char *text;
	Node *parent;
	Node *suffix_link;
	std::unordered_map<char, Node*> children;
};


template<unsigned S>
struct SuffixTree {

	SuffixTree() : root(0), end(0) {}

	Node* build(const char* x, unsigned size) {

		active_length = 0;
		active_begin = 0;
		active_end = 0;
		prev_split_node = 0;
		remainder = 1;

		for (int i = 0; i < size; ++i)
			text[i] = x[i];

		root = new Node(0, 0, text, 0);
		active_dst = new Node(0, 0, text, root);
		root->add_child(text[0], active_dst);
		active_node = root;

		for (int i = 1; i < size; ++i)
			add_prefix(i);

		return 0;
	}

	void add_prefix(unsigned end_idx) {

		std::cout << "add prefix " << text[end_idx] << std::endl;


		Node *dst;
		dst = find_edge(active_node, text[end_idx]);
		// if (active_dst->begin <= active_dst->end) {

		// 	dst = find_edge(active_node, text[active_dst->begin]);
		// 	long long span = (long long)active_dst->end - active_dst->begin;

		// 	if (text[end_idx] != text[dst->begin + span + 1]) {
		// 		std::cout << "???" << std::endl;
		// 	}

		// } else {
		// 	dst = find_edge(active_node, text[end_idx]);
		// }

		if (dst != 0) {
			// dst->end++;

			if (remainder == 1) {
				active_begin = end_idx;
			}
			remainder++;
			// active_node = dst->parent; // ???
			// active_end = end_idx;
			active_length++;
			std::cout << "\nextend r=" << remainder << std::endl;
			std::cout << dst->begin << " " << dst->end << " " << end << std::endl;
			if (active_begin+active_length <= active_node->end) {
				std::cout << "end of edge" << std::endl;
			}
		} else if (!dst && remainder == 1) {
			std::cout << "Add " << text[end_idx] << std::endl;
			dst = new Node(end_idx, Inf, text, active_node);
			root->add_child(text[end_idx], dst);
		} else {

			while (remainder > 0) {

				std::cout << "Split " << std::endl;

				Node *split_node;
				if (active_length > 0) {
					split_node = find_edge(active_node, text[active_begin]);
					unsigned split_point = split_node->begin+active_length;

					std::cout << "Truncate " << text[split_node->begin]  << " to " << split_point-1 << std::endl;
					split_node->end = split_point-1;

					std::cout << "Add " << text[split_point] << " to " << text[split_node->begin] << std::endl;
					split_node->add_child(text[split_point], new Node(split_point, Inf, text, split_node));

					// TODO: add only when possible
					std::cout << "Add " << text[end_idx] << " to " << text[split_node->begin] << std::endl;
					split_node->add_child(text[end_idx], new Node(end_idx, Inf, text, split_node));
				} else {
					std::cout << "Add " << text[end_idx] << " to root" << std::endl;
					root->add_child(text[end_idx], new Node(end_idx, Inf, text, root));
				}

				remainder--;

				if (active_node == root) {
					std::cout << "rule 1" << std::endl;
					active_begin++;
					active_length--;
				}

				if (prev_split_node) {
					std::cout << "rule 2" << std::endl;
					prev_split_node->suffix_link = split_node;
				}
				prev_split_node = split_node;
			}
			prev_split_node = 0;
		}
		// active_dst->end++;
		end++;


		for (int j = 0; j < end_idx; ++j) {


			// // Find the end of the path from the root labelled S[j..i] in the current tree.
			// Node *node = find_node(i, j);
			// int pos -1 //= find_pos(node, )

			// // case 1
			// if (node->is_leaf()) {
			// 	// ++node->len;
			// 	// do nothing
			// }
			// // case 2: add a leaf edge
			// else if (!node->is_leaf()) {
			// 	if (pos == Inf || pos) {
			// 		node->add_child(new Node(i, Inf, text, node));
			// 	} else {
			// 		node->split(pos);
			// 	}
			// }
			// // case 3
			// // else if () {

			// // }

			// // Extend that path by adding character S[i+l] if it is not there already
			// ++end;
		}
	}


	Node* find_edge(Node *node, char c) {
		if (node->children.count(c))
			return node->children[c];
		return 0;
	}


	Node *root;
	Node *active_node;
	Node *active_dst;
	Node *prev_split_node;

	unsigned active_begin;
	unsigned active_end;
	unsigned active_length;
	unsigned remainder;

	unsigned end;
	char text[S];
};


int main() {
	// std::string x = "banana";
	std::string x = "abcabxabcd";
	SuffixTree<1000> tree;
	tree.build(x.c_str(), x.length());
	return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              templates/text/suffix_tree.cpp                                                                      0000644 0001751 0001752 00000005561 12607240166 017200  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <cassert>
#include <iostream>
#include <string>
#include <map>

using namespace std;


int longest = 0;

template<unsigned S>
struct SufixTree {

	struct Node;

	void build(const char *T, unsigned s) {
		assert(s < S);

		size = s;
		for (unsigned i = 0; i < s; ++i)
			text[i] = T[i];
		text[s] = 0;

		for (unsigned i = s; i > 0; --i)
			add_sufix(i-1);
	}

	void add_sufix(unsigned pos) {
		root.add_sufix(text, pos, pos);
	}

	void print() {
		in_order_print(root, 0);
	}

	void in_order_print(Node &n, int depth) {

		// if (n.start_idx != S) 
			// std::cout  << text + (n.start_idx - (depth-1)) << endl;
		// cout  << "At node " << n.start_idx << endl;

		Node *child;
		// cout << "pre\n";
		for (child = n.first_child; child != 0 && text[n.start_idx+1] > text[child->start_idx]; child = child->sibling) {
			// cout << "investigate child " << text[child->start_idx] << endl;
			in_order_print(*child, depth+1);
		}

		// cout << "in\n";
		if (n.start_idx != S) 
			std::cout  << text + (n.start_idx - (depth-1)) << " " << depth << endl;

		// cout << "post\n";
		for (; child != 0; child = child->sibling)
			in_order_print(*child, depth+1);

		if (depth > longest)
			longest = depth;

	}

	unsigned size;
	char text[S];
	Node root;
};

template<unsigned S>
struct SufixTree<S>::Node {

	Node() : start_idx(S), first_child(0), sibling(0) { }

	Node(unsigned s, Node *sib = 0) : start_idx(s), first_child(0), sibling(sib) { }

	void add_sufix(char *text, unsigned pos, unsigned acc) {
		assert(text);

		cout << "adding " << pos << " " << acc << endl;

		if (!text[acc])
			return;

		Node **edge = &first_child;
		for (; edge != 0 && *edge != 0 && text[(*edge)->start_idx] <= text[pos]; edge = &(*edge)->sibling) {
			if (text[(*edge)->start_idx] == text[pos])
				break;
		}
		// if (*prev != 0 && text[(*prev)->start_idx] <= text[pos])
		// 	edge = prev;
		// while (edge != 0 && *edge != 0) {
		// 	Node **next = &(*edge)->sibling;
		// 	cout << text[(*edge)->start_idx]  << ", ";
		// 	if (*next && text[(*next)->start_idx] <= text[acc])
		// 		edge = &(*edge)->sibling;
		// 	else 
		// 		break;
		// }
		// cout << endl;

		// cout << *edge << endl;
		if (*edge) {
			// cout << "start_idx: " << (*edge)->start_idx;
			// cout << " \"" << text[(*edge)->start_idx] << "\"";
			// cout << " pos: " << pos;
			// cout << " \"" << text[pos] << "\"" << endl;
		}
		if (*edge && text[(*edge)->start_idx] == text[acc]) {
			(*edge)->add_sufix(text, pos+1, acc+1);
		}
		else {
			if (*edge)
				cout << "Create before " << (*edge)->start_idx << endl;
			*edge = new Node(acc, *edge);
			// (*edge)->add_sufix(text, pos+1, acc+1);
		}
	}

	unsigned start_idx;
	Node *first_child;
	Node *sibling;
};


int main() {
	static SufixTree<1000> tree;
	// string b = "0111156";
	string b = "1234123";
    //         "0123456789"
	tree.build(b.c_str(), b.length());
	tree.print();

	cout << longest << endl;

	return 0;
}                                                                                                                                               templates/text/kmp.cc                                                                               0000666 0001751 0001752 00000004066 12567044355 015261  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
using namespace std;

template<typename T>
struct Maybe {
    Maybe(bool v, const T& v = T()) : valid(v), value(v)  {}
    union {
        bool something;
        bool success;
        bool valid;
    };
    T value;
};

template <typename T>
Maybe<T> Nothing() {
    return Maybe<T>(false);
}

template <typename T>
Maybe<T> Just(T v) {
    return Maybe<T>(true, v);
}


unsigned strlen(const char *s) {
    char *end = (char *)s;
    while (*end)
        ++end;
    return (end - s);
}

unsigned* create_kmp_tab(const char *str, unsigned size) {
    unsigned *shifts = new unsigned[size];
    shifts[0] = 0;

    unsigned prefix = 0;
    for (int idx = 1; idx < size; ) {
        if (str[idx] == str[prefix]) {
            // substring continues
            shifts[idx++] = ++prefix;
        } else if (prefix > 0) {
            // substring stops, fallback to previous prefix
            prefix = shifts[prefix-1];
        } else {
            // no fallback option
            shifts[idx++] = 0;
        }
    }

    return shifts;
}

Maybe<unsigned> kmp_find(const char *pattern, const char *text) {
    Maybe<unsigned> result = Nothing<unsigned>();

    unsigned pattern_size = strlen(pattern);
    unsigned text_size = strlen(text);
    unsigned *shifts = create_kmp_tab(pattern, pattern_size);

    unsigned remaining = text_size;
    unsigned tidx = 0, pidx = 0;
    while (remaining > 0) {

        if (pattern[pidx] == text[tidx]) {
            ++pidx;
            ++tidx;
            --remaining;
        } else if (pidx == 0) {
            ++tidx;
            --remaining;
        } else {
            pidx = shifts[pidx - 1];
        }

        if (pidx == pattern_size) {
            result = Just<unsigned>(tidx - pidx);
            break;
        }
    }

    delete []shifts;
    return result;
}





int main(int argc, char const *argv[]) {

    const char *S = "ABC ABCDAB ABCDABCDABDX";
    const char *P = "ABCDAB";

    Maybe<unsigned> ret = kmp_find(P, S);
    if (ret.something)
        cout << ret.value << " " << (S + ret.value) << endl;

    return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                          templates/tree/                                                                                     0000755 0001751 0001752 00000000000 12647420021 014107  5                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             templates/tree/cover_tree.cpp                                                                       0000664 0001751 0001752 00000010656 12462707337 016777  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <assert.h>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::sort;
using std::abs;
using std::max;
using std::pair;
using std::reverse;
using std::swap;

/*
 * Interval Power Tree (drzewpo toęgowe)
 */
template<typename ValType>
class IntervalPTree {
public:

	IntervalPTree(size_t size) {
		this->size = size;
		load.resize(size, 0);
	}

	ValType sumFromStart(size_t end) {
		assert(end < size);

		ValType result = ValType();

		while (end > 0) {
			result += load[end];
			end -= ((end ^ (end - 1)) + 1)/2;
		}
		return result;
	}

	ValType query(size_t begin, size_t end) {
		assert(begin <= end);
		assert(end < size);

		if (begin != 0)
			--begin;

		return sumFromStart(end) - sumFromStart(begin);
	}

	void insert(size_t pos, ValType val) {
		assert(pos < size);

		while (pos < size) {
			load[pos] += val;
			pos += ((pos ^ (pos - 1)) + 1) / 2;
		}
	}

private:
	size_t size;
	vector<ValType> load;
};




/*
 * Interval Cover Tree
 */
template<typename ValType>
class IntervalCTree {

private:
	inline size_t leftChild(size_t pos) {
		return 2 * pos;
	}

	inline size_t rightChild(size_t pos) {
		return 2 * pos + 1;
	}

	inline bool isLeftChild(size_t pos) {
		return pos % 2 == 0;
	}

	inline bool isRightChild(size_t pos) {
		return pos % 2 == 1;
	}

	inline size_t rightSibling(size_t pos) {
		return pos+1;
	}

	inline size_t leftSibling(size_t pos) {
		assert(pos > 0);
		return pos-1;
	}

	inline size_t parrent(size_t pos) {
		return pos / 2;
	}

public:

	IntervalCTree(size_t size) {

		// Compute the next power of 2 the evil way
		size--;
		size |= size >> 1;
		size |= size >> 2;
		size |= size >> 4;
		size |= size >> 8;
		size |= size >> 16;
		size++;

		this->size = size;

		load.resize(2 * size, ValType());
		sub.resize(2 * size, ValType());
	}


	ValType query(size_t begin, size_t end) {
		assert(begin <= end);
		assert(end < size);

		ValType result = ValType();

		int length = 1;
		int left_length = 1;
		int right_length = 0;
		if (begin != end)
			right_length = 1;

		begin += size;
		end   += size;


		while (begin >= 1) {

			result += left_length*load[begin] + right_length*load[end];

			if (begin < end - 1) {
				if (isLeftChild(begin)) {
					result += sub[rightSibling(begin)];
					left_length += length;
				}
				if (isRightChild(end)) {
					result += sub[leftSibling(end)];
					right_length += length;
				}
			}

			// Move one level upward, length of interval increses twice
			begin  = parrent(begin);
			end = parrent(end);
			length *= 2;
		}


		return result;
	}

	void insert(size_t begin, size_t end, ValType val) {
		assert(begin <= end);
		assert(end <= size);

		int length = 1;

		begin += size;
		end   += size;
		load[begin] += val;
		sub[begin]  += val;

		if (begin != end) {
			load[begin] += val;
			sub[begin]  += val * length;			
		}

		while (begin >= 1) {

			if (begin < end - 1) {
				if (isLeftChild(begin)) {
					load[rightSibling(begin)] += val;
					sub[rightSibling(begin)]  += val * length;
				}
				if (isRightChild(end)) {
					load[leftSibling(end)] += val;
					sub[leftSibling(end)]  += val * length;
				}
			}

			// Not a a leaf, update sub
			if (end < size) {
				sub[begin] = sub[leftChild(begin)] + sub[rightChild(begin)] + load[begin] * length;
				sub[end] = sub[leftChild(end)] + sub[rightChild(end)] + load[end] * length;
			}

			// Move one level upward, length of interval increses twice
			begin  = parrent(begin);
			end = parrent(end);
			length *= 2;
		}
	}

private:
	size_t size;
	vector<ValType> load;
	vector<ValType> sub;
};


int main(int argc, char* argv[]) {
	std::ios::sync_with_stdio(false);

	// IntervalPTree<double> intervals(10);
	// intervals.insert(1, 1.0);
	// intervals.insert(2, 1.2);
	// intervals.insert(4, 1);

	// cout << intervals.query(0, 0) << endl;
	// cout << intervals.query(1, 1) << endl;
	// cout << intervals.query(2, 2) << endl;
	// cout << intervals.query(3, 3) << endl;


	IntervalCTree<double> intervals2(10);
	intervals2.insert(0, 0, 1);
	intervals2.insert(1, 1, 1.0);
	intervals2.insert(2, 2, 1.2);
	intervals2.insert(2, 4, 1);
	intervals2.insert(10, 10, 1);

	cout << intervals2.query(0, 0) << endl;
	cout << intervals2.query(1, 1) << endl;
	cout << intervals2.query(2, 2) << endl;
	cout << intervals2.query(2, 4) << endl;
	cout << intervals2.query(10, 10) << endl;
	cout << intervals2.query(0, 10) << endl;


	return 0;
}                                                                                  templates/tree/bintree.cpp                                                                          0000666 0001751 0001752 00000012431 12561066241 016255  0                                                                                                    ustar   jmroz                           developers                                                                                                                                                                                                             #include <cstdlib>
#include <time.h>
#include <iostream>
#include <new>
#include <cassert>
#include <vector>
#include <string>

static const unsigned long long RAND_A = 123456789;
static const unsigned long long RAND_B = 12345;
unsigned long long RAND_SEED = 0x123986734;
unsigned long long _rand() {
	RAND_SEED = RAND_A*RAND_SEED + RAND_B;
	return RAND_SEED;
}

template <typename T, unsigned min = 0, unsigned max = 101> 
struct TreapNode {

	TreapNode() : val(), parrent(0), left(0), right(0) {
		randomize();
	}

	TreapNode(T v, TreapNode *p, TreapNode *l, TreapNode *r) 
			: val(v), parrent(p), left(l), right(r) {
		randomize();
	}

	void randomize() {
		priority = _rand() % (max-min) + min;
	}

	T val;
	TreapNode *parrent, *left, *right;
	unsigned int priority;
};

template <typename T>
struct TreapTree {

	typedef TreapNode<T> Node;

	TreapTree() {
		root = 0;
		RAND_SEED = (unsigned long long)&root;
	}

	~TreapTree() {
		clear();
	}

	void clear() {

	}

	Node* insert(T val) {

		// std::cout << "insert: " << val << std::endl;

		Node* node = insert(val, &root, 0);

		while (node != root && node->priority < node->parrent->priority) {
			assert(node->parrent);

			Node *n_parent = node->parrent;
			Node *n_grandpa = node->parrent->parrent;

			Node **edge = 0;
			if (n_parent == root) {
				edge = &root;
			} else if (n_grandpa->left == n_parent) {
				edge = &n_grandpa->left;
			} else {
				assert(n_parent == n_grandpa->right);
				edge = &n_grandpa->right;
			}

			if (node == n_parent->left) {
				rotate_right(edge);
			} else {
				rotate_left(edge);
			}
		}
		assert(node != node->left);
		assert(node != node->right);

		return node;
	}

	Node* insert(T& val, Node **edge, Node *parent) {
		assert(edge);
		Node *node = *edge;

		if (node) {
			if (val < node->val)
				return insert(val, &node->left, node);
			else
				return insert(val, &node->right, node);
		}
		*edge = new (std::nothrow) Node(val, parent, 0, 0);
		return *edge;
	}


	void remove(const T &val) {

	}

	void remove(const T *val) {

	}

	void remove(Node *node) {
		assert(node);
		// std::cout << "remove: " << node->val << std::endl;

		Node **edge = &node->parrent;
		if (node == root) {
			// std::cout << "edge is root" << std::endl;
			edge = &root;
		}

		if (!node->right) {
			// std::cout << "right" << std::endl;
			assert(node != node->left);
			*edge = node->left;
		} else if (!node->left) {
			// std::cout << "left" << std::endl;
			assert(node != node->right);
			*edge = node->right;
		} else {
			// std::cout << "center" << std::endl;
			Node *u = node->right;
			while (u->left)
				u = u->left;
			assert(u != node);

			if (u->parrent != node) {
				u->parrent->left = 0;
				if (u->right) {
					u->parrent->left = u->right;
					u->right->parrent = u->parrent;
				}
			}

			*edge = u;
			u->left = node->left;
			if (node->right != u) 
				u->right = node->right;
			u->parrent = node->parrent;
			assert(u != u->left);
			assert(u != u->right);

			if (node->left && node->left != u)
				node->left->parrent = u;
			if (node->right && node->right != u)
				node->right->parrent = u;
		}

		assert(node != root && "root not change!");


		node->parrent = 0;
		node->left = 0;
		node->right = 0;

		// TODO: rebalance!
		Node *x = *edge;
		if (x) {
			std::cout << "rebalance" << std::endl;
			bool left_unbalanced = x->left && x->left->priority < x->priority;
			bool right_unbalanced = x->right && x->right->priority < x->priority;
			while (left_unbalanced || right_unbalanced) {
				if (left_unbalanced) {
					rotate_right(&x->left);
				} else if (right_unbalanced) {
					rotate_left(&x->right);
				}
			}
		}

	}

	void rotate_left(Node **edge) {
		// std::cout << "rotate_left" << std::endl;
		assert(edge);

		Node *x = *edge;
		assert(x->right);

		Node *y = x->right;

		x->right = y->left;
		if (y->left)
			y->left->parrent = x;
		y->parrent = x->parrent;

		*edge = y;

		y->left = x;
		x->parrent = y;
	}

	void rotate_right(Node **edge) {
		// std::cout << "rotate_right" << std::endl;
		assert(edge);

		Node *x = *edge;
		assert(x->left);

		Node *y = x->left;

		x->left = y->right;
		if (y->right)
			y->right->parrent = x;
		y->parrent = x->parrent;

		*edge = y;

		y->right = x;
		x->parrent = y;
	}

	TreapNode<T> *root;
};


template<typename N>
void print_inorder(N *node) {
	if (!node)
		return;
	print_inorder(node->left);
	// std::cout << "p:" << node->priority << "\tv:" << node->val << std::endl;
	print_inorder(node->right);

}

int tabs = 0;
template<typename N>
void print_tree(N *node) {
	if (!node)
		return;
	// std::cout << "\n" << std::string(2*tabs, ' ') << "p:" << node->priority << "\tv:" << node->val << std::endl;
	
	// std::cout << std::string(2*tabs, ' ')  << "{" ;
	++tabs;
	print_tree(node->left);
	--tabs;
	// std::cout << std::string(2*tabs, ' ')  << "}" << std::endl ;

	// std::cout << std::string(2*tabs, ' ')  << "{";
	++tabs;
	print_tree(node->right);
	--tabs;
	// std::cout << std::string(2*tabs, ' ')  << "}" << std::endl;

}

int main() {

	typedef TreapTree<int> Tree;
	Tree tree;
	
	std::vector<Tree::Node *> v;
	for (int i = 0; i < 10; ++i)
		v.push_back( tree.insert(i) );


	for (int i = 10; i > 0; --i) {	
		if (!tree.root) 
			// std::cout << "tree root is null!!" << std::endl;
		// tree.remove(tree.root);
		tree.remove(v[i]);
		// print_tree(tree.root);
		// std::cout << std::endl;
	}

	return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       