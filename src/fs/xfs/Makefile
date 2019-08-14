CXXFLAGS=-std=c++11
CFLAGS=-std=c99
CC=gcc
XFSPROGS=xfsprogs-dev

all: lib standalone wrapper general_standalone general_wrapper

lib:
	cd $(XFSPROGS);\
		make

standalone: 
	$(CXX) $(CXXFLAGS) -I../ -c xfs_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c xfs_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o xfs_standalone xfs_standalone.o xfs_fuzzer.o xfsprogs-dev/libxfs/crc32.o

general_standalone: 
	$(CXX) $(CXXFLAGS) -I../ -c xfs_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c xfs_general_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o xfs_general_standalone xfs_general_standalone.o xfs_fuzzer.o xfsprogs-dev/libxfs/crc32.o

wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c xfs_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c xfs_wrapper.cc -g 
	$(CXX) -fpic -shared -o xfs_wrapper.so xfs_wrapper.o xfs_fuzzer.o xfsprogs-dev/libxfs/crc32.o

general_wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c xfs_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c xfs_general_wrapper.cc -g 
	$(CXX) -fpic -shared -o xfs_general_wrapper.so xfs_general_wrapper.o xfs_fuzzer.o xfsprogs-dev/libxfs/crc32.o

clean:
	rm -f *.o *.so *.a
	rm -f xfs_standalone
	rm -f xfs_wrapper.so
	rm -f meta.img out.img log
	rm -f xfs_general_standalone
