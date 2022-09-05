CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
USERID=cesarmarcondes
#CLASSES=SUA_LIB_COMUM
all: echo-server echo-client multi-thread showip
echo-server: 
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp
echo-client: 
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp
multi-thread:
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp
showip:
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp
clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM multi-thread showip echo-server echo-client *.tar.gz
tarball: clean
	tar -cvf lab02-$(USERID).tar.gz *
