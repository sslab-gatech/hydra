CXXFLAGS=-std=c++11
CFLAGS=-std=c99
CC=gcc

all: standalone wrapper general_standalone general_wrapper

standalone: 
	$(CXX) $(CXXFLAGS) -I../ -c reiserfs_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c reiserfs_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o reiserfs_standalone reiserfs_standalone.o reiserfs_fuzzer.o 

wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c reiserfs_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c reiserfs_wrapper.cc -g 
	$(CXX) -fpic -shared -o reiserfs_wrapper.so reiserfs_wrapper.o reiserfs_fuzzer.o

general_standalone: 
	$(CXX) $(CXXFLAGS) -I../ -c reiserfs_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c reiserfs_general_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o reiserfs_general_standalone reiserfs_general_standalone.o reiserfs_fuzzer.o 

general_wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c reiserfs_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c reiserfs_general_wrapper.cc -g 
	$(CXX) -fpic -shared -o reiserfs_general_wrapper.so reiserfs_general_wrapper.o reiserfs_fuzzer.o

clean:
	rm -f *.o *.so *.a
	rm -f reiserfs_standalone reiserfs_general_standalone
	rm -f meta.img out.img log
