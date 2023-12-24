CC = g++
PYFLAGS = -O3 -Wall --shared -std=c++17 -fPIC `python3 -m pybind11 --includes` `python3-config --includes --ldflags`
SRC = myBoolOp.cpp

.PHONY: all test clean
myBoolOp.so: $(SRC)
	$(CC) $(PYFLAGS) $^ -o $@

all: myBoolOp.so

run:
	python myUI.py

test: 
	python myPytest.py

clean:
	rm -rf *.o *.so __pycache__ .pytest_cache/  