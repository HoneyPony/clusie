SRCS=\
src/main.cpp\
src/vm.cpp

clusie: $(SRCS)
	g++ src/main.cpp -o clusie -O2