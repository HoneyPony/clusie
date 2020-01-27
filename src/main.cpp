#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <iostream>

#include <chrono>

#include "vm.cpp"

int main() {
	using namespace Clusie;
	
	uint8_t code[] {
		StorePtr, 4, SlotAddress, 0,
		WriteU32, Load, 4, Constant, 15, 0, 0, 0,
		Terminate
	};
	
	VM vm(code);
	vm.run();
	
	vm.output_stack(0, types<uint32_t, uintptr_t> {});
}