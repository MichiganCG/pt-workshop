OUT     := pt
CXX     := g++
FLAGS   := -std=c++20 -Wall

FLAGS_RELEASE := -O3 -DNDEBUG
COMMAND        = $(CXX) $(FLAGS) $^ -o

debug: FLAGS += -g3 -DDEBUG
debug: main.o library.o
	$(COMMAND) $(OUT)

release: FLAGS += -O3 -DNDEBUG
release: main.o library.o
	$(COMMAND) $(OUT)_release

reference: FLAGS += -O3 -DNDEBUG
reference: reference.o library.o
	$(COMMAND) $(OUT)_reference

%.o: %.cpp
	$(CXX) $(FLAGS) -c $<

clean:
	rm -f *.o $(OUT)*

.PHONY: clean

