#include "Pythia8/Pythia.h"
#include "Pythia8/Pythia8ToHepMC.h"

#include "HepMC/GenEvent.h"   
#include "HepMC/IO_GenEvent.h"

#include <cstdio>
#include <cstring>

#include <algorithm>
#include <iostream>

#include <map> 

using namespace Pythia8; 
template < class T> bool contains(const std::vector<T>& container, const T& value){
  return std::find(container.begin(),container.end(),value)!=container.end();
}

int main(int argc, char* argv[]) 
{
  char tune[25];
  char seed[25];

  //tune = "5";
  //seed = "1370";
  strncpy(tune,"5",25);
  strncpy(seed,"1370",25);

  if(argc < 2)
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

  if(argc > 4){
    strncpy(seed,argv[3],25);
  }
  if(argc > 5){
    strncpy(tune,argv[4],25);
  }
  cout <<"Using Seed " << tune <<endl;
  cout <<"Using Tune "<< seed <<endl;
  cout <<"Warning, command line arguments aren't type-checked, don't be stupid." <<endl;
  HepMC::Pythia8ToHepMC ToHepMC;
  HepMC::IO_GenEvent ascii_io(argv[2], std::ios::out);
  char processline[128];
  Pythia pythia;
  pythia.readFile(argv[1]);
  int    nRequested    = 100;
  nRequested = pythia.mode("Main:numberOfEvents");
  int    nAbort    = pythia.mode("Main:timesAllowErrors");
  // Set the seed
  pythia.readString("Random:setSeed = on");
  sprintf(processline,"Random:seed = %s",seed);
  pythia.readString(processline);
  // Set the tune
  sprintf(processline,"Tune:pp = %s",tune);
  pythia.readString(processline);

  pythia.init();
  
  int iAbort = 0;
  int nGenerated=0;
  std::vector<int> requestedPdgId;//(3,0);
  std::map<int,int> particleMap;
  while(nGenerated < nRequested) {
    requestedPdgId.clear();
    particleMap.clear();
    if (!pythia.next()) {
      if (pythia.info.atEndOfFile()) {
	cout << " Aborted since reached end of Les Houches Event File\n"; 
	break; 
      }
	  
      if (++iAbort < nAbort) continue;
      cout << " Event generation aborted prematurely, owing to error!\n"; 
      break;
    }
    for(int i=0; i < pythia.event.size(); i++){
      if(abs(pythia.event[i].id())==443 || abs(pythia.event[i].id())==13){
	requestedPdgId.push_back(pythia.event[i].id());
      }
    }
    if(!(contains(requestedPdgId, 443) && 
	 contains(requestedPdgId, 13)  &&
	 contains(requestedPdgId, -13))){
      continue;
    }
    for(int i=0; i < pythia.event.size(); i++){
      //cout << pythia.event[i].id() << ", ";
      const Particle& ptcl = pythia.event[i];
      particleMap[ptcl.id()]++;
      cout << ptcl.daughterList().size()<<endl;
      cout << ptcl.motherList().size()<<endl;
    }
    //cout << endl << endl;
    cout <<" ID | Multiplicity" << endl;
    for(std::map<int,int>::const_iterator it=particleMap.begin(); it != particleMap.end(); ++it){
      cout <<it->first << " | "<< it->second << endl;
    }

    HepMC::GenEvent* hepmcevt = new HepMC::GenEvent(HepMC::Units::GEV, HepMC::Units::MM);
    ToHepMC.fill_next_event( pythia, hepmcevt );
    ascii_io << hepmcevt ;
    delete hepmcevt;
    nGenerated++;
  }

  pythia.stat();
  return 0;
}
