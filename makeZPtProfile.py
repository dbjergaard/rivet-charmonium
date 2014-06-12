#! /usr/bin/env python
from optparse import OptionParser, OptionGroup

import rivet, yoda, sys, os, copy
rivet.util.check_python_version()
rivet.util.set_process_name(os.path.basename(__file__))

parser = OptionParser(usage='Usage: %prog [options] yodafile1 yodafile2 ...')
# parser.add_option('-o','--outTarball',   dest='outTarBall',
#                   help='name of tarball outputted by prun',metavar='TARBALL')
(options, args)=parser.parse_args()

# ripped off of rivet-cmphistos 
def getPtHistos(filelist):
    histos = {}
    for yFile in filelist:
        histos.setdefault(yFile,{})
        objs = yoda.readYODA(yFile)
        for path, obj in objs.iteritems():
            if (not histos[yFile].has_key(path)) and ('_pt' in path):
                histos[yFile][path] = obj
    return histos
nPtBins=10
binWidth=25 
def writeCoords(histos, fName):
    name='/MC_GENSTUDY_CHARMONIUM/JetZ_pt%d_%d'
    datFile=open(fName.split('.')[0]+'.txt','w')
    datFile.write("#+BEGIN_PLOT %s\n"%(fName.split('.')[0],))
    datFile.write("# X\tY\t\tY_err\n")
    for i in range(0,nPtBins):
        hist=histos[name%(binWidth*i,binWidth*(i+1))]
        if hist.sumW()!=0:
            yErr=0.
            if(hist.numEntries() > 1):
                yErr=hist.stdDev()
            datFile.write("%.5g\t%.7g\t%.7g\n"%(binWidth*(i+0.5), hist.mean(),yErr))
    datFile.write("#+END_PLOT\n")
    datFile.close()
def printGnuPlot(outBName,plots):
    outFile=open(outBName+'.gnu','w')
    outFile.write('set term tikz standalone color solid size 5in,3in\n')
    outFile.write('set output \'%s.tex\'\n\n'%(outBName,))
    outFile.write('set key bottom left\n')
    outFile.write('set key spacing 1.5\n')
    outFile.write('set title "Charmed jet p$_{T}$ vs $\\\\langle z \\\\rangle$"\n')
    outFile.write('set ylabel "$\\\\langle z \\\\rangle$"\n')
    outFile.write('set xlabel "Jet $p_T$"\n')
    outFile.write('set xrange[0:%d]\n'%(nPtBins*binWidth))
    outFile.write('set yrange[0:1.10]\n')
    # from colorbrew2.org, not color blind safe, not print safe, not copy safe 
    colorWheel=['#92c5de','#4393c3','#2166ac',
                '#b2182b','#d6604d','#f4a582',
                '#a6cee3', '#1f78b4', '#b2df8a',
                '#33a02c', '#fb9a99', '#e31a1c',
                '#fdbf6f', '#ff7f00', '#cab2d6',
                '#6a3d9a', '#ffff99', '#b15928']

    lineStyle="set style line %d lc rgb '%s' lt 1 lw 2 pt 7 ps .75\n"
    # not idiomatic, but required, gnuplot must have linestyles
    # printed before the plot command
    for i in range(len(plots)):
        outFile.write(lineStyle%(i+1,colorWheel[i]))
    outFile.write('plot ')
    plotLine="'%s' \ttitle\t'%s' with \tyerrorbars ls %d"
    lPlots=[(k,plots[k]) for k in plots]
    #plots=dict(lPlots[:-1])
    for i,plot in enumerate(lPlots[:-1]):
        # print i,plot[0]
        outFile.write(plotLine%(plot[0].split('.')[0]+'.txt',plot[0].split('.')[0],i+1)+',\\\n')
    outFile.write(plotLine%(lPlots[-1][0].split('.')[0]+'.txt',
                               lPlots[-1][0].split('.')[0],len(lPlots))+'\n')
    outFile.close()

def main():
    if(len(args)==0):
        parser.print_help()
        return 1
    histos = getPtHistos(args)
    for yFile in histos:
        writeCoords(histos[yFile],yFile)
    printGnuPlot('JetZvsPtProfile',histos)

if __name__ == '__main__':
    sys.exit(main())
