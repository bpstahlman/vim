#include <iostream>
#include <string>
#include <random>
#include <functional>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

// Precondition: xs[] not empty
// Note: This is an slightly modified modification of the unified binary search documented on Wikipedia.
short * binsearch(short x, short xs[], int n)
{
	int delta = (n + 1) >> 1;
	// Note: The following initialization is equivalent to...
	//     xs + delta - 1
	// ...with delta calculated as below, with exp==0
	short *px = xs + ((n + 1) >> 1) - 1;
	int exp = 1;
	for (;;) {
		if (x == *px) {
			return px;
		} else {
			//cout << "*px=" << *px << endl;
			delta = n + (1 << exp);
			delta >>= ++exp;
			//cout << "delta=" << delta << endl;
			if (!delta)
				return NULL;
			else if (x < *px)
				px -= delta;
			else
				px += delta;
		}
	}
}

short * linsearch(short x, short xs[], int n)
{
	short *px = xs;
	for (short *px = xs; n--; px++)
		if (*px == x)
			return px;
	return NULL;
}

using SFunc = short *(*)(short x, short xs[], int n);
struct SearchInfo {
	const char *name;
	SFunc fn;
	bool sorted;
};
struct SearchInfo search_info[] = {
	{"linear", linsearch, false},
	{"binary", binsearch, true}
};

vector<short> create_randoms(int n, int max_d)
{
	auto gen = bind(uniform_int_distribution<short>(0, max_d), default_random_engine {});
	vector<short> vec;
	short i = 0;
	generate_n(back_inserter(vec), n,
			[&i,&gen] { return i += gen(); });
	return vec;

}

vector<short> shuffle_vector(const vector<short> &vec)
{
	vector<short> ret(vec);
	shuffle(ret.begin(), ret.end(), default_random_engine {});
	return ret;
}

// Do a single run of all search types, returning durations in vector.
vector<duration<long, nano> > do_all_searches(int len, int iters)
{
	vector<duration<long, nano> > durs;
	using DType = decltype(high_resolution_clock::now() - high_resolution_clock::now());
	for (auto &si : search_info) {

		DType dur {}; // accumulator
		for (int i = 0; i < iters; i++) {
			auto vec = create_randoms(len, 50);
			short key = vec[rand() % vec.size()];
			if (!si.sorted)
				vec = shuffle_vector(vec);
			auto t0 = high_resolution_clock::now();
			short *px = si.fn(key, vec.data(), vec.size());
			dur += high_resolution_clock::now() - t0;
		}
		durs.push_back(dur);
	}
	return durs;
}

int main(int argc, const char *argv[])
{
	// Do this once.
	srand(time(NULL));

	int len = 4, iters = 100;
	auto durs = do_all_searches(len, iters);
	for (auto d : durs) {
		cout << "Duration cum=" << duration_cast<microseconds>(d).count()
			<< " avg=" << duration_cast<microseconds>(d).count() / iters
			<< endl;
	}

	return 0;
}

/* vim:ts=4:sw=4
 */
