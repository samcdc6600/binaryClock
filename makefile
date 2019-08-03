CXX   = clang++
BIN_NAME = binclk
CXXFLAGS = -std=c++1z -Wall -Wpedantic -Wextra -O3
LIBS  = -lX11 -L/usr/local/lib/ -I/usr/local/include/
SRC   = ./src
IDIR  = $(SRC)/include
ODIR  = ./obj
_DEPS = util.hpp
DEPS  = $(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ  = main.o
OBJ   = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRC)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BIN_NAME): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm $(BIN_NAME) $(ODIR)/*.o *.core ../*~ *~ ../\#*\# \#*\#
