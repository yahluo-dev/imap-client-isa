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
test_objects=test/unit/test_main.o test/unit/make_tcp.o test/unit/parse_response.o test/unit/imf_message.cpp

all: release
tar: $(LOGIN).tar

debug: CXXFLAGS += $(debug_flags)
debug: $(EXE)

release: CXXFLAGS += $(release_flags)
release: $(EXE)

test-all: test system-test

test: CXXFLAGS += $(test_flags)
test: CXXFLAGS += $(debug_flags)
test: $(TESTEXE)
	./$(TESTEXE)

system-test: release
	echo "Running system tests..."
	./test/running/test_main.sh

$(EXE): main.o command.o response.o session.o server.o response_parser.o tls_server.o fnv.o logger.o parser_logger.o receiver.o tls_receiver.o credential.o client.o imf_message.o directory_writer.o
	$(CXX) $(CXXFLAGS) -lssl -lcrypto $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(TESTEXE): $(test_objects) command.o response_parser.o response.o logger.o parser_logger.o imf_message.o directory_writer.o fnv.o
	$(CXX) $(CXXFLAGS) -lgtest $^ -o $@

test/unit/%.o: test/unit/%.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(LOGIN).tar:
	cp doc/main.pdf ./manual.pdf
	tar -cvf $@ *.cpp *.hpp README.md test/*.cpp manual.pdf Makefile

clean:
	$(RM) $(EXE) *.o test/unit/*.o

.PHONY: clean $(LOGIN).tar

# end
