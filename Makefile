##
# imapcl
#
# @file
# @version 0.1

CXXFLAGS += -std=c++20
debug_flags=-Wall -Wenum-compare -Wenum-conversion -Wpedantic -ggdb -O0 -g
release_flags=-DNDEBUG
test_flags=-g
EXE=imapcl
TESTEXE=imapcl-tests
LOGIN=xvasil10

all: debug
tar: $(LOGIN).tar

debug: CXXFLAGS += $(debug_flags)
debug: $(EXE)

test: CXXFLAGS += $(test_flags)
test: $(TESTEXE)
test: ./$(TESTEXE)

$(EXE): main.o command.o response.o response_factory.o session.o server.o response_parser.o
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

command.o: command.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

response.o: response.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

session.o: session.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

server.o: server.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

response_factory.o: response_factory.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

response_parser.o: response_parser.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(TESTEXE): test/test_main.o test/make_tcp.o command.o test/parse_response.o response_parser.o response.o
	$(CXX) $(CXXFLAGS) -lgtest $^ -o $@

test/test_main.o: test/test_main.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

test/make_tcp.o: test/make_tcp.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

test/parse_response.o: test/parse_response.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(LOGIN).tar:
	tar -cvf $@ *.cpp *.hpp README.md


clean:
	$(RM) $(EXE) *.o

.PHONY: clean $(LOGIN).tar

# end
