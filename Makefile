CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++11
AR       = ar

ifeq ($(OS),Windows_NT)
    EXE = .exe
else
    EXE =
endif

TARGET = calc$(EXE)
LIB    = libcalculator.a

all: $(TARGET)

$(TARGET): main.o $(LIB)
	$(CXX) $(CXXFLAGS) main.o -L. -lcalculator -o $@

$(LIB): calculator.o
	$(AR) -rcs $@ $^

main.o: main.cpp calculator.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

calculator.o: calculator.cpp calculator.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(LIB) $(TARGET)

.PHONY: all clean
