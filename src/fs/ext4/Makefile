CXXFLAGS=-std=c++11

all: standalone wrapper general_wrapper general_standalone

standalone:
	$(CXX) $(CXXFLAGS) -I$(E2FSPROGS)/lib/ -I../ -c ext4_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c ext4_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o ext4_standalone ext4_standalone.o ext4_fuzzer.o -lext2fs -lcom_err -lpthread

general_standalone: 
	$(CXX) $(CXXFLAGS) -I$(E2FSPROGS)/lib/ -I../ -c ext4_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c ext4_general_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o ext4_general_standalone ext4_general_standalone.o ext4_fuzzer.o -lext2fs -lcom_err -lpthread

wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I$(E2FSPROGS)/lib/ -I../ -c ext4_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c ext4_wrapper.cc -g 
	$(CXX) -fPIC -shared -o ext4_wrapper.so ext4_wrapper.o ext4_fuzzer.o -lext2fs -lcom_err -lpthread

general_wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I$(E2FSPROGS)/lib/ -I../ -c ext4_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c ext4_general_wrapper.cc -g 
	$(CXX) -fpic -shared -o ext4_general_wrapper.so ext4_general_wrapper.o ext4_fuzzer.o -lext2fs -lcom_err -lpthread

clean:
	rm -f *.o *.so *.a
	rm -f ext4_standalone ext4_general_standalone
