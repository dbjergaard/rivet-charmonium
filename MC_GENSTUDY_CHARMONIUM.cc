// -*- C++ -*-

//System 
#include <map>
#include <algorithm>
#include <cmath>

// Rivet 
#include "Rivet/Analysis.hh"
#include "Rivet/AnalysisLoader.hh"
#include "Rivet/Tools/BinnedHistogram.hh"
//#include "Rivet/RivetAIDA.hh"
// YODA
// #include "YODA/Histo2D.h"

//Projections
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/ChargedLeptons.hh"
#include "Rivet/Projections/FastJets.hh"

// Local Includes
#include "BOOSTFastJets.h"

typedef std::map<std::string,Rivet::Histo1DPtr> BookedHistos;
template <typename lvec> static void dump4vec(lvec four_mom){
  std::cout<<"( "<<four_mom.pt()<<" [GeV], "<<four_mom.eta()<<", "<<four_mom.phi()<<", "<<four_mom.m()<<" [GeV])"<<std::endl;
}

namespace Rivet {


  /// Generic analysis looking at various distributions of final state particles
  class MC_GENSTUDY_CHARMONIUM : public Analysis {
  public:

    /// Constructor
    MC_GENSTUDY_CHARMONIUM()
      : Analysis("MC_GENSTUDY_CHARMONIUM"),
	jetR(0.4),
	nPtBins(10),
	binWidth(25)
    {    }


  public:

    /// @name Analysis methods
    //@{

    /// Book histograms and initialise projections before the run
    void init() {

      // Projections
      const FinalState fs(-4.2, 4.2, .5*GeV);
      addProjection(fs, "FS");
      addProjection(ChargedFinalState(-4.2, 4.2, 500*MeV), "CFS");

      ChargedLeptons lfs(fs);
      addProjection(lfs, "LFS");
      FastJets JetProjection(fs,FastJets::ANTIKT, jetR);
      addProjection(JetProjection,"Jets");

      // Histograms
      _histograms["JetPt"] = bookHisto1D("JetPt" , 50, 0, 250);
      _histograms["JetM"] = bookHisto1D("JetM" , 50, 0, 100);
      _histograms["JetEta"] = bookHisto1D("JetEta" , 25, -3, 3);
      _histograms["JetMult"] = bookHisto1D("JetMult",8,-0.5,8.5);

      _histograms["JPsiPt"] = bookHisto1D("JPsiPt" , 50, 0, 250);
      _histograms["JPsiM"] = bookHisto1D("JPsiM" , 50, 2.95, 3.2);
      _histograms["JPsiEta"] = bookHisto1D("JPsiEta" , 25, -3, 3);

      _histograms["JPsiJetPt"] = bookHisto1D("JPsiJetPt" , 50, 0, 450);
      _histograms["JPsiJetM"] = bookHisto1D("JPsiJetM" , 50, 0, 225);
      _histograms["JPsiJetEta"] = bookHisto1D("JPsiJetEta" , 25, -3, 3);

      _histograms["JPsiJetPtZ3"] = bookHisto1D("JPsiJetPtZ3" , 50, 0, 450);
      _histograms["JPsiJetPtZ5"] = bookHisto1D("JPsiJetPtZ5" , 50, 0, 450);
      _histograms["JPsiJetPtZ8"] = bookHisto1D("JPsiJetPtZ8" , 50, 0, 450);

      _histograms["JetPtZ3"] = bookHisto1D("JetPtZ3" , 50, 0, 250);
      _histograms["JetPtZ5"] = bookHisto1D("JetPtZ5" , 50, 0, 250);
      _histograms["JetPtZ8"] = bookHisto1D("JetPtZ8" , 50, 0, 250);
      
      // Substructure variables
      _histograms["DeltaR"] = bookHisto1D("DeltaR",50,0,jetR+0.1);
      _histograms["JetZ"] = bookHisto1D("JetZ",50,0,1.10);
      _histograms["JPsiJetZ"] = bookHisto1D("JetZ",50,0,1.10);

      //Dipolarity 
      _histograms["Dipolarity"]         =  bookHisto1D("Dipolarity" ,50,0.0,2);

      //N-subjettiness histos	
      // _histograms["JetMassFilt"]	= bookHisto1D("JetMassFilt" , 60, 0, 50);
      // _histograms["JetMassTrim"]	= bookHisto1D("JetMassTrim" , 60, 0, 50);
      // _histograms["JetMassPrune"]	= bookHisto1D("JetMassPrune" , 60, 0, 20);
      // _histograms["NSubJettiness"]	= bookHisto1D("NSubJettiness" , 40, -0.005, 1.005);
      // _histograms["NSubJettiness1Iter"]	= bookHisto1D("NSubJettiness1Iter" , 40, -0.005, 1.005);
      // _histograms["NSubJettiness2Iter"]	= bookHisto1D("NSubJettiness2Iter" , 40, -0.005, 1.005);
      _histograms["JetPullTheta"]       = bookHisto1D("JetPullTheta" ,50,-PI,PI);
      _histograms["JetPullMag"]         = bookHisto1D("JetPullMag" ,50,0,0.06);


      char histName[25];
      for(int i=0; i < nPtBins; i++) {
	sprintf(histName,"JetZ_pt%d_%d",binWidth*i,binWidth*(i+1));
	_histograms[string(histName)] = bookHisto1D(histName,50,0,2.0);
      }
      for(int i=0; i < nPtBins; i++) {
	//FIXME this should be its own binwidth, its set to scale 250
	//to 450 for the highest bin
	sprintf(histName,"JPsiJetZ_pt%d_%d",int(1.8*binWidth*i),int(1.8*binWidth*(i+1)));
	_histograms[string(histName)] = bookHisto1D(histName,50,0,2.0);
      }

    }

