CXXFLAGS=-std=c++11
CFLAGS=-std=c99
CC=gcc

all: standalone wrapper general_standalone general_wrapper

standalone: 
	$(CXX) $(CXXFLAGS) -I../ -c gfs2_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c gfs2_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o gfs2_standalone gfs2_standalone.o gfs2_fuzzer.o

wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c gfs2_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c gfs2_wrapper.cc -g 
	$(CXX) -fpic -shared -o gfs2_wrapper.so gfs2_wrapper.o gfs2_fuzzer.o

general_standalone: 
	$(CXX) $(CXXFLAGS) -I../ -c gfs2_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c gfs2_general_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o gfs2_general_standalone gfs2_general_standalone.o gfs2_fuzzer.o

general_wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c gfs2_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c gfs2_general_wrapper.cc -g 
	$(CXX) -fpic -shared -o gfs2_general_wrapper.so gfs2_general_wrapper.o gfs2_fuzzer.o

clean:
	rm -f *.o *.so *.a
	rm -f *_standalone
	rm -f meta.img out.img log
