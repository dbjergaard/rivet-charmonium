CC=g++

LIBDIR:=$(shell rivet-config --libdir)

INCDIR=$(PWD)/include
RIVETINCDIR:=$(shell rivet-config --cppflags)
LDFLAGS:=$(shell rivet-config --ldflags)
WFLAGS= -Wall -Wno-long-long -Wno-format #-Werror=uninitialized -Werror=delete-non-virtual-dtor  -Wno-unused-local-typedefs
CFLAGS= -I$(INCDIR) $(RIVETINCDIR) -pedantic -ansi $(WFLAGS) -O2 -Wl,--no-as-needed -lRivet
SAMPLES=1S0_8 3PJ_8 3S1_8 3PJ_1 3S1_1
YODAFILES:=$(addsuffix .yoda,$(SAMPLES))
PLOTFILES:=$(addsuffix Plot.pdf,$(addprefix JetZvsPt,$(SAMPLES)))

.PHONY: all plots rivet-plots plot-book
# Code related rules
all: rivet-lib 
rivet-lib: RivetMC_GENSTUDY_CHARMONIUM.so libBOOSTFastJets.so
RivetMC_GENSTUDY_CHARMONIUM.so:  MC_GENSTUDY_CHARMONIUM.cc libBOOSTFastJets.so
	$(CC) -o "$@" -shared -fPIC $(CFLAGS) $< -lBOOSTFastJets $(LDFLAGS)
libBOOSTFastJets.so: src/BOOSTFastJets.cxx
	$(CC) -shared -fPIC $(CFLAGS) $< -o $@ -lfastjet -lfastjettools $(LDFLAGS)
# Plotting rules for making all pdf plots
plots: JetZvsPtProfile.pdf $(PLOTFILES) rivet-plots
plot-book: plotBook.pdf
plotBook.pdf: plots
	pdftk $(PLOTFILES) JetZvsPtProfile.pdf plots/*.pdf cat output plotBook.pdf
%.pdf: %.tex
	pdflatex $<
%.tex: %.gnu
	gnuplot $<
rivet-plots: dat-files
	make-plots --pdf $(wildcard plots/*.dat)
dat-files: $(YODAFILES)
	rivet-cmphistos $^ -o plots/

%ZvsPtProfile.gnu: $(YODAFILES)
	./makeZPtProfile.py $^
JetZvsPt%Plot.gnu: $(YODAFILES)
	./makeZPtPlot.py $^
# Cleaning and installing rules
install:
	cp libBOOSTFastJets.so $(LIBDIR)
plot-clean: 
	-rm -f *.aux *.log *.txt plots/*.dat
clean:
	-rm -f *.o  *.so 
dist-clean: clean plot-clean
