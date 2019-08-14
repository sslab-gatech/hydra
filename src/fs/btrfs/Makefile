CXXFLAGS=-std=c++11 -I../
CFLAGS=-std=c99 -I../
CC=gcc

all: standalone wrapper general_standalone general_wrapper

standalone:
	$(CC) $(CFLAGS) -c crc32c.c
	$(CXX) $(CXXFLAGS) -c btrfs_fuzzer.cc
	$(CXX) $(CXXFLAGS) -c btrfs_standalone.cc
	$(CXX) $(CXXFLAGS) -o btrfs_standalone btrfs_standalone.o btrfs_fuzzer.o crc32c.o

wrapper:
	$(CC) -fPIC $(CFLAGS) -c crc32c.c
	$(CXX) -fPIC $(CXXFLAGS) -c btrfs_fuzzer.cc
	$(CXX) -fPIC $(CXXFLAGS) -c btrfs_wrapper.cc
	$(CXX) -fPIC -shared $(CXXFLAGS) -o btrfs_wrapper.so btrfs_wrapper.o btrfs_fuzzer.o crc32c.o

general_standalone:
	$(CC) $(CFLAGS) -c crc32c.c
	$(CXX) $(CXXFLAGS) -c btrfs_fuzzer.cc
	$(CXX) $(CXXFLAGS) -c btrfs_general_standalone.cc
	$(CXX) $(CXXFLAGS) -o btrfs_general_standalone btrfs_general_standalone.o btrfs_fuzzer.o crc32c.o

general_wrapper:
	$(CC) -fPIC $(CFLAGS) -c crc32c.c
	$(CXX) -fPIC $(CXXFLAGS) -c btrfs_fuzzer.cc
	$(CXX) -fPIC $(CXXFLAGS) -c btrfs_general_wrapper.cc
	$(CXX) -fPIC -shared $(CXXFLAGS) -o btrfs_general_wrapper.so btrfs_general_wrapper.o btrfs_fuzzer.o crc32c.o

clean:
	rm -f *.o
	rm -f *_standalone
	rm -f btrfs_wrapper.so
