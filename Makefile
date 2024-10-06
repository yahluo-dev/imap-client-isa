##
# imapcl
#
# @file
# @version 0.1

CXXFLAGS += -std=c++20
debug_flags=-Wall -Wenum-compare -Wenum-conversion -Wpedantic -ggdb -O0
release_flags=-DNDEBUG
EXE=imapcl
LOGIN=xvasil10

all: debug
zip: $(LOGIN).zip

debug: CXXFLAGS += $(debug_flags)
debug: $(EXE)

$(EXE): main.o command.o command_factory.o
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

command.o: command.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

command_factory.o: command_factory.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	$(RM) $(EXE) *.o

# end
