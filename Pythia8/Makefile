CC=g++

WFLAGS=-Wall -Wextra
CFLAGS=-pg -O3 $(WFLAGS)  $(shell pythia8-config --cxxflags)
LDFLAGS=-pg $(shell pythia8-config --libs) \
-L$(shell pythia8-config --libdir)/archive -lpythia8tohepmc -lHepMC

all: runPythia

runPythia: runPythia.o 
	$(CC) runPythia.o -o runPythia $(LDFLAGS) 

runPythia.o: ./runPythia.C 
	$(CC) $(CFLAGS) -c ./runPythia.C 
clean:
	rm -rf *o  runPythia
