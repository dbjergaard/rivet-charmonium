#! /usr/bin/env python
from optparse import OptionParser, OptionGroup

import rivet, yoda, sys, os
rivet.util.check_python_version()
rivet.util.set_process_name(os.path.basename(__file__))

parser = OptionParser(usage='Usage: %prog [options] yodafile1 yodafile2 ...')
# parser.add_option('-o','--outTarball',   dest='outTarBall',
#                   help='name of tarball outputted by prun',metavar='TARBALL')
(options, args)=parser.parse_args()

def main():
    if(len(args)==0):
        parser.print_help()
        return 1
    
if __name__ == '__main__':
    sys.exit(main())
