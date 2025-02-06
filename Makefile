OUT    := pt
CXX    := g++
FLAGS  := -std=c++20 -Wall
COMMAND = $(CXX) $(FLAGS) $^ -o

debug: FLAGS += -g3 -DDEBUG
debug: main.cpp library.o
	$(COMMAND) $(OUT)

release: FLAGS += -O3 -DNDEBUG
release: main.cpp library.o
	$(COMMAND) $(OUT)_release

library.o: library/library.cpp
	$(CXX) $(FLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(OUT)*

.PHONY: clean

