CC=g++

INCDIR=$(PWD)/include
LIBDIR:=$(shell rivet-config --libdir)
PREFIX:=$(shell rivet-config --prefix)
RIVETINCDIR:=$(shell rivet-config --includedir)
LDFLAGS:=$(shell rivet-config --ldflags)
WFLAGS= -Wall -Wextra
CFLAGS=-m64 -pg -I$(INCDIR) -I$(RIVETINCDIR) -O2 $(WFLAGS) -pedantic -ansi
.PHONY: all plots
all: rivet-lib 
rivet-lib: RivetMC_GENSTUDY_CHARMONIUM.so libBOOSTFastJets.so
RivetMC_GENSTUDY_CHARMONIUM.so: libBOOSTFastJets.so MC_GENSTUDY_CHARMONIUM.cc
	$(CC) -shared -fPIC $(CFLAGS) -o "$@" $< -lBOOSTFastJets -L ./ $(LDFLAGS)
libBOOSTFastJets.so: src/BOOSTFastJets.cxx
	$(CC) -shared -fPIC $(CFLAGS) $< -o $@ -lfastjet -lfastjettools $(LDFLAGS)

plots: JetZvsPtProfile.pdf 
%.pdf: %.tex
	pdflatex $<
%.tex: %.gnu
	gnuplot $<
plots/%.pdf: plots/%.dat
	make-plots --pdf $^
plots/%.dat: %.yoda
	rivet-cmphistos $^ -o plots/

%ZvsPtProfile.gnu:
	./makeZPtProfile.py 1S0\(8\).yoda 3PJ\(8\).yoda 3S1\(8\).yoda 3PJ\(1\).yoda 3S1\(1\).yoda
%ZvsPt%Plot.gnu:
	./makeZPtPlot.py 1S0\(8\).yoda 3PJ\(8\).yoda 3S1\(8\).yoda 3PJ\(1\).yoda 3S1\(1\).yoda

install:
	cp libBOOSTFastJets.so $(LIBDIR)

clean:
	rm -f *.o  *.so
