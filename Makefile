CC = g++
PYFLAGS = -O3 -Wall --shared -std=c++17 -fPIC `python3 -m pybind11 --includes` `python3-config --includes --ldflags`
SRC = boolOp.cpp

.PHONY: all test clean
boolOp.so: $(SRC)
	$(CC) $(PYFLAGS) $^ -o $@

all: boolOp.so

test: 
	python ui.py

clean:
	rm -rf *.o *.so __pycache__ .pytest_cache/  