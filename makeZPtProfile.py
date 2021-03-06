#! /usr/bin/env python
from optparse import OptionParser, OptionGroup

import rivet, yoda, sys, os, copy
rivet.util.check_python_version()
rivet.util.set_process_name(os.path.basename(__file__))

parser = OptionParser(usage='Usage: %prog [options] yodafile1 yodafile2 ...')
parser.add_option('-k','--key',   dest='nameKey',
                  help='Prefix for retrieving histograms of the form keyZ_ptLow_High',metavar='TARBALL')
(options, args)=parser.parse_args()

# ripped off of rivet-cmphistos 
def getPtHistos(filelist,key):
    histos = {}
    for yFile in filelist:
        histos.setdefault(yFile,{})
        objs = yoda.readYODA(yFile)
        for path, obj in objs.iteritems():
            if (not histos[yFile].has_key(path)) and (key+'Z_pt' in path):
                histos[yFile][path] = obj
    return histos
nPtBins=10
binWidth=25 
if options.nameKey=='ConeJet':
    binWidth=45
def writeCoords(histos, fName,key):
    name='/MC_GENSTUDY_CHARMONIUM/%sZ_pt%d_%d'
    datFile=open(fName.split('.')[0]+'.txt','w')
    datFile.write("#+BEGIN_PLOT %s\n"%(fName.split('.')[0],))
    datFile.write("# X\tY\t\tY_err\n")
    for i in range(0,nPtBins):
        hist=histos[name%(key,binWidth*i,binWidth*(i+1))]
        if hist.sumW()!=0:
            yErr=0.
            if(hist.numEntries() > 1):
                yErr=hist.stdDev()
            datFile.write("%.5g\t%.7g\t%.7g\n"%(binWidth*(i+0.5), hist.mean(),yErr))
    datFile.write("#+END_PLOT\n")
    datFile.close()
def getTitle(fileName):
    titles={'1S0_8.yoda':'Pythia 8 1S$_0^{(8)}$', '3PJ_8.yoda':'Pythia 8 3P$_J^{(8)}$',
            '3S1_8.yoda':'Pythia 8 3S$_1^{(8)}$', '3PJ_1.yoda':'Pythia 8 3P$_J^{(1)}$',
            '3S1_1.yoda':'Pythia 8 3S$_1^{(1)}$',
            'P6_1S0_8.yoda':'Pythia 6 1S$_0^{(8)}$', 'P6_3PJ_8.yoda':'Pythia 6 3P$_J^{(8)}$',
            'P6_3S1_8.yoda':'Pythia 6 3S$_1^{(8)}$', 'P6_3PJ_1.yoda':'Pythia 6 3P$_J^{(1)}$',
            'P6_3S1_1.yoda':'Pythia 6 3S$_1^{(1)}$'}
    title=fileName.split('.')[0]
    try:
        title=titles[fileName]
    except KeyError:
        pass
    return title
def printGnuPlot(outBName,plots):
    outFile=open(outBName+'.gnu','w')
    outFile.write('set term tikz standalone color solid size 5in,3in\n')
    outFile.write('set output \'%s.tex\'\n\n'%(outBName,))
    outFile.write('set key bottom left\n')
    outFile.write('set key spacing 2.25\n')
    sampleName='Anti-k$_{t}$'
    if 'ConeJet' in outBName:
        sampleName='CA Cone'
    outFile.write('set title "%s jet p$_{T}$ vs $\\\\langle z \\\\rangle$"\n'%sampleName)
    outFile.write('set ylabel "$\\\\langle z \\\\rangle$"\n')
    outFile.write('set xlabel "$p_T$"\n')
    outFile.write('set xrange[0:%d]\n'%(nPtBins*binWidth))
    outFile.write('set yrange[0:1.10]\n')
    # from colorbrew2.org, not color blind safe, not print safe, not
    # copy safe, but they look good :)
    colorWheel=['#b2182b','#d6604d', '#92c5de','#4393c3','#2166ac']
    lineStyle="set style line %d lc rgb '%s' lt 1 lw 2 pt 7 ps .75\n"
    # not idiomatic, but required, gnuplot must have linestyles
    # printed before the plot command
    for i in range(len(plots)):
        outFile.write(lineStyle%(i+1,colorWheel[i]))
    outFile.write('plot ')
    plotLine="'%s' \ttitle\t'%s' with \tyerrorbars ls %d"
    lPlots=[(k,plots[k]) for k in plots]
    for i,plot in enumerate(lPlots[:-1]):
        outFile.write(plotLine%(plot[0].split('.')[0]+'.txt',getTitle(plot[0]),i+1)+',\\\n')
    outFile.write(plotLine%(lPlots[-1][0].split('.')[0]+'.txt',
                            getTitle(lPlots[-1][0]),len(lPlots))+'\n')
    outFile.close()

def main():
    if(len(args)==0):
        parser.print_help()
        return 1
    key=options.nameKey
    if key == None:
        key='Jet'
    histos=getPtHistos(args,key)
    for yFile in histos:
        writeCoords(histos[yFile],yFile,key)
    printGnuPlot(key+'ZvsPtProfile',histos)
    return 0
if __name__ == '__main__':
    sys.exit(main())
