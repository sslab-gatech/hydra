LDFLAGS=
CXXFLAGS=-fPIC -std=c++11 -I../

all: standalone wrapper general_standalone general_wrapper

# %.o: %.cc
#	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

standalone: 
	$(CXX) $(CXXFLAGS) -c -o meta.o meta.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o hfsplus_fuzzer.o hfsplus_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o hfsplus_standalone.o hfsplus_standalone.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -o hfsplus_standalone hfsplus_standalone.o hfsplus_fuzzer.o meta.o

wrapper: 
	$(CXX) $(CXXFLAGS) -c -o meta.o meta.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o hfsplus_fuzzer.o hfsplus_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o hfsplus_wrapper.o hfsplus_wrapper.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -fpic -shared -o hfsplus_wrapper.so hfsplus_wrapper.o hfsplus_fuzzer.o meta.o

general_standalone: 
	$(CXX) $(CXXFLAGS) -c -o meta.o meta.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o hfsplus_fuzzer.o hfsplus_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o hfsplus_general_standalone.o hfsplus_general_standalone.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -o hfsplus_general_standalone hfsplus_general_standalone.o hfsplus_fuzzer.o meta.o

general_wrapper: 
	$(CXX) $(CXXFLAGS) -c -o meta.o meta.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o hfsplus_fuzzer.o hfsplus_fuzzer.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c -o hfsplus_general_wrapper.o hfsplus_general_wrapper.cc $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -fpic -shared -o hfsplus_general_wrapper.so hfsplus_general_wrapper.o hfsplus_fuzzer.o meta.o

clean:
	rm -f *.o *.so *.a
	rm -f *_standalone
	rm -f meta.img out.img log

