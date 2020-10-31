#include <skiplist.h>

#include <vector>
#include <list>
#include <iostream>
#include <chrono>
#include <iostream>
#include <set>
#include <random>
#include <stdlib.h>

#ifdef _DEBUG
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif

using namespace std;
using namespace std::chrono;

class LogDuration {
public:
	explicit LogDuration(const string& msg = "")
		: message(msg + ": ")
	{
		//_sleep(250);
		start = steady_clock::now();
	}

	~LogDuration() {
		auto finish = steady_clock::now();
		auto dur = finish - start;
		cerr << message
			<< duration_cast<milliseconds>(dur).count()
			<< " ms" << endl;
		//_sleep(250);
	}
private:
	string message;
	steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};

void fill_vec(vector<int>& v, int count) {
	random_device rd;
	v.resize(count);
	for (int& x : v) {
		x = rd();
	}
}
void print(int n) {
	cout << "\n" <<  "count elements = " << n << "\n";
}

int main() {
	int n = 1e5;
	cout << "START!" << "\n\n";
	{
		cout << "INSERT" << "\n";
		cout << "========================\n";
		
		vector<int> v(n);
		for (int i = 0; i < 3; ++i) {
			fill_vec(v, n);
			print(n);
			{
				set<int> s;
				LOG_DURATION("inserting in set");
				for (auto x : v) {
					s.insert(x);
				}
			}
			{
				skiplist<int> sk;
				LOG_DURATION("inserting in skiplist");
				for (auto x : v) {
					sk.insert(x);
				}
			}
			n *= 10;
		}
		
	}
	{
		cout << "\n" << "ERASE" << "\n";
		cout << "========================\n";
		
		n /= 1000;
		vector<int> v(n), erasing(n);

		for (int i = 0; i < 3; ++i) {
			fill_vec(v, n);
			fill_vec(erasing, n);
			set<int> s;
			skiplist<int> sk;
			for (int x : v) {
				s.insert(x);
				sk.insert(x);
			}
			print(n);
			{
				LOG_DURATION("erasing from set");
				for (auto x : erasing) {
					s.erase(x);
				}
			}
			{
				LOG_DURATION("erasing from skiplist");
				for (auto x : erasing) {
					sk.erase(x);
				}
			}
			n *= 10;
		}
	}
	{
		cout << "\n" << "FIND" << "\n";
		cout << "========================\n";

		n /= 1000;
		vector<int> v(n), finding(n);

		for (int i = 0; i < 3; ++i) {
			fill_vec(v, n);
			fill_vec(finding, n);
			set<int> s;
			skiplist<int> sk;
			for (int x : v) {
				s.insert(x);
				sk.insert(x);
			}
			print(n);
			{
				LOG_DURATION("erasing from set");
				for (auto x : finding) {
					s.find(x);
				}
			}
			{
				LOG_DURATION("erasing from skiplist");
				for (auto x : finding) {
					sk.find(x);
				}
			}
			n *= 10;
		}
	}
	{
		cout << "\n" << "ITERATIONS" << "\n";
		cout << "========================\n";

		n = 1e6;
		vector<int> v(n);
		for (int i = 0; i < n; ++i) {
			v[i] = i;
		}	
		set<int> s(v.begin(), v.end());
		skiplist<int> sk(v.begin(), v.end());
		print(n);
		{
			int k = 0;
			LOG_DURATION("iterating over skiplist");
			for (int x : sk) {
				++k;
			}
			cout << k << " operations is done\n";
		}
		{
			int k = 0;
			LOG_DURATION("iterating over set");
			for (int x : s) {
				++k;
			}
			cout << k << " operations is done\n";
		}

	}
	_CrtDumpMemoryLeaks();
}