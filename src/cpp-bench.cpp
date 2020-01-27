#include <chrono>
#include <iostream>

void func() {
	int x = 6;
	int y = 1;
	while(x) y *= x--;
}

#define BENCH 2000000

int main() {
	using namespace std::chrono;
	auto start = high_resolution_clock::now();
	
	for(int i = 0; i < BENCH; ++i) {
		func();
	}
	
	auto end = high_resolution_clock::now();
	
	auto duration = duration_cast<microseconds>(end - start);
	std::cout << "benchmark took " << duration.count() << "us (avg " << (duration.count() / double(BENCH)) << ")\n";
}