HEADERS = $(wildcard src/*.h) $(wildcard src/*.h)
CPPFLAGS = -I./include -Wall -ggdb -std=c++23

all: pool timer trigger any bit math
	@echo "My-lib compiled! yes!"

pool: $(HEADERS) src/memory-pool.cpp tests/test-memory-pool.cpp
	g++ -O3 src/memory-pool.cpp tests/test-memory-pool.cpp -o test-memory-pool $(CPPFLAGS)

timer: $(HEADERS) tests/test-timer.cpp
	g++ tests/test-timer.cpp -o test-timer $(CPPFLAGS)

trigger: $(HEADERS) tests/test-trigger.cpp
	g++ tests/test-trigger.cpp -o test-trigger $(CPPFLAGS)

any: $(HEADERS) tests/test-any.cpp
	g++ tests/test-any.cpp -o test-any $(CPPFLAGS)

bit: $(HEADERS) tests/test-bit.cpp
	g++ tests/test-bit.cpp -o test-bit $(CPPFLAGS)

math: $(HEADERS) tests/test-math.cpp
	g++ tests/test-math.cpp -o test-math $(CPPFLAGS)

clean:
	- rm -rf test-pool-alloc test-stl-alloc test-timer