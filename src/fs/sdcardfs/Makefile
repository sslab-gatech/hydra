LDFLAGS=
CXXFLAGS=-fPIC -std=c++11 -I../

all: standalone wrapper general_standalone general_wrapper

# %.o: %.cc
#	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

standalone: 
	$(CXX) $(CXXFLAGS) -c -o sdcardfs.o sdcardfs.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o sdcardfs_fuzzer.o sdcardfs_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o sdcardfs_standalone.o sdcardfs_standalone.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -o sdcardfs_standalone sdcardfs_standalone.o sdcardfs_fuzzer.o sdcardfs.o

wrapper: 
	$(CXX) $(CXXFLAGS) -c -o sdcardfs.o sdcardfs.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o sdcardfs_fuzzer.o sdcardfs_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o sdcardfs_wrapper.o sdcardfs_wrapper.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -fpic -shared -o sdcardfs_wrapper.so sdcardfs_wrapper.o sdcardfs_fuzzer.o sdcardfs.o

general_standalone: 
	$(CXX) $(CXXFLAGS) -c -o sdcardfs.o sdcardfs.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o sdcardfs_fuzzer.o sdcardfs_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o sdcardfs_general_standalone.o sdcardfs_general_standalone.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -o sdcardfs_general_standalone sdcardfs_general_standalone.o sdcardfs_fuzzer.o sdcardfs.o

general_wrapper: 
	$(CXX) $(CXXFLAGS) -c -o sdcardfs.o sdcardfs.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o sdcardfs_fuzzer.o sdcardfs_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o sdcardfs_general_wrapper.o sdcardfs_general_wrapper.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -fpic -shared -o sdcardfs_general_wrapper.so sdcardfs_general_wrapper.o sdcardfs_fuzzer.o sdcardfs.o

clean:
	rm -f *.o *.so *.a
	rm -f sdcardfs_standalone sdcardfs_general_standalone
	rm -f meta.img out.img log

