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
plots: JetZvsPtProfile.pdf $(PLOTFILES) rivet-plots

plotBook.pdf: plots
	pdftk $(PLOTFILES) JetZvsPtProfile.pdf plots/*.pdf cat output plotBook.pdf
%.pdf: %.tex
	pdflatex $<

%.tex: %.gnu
	gnuplot $<
rivet-plots: plots/*.dat
	make-plots --pdf $^
plots/%.dat: $(YODAFILES)
	rivet-cmphistos $^ -o plots/

%ZvsPtProfile.gnu: $(YODAFILES)
	./makeZPtProfile.py $^
JetZvsPt%Plot.gnu: $(YODAFILES)
	./makeZPtPlot.py $^
install:
	cp libBOOSTFastJets.so $(LIBDIR)

clean:
	rm -f *.o  *.so
