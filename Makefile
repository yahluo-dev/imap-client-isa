##
# imapcl
#
# @file
# @version 0.1

CXXFLAGS += -std=c++20
debug_flags=-Wall -Wenum-compare -Wenum-conversion -Wpedantic -ggdb -O0
release_flags=-DNDEBUG
EXE=imapcl
TESTEXE=imapcl-tests
LOGIN=xvasil10

all: debug
zip: $(LOGIN).zip

debug: CXXFLAGS += $(debug_flags)
debug: $(EXE)

test: $(TESTEXE)

$(EXE): main.o command.o response.o
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

command.o: command.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

response.o: response.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(TESTEXE): test/test_main.o test/make_tcp.o command.o
	$(CXX) $(CXXFLAGS) -lgtest $^ -o $@

test/test_main.o: test/test_main.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

test/make_tcp.o: test/make_tcp.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	$(RM) $(EXE) *.o

# end
