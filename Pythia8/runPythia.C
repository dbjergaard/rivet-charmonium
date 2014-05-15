#include "Pythia8/Pythia.h"
#include "Pythia8/Pythia8ToHepMC.h"

#include "HepMC/GenEvent.h"   
#include "HepMC/IO_GenEvent.h"

#include <cstdio>

using namespace Pythia8; 

int main(int argc, char* argv[]) 
{
  if(argc !=5)
    {
      cerr<<"Usage: "<<argv[0]<<" [config file] [out file] [seed] [tune]"<<endl;
      return 1;
    }
  ifstream is(argv[1]);  
  if (!is) {
    cerr << " Command-line file " << argv[1] << " was not found. \n"
         << " Program stopped! " << endl;
    return 1;
  }
  cout<<"Configuring PYTHIA from "<<argv[1]<<endl;
  cout<<"Writing output to "<<argv[2]<<endl;
  cout <<"Using Seed" << argv[3] <<endl;
  cout <<"Using Tune "<< argv[4] <<endl;
  cout <<"Warning, command line arguments aren't type-checked, don't be stupid." <<endl;
  HepMC::Pythia8ToHepMC ToHepMC;
  HepMC::IO_GenEvent ascii_io(argv[2], std::ios::out);
  char processline[128];
  Pythia pythia;
  pythia.readFile(argv[1]);
  int    nEvent    = 100;
  nEvent = pythia.mode("Main:numberOfEvents");
  int    nAbort    = pythia.mode("Main:timesAllowErrors");
  // Set the seed
  pythia.readString("Random:setSeed = on");
  sprintf(processline,"Random:seed = %s",argv[3]);
  pythia.readString(processline);
  // Set the tune
  sprintf(processline,"Tune:pp = %s",argv[4]);
  pythia.readString(processline);

  pythia.init();
  
  int iAbort = 0;
  for(int iEvent =0; iEvent < nEvent; ++iEvent)
    {
      if (!pythia.next()) 
	{
	  if (pythia.info.atEndOfFile()) 
	    {
	      cout << " Aborted since reached end of Les Houches Event File\n"; 
	      break; 
	    }
	  
	  if (++iAbort < nAbort) continue;
	  cout << " Event generation aborted prematurely, owing to error!\n"; 
	  break;
	}
      HepMC::GenEvent* hepmcevt = new HepMC::GenEvent(HepMC::Units::GEV, HepMC::Units::MM);
      ToHepMC.fill_next_event( pythia, hepmcevt );
      ascii_io << hepmcevt;
      delete hepmcevt;
    }

  pythia.stat();
  return 0;
}
