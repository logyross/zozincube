SRC    := zozincube.cpp
BIN    := zozincube

release: CXXFLAGS = -O2
release: $(BIN)

debug: CXXFLAGS = -fsanitize=address -Wall -Wextra -Wpedantic -Weffc++ -pedantic-errors -Werror -O0 -fwrapv -Wfloat-equal -Wconversion -Wsign-conversion -std=c++17
debug: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

.PHONY: release debug clean
clean:
	rm -f $(BIN)