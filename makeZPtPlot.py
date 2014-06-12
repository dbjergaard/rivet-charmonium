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
def getPtHistos(filelist,key):
    histos = {}
    for yFile in filelist:
        histos.setdefault(yFile,{})
        objs = yoda.readYODA(yFile)
        for path, obj in objs.iteritems():
            if (not histos[yFile].has_key(path)) and (key+'_pt' in path):
                histos[yFile][path] = obj
    return histos
nPtBins=10
binWidth=25 
def writeCoords(histos, fName):
    name='/MC_GENSTUDY_CHARMONIUM/JetZ_pt%d_%d'
    #datFile=open(fName.split('.')[0]+'.txt','w')
    sys.stdout.write("#+BEGIN_PLOT %s\n"%(fName.split('.')[0],))
    sys.stdout.write("# X\tY\t\tY_err\n")
    for i in range(0,nPtBins):
        hist=histos[name%(binWidth*i,binWidth*(i+1))]
        if hist.sumW()!=0:
            yErr=0.
            if(hist.numEntries() > 1):
                yErr=hist.stdDev()
            sys.stdout.write("%.5g\t%.7g\t%.7g\n"%(binWidth*(i+0.5), hist.mean(),yErr))
    sys.stdout.write("#+END_PLOT\n")
    #datFile.close()
# from colorbrewer2.org
color_palettes={'blue':['#eff3ff','#c6dbef','#9ecae1','#6baed6','#4292c6','#2171b5','#084594'],
                'red':['#fee5d9','#fcbba1','#fc9272','#fb6a4a','#ef3b2c','#cb181d','#99000d'],
                'green':['#edf8e9','#c7e9c0','#a1d99b','#74c476','#41ab5d','#238b45','#005a32'],
                'purple':['#f2f0f7','#dadaeb','#bcbddc','#9e9ac8','#807dba','#6a51a3','#4a1486'],
                'orange':['#feedde','#fdd0a2','#fdae6b','#fd8d3c','#f16913','#d94801','#8c2d04'],
                'black':['#f7f7f7','#d9d9d9','#bdbdbd','#969696','#737373','#525252','#252525']}
def writeGnuPlot(outBName):
    outFile=open(outBName+'.gnu','w')
    outFile.write('set term tikz standalone color solid size 5in,3in\n')
    outFile.write('set output \'%s.tex\'\n\n'%(outBName,))
    outFile.write('unset key\n')
    outFile.write('set view map\n')
    outFile.write('set xtics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autojustify\n')
    outFile.write('set ytics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autojustify\n')
    outFile.write('set ztics border in scale 0,0 nomirror norotate  offset character 0, 0, 0 autojustify\n')
    outFile.write('set nocbtics\n')
    outFile.write('set rtics axis in scale 0,0 nomirror norotate  offset character 0, 0, 0 autojustify\n')
    
    outFile.write('set title "Charmed jet p$_{T}$ vs z"\n')
    outFile.write('set ylabel "$z$"\n')
    outFile.write('set xlabel "Jet $p_T$"\n')
    outFile.write('set xrange[0:%d]\n'%(nPtBins*binWidth))
    outFile.write('set yrange[0:1.10]\n')
    outFile.write('set cblabel "Entries"\n')
    writeColorPalette(color_palettes['red'],outFile)
    # outFile.write('set palette rgbformulae -7, 2, -7\n')
    outFile.write('plot ')
    plotLine="'%s' with image"
    outFile.write(plotLine%(outBName+'.txt',)+'\n')
    outFile.close()
def writeColorPalette(palette,outFile):
    outFile.write('set palette negative defined (')
    for i,color in enumerate(palette[:-1]):
        outFile.write(" %d '%s',"%(i,color))
    outFile.write(" %d '%s'"%(len(palette)-1,palette[-1]))
    outFile.write(')\n')
def dumpCoords(hist,xval,outFile):
    for bin in hist:
        outFile.write('%.3g\t%.3g\t%.3g\n'%(xval, bin.midpoint, bin.height))
def write2DHist(key,histos):
    for fName in histos:
        outName=key+'vsPt'+fName.split('.')[0]+'Plot.txt'
        outFile=open(outName,'w');
        name='/MC_GENSTUDY_CHARMONIUM/JetZ_pt%d_%d'
        for i in range(0,nPtBins):
            hist=histos[fName][name%(binWidth*i,binWidth*(i+1))]
            dumpCoords(hist,binWidth*(i+0.5),outFile)
            outFile.write('\n')
        outFile.close()
        writeGnuPlot(outName.split('.')[0])

def main():
    if(len(args)==0):
        parser.print_help()
        return 1
    histos=getPtHistos(args,'JetZ')
    write2DHist('JetZ', histos)
if __name__ == '__main__':
    sys.exit(main())
