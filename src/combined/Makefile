ALL = image_test create_corpus create_corpus_consistency program_show program_test program_reserialize fscq-cc yxv6-cc
CXX ?= g++
CXXFLAGS = -std=c++11 -g
TEMP = temp

all: $(ALL)

image_test: image_test.o Image.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

create_corpus: create_corpus.o Image.o Program.o Utils.o Constants.o SyscallMutator.o
	$(CXX) $(CXXFLAGS) -o $@ $^

create_corpus_consistency: create_corpus_consistency.o Image.o Program.o Utils.o Constants.o SyscallMutator.o
	$(CXX) $(CXXFLAGS) -o $@ $^

program_show: program_show.o Image.o Program.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

program_reserialize: program_reserialize.o Image.o Program.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

program_test: program_test.o Image.o Program.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

fscq-cc: FSCQ-consistency-exec.o Image.o Program.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

yxv6-cc: yxv6-consistency-exec.o Image.o Program.o Utils.o Constants.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c -o $@ $<

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf $(ALL)

clean-temp:
	rm -rf $(TEMP)/corpus/*
	rm -rf $(TEMP)/out/*
