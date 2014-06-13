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
SAMPLES=1S0_8 3PJ_8 3S1_8 3PJ_1 3S1_1
YODAFILES:=$(addsuffix .yoda,$(SAMPLES))
PLOTFILES:=$(addsuffix Plot.pdf,$(addprefix JetZvsPt,$(SAMPLES)))
plots: JetZvsPtProfile.pdf #$(PLOTFILES)

%.pdf: %.tex
	pdflatex $<
%.tex: %.gnu
	gnuplot $<
plots/%.pdf: plots/%.dat
	make-plots --pdf $^
plots/%.dat: %.yoda
	rivet-cmphistos $^ -o plots/

%ZvsPtProfile.gnu: 
	./makeZPtProfile.py $(YODAFILES)
%ZvsPt%Plot.gnu: $(YODAFILES)
	./makeZPtPlot.py $(YODAFILES)

install:
	cp libBOOSTFastJets.so $(LIBDIR)

clean:
	rm -f *.o  *.so
