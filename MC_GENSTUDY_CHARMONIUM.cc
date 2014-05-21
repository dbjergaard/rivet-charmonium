// -*- C++ -*-

//System 
#include <map>
#include <algorithm>

// Rivet 
#include "Rivet/Analysis.hh"
#include "Rivet/AnalysisLoader.hh"
//#include "Rivet/RivetAIDA.hh"

//Projections
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/ChargedLeptons.hh"
#include "Rivet/Projections/FastJets.hh"

typedef std::map<std::string,Rivet::Histo1DPtr> BookedHistos;
template <typename lvec> static void dump4vec(lvec four_mom){
  std::cout<<"( "<<four_mom.pt()<<" [GeV], "<<four_mom.eta()<<", "<<four_mom.phi()<<", "<<four_mom.mass()<<" [GeV])"<<std::endl;
}
namespace Rivet {


  /// Generic analysis looking at various distributions of final state particles
  class MC_GENSTUDY_CHARMONIUM : public Analysis {
  public:

    /// Constructor
    MC_GENSTUDY_CHARMONIUM()
      : Analysis("MC_GENSTUDY_CHARMONIUM"),
	jetR(0.6)
    {    }


  public:

    /// @name Analysis methods
    //@{

    /// Book histograms and initialise projections before the run
    void init() {

      // Projections
      const FinalState fs(-4.2, 4.2, .5*GeV);
      addProjection(fs, "FS");
      ChargedLeptons lfs(fs);
      addProjection(lfs, "LFS");
      FastJets JetProjection(fs,FastJets::ANTIKT, jetR);
      addProjection(JetProjection,"Jets");

      // Histograms
      _histograms["JetPt"] = bookHisto1D("JetPt" , 50, 0, 20);
      _histograms["JetM"] = bookHisto1D("JetM" , 25, 0, 20);
      _histograms["JetEta"] = bookHisto1D("JetEta" , 25, -3, 3);
      _histograms["JetMult"] = bookHisto1D("JetMult",10,-0.5,10.5);

      _histograms["JPsiPt"] = bookHisto1D("JPsiPt" , 50, 0, 20);
      _histograms["JPsiM"] = bookHisto1D("JPsiM" , 25, 0, 10);
      _histograms["JPsiEta"] = bookHisto1D("JPsiEta" , 25, -3, 3);

      // Substructure variables
      _histograms["JetZ"] = bookHisto1D("JetZ",50,0,1);
    }



    /// Perform the per-event analysis
    void analyze(const Event& event) {
      cutFlow["Nominal"]++;
      const double weight = event.weight();
      const ChargedLeptons& lProj = applyProjection<ChargedLeptons>(event, "LFS");
      if(lProj.chargedLeptons().empty()){
	vetoEvent;
      }
      cutFlow["Leptons"]++;
      Particles muons;
      foreach(const Particle& lepton,lProj.chargedLeptons()){
	if(lepton.abspid()==13){
	  muons.push_back(lepton);
	}
      }
      if(muons.size() < 2){
      	vetoEvent;
      }
      cutFlow["2Muons"]++;
      const FastJets& jetProj = applyProjection<FastJets>(event, "Jets");
      const Jets jets = jetProj.jetsByPt();
      if(jets.empty()){
      	vetoEvent;
      }

      _histograms["JetMult"]->fill(jets.size(),weight);
      cutFlow["Jets"]++;
      //Process the particles

      FourMomentum j_psi=muons[0].momentum()+muons[1].momentum();
      //dump4vec(j_psi);
      //fill j_psi histos
      _histograms["JPsiEta"]->fill(j_psi.eta(),weight);
      _histograms["JPsiPt"]->fill(j_psi.pt(),weight);
      _histograms["JPsiM"]->fill(j_psi.mass(),weight);

      Jet charmJet;
      foreach(const Jet& j, jets){
      	if( j.mass() > 0 && deltaR(j.momentum(), j_psi) < jetR) {
      	  charmJet=j;
      	}
      }
      if(charmJet.mass()==0.){
	vetoEvent;
      }
      cutFlow["charmJetMass"]++;
      //fill charmjet histos
      _histograms["JetPt"]->fill(charmJet.pt(),weight);
      _histograms["JetM"]->fill(charmJet.mass(),weight);
      _histograms["JetEta"]->fill(charmJet.eta(),weight);


      //calculate substructure variables
      const double z(charmJet.pt() > 0 ? j_psi.pt()/charmJet.pt() : -1.);
      
      //fill substructure histos
      _histograms["JetZ"]->fill(z,weight);

    }

    
    


    /// Finalize
    void finalize() {
      cout<<"Cut flow"<<endl;
      cout<<"|-"<<endl;
      for(std::map<std::string, size_t>::const_iterator cut=cutFlow.begin(); cut != cutFlow.end(); ++cut){
	cout<<"| "<<cut->first<<" | "<<cut->second <<" |"<<endl;
      }
      cout<<"|-"<<endl;
    }

    //@}


  private:
    double jetR;
    /// @name Histograms
    //@{
    BookedHistos _histograms;
    //@}
    std::map<std::string, size_t> cutFlow;
    
  };


  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(MC_GENSTUDY_CHARMONIUM);

}
