CC=g++

LIBDIR:=$(shell rivet-config --libdir)

INCDIR=$(PWD)/include
RIVETINCDIR:=$(shell rivet-config --cppflags)
LDFLAGS:=$(shell rivet-config --ldflags)
WFLAGS= -Wall -Wno-long-long -Wno-format #-Werror=uninitialized -Werror=delete-non-virtual-dtor  -Wno-unused-local-typedefs
CFLAGS= -g3 -fno-inline -I$(INCDIR) $(RIVETINCDIR) -pedantic -ansi $(WFLAGS) -O0 -Wl,--no-as-needed -lRivet
JETTYPES=Jet ConeJet

SAMPLES=1S0_8 3PJ_8 3S1_8 3PJ_1 3S1_1
YODAFILES:=$(addsuffix .yoda,$(SAMPLES))
PLOTFILES:=$(addsuffix Plot.pdf,$(foreach jet,$(JETTYPES),$(addprefix $(jet)ZvsPt,$(SAMPLES)))) $(addsuffix ZvsPtProfile.pdf,$(JETTYPES))

P6SAMPLES=$(addprefix P6_,$(SAMPLES))
P6YODAFILES:=$(addsuffix .yoda,$(P6SAMPLES))
P6PLOTFILES:=$(addsuffix Plot.pdf,$(foreach jet,$(JETTYPES),$(addprefix $(jet)ZvsPt,$(P6SAMPLES)))) $(addsuffix ZvsPtProfile.pdf,$(JETTYPES))


.PHONY: all plots rivet-plots plot-book
# Code related rules
all: rivet-lib 
rivet-lib: RivetMC_GENSTUDY_CHARMONIUM.so libBOOSTFastJets.so
RivetMC_GENSTUDY_CHARMONIUM.so:  MC_GENSTUDY_CHARMONIUM.cc libBOOSTFastJets.so
	$(CC) -o "$@" -shared -fPIC $(CFLAGS) $< -lBOOSTFastJets $(LDFLAGS) -lfastjetcontribfragile
libBOOSTFastJets.so: src/BOOSTFastJets.cxx
	$(CC)  -shared -fPIC $(CFLAGS) $< -o $@ -lfastjet -lfastjettools $(LDFLAGS)
# Plotting rules for making all pdf plots
plots: $(PLOTFILES) $(P6PLOTFILES)  rivet-plots

plot-book: plotBook.pdf P6PlotBook.pdf
plotBook.pdf: plots
	pdftk $(PLOTFILES) plots/*.pdf cat output $@
P6PlotBook.pdf: plots
	pdftk $(P6PLOTFILES) p6-plots/*.pdf cat output $@
%.pdf: %.tex
	pdflatex $<
%.tex: %.gnu
	gnuplot $<
rivet-plots: dat-files p6-dat-files
	make-plots --pdf $(wildcard plots/*.dat) $(wildcard p6-plots/*.dat)
dat-files: $(YODAFILES)
	rivet-cmphistos --no-ratio $^ -o plots/
p6-dat-files: $(addsuffix .yoda,$(P6SAMPLES))
	rivet-cmphistos --no-ratio $^ -o p6-plots/
P6_%ZvsPtProfile.gnu: $(P6YODAFILES)
	./makeZPtProfile.py -k '$*' $^
JetZvsPtP6_%Plot.gnu: $(P6YODAFILES)
	./makeZPtPlot.py -k 'Jet' $^
ConeJetZvsPtP6_%Plot.gnu: $(P6YODAFILES)
	./makeZPtPlot.py -k 'ConeJet' $^

%ZvsPtProfile.gnu: $(YODAFILES)
	./makeZPtProfile.py -k '$*' $^
JetZvsPt%Plot.gnu: $(YODAFILES)
	./makeZPtPlot.py -k 'Jet' $^
ConeJetZvsPt%Plot.gnu: $(YODAFILES)
	./makeZPtPlot.py -k 'ConeJet' $^

# Cleaning and installing rules
install:
	cp libBOOSTFastJets.so $(LIBDIR)
plot-clean: 
	-rm -f *.aux *.log *.txt plots/*.dat
clean:
	-rm -f *.o  *.so 
dist-clean: clean plot-clean
