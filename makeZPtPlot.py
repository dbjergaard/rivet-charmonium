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
# from colorbrewer2.org
color_palettes={"blue":["#084594","#2171b5","#4292c6","#6baed6","#9ecae1","#c6dbef","#eff3ff"],
                "red":["#99000d","#cb181d","#ef3b2c","#fb6a4a","#fc9272","#fcbba1","#fee5d9"],
                "green":["#005a32","#238b45","#41ab5d","#74c476","#a1d99b","#c7e9c0","#edf8e9"],
                "purple":["#4a1486","#6a51a3","#807dba","#9e9ac8","#bcbddc","#dadaeb","#f2f0f7"],
                "orange":["#8c2d04","#d94801","#f16913","#fd8d3c","#fdae6b","#fdd0a2","#feedde"],
                "black":["#252525","#525252","#737373","#969696","#bdbdbd","#d9d9d9","#f7f7f7"]}
def getTitle(fileName):
    titles={'1S0_8.yoda':['1S$_0^{(8)}$','blue'], '3PJ_8.yoda':['3P$_J^{(8)}$','blue'],
            '3S1_8.yoda':['3S$_1^{(8)}$','blue'], '3PJ_1.yoda':['3P$_J^{(1)}$','red'],
            '3S1_1.yoda':['3S$_1^{(1)}$','blue']}
    title=[fileName.split('.')[0],'red']
    try:
        title=titles[fileName]
    except KeyError:
        pass
    return title
def writeGnuPlot(outBName,key):
    outName=key+'ZvsPt'+outBName+'Plot.gnu'
    outFile=open(outName,'w')
    outFile.write('set term tikz standalone color solid size 5in,3in\n')
    outFile.write('set output \'%s.tex\'\n\n'%(outName.split('.')[0],))
    outFile.write('unset key\n')
    outFile.write('set view map\n')
    outFile.write('set xtics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autojustify\n')
    outFile.write('set ytics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autojustify\n')
    outFile.write('set ztics border in scale 0,0 nomirror norotate  offset character 0, 0, 0 autojustify\n')
    outFile.write('set nocbtics\n')
    outFile.write('set rtics axis in scale 0,0 nomirror norotate  offset character 0, 0, 0 autojustify\n')
    sampleName='Anti-k$_{t}$'
    if key=='ConeJet':
        sampleName='CA Cone'
    title,color=getTitle(outBName+'.yoda')
    outFile.write('set title "%s jet p$_{T}$ vs z %s"\n'%(sampleName,title))
    outFile.write('set ylabel "$z$"\n')
    outFile.write('set xlabel " $p_T$"\n')
    outFile.write('set xrange[0:%d]\n'%(nPtBins*binWidth))
    outFile.write('set yrange[0:1.10]\n')
    outFile.write('set cblabel "Entries"\n')
    writeColorPalette(color_palettes[color],outFile)
    # outFile.write('set palette rgbformulae -7, 2, -7\n')
    outFile.write('plot ')
    plotLine="'%s' with image"
    outFile.write(plotLine%(outName.split('.')[0]+'.txt',)+'\n')
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
        outName=key+'ZvsPt'+fName.split('.')[0]+'Plot.txt'
        outFile=open(outName,'w')
        name='/MC_GENSTUDY_CHARMONIUM/%sZ_pt%d_%d'
        for i in range(0,nPtBins):
            hist=histos[fName][name%(key,binWidth*i,binWidth*(i+1))]
            dumpCoords(hist,binWidth*(i+0.5),outFile)
            outFile.write('\n')
        outFile.close()
        writeGnuPlot(fName.split('.')[0],key)

def main():
    if(len(args)==0):
        parser.print_help()
        return 1
    key=options.nameKey
    if key == None:
        key='Jet'
    histos=getPtHistos(args,key)
    write2DHist(key, histos)
    return 0
if __name__ == '__main__':
    sys.exit(main())
