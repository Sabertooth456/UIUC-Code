
INCLUDES = -I includes/
CXXFLAGS = -std=c++20 -g -fstandalone-debug -O0 -Wall -Wextra -Werror -pedantic $(INCLUDES)
CXX = clang++ 

clean:
	rm -f bin/*

exec: src/driver.cc src/solution.cc
	$(CXX) $(CXXFLAGS) $^ -o bin/exec
	
tests: tests/tests.cc src/solution.cc
	$(CXX) $(CXXFLAGS) $^ -o bin/tests

.DEFAULT_GOAL := exec
.PHONY: exec tests clean