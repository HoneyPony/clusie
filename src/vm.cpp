template<class ...Types>
struct types { };

namespace Clusie {

typedef uintptr_t Ptr;

enum Bytecode {
	Terminate,
	StoreU32,
	
	StorePtr,
	
	WriteU32,
	
	UJmpAbs,
	UJmpAbs2,
	UJmpAbs4,
	
	CJmpAbs,
	CJmpAbs2,
	CJmpAbs4,
	
	C2JmpAbs,
	C2JmpAbs2,
	C2JmpAbs4,
	
	C4JmpAbs,
	C4JmpAbs2,
	C4JmpAbs4
};

enum Opcode {
	Constant,
	Load,
	Load2,
	Add,
	Subtract,
	Multiply,
	Divide,
	Not,
	SlotAddress
};

template<class T>
T read_ptr(const uint8_t *ptr) {	
	T result;
	uint8_t *result_ptr = reinterpret_cast<uint8_t*>(&result);
	
	std::memcpy(result_ptr, ptr, sizeof(T));
	
	return result;
}

class VM {
	const uint8_t * code;
	size_t ip = 0;
	
	uint8_t *stack = nullptr;
	size_t stack_size;
	size_t stack_top = 0;
	
public:
	VM(const uint8_t * const code_) : code(code_) {
		stack_size = 256;
		stack = (uint8_t*)malloc(stack_size * sizeof(*stack));
	}
	
	~VM() {
		free(stack);
	}
	
	void reset() {
		ip = 0;
	}
	
	
	template<class A, class ...Args> void output_stack(size_t where, types<A, Args...>) {
		std::cout << read_stack<A>(where) << "\n";
		output_stack<Args...>(where + sizeof(A), types<Args...>{});
	}
	
	template<class A> void output_stack(size_t where, types<A>) {
		std::cout << read_stack<A>(where) << "\n";
	}
	
	uint8_t read_u8() {
		return code[ip++];
	}
	
	template<class T> 
	T read_stack(size_t where) { return read_ptr<T>(stack + stack_top + where); }
	
	template<class T>
	T read_code() {
		T result = read_ptr<T>(code + ip);
		ip += sizeof(T);
		return result;
	}
	
	void resize_stack() {
		size_t new_size = stack_size * 2;
		
		stack = (uint8_t*)realloc(stack, new_size * sizeof(*stack));
		
		stack_size = new_size;
	}
	
	template<class AddrT, class CondT>
	void cjmp_abs() {
		AddrT address = read_code<AddrT>();
		CondT cond = calculate<CondT>();
		
		if(cond) {
			ip = size_t(address);
		}
	}
	
	template<class AddrT>
	void ujmp_abs() {
		AddrT address = read_code<AddrT>();

		ip = size_t(address);
	}
	
	bool step() {
		switch(read_u8()) {
		case Terminate:
			return false;
		case StoreU32:
			store<uint32_t>();
			break;
		case StorePtr:
			store<Ptr>();
			break;
		case WriteU32: {
			auto ptr = calculate<Ptr>();
			auto value = calculate<uint32_t>();
			*reinterpret_cast<uint32_t*>(ptr) = value;
		}
		case UJmpAbs:
			ujmp_abs<uint8_t>();
		case C4JmpAbs:
			cjmp_abs<uint8_t, uint32_t>();
			break;
		}
		return true;
	}
	
	void run() {
		while(step());
	}
	
	template<class T>
	T calculate() {
		switch(read_u8()) {
		case Constant:
			return read_code<T>();
		case Load:
			return read_stack<T>(read_u8());
		case Load2:
			return read_stack<T>(read_code<uint16_t>());
		case Add:
			return calculate<T>() + calculate<T>();
		case Subtract:
			return calculate<T>() - calculate<T>();
		case Multiply:
			return calculate<T>() * calculate<T>();
		case Divide:
			return calculate<T>() / calculate<T>();
		case Not: {
			T val = calculate<T>();
			if(val) return T(0);
			else return T(1);
		}
		case SlotAddress: {
			uint8_t *ptr = stack + stack_top + read_u8();
			uintptr_t uint = reinterpret_cast<uintptr_t>(ptr);
			
			return T(uint);
		}
		}
	}
	
	template<class T>
	void store() {
		size_t address = stack_top + read_u8();
		
		if(address + sizeof(T) > stack_size) {
			resize_stack();
		}
		
		uint8_t *ptr = stack + address;
		T *as_t = reinterpret_cast<T*>(ptr);
		
		*as_t = calculate<T>();
	}
};

}