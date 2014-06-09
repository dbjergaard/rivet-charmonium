CC=g++

INCDIR=$(PWD)/include
LIBDIR:=$(shell rivet-config --libdir)
PREFIX:=$(shell rivet-config --prefix)
RIVETINCDIR:=$(shell rivet-config --includedir)
LDFLAGS:=$(shell rivet-config --ldflags)
WFLAGS= -Wall -Wextra
CFLAGS=-m64 -pg -I$(INCDIR) -I$(RIVETINCDIR) -O2 $(WFLAGS) -pedantic -ansi
all: rivet-lib 
rivet-lib: RivetMC_GENSTUDY_CHARMONIUM.so libBOOSTFastJets.so
RivetMC_GENSTUDY_CHARMONIUM.so: libBOOSTFastJets.so
	$(CC) -shared -fPIC $(CFLAGS) -o "RivetMC_GENSTUDY_CHARMONIUM.so" MC_GENSTUDY_CHARMONIUM.cc -lBOOSTFastJets -L ./ $(LDFLAGS)
libBOOSTFastJets.so:
	$(CC) -shared -fPIC $(CFLAGS) src/BOOSTFastJets.cxx -o libBOOSTFastJets.so -lfastjet -lfastjettools $(LDFLAGS)
plots: JetZvsPt.pdf
JetZvsPt.pdf:
	pdflatex JetZvsPt.tex
JetZvsPt.tex:
	gnuplot JetZvsPt.plt
JetZvsPt.plt:
	./makeZPtPlot.py 1S0(8).yoda 3PJ(8).yoda 3S1(8).yoda 3PJ(1).yoda 3S1(1).yoda

install:
	cp libBOOSTFastJets.so $(LIBDIR)
#	cp RivetMC_GENSTUDY_CHARMONIUM.so $(LIBDIR) 
#	cp MC_GENSTUDY_CHARMONIUM.plot $(PREFIX)/share
#	cp MC_GENSTUDY_CHARMONIUM.info $(PREFIX)/share
clean:
	rm -f *.o  *.so
