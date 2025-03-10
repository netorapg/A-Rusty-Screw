HEADERS = $(wildcard src/*.h) $(wildcard src/*.h)
CPPFLAGS = -I./include -Wall -ggdb -std=c++23
CPP = g++

all: pool timer trigger any bit math
	@echo "My-lib compiled! yes!"

pool: $(HEADERS) src/memory-pool.cpp tests/test-memory-pool.cpp
	$(CPP) -O3 src/memory-pool.cpp tests/test-memory-pool.cpp -o test-memory-pool $(CPPFLAGS)

timer: $(HEADERS) tests/test-timer.cpp
	$(CPP) tests/test-timer.cpp -o test-timer $(CPPFLAGS)

event: $(HEADERS) tests/test-event.cpp
	$(CPP) tests/test-event.cpp -o test-event $(CPPFLAGS)

any: $(HEADERS) tests/test-any.cpp
	$(CPP) tests/test-any.cpp -o test-any $(CPPFLAGS)

bit: $(HEADERS) tests/test-bit.cpp
	$(CPP) tests/test-bit.cpp -o test-bit $(CPPFLAGS)

math: $(HEADERS) tests/test-math.cpp
	$(CPP) tests/test-math.cpp -o test-math $(CPPFLAGS)

matrix: $(HEADERS) tests/test-matrix.cpp
	$(CPP) tests/test-matrix.cpp -o test-matrix $(CPPFLAGS)

interpolation: $(HEADERS) tests/test-interpolation.cpp
	$(CPP) tests/test-interpolation.cpp -o test-interpolation $(CPPFLAGS)

clean:
	- rm -rf test-pool-alloc test-stl-alloc test-timer