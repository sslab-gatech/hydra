E2FSPROGS=e2fsprogs
CXXFLAGS=-std=c++11

all: lib standalone wrapper general_wrapper general_standalone

lib:
	cd $(E2FSPROGS);\
		CFLAGS="-fPIC" CPPFLAGS="-fPIC" LDFLAGS="-lblkid -luuid" ./configure --enable-libblkid --enable-libuuid;\
		make

standalone:
	$(CXX) $(CXXFLAGS) -I$(E2FSPROGS)/lib/ -I../ -c ext4_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c ext4_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o ext4_standalone ext4_standalone.o ext4_fuzzer.o $(E2FSPROGS)/lib/libext2fs.a $(E2FSPROGS)/lib/libcom_err.a  -lpthread  $(E2FSPROGS)/lib/libsupport.a -lblkid  -luuid $(E2FSPROGS)/lib/libext2fs.a

general_standalone: 
	$(CXX) $(CXXFLAGS) -I$(E2FSPROGS)/lib/ -I../ -c ext4_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -I../ -c ext4_general_standalone.cc -g
	$(CXX) $(CXXFLAGS) -o ext4_general_standalone ext4_general_standalone.o ext4_fuzzer.o $(E2FSPROGS)/lib/libext2fs.a $(E2FSPROGS)/lib/libcom_err.a  -lpthread  $(E2FSPROGS)/lib/libsupport.a -lblkid  -luuid $(E2FSPROGS)/lib/libext2fs.a

wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I$(E2FSPROGS)/lib/ -I../ -c ext4_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c ext4_wrapper.cc -g 
	$(CXX) -fPIC -shared -o ext4_wrapper.so ext4_wrapper.o ext4_fuzzer.o $(E2FSPROGS)/lib/libext2fs.a $(E2FSPROGS)/lib/libcom_err.a -lpthread

general_wrapper:
	$(CXX) $(CXXFLAGS) -fPIC -I$(E2FSPROGS)/lib/ -I../ -c ext4_fuzzer.cc -g
	$(CXX) $(CXXFLAGS) -fPIC -I../ -c ext4_general_wrapper.cc -g 
	$(CXX) -fpic -shared -o ext4_general_wrapper.so ext4_general_wrapper.o ext4_fuzzer.o $(E2FSPROGS)/lib/libext2fs.a $(E2FSPROGS)/lib/libcom_err.a -lpthread

clean:
	rm -f *.o *.so *.a
	rm -f ext4_standalone ext4_general_standalone
