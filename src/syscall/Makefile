ALL = image_test create_corpus program_show program_test afl # sanity-check 
CXX ?= g++
FUZZ_CXX ?= afl-syscall/afl-g++
CXXFLAGS = -std=c++11 -g
TEMP = temp

all: $(ALL)

image_test: image_test.o Image.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

create_corpus: create_corpus.o Image.o Program.o Utils.o Constants.o SyscallMutator.o
	$(CXX) $(CXXFLAGS) -o $@ $^

program_show: program_show.o Image.o Program.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

program_test: program_test.o Image.o Program.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# sanity-check: sanity-check.cpp Image.cpp Program.cpp Utils.cpp Constants.cpp
#	$(FUZZ_CXX) $(CXXFLAGS) -Wno-unused-result -o $@ $^ 

afl:
	make -C afl-syscall 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c -o $@ $<

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf $(ALL)

clean-temp:
	rm -rf $(TEMP)/corpus/*
	rm -rf $(TEMP)/out/*
