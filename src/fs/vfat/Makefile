LDFLAGS=
CXXFLAGS=-fPIC -std=c++11 -I../

all: standalone wrapper general_standalone general_wrapper

# %.o: %.cc
#	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

standalone: 
	$(CXX) $(CXXFLAGS) -c -o vfat.o vfat.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o vfat_fuzzer.o vfat_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o vfat_standalone.o vfat_standalone.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -o vfat_standalone vfat_standalone.o vfat_fuzzer.o vfat.o

wrapper: 
	$(CXX) $(CXXFLAGS) -c -o vfat.o vfat.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o vfat_fuzzer.o vfat_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o vfat_wrapper.o vfat_wrapper.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -fpic -shared -o vfat_wrapper.so vfat_wrapper.o vfat_fuzzer.o vfat.o

general_standalone: 
	$(CXX) $(CXXFLAGS) -c -o vfat.o vfat.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o vfat_fuzzer.o vfat_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o vfat_general_standalone.o vfat_general_standalone.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -o vfat_general_standalone vfat_general_standalone.o vfat_fuzzer.o vfat.o

general_wrapper: 
	$(CXX) $(CXXFLAGS) -c -o vfat.o vfat.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o vfat_fuzzer.o vfat_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o vfat_general_wrapper.o vfat_general_wrapper.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -fpic -shared -o vfat_general_wrapper.so vfat_general_wrapper.o vfat_fuzzer.o vfat.o

clean:
	rm -f *.o *.so *.a
	rm -f vfat_standalone vfat_general_standalone
	rm -f meta.img out.img log

