#!/bin/bash

# While not specific to rivet, I have this alias:
# alias setupRivet='source ~/rivet/local/rivetenv.sh; export PYTHIA8DATA=$(pythia8-config --datadir)/xmldoc'
# So PYTHIA8DATA doesn't get set unless rivetenv.sh has been called... 
if [ -z "${PYTHIA8DATA:-}" ]; then
    echo "Please setup Rivet, see comment in this script"
    exit 1
fi
if [ ! -d ./plots  ]; then
    mkdir plots; 
fi
rivet-cmphistos "$@" -o ./plots/ 
cd plots; make-plots --pdf *.dat; cd ../