    /// Perform the per-event analysis
    void analyze(const Event& event) {
      cutFlow["Nominal"]++;
      const double weight = event.weight();
      const FinalState& cfs = applyProjection<FinalState>(event,"CFS");
      const ChargedLeptons& lProj = applyProjection<ChargedLeptons>(event, "LFS");
      if(lProj.chargedLeptons().empty()){
	vetoEvent;
      }
      cutFlow["Leptons"]++;

      Particles muons;
      foreach(const Particle& lepton,lProj.chargedLeptons()){
	if(abs(lepton.pid())==13){
	  muons.push_back(lepton);
	}
      }
      if(muons.size() < 2){
	vetoEvent;
      }
      cutFlow["2Muons"]++;

      FourMomentum j_psi;
      FourMomentum cand;
      double deltaM=10000.;
      const double j_psi_m = 3.096916;
      foreach(const Particle& mu1, muons){
	foreach(const Particle& mu2, muons){
	  cand=mu1.momentum()+mu2.momentum();
	  if(mu1.pid()*mu2.pid() < 0 && fabs(cand.mass()-j_psi_m) < deltaM ){
	    j_psi=cand;
	    deltaM=fabs(cand.mass()-j_psi_m);
	  }
	}
      }
      if(j_psi.mass()==0 || deltaM > .2) { //reject if ~2*width of J/psi
	vetoEvent;
      }

      const FastJets& jetProj = applyProjection<FastJets>(event, "Jets");
      const Jets jets = jetProj.jetsByPt(45*GeV);
      if(jets.empty()){
	vetoEvent;
      }
      cutFlow["Jets"]++;
      _histograms["JetMult"]->fill(jets.size(),weight);

      //Process the particles
      PseudoJets jPsiJetPtcls; 
      foreach(const Particle& p, cfs.particles()){
	if(deltaR(p,j_psi) < jetR){
	  jPsiJetPtcls.push_back(fastjet::PseudoJet(p.momentum()));
	}
      }
      if(jPsiJetPtcls.empty()){
	vetoEvent;
      }
      fastjet::ClusterSequence cs(jPsiJetPtcls,fastjet::JetDefinition(fastjet::cambridge_algorithm,jetR));
      PseudoJets jPsiJets = fastjet::sorted_by_pt(cs.inclusive_jets());
      if(jPsiJets.empty()){
	vetoEvent;
      }
      cutFlow["jPsiJetRad"]++;
      fastjet::PseudoJet& jPsiJet = jPsiJets[0];
      _histograms["JPsiJetPt"]->fill(jPsiJet.pt());
      _histograms["JPsiJetEta"]->fill(jPsiJet.eta());
      //dump4vec(jPsiJet);
      //FIXME The mass is negative!
      _histograms["JPsiJetM"]->fill(fabs(jPsiJet.m()));
      
      _histograms["Dipolarity"]->fill(Dipolarity(jPsiJet),weight);
      const std::pair<double,double> tvec=JetPull(cs,jPsiJet);
      _histograms["JetPullMag"]->fill(tvec.first,weight);
      if(tvec.first > 0) {
	_histograms["JetPullTheta"]->fill(tvec.second,weight);
      }
      // _histograms["JetMassFilt"]->fill(Filter(cs,jPsiJet, FastJets::CAM, 3, 0.3).m(), weight);
      // _histograms["JetMassTrim"]->fill(Trimmer(cs,jPsiJet, FastJets::CAM, 0.03, 0.3).m(), weight);
      // _histograms["JetMassPrune"]->fill(Pruner(cs,jPsiJet, FastJets::CAM, 0.4, 0.1).m(), weight);
      // PseudoJets constituents = jPsiJet.constituents();
      // if (constituents.size() > 10) {
      // 	PseudoJets axes(GetAxes(cs, 2, constituents, FastJets::CAM, 0.4));
      // 	_histograms["NSubJettiness"]->fill(TauValue(2, 1, constituents, axes), weight);
      // 	UpdateAxes(2, constituents, axes);
      // 	_histograms["NSubJettiness1Iter"]->fill(TauValue(2, 1, constituents, axes), weight);
      // 	UpdateAxes(2, constituents, axes);
      // 	_histograms["NSubJettiness2Iter"]->fill(TauValue(2, 1, constituents, axes), weight);
      // }


      //fill j_psi histos
      _histograms["JPsiEta"]->fill(j_psi.eta(),weight);
      _histograms["JPsiPt"]->fill(j_psi.pt(),weight);
      _histograms["JPsiM"]->fill(j_psi.mass(),weight);

      Jet charmJet;
      double delR(99.);
      double candDelR(99.);
      foreach(const Jet& j, jets){
	delR=deltaR(j.momentum(), j_psi);
      	if( delR < jetR && delR < candDelR) {
      	  charmJet=j;
	  candDelR=delR;
      	}
      }
      if(isinf(deltaR(charmJet,j_psi))){
	vetoEvent;
      }
      cutFlow["charmJetMatch"]++;

      _histograms["DeltaR"]->fill(deltaR(j_psi,charmJet),weight);
      _histograms["JetPt"]->fill(charmJet.pt(),weight);
      _histograms["JetM"]->fill(charmJet.mass(),weight);
      _histograms["JetEta"]->fill(charmJet.eta(),weight);

      //calculate substructure variables
      const double z(charmJet.pt() > 0 ? j_psi.pt()/charmJet.pt() : -1.);
      const double z_jpsi(jPsiJet.pt() > 0 ? j_psi.pt()/jPsiJet.pt() : -1.);
      //fill substructure histos
      _histograms["JetZ"]->fill(z,weight);
      _histograms["JPsiJetZ"]->fill(z_jpsi,weight);
      fillJetZ("Jet",3,z,charmJet.pt(),weight);
      fillJetZ("Jet",5,z,charmJet.pt(),weight);
      fillJetZ("Jet",8,z,charmJet.pt(),weight);

      fillJetZ("JPsiJet",3,z_jpsi,jPsiJet.pt(),weight);
      fillJetZ("JPsiJet",5,z_jpsi,jPsiJet.pt(),weight);
      fillJetZ("JPsiJet",8,z_jpsi,jPsiJet.pt(),weight);
      
      char histName[25];
      for(int i=0; i < nPtBins; i++ ){
	if(inRange(charmJet.pt(),double(binWidth*i),double(binWidth*(i+1)))){
	  sprintf(histName,"JetZ_pt%d_%d",binWidth*i,binWidth*(i+1));
	  _histograms[histName]->fill(z,weight);
	}
	if(inRange(jPsiJet.pt(),double(1.8*binWidth*i),double(1.8*binWidth*(i+1)))){
	  sprintf(histName,"JPsiJetZ_pt%d_%d",int(1.8*binWidth*i),int(1.8*binWidth*(i+1)));
	  _histograms[histName]->fill(z,weight);
	}
      }
    }

    /// Finalize
    void finalize() {
      cout<<"Cut flow"<<endl;
      cout<<"|-"<<endl;
      for(std::map<std::string, size_t>::const_iterator cut = cutFlow.begin();
	  cut != cutFlow.end(); ++cut){
	cout<<"| "<<cut->first << " | "<<cut->second<<" |"<<endl;
      }
      cout<<"|-"<<endl;
    }

    //@}


  private:
    void fillJetZ(const char* key,const int zVal, const double z,const double pt,const double weight){
      char histName[25];
      snprintf(histName,25,"%sZ%d",key,zVal);
      if(fuzzyEquals(z,zVal*0.1,0.1)){
	_histograms[histName]->fill(pt,weight);
      }
    }
    double jetR;
    /// @name Histograms
    //@{
    BookedHistos _histograms;
    //@}
    std::map<std::string, size_t> cutFlow;
    const int nPtBins;
    const int binWidth;
  };


  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(MC_GENSTUDY_CHARMONIUM);

}
