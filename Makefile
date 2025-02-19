OUT    := pt
CXX    := g++
FLAGS  := -std=c++20 -Wall
COMMAND = $(CXX) $(FLAGS) $^ -o

$(OUT): FLAGS += -g3 -DDEBUG
$(OUT): main.cpp library.o
	$(COMMAND) $(OUT)

$(OUT)_release: FLAGS += -O3 -DNDEBUG
$(OUT)_release: main.cpp library.o
	$(COMMAND) $(OUT)_release

library.o: library/library.cpp
	$(CXX) $(FLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(OUT)*

.PHONY: clean

