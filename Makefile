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
test: CXXFLAGS += $(debug_flags)
test: $(TESTEXE)
test: ./$(TESTEXE)

$(EXE): main.o command.o response.o session.o server.o response_parser.o tls_server.o fnv.o logger.o parser_logger.o receiver.o tls_receiver.o credential.o
	$(CXX) $(CXXFLAGS) -lssl -lcrypto $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(TESTEXE): test/test_main.o test/make_tcp.o command.o test/parse_response.o response_parser.o response.o logger.o parser_logger.o
	$(CXX) $(CXXFLAGS) -lgtest $^ -o $@

test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(LOGIN).tar:
	tar -cvf $@ *.cpp *.hpp README.md

clean:
	$(RM) $(EXE) *.o test/*.o

.PHONY: clean $(LOGIN).tar

# end
