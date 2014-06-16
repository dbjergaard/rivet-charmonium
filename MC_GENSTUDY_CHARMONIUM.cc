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
      _histograms["JetMult"] = bookHisto1D("JetMult",8,-0.5,8.5);

      _histograms["JPsiPt"] = bookHisto1D("JPsiPt" , 50, 0, 250);
      _histograms["JPsiM"] = bookHisto1D("JPsiM" , 50, 2.95, 3.2);
      _histograms["JPsiEta"] = bookHisto1D("JPsiEta" , 25, -3, 3);

      //N-subjettiness histos	
      // _histograms["JetMassFilt"]	= bookHisto1D("JetMassFilt" , 60, 0, 50);
      // _histograms["JetMassTrim"]	= bookHisto1D("JetMassTrim" , 60, 0, 50);
      // _histograms["JetMassPrune"]	= bookHisto1D("JetMassPrune" , 60, 0, 20);
      // _histograms["NSubJettiness"]	= bookHisto1D("NSubJettiness" , 40, -0.005, 1.005);
      // _histograms["NSubJettiness1Iter"]	= bookHisto1D("NSubJettiness1Iter" , 40, -0.005, 1.005);
      // _histograms["NSubJettiness2Iter"]	= bookHisto1D("NSubJettiness2Iter" , 40, -0.005, 1.005);


      _histograms["ConeJetPullTheta"]       = bookHisto1D("JetPullTheta" ,50,-PI,PI);
      _histograms["ConeJetPullMag"]         = bookHisto1D("ConeJetPullMag" ,50,0,0.06);

      _histograms["CJPThetaPsi"]       = bookHisto1D("CJPThetaPsi" ,50,-PI,PI);
      _histograms["CJPThetaPtn"]       = bookHisto1D("CJPThetaPtn" ,50,-PI,PI);

      char histName[25];
      for(int i=0; i < nPtBins; i++) {
	sprintf(histName,"JetZ_pt%d_%d",binWidth*i,binWidth*(i+1));
	_histograms[string(histName)] = bookHisto1D(histName,50,0,2.0);
      }
      for(int i=0; i < nPtBins; i++) {
	//FIXME this should be its own binwidth, its set to scale 250
	//to 450 for the highest bin
	sprintf(histName,"ConeJetZ_pt%d_%d",int(1.8*binWidth*i),int(1.8*binWidth*(i+1)));
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
      const PseudoJets jets = jetProj.pseudoJetsByPt(45*GeV);
      if(jets.empty()){
	vetoEvent;
      }
      cutFlow["Jets"]++;
      _histograms["JetMult"]->fill(jets.size(),weight);

      //Process the particles
      PseudoJets ConeJetPtcls; 
      foreach(const Particle& p, cfs.particles()){
	if(deltaR(p,j_psi) < jetR){
	  ConeJetPtcls.push_back(fastjet::PseudoJet(p.momentum()));
	}
      }
      ConeJetPtcls.push_back(fastjet::PseudoJet(j_psi.px(),j_psi.py(),j_psi.pz(),j_psi.E()));
      if(ConeJetPtcls.empty()){
	vetoEvent;
      }
      fastjet::ClusterSequence cs(ConeJetPtcls,fastjet::JetDefinition(fastjet::cambridge_algorithm,0.8));
      PseudoJets ConeJets = fastjet::sorted_by_pt(cs.inclusive_jets());
      if(ConeJets.empty()){
	vetoEvent;
      }
      cutFlow["ConeJetRad"]++;
      fastjet::PseudoJet& ConeJet = ConeJets[0];
      // _histograms["JetMassFilt"]->fill(Filter(cs,ConeJet, FastJets::CAM, 3, 0.3).m(), weight);
      // _histograms["JetMassTrim"]->fill(Trimmer(cs,ConeJet, FastJets::CAM, 0.03, 0.3).m(), weight);
      // _histograms["JetMassPrune"]->fill(Pruner(cs,ConeJet, FastJets::CAM, 0.4, 0.1).m(), weight);
      // PseudoJets constituents = ConeJet.constituents();
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
      
      fastjet::PseudoJet charmJet;
      double delR(99.);
      double candDelR(99.);
      foreach(const fastjet::PseudoJet& j, jets){
	delR=deltaR(FourMomentum(j.e(),j.px(),j.py(),j.pz()), j_psi);
      	if( delR < jetR && delR < candDelR) {
      	  charmJet=j;
	  candDelR=delR;
      	}
      }
      if(isinf(deltaR(FourMomentum(charmJet.e(),
				   charmJet.px(),
				   charmJet.py(),
				   charmJet.pz()),j_psi))){
	vetoEvent;
      }
      cutFlow["charmJetMatch"]++;
      fastjet::PseudoJet parton=jets[0];
      if(parton.m2()==charmJet.m2()){
	parton=jets[1];
      }
      fillJetHistos("ConeJet",ConeJet,parton,j_psi,cs,weight); 
      fillJetHistos("Jet",charmJet,parton,j_psi,*jetProj.clusterSeq(),weight); 
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
      snprintf(histName,25,"%sPtZ%d",key,zVal);
      if(fuzzyEquals(z,zVal*0.1,0.1)){
	_histograms[histName]->fill(pt,weight);
      }
    }
    void bookJetHistos(const string& key){
      _histograms[key+"Pt"] = bookHisto1D(key+"Pt" , 50, 0, 250);
      _histograms[key+"M"] = bookHisto1D(key+"M" , 50, 0, 100);
      _histograms[key+"Eta"] = bookHisto1D(key+"Eta" , 25, -3, 3);

      _histograms[key+"PtZ3"] = bookHisto1D(key+"PtZ3" , 50, 0, 250);
      _histograms[key+"PtZ5"] = bookHisto1D(key+"PtZ5" , 50, 0, 250);
      _histograms[key+"PtZ8"] = bookHisto1D(key+"PtZ8" , 50, 0, 250);

      _histograms[key+"DeltaR"] = bookHisto1D(key+"DeltaR",50,0,jetR+0.1);
      _histograms[key+"Z"] = bookHisto1D(key+"Z",50,0,1.10);
      _histograms[key+"Dipolarity"]         =  bookHisto1D(key+"Dipolarity" ,50,0.0,2);

      _histograms[key+"PTheta"]       = bookHisto1D(key+"PTheta" ,50,-PI,PI);
      _histograms[key+"PMag"]         = bookHisto1D(key+"PMag" ,50,0,0.06);

      _histograms[key+"PThetaJPsi"]       = bookHisto1D(key+"PThetaJPsi" ,50,-PI,PI);
      _histograms[key+"PThetaPtn"]       = bookHisto1D(key+"PThetaPtn" ,50,-PI,PI);

      char histName[25];
      for(int i=0; i < nPtBins; i++) {
	sprintf(histName,(key+"Z_pt%d_%d").c_str(),binWidth*i,binWidth*(i+1));
	_histograms[string(histName)] = bookHisto1D(histName,50,0,2.0);
      }
    }
    void fillJetHistos(const string& key, const fastjet::PseudoJet& jet, 
		       const PseudoJet& parton, const FourMomentum& j_psi,
		       const fastjet::ClusterSequence& clusterSeq, const double weight){
      //cache variables for faster access.
      const double jet_pt(jet.pt());
      const double z(jet_pt > 0 ? j_psi.pt()/jet_pt : -1. );

      //kinematics
      _histograms[key+"DeltaR"]->fill(deltaR(j_psi,FourMomentum(jet.e(),jet.px(),jet.py(),jet.pz())),weight);
      _histograms[key+"Pt"]->fill(jet_pt,weight);
      _histograms[key+"M"]->fill(jet.m(),weight);
      _histograms[key+"Eta"]->fill(jet.eta(),weight);
      
      //substructure
      _histograms[key+"Z"]->fill(z,weight);
      fillJetZ(key.c_str(),3,z,jet_pt,weight);
      fillJetZ(key.c_str(),5,z,jet_pt,weight);
      fillJetZ(key.c_str(),8,z,jet_pt,weight);
      
      std::vector<double> pull=JetPull(clusterSeq, jet);
      _histograms[key+"PTheta"]->fill(pull[1],weight);
      _histograms[key+"PMag"]->fill(pull[0],weight);
      pull=JetPull(clusterSeq, j_psi);
      _histograms[key+"PThetaJPsi"]->fill(pull[1],weight);
      pull=JetPull(clusterSeq, parton);
      _histograms[key+"PThetaPtn"]->fill(pull[1],weight);

      _histograms[key+"Dipolarity"]->fill(Dipolarity(jet),weight);
      char histName[25];
      for(int i=0; i < nPtBins; i++ ){
      	if(inRange(jet.pt(),double(binWidth*i),double(binWidth*(i+1)))){
      	  sprintf(histName,(key+"Z_pt%d_%d").c_str(),binWidth*i,binWidth*(i+1));
      	  _histograms[histName]->fill(z,weight);
      	}
	/*
      	if(inRange(ConeJet.pt(),double(1.8*binWidth*i),double(1.8*binWidth*(i+1)))){
      	  sprintf(histName,"ConeJetZ_pt%d_%d",int(1.8*binWidth*i),int(1.8*binWidth*(i+1)));
      	  _histograms[histName]->fill(z,weight);
      	}
	*/
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
