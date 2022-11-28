//////////////////////////////////////////////////////////////////////
//                                                                  //
//  Analyzer for making mini-ntuple for L1 track performance plots  //
//                                                                  //
//////////////////////////////////////////////////////////////////////

////////////////////
// FRAMEWORK HEADERS
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

///////////////////////
// DATA FORMATS HEADERS
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"

#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
#include "DataFormats/L1TrackTrigger/interface/TTCluster.h"
#include "DataFormats/L1TrackTrigger/interface/TTStub.h"
#include "DataFormats/L1TrackTrigger/interface/TTTrack.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTClusterAssociationMap.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTStubAssociationMap.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTTrackAssociationMap.h"
#include "Geometry/Records/interface/StackedTrackerGeometryRecord.h"

#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"

////////////////////////////
// DETECTOR GEOMETRY HEADERS
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/RectangularPixelTopology.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"

#include "Geometry/CommonTopologies/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelGeomDetType.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelTopologyBuilder.h"
#include "Geometry/Records/interface/StackedTrackerGeometryRecord.h"

////////////////
// PHYSICS TOOLS
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Math/interface/LorentzVector.h"


///////////////
// ROOT HEADERS
#include <TROOT.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TF1.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TLorentzVector.h>

//////////////
// STD HEADERS
#include <memory>
#include <string>
#include <iostream>

//////////////
// NAMESPACES
using namespace std;
using namespace edm;

//////////////////////////////
//                          //
//     CLASS DEFINITION     //
//                          //
//////////////////////////////

class L1TrackReducedMaker : public edm::one::EDAnalyzer<edm::one::SharedResources> {
private:
  typedef TTTrack<Ref_Phase2TrackerDigi_> L1Track;
  typedef edm::Ptr<L1Track> L1TrackPtr;
  typedef std::vector<L1TrackPtr> L1TrackPtrCollection;
  typedef std::vector<L1Track> L1TrackCollection;
  typedef edm::Ref<L1TrackCollection> L1TrackRef;
  typedef edm::RefVector<L1TrackCollection> L1TrackRefCollection;

public:
  // Constructor/destructor
  explicit L1TrackReducedMaker(const edm::ParameterSet& iConfig);
  ~L1TrackReducedMaker() override;

  // Mandatory methods
  void beginJob() override;
  void endJob() override;
  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;

private:
  //-----------------------------------------------------------------------------------------------
  // Containers of parameters passed by python configuration file
  edm::ParameterSet config;

  int MyProcess;       // 11/13/211 for single electrons/muons/pions, 6/15 for pions from ttbar/taus, 1 for inclusive
  bool DebugMode;      // lots of debug printout statements
  bool SaveAllTracks;  // store in ntuples not only truth-matched tracks but ALL tracks
  bool SaveStubs;      // option to save also stubs in the ntuples (makes them large...)
  int TP_minNStub;  // require TPs to have >= minNStub (defining efficiency denominator) (==0 means to only require >= 1 cluster)
  int TP_minNStubLayer;  // require TPs to have stubs in >= minNStubLayer layers/disks (defining efficiency denominator)
  double TP_minPt;       // save TPs with pt > minPt
  double TP_maxEta;      // save TPs with |eta| < maxEta
  double TP_maxZ0;       // save TPs with |z0| < maxZ0
  int L1Tk_minNStub;     // require L1 tracks to have >= minNStub (this is mostly for tracklet purposes)
  bool SaveTrackMET;

  bool SaveVertexEmulation;


  edm::InputTag L1TrackInputTag;               // L1 track collection
  edm::InputTag L1GTTTrackInputTag;               // L1 GTT track collection
  edm::InputTag MCTruthTrackInputTag;          // MC truth collection
  edm::InputTag MCTruthClusterInputTag;
  edm::InputTag L1StubInputTag;
  edm::InputTag MCTruthStubInputTag;
  edm::InputTag TrackingParticleInputTag;
  edm::InputTag TrackingVertexInputTag;
  edm::InputTag RecoVertexInputTag;
  edm::InputTag GenParticleInputTag;
  edm::InputTag GenVertexInputTag;

  edm::InputTag TrackMETInputTag;

  edm::EDGetTokenT<edmNew::DetSetVector<TTCluster<Ref_Phase2TrackerDigi_> > > ttClusterToken_;
  edm::EDGetTokenT<edmNew::DetSetVector<TTStub<Ref_Phase2TrackerDigi_> > > ttStubToken_;
  edm::EDGetTokenT<TTClusterAssociationMap<Ref_Phase2TrackerDigi_> > ttClusterMCTruthToken_;
  edm::EDGetTokenT<TTStubAssociationMap<Ref_Phase2TrackerDigi_> > ttStubMCTruthToken_;

  edm::EDGetTokenT<std::vector<TTTrack<Ref_Phase2TrackerDigi_> > > ttTrackToken_;
  edm::EDGetTokenT<std::vector<TTTrack<Ref_Phase2TrackerDigi_> > > GTTttTrackToken_;
  edm::EDGetTokenT<TTTrackAssociationMap<Ref_Phase2TrackerDigi_> > ttTrackMCTruthToken_;

  edm::EDGetTokenT<std::vector<TrackingParticle> > TrackingParticleToken_;
  edm::EDGetTokenT<HepMCProduct> GenVertexToken_;

  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> tTopoToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> tGeomToken_;


  //-----------------------------------------------------------------------------------------------
  // tree & branches for mini-ntuple

  bool available_;  // ROOT file for histograms is open.
  TTree* eventTree;

  // primary vertex
  std::vector<float>* m_pv_MC;

  // all L1 tracks (prompt)
  std::vector<float>* m_trk_pt;
  std::vector<float>* m_trk_eta;
  std::vector<float>* m_trk_phi;
  std::vector<float>* m_trk_phi_local;
  std::vector<float>* m_trk_d0;  // (filled if nFitPar==5, else 999)
  std::vector<float>* m_trk_z0;
  std::vector<float>* m_trk_chi2;
  std::vector<float>* m_trk_chi2dof;
  std::vector<float>* m_trk_chi2rphi;
  std::vector<float>* m_trk_chi2rz;
  std::vector<float>* m_trk_bendchi2;
  std::vector<float>* m_trk_MVA1;
  std::vector<float>* m_trk_MVA2;
  std::vector<int>* m_trk_nstub;
  std::vector<int>* m_trk_lhits;
  std::vector<int>* m_trk_dhits;
  std::vector<int>* m_trk_seed;
  std::vector<int>* m_trk_hitpattern;
  std::vector<unsigned int>* m_trk_phiSector;
  std::vector<int>* m_trk_genuine;
  std::vector<int>* m_trk_loose;
  std::vector<int>* m_trk_unknown;
  std::vector<int>* m_trk_combinatoric;
  std::vector<int>* m_trk_fake;  //0 fake, 1 track from primary interaction, 2 secondary track
  std::vector<int>* m_trk_matchtp_pdgid;
  std::vector<float>* m_trk_matchtp_pt;
  std::vector<float>* m_trk_matchtp_eta;
  std::vector<float>* m_trk_matchtp_phi;
  std::vector<float>* m_trk_matchtp_z0;
  std::vector<float>* m_trk_matchtp_dxy;
  std::vector<float>* m_trk_gtt_pt;
  std::vector<float>* m_trk_gtt_eta;
  std::vector<float>* m_trk_gtt_phi;

  // All GTT tracks

  std::vector<unsigned int>*  m_trk_word_valid;
  std::vector<unsigned int>*  m_trk_word_InvR;
  std::vector<unsigned int>*  m_trk_word_pT;
  std::vector<unsigned int>*  m_trk_word_Phi;
  std::vector<unsigned int>*  m_trk_word_TanL;
  std::vector<unsigned int>*  m_trk_word_eta;
  std::vector<unsigned int>*  m_trk_word_Z0;
  std::vector<unsigned int>*  m_trk_word_D0;
  std::vector<unsigned int>*  m_trk_word_chi2rphi;
  std::vector<unsigned int>*  m_trk_word_chi2rz;
  std::vector<unsigned int>*  m_trk_word_bendchi2;
  std::vector<unsigned int>*  m_trk_word_hitpattern;
  std::vector<unsigned int>*  m_trk_word_MVAquality;
  std::vector<unsigned int>*  m_trk_word_MVAother;


  // all tracking particles
  std::vector<float>* m_tp_pt;
  std::vector<float>* m_tp_eta;
  std::vector<float>* m_tp_phi;
  std::vector<float>* m_tp_dxy;
  std::vector<float>* m_tp_d0;
  std::vector<float>* m_tp_z0;
  std::vector<float>* m_tp_d0_prod;
  std::vector<float>* m_tp_z0_prod;
  std::vector<int>* m_tp_pdgid;
  std::vector<int>* m_tp_nmatch;
  std::vector<int>* m_tp_nstub;
  std::vector<int>* m_tp_eventid;
  std::vector<int>* m_tp_charge;

};

//////////////////////////////////
//                              //
//     CLASS IMPLEMENTATION     //
//                              //
//////////////////////////////////

//////////////
// CONSTRUCTOR
L1TrackReducedMaker::L1TrackReducedMaker(edm::ParameterSet const& iConfig) : config(iConfig) {
  MyProcess = iConfig.getParameter<int>("MyProcess");
  DebugMode = iConfig.getParameter<bool>("DebugMode");
  SaveAllTracks = iConfig.getParameter<bool>("SaveAllTracks");
  TP_minNStub = iConfig.getParameter<int>("TP_minNStub");
  TP_minNStubLayer = iConfig.getParameter<int>("TP_minNStubLayer");
  TP_minPt = iConfig.getParameter<double>("TP_minPt");
  TP_maxEta = iConfig.getParameter<double>("TP_maxEta");
  TP_maxZ0 = iConfig.getParameter<double>("TP_maxZ0");
  L1Tk_minNStub = iConfig.getParameter<int>("L1Tk_minNStub");

  L1StubInputTag = iConfig.getParameter<edm::InputTag>("L1StubInputTag");
  MCTruthClusterInputTag = iConfig.getParameter<edm::InputTag>("MCTruthClusterInputTag");
  MCTruthStubInputTag = iConfig.getParameter<edm::InputTag>("MCTruthStubInputTag");
  TrackingParticleInputTag = iConfig.getParameter<edm::InputTag>("TrackingParticleInputTag");
  GenVertexInputTag   = iConfig.getParameter<InputTag >("GenVertexInputTag");

  L1TrackInputTag = iConfig.getParameter<edm::InputTag>("L1TrackInputTag");
  L1GTTTrackInputTag = iConfig.getParameter<edm::InputTag>("L1GTTTrackInputTag");
  MCTruthTrackInputTag = iConfig.getParameter<edm::InputTag>("MCTruthTrackInputTag");

  ttTrackToken_ = consumes<std::vector<TTTrack<Ref_Phase2TrackerDigi_> > >(L1TrackInputTag);
  GTTttTrackToken_ = consumes<std::vector<TTTrack<Ref_Phase2TrackerDigi_> > >(L1GTTTrackInputTag);
  ttTrackMCTruthToken_ = consumes<TTTrackAssociationMap<Ref_Phase2TrackerDigi_> >(MCTruthTrackInputTag);

  ttStubToken_ = consumes<edmNew::DetSetVector<TTStub<Ref_Phase2TrackerDigi_> > >(L1StubInputTag);
  ttClusterMCTruthToken_ = consumes<TTClusterAssociationMap<Ref_Phase2TrackerDigi_> >(MCTruthClusterInputTag);
  ttStubMCTruthToken_ = consumes<TTStubAssociationMap<Ref_Phase2TrackerDigi_> >(MCTruthStubInputTag);
  TrackingParticleToken_ = consumes<std::vector<TrackingParticle> >(TrackingParticleInputTag);
  GenVertexToken_ = consumes<HepMCProduct>(GenVertexInputTag);

  tTopoToken_ = esConsumes<TrackerTopology, TrackerTopologyRcd>(edm::ESInputTag("", ""));
  tGeomToken_ = esConsumes<TrackerGeometry, TrackerDigiGeometryRecord>(edm::ESInputTag("", ""));

  usesResource(TFileService::kSharedResource);

}

/////////////
// DESTRUCTOR
L1TrackReducedMaker::~L1TrackReducedMaker() {}

//////////
// END JOB
void L1TrackReducedMaker::endJob() {
  // things to be done at the exit of the event Loop
  //  edm::LogVerbatim("Tracklet") << "L1TrackReducedMaker::endJob";
}

////////////
// BEGIN JOB
void L1TrackReducedMaker::beginJob() {
  // things to be done before entering the event Loop
  //  edm::LogVerbatim("Tracklet") << "L1TrackReducedMaker::beginJob";

  //-----------------------------------------------------------------------------------------------
  // book histograms / make ntuple
  edm::Service<TFileService> fs;
  available_ = fs.isAvailable();
  if (not available_)
    return;  // No ROOT file open.

  // initilize
  m_trk_pt = new std::vector<float>;
  m_trk_eta = new std::vector<float>;
  m_trk_phi = new std::vector<float>;
  m_trk_phi_local = new std::vector<float>;
  m_trk_z0 = new std::vector<float>;
  m_trk_d0 = new std::vector<float>;
  m_trk_chi2 = new std::vector<float>;
  m_trk_chi2dof = new std::vector<float>;
  m_trk_chi2rphi = new std::vector<float>;
  m_trk_chi2rz = new std::vector<float>;
  m_trk_bendchi2 = new std::vector<float>;
  m_trk_MVA1 = new std::vector<float>;
  m_trk_MVA2 = new std::vector<float>;
  m_trk_nstub = new std::vector<int>;
  m_trk_lhits = new std::vector<int>;
  m_trk_dhits = new std::vector<int>;
  m_trk_seed = new std::vector<int>;
  m_trk_hitpattern = new std::vector<int>;
  m_trk_phiSector = new std::vector<unsigned int>;
  m_trk_genuine = new std::vector<int>;
  m_trk_loose = new std::vector<int>;
  m_trk_unknown = new std::vector<int>;
  m_trk_combinatoric = new std::vector<int>;
  m_trk_fake = new std::vector<int>;
  m_trk_matchtp_pdgid = new std::vector<int>;
  m_trk_matchtp_pt = new std::vector<float>;
  m_trk_matchtp_eta = new std::vector<float>;
  m_trk_matchtp_phi = new std::vector<float>;
  m_trk_matchtp_z0 = new std::vector<float>;
  m_trk_matchtp_dxy = new std::vector<float>;
  m_trk_gtt_pt = new std::vector<float>;
  m_trk_gtt_eta = new std::vector<float>;
  m_trk_gtt_phi = new std::vector<float>;

  m_trk_word_valid = new std::vector<unsigned int>;
  m_trk_word_InvR = new std::vector<unsigned int>;
  m_trk_word_pT = new std::vector<unsigned int>;
  m_trk_word_Phi = new std::vector<unsigned int>;
  m_trk_word_TanL = new std::vector<unsigned int>;
  m_trk_word_eta = new std::vector<unsigned int>;
  m_trk_word_Z0 = new std::vector<unsigned int>;
  m_trk_word_D0 = new std::vector<unsigned int>;
  m_trk_word_chi2rphi = new std::vector<unsigned int>;
  m_trk_word_chi2rz = new std::vector<unsigned int>;
  m_trk_word_bendchi2 = new std::vector<unsigned int>;
  m_trk_word_hitpattern = new std::vector<unsigned int>;
  m_trk_word_MVAquality = new std::vector<unsigned int>;
  m_trk_word_MVAother = new std::vector<unsigned int>;

  m_tp_pt = new std::vector<float>;
  m_tp_eta = new std::vector<float>;
  m_tp_phi = new std::vector<float>;
  m_tp_dxy = new std::vector<float>;
  m_tp_d0 = new std::vector<float>;
  m_tp_z0 = new std::vector<float>;
  m_tp_d0_prod = new std::vector<float>;
  m_tp_z0_prod = new std::vector<float>;
  m_tp_pdgid = new std::vector<int>;
  m_tp_nmatch = new std::vector<int>;
  m_tp_nstub = new std::vector<int>;
  m_tp_eventid = new std::vector<int>;
  m_tp_charge = new std::vector<int>;

  m_pv_MC = new std::vector<float>;
  // ntuple
  eventTree = fs->make<TTree>("eventTree", "Event tree");

  if (SaveAllTracks) {
    eventTree->Branch("trk_pt", &m_trk_pt);
    eventTree->Branch("trk_eta", &m_trk_eta);
    eventTree->Branch("trk_phi", &m_trk_phi);
    eventTree->Branch("trk_phi_local", &m_trk_phi_local);
    eventTree->Branch("trk_d0", &m_trk_d0);
    eventTree->Branch("trk_z0", &m_trk_z0);
    eventTree->Branch("trk_chi2", &m_trk_chi2);
    eventTree->Branch("trk_chi2dof", &m_trk_chi2dof);
    eventTree->Branch("trk_chi2rphi", &m_trk_chi2rphi);
    eventTree->Branch("trk_chi2rz", &m_trk_chi2rz);
    eventTree->Branch("trk_bendchi2", &m_trk_bendchi2);
    eventTree->Branch("trk_MVA1", &m_trk_MVA1);
    eventTree->Branch("trk_MVA2", &m_trk_MVA2);
    eventTree->Branch("trk_nstub", &m_trk_nstub);
    eventTree->Branch("trk_lhits", &m_trk_lhits);
    eventTree->Branch("trk_dhits", &m_trk_dhits);
    eventTree->Branch("trk_seed", &m_trk_seed);
    eventTree->Branch("trk_hitpattern", &m_trk_hitpattern);
    eventTree->Branch("trk_phiSector", &m_trk_phiSector);
    eventTree->Branch("trk_genuine", &m_trk_genuine);
    eventTree->Branch("trk_loose", &m_trk_loose);
    eventTree->Branch("trk_unknown", &m_trk_unknown);
    eventTree->Branch("trk_combinatoric", &m_trk_combinatoric);
    eventTree->Branch("trk_fake", &m_trk_fake);
    eventTree->Branch("trk_matchtp_pdgid", &m_trk_matchtp_pdgid);
    eventTree->Branch("trk_matchtp_pt", &m_trk_matchtp_pt);
    eventTree->Branch("trk_matchtp_eta", &m_trk_matchtp_eta);
    eventTree->Branch("trk_matchtp_phi", &m_trk_matchtp_phi);
    eventTree->Branch("trk_matchtp_z0", &m_trk_matchtp_z0);
    eventTree->Branch("trk_matchtp_dxy", &m_trk_matchtp_dxy);
    eventTree->Branch("trk_gtt_pt", &m_trk_gtt_pt);
    eventTree->Branch("trk_gtt_eta", &m_trk_gtt_eta);
    eventTree->Branch("trk_gtt_phi", &m_trk_gtt_phi);

    eventTree->Branch("trk_word_valid",&m_trk_word_valid);
    eventTree->Branch("trk_word_InvR",&m_trk_word_InvR);
    eventTree->Branch("trk_word_pT",&m_trk_word_pT);
    eventTree->Branch("trk_word_Phi",&m_trk_word_Phi);
    eventTree->Branch("trk_word_TanL",&m_trk_word_TanL);
    eventTree->Branch("trk_word_eta",&m_trk_word_eta);
    eventTree->Branch("trk_word_Z0",&m_trk_word_Z0);
    eventTree->Branch("trk_word_D0",&m_trk_word_D0);
    eventTree->Branch("trk_word_chi2rphi",&m_trk_word_chi2rphi);
    eventTree->Branch("trk_word_chi2rz",&m_trk_word_chi2rz);
    eventTree->Branch("trk_word_bendchi2",&m_trk_word_bendchi2);
    eventTree->Branch("trk_word_hitpattern",&m_trk_word_hitpattern);
    eventTree->Branch("trk_word_MVAquality",&m_trk_word_MVAquality);
    eventTree->Branch("trk_word_MVAother",&m_trk_word_MVAother);
  }

  eventTree->Branch("tp_pt", &m_tp_pt);
  eventTree->Branch("tp_eta", &m_tp_eta);
  eventTree->Branch("tp_phi", &m_tp_phi);
  eventTree->Branch("tp_dxy", &m_tp_dxy);
  eventTree->Branch("tp_d0", &m_tp_d0);
  eventTree->Branch("tp_z0", &m_tp_z0);
  eventTree->Branch("tp_d0_prod", &m_tp_d0_prod);
  eventTree->Branch("tp_z0_prod", &m_tp_z0_prod);
  eventTree->Branch("tp_pdgid", &m_tp_pdgid);
  eventTree->Branch("tp_nmatch", &m_tp_nmatch);
  eventTree->Branch("tp_nstub", &m_tp_nstub);
  eventTree->Branch("tp_eventid", &m_tp_eventid);
  eventTree->Branch("tp_charge", &m_tp_charge);

  eventTree->Branch("pv_MC", &m_pv_MC);

}

//////////
// ANALYZE
void L1TrackReducedMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  if (not available_)
    return;  // No ROOT file open.

  if (!(MyProcess == 13 || MyProcess == 11 || MyProcess == 211 || MyProcess == 6 || MyProcess == 15 ||
        MyProcess == 1)) {
    edm::LogVerbatim("Tracklet") << "The specified MyProcess is invalid! Exiting...";
    return;
  }

  // clear variables
  if (SaveAllTracks) {
    m_trk_pt->clear();
    m_trk_eta->clear();
    m_trk_phi->clear();
    m_trk_phi_local->clear();
    m_trk_d0->clear();
    m_trk_z0->clear();
    m_trk_chi2->clear();
    m_trk_chi2dof->clear();
    m_trk_chi2rphi->clear();
    m_trk_chi2rz->clear();
    m_trk_bendchi2->clear();
    m_trk_MVA1->clear();
    m_trk_MVA2->clear();
    m_trk_nstub->clear();
    m_trk_lhits->clear();
    m_trk_dhits->clear();
    m_trk_seed->clear();
    m_trk_hitpattern->clear();
    m_trk_phiSector->clear();
    m_trk_genuine->clear();
    m_trk_loose->clear();
    m_trk_unknown->clear();
    m_trk_combinatoric->clear();
    m_trk_fake->clear();
    m_trk_matchtp_pdgid->clear();
    m_trk_matchtp_pt->clear();
    m_trk_matchtp_eta->clear();
    m_trk_matchtp_phi->clear();
    m_trk_matchtp_z0->clear();
    m_trk_matchtp_dxy->clear();
    m_trk_gtt_pt->clear();
    m_trk_gtt_eta->clear();
    m_trk_gtt_phi->clear();

    m_trk_word_valid->clear();
    m_trk_word_InvR->clear();
    m_trk_word_pT->clear();
    m_trk_word_Phi->clear();
    m_trk_word_TanL->clear();
    m_trk_word_eta->clear();
    m_trk_word_Z0->clear();
    m_trk_word_D0->clear();
    m_trk_word_chi2rphi->clear();
    m_trk_word_chi2rz->clear();
    m_trk_word_bendchi2->clear();
    m_trk_word_hitpattern->clear();
    m_trk_word_MVAquality->clear();
    m_trk_word_MVAother->clear();
  }

  m_tp_pt->clear();
  m_tp_eta->clear();
  m_tp_phi->clear();
  m_tp_dxy->clear();
  m_tp_d0->clear();
  m_tp_z0->clear();
  m_tp_d0_prod->clear();
  m_tp_z0_prod->clear();
  m_tp_pdgid->clear();
  m_tp_nmatch->clear();
  m_tp_nstub->clear();
  m_tp_eventid->clear();
  m_tp_charge->clear();

  m_pv_MC->clear();

    // -----------------------------------------------------------------------------------------------
    // retrieve various containers
    // -----------------------------------------------------------------------------------------------

  // L1 stubs
  edm::Handle<edmNew::DetSetVector<TTStub<Ref_Phase2TrackerDigi_> > > TTStubHandle;
  if (SaveStubs)
    iEvent.getByToken(ttStubToken_, TTStubHandle);

  // MC truth association maps
  edm::Handle<TTClusterAssociationMap<Ref_Phase2TrackerDigi_> > MCTruthTTClusterHandle;
  iEvent.getByToken(ttClusterMCTruthToken_, MCTruthTTClusterHandle);
  edm::Handle<TTStubAssociationMap<Ref_Phase2TrackerDigi_> > MCTruthTTStubHandle;
  iEvent.getByToken(ttStubMCTruthToken_, MCTruthTTStubHandle);

  // tracking particles
  edm::Handle<std::vector<TrackingParticle> > TrackingParticleHandle;
  iEvent.getByToken(TrackingParticleToken_, TrackingParticleHandle);

  // -----------------------------------------------------------------------------------------------
  // more for TTStubs
  const TrackerTopology& tTopo = iSetup.getData(tTopoToken_);
  const TrackerGeometry& tGeom = iSetup.getData(tGeomToken_);

  edm::Handle<HepMCProduct> GenVertexHandle;
  iEvent.getByToken(GenVertexToken_, GenVertexHandle);

  // L1 tracks
  edm::Handle<L1TrackCollection> TTTrackHandle;
  edm::Handle<std::vector<TTTrack<Ref_Phase2TrackerDigi_> > > GTTTTTrackHandle;
  edm::Handle<TTTrackAssociationMap<Ref_Phase2TrackerDigi_> > MCTruthTTTrackHandle;
 
  std::vector<TTTrack<Ref_Phase2TrackerDigi_> >::const_iterator iterL1Track;

  iEvent.getByToken(ttTrackToken_, TTTrackHandle);
  iEvent.getByToken(GTTttTrackToken_, GTTTTTrackHandle);
  iEvent.getByToken(ttTrackMCTruthToken_, MCTruthTTTrackHandle);
  
  if (GenVertexHandle.isValid()) {
    const HepMC::GenEvent* MCEvt = GenVertexHandle->GetEvent();
    for (HepMC::GenEvent::vertex_const_iterator ivertex =
             MCEvt->vertices_begin();
         ivertex != MCEvt->vertices_end(); ++ivertex) {
      bool hasParentVertex = false;
      // Loop over the parents looking to see if they are coming from a
      // production vertex
      for (HepMC::GenVertex::particle_iterator iparent =
               (*ivertex)->particles_begin(HepMC::parents);
           iparent != (*ivertex)->particles_end(HepMC::parents); ++iparent)
        if ((*iparent)->production_vertex()) {
          hasParentVertex = true;
          break;
        }

          // Reject those vertices with parent vertices
    if (hasParentVertex) continue;
      // Get the position of the vertex
      HepMC::FourVector pos = (*ivertex)->position();
      const double mm = 0.1;  // [mm] --> [cm]
      m_pv_MC->push_back(pos.z() * mm);
      break;  // there should be one single primary vertex
    }         // end loop over gen vertices
  }


  // ----------------------------------------------------------------------------------------------
  // loop over (prompt) L1 tracks
  // ----------------------------------------------------------------------------------------------
  if (SaveAllTracks) {
    if (DebugMode) {
      edm::LogVerbatim("Tracklet") << "\n Loop over L1 tracks!";
    }

    int this_l1track = 0;
    for (iterL1Track = TTTrackHandle->begin(); iterL1Track != TTTrackHandle->end(); iterL1Track++) {
      L1TrackPtr l1track_ptr(TTTrackHandle, this_l1track);
      L1TrackRef l1track_ref(GTTTTTrackHandle, this_l1track);

      this_l1track++;

      float tmp_trk_pt = iterL1Track->momentum().perp();
      float tmp_trk_eta = iterL1Track->momentum().eta();
      float tmp_trk_phi = iterL1Track->momentum().phi();
      float tmp_trk_phi_local = iterL1Track->localPhi();
      float tmp_trk_z0 = iterL1Track->z0();            //cm
      int tmp_trk_nFitPars = iterL1Track->nFitPars();  //4 or 5

      float tmp_trk_d0 = -999;
      if (tmp_trk_nFitPars == 5) {
        float tmp_trk_x0 = iterL1Track->POCA().x();
        float tmp_trk_y0 = iterL1Track->POCA().y();
        tmp_trk_d0 = -tmp_trk_x0 * sin(tmp_trk_phi) + tmp_trk_y0 * cos(tmp_trk_phi);
        // tmp_trk_d0 = iterL1Track->d0();
      }

      float tmp_trk_chi2 = iterL1Track->chi2();
      float tmp_trk_chi2dof = iterL1Track->chi2Red();
      float tmp_trk_chi2rphi = iterL1Track->chi2XY();
      float tmp_trk_chi2rz = iterL1Track->chi2Z();
      float tmp_trk_bendchi2 = iterL1Track->stubPtConsistency();
      float tmp_trk_MVA1 = iterL1Track->trkMVA1();
      float tmp_trk_MVA2 = iterL1Track->trkMVA2();

      std::vector<edm::Ref<edmNew::DetSetVector<TTStub<Ref_Phase2TrackerDigi_> >, TTStub<Ref_Phase2TrackerDigi_> > >
          stubRefs = iterL1Track->getStubRefs();
      int tmp_trk_nstub = (int)stubRefs.size();
      int tmp_trk_seed = 0;
      tmp_trk_seed = (int)iterL1Track->trackSeedType();
      int tmp_trk_hitpattern = 0;
      tmp_trk_hitpattern = (int)iterL1Track->hitPattern();
      unsigned int tmp_trk_phiSector = iterL1Track->phiSector();

      // ----------------------------------------------------------------------------------------------
      // loop over stubs on tracks
      int tmp_trk_dhits = 0;
      int tmp_trk_lhits = 0;
      if (true) {
        // loop over stubs
        for (int is = 0; is < tmp_trk_nstub; is++) {
          //detID of stub
          DetId detIdStub = tGeom.idToDet((stubRefs.at(is)->clusterRef(0))->getDetId())->geographicalId();
          MeasurementPoint coords = stubRefs.at(is)->clusterRef(0)->findAverageLocalCoordinatesCentered();
          const GeomDet* theGeomDet = tGeom.idToDet(detIdStub);
          Global3DPoint posStub = theGeomDet->surface().toGlobal(theGeomDet->topology().localPosition(coords));

          double x = posStub.x();
          double y = posStub.y();
          double z = posStub.z();

          int layer = -999999;
          if (detIdStub.subdetId() == StripSubdetector::TOB) {
            layer = static_cast<int>(tTopo.layer(detIdStub));
            if (DebugMode)
              edm::LogVerbatim("Tracklet")
                  << "   stub in layer " << layer << " at position x y z = " << x << " " << y << " " << z;
            tmp_trk_lhits += pow(10, layer - 1);
          } else if (detIdStub.subdetId() == StripSubdetector::TID) {
            layer = static_cast<int>(tTopo.layer(detIdStub));
            if (DebugMode)
              edm::LogVerbatim("Tracklet")
                  << "   stub in disk " << layer << " at position x y z = " << x << " " << y << " " << z;
            tmp_trk_dhits += pow(10, layer - 1);
          }
        }  //end loop over stubs
      }
      // ----------------------------------------------------------------------------------------------

      int tmp_trk_genuine = 0;
      int tmp_trk_loose = 0;
      int tmp_trk_unknown = 0;
      int tmp_trk_combinatoric = 0;
      if (MCTruthTTTrackHandle->isLooselyGenuine(l1track_ptr))
        tmp_trk_loose = 1;
      if (MCTruthTTTrackHandle->isGenuine(l1track_ptr))
        tmp_trk_genuine = 1;
      if (MCTruthTTTrackHandle->isUnknown(l1track_ptr))
        tmp_trk_unknown = 1;
      if (MCTruthTTTrackHandle->isCombinatoric(l1track_ptr))
        tmp_trk_combinatoric = 1;

      if (DebugMode) {
        edm::LogVerbatim("Tracklet") << "L1 track,"
                                     << " pt: " << tmp_trk_pt << " eta: " << tmp_trk_eta << " phi: " << tmp_trk_phi
                                     << " z0: " << tmp_trk_z0 << " chi2: " << tmp_trk_chi2
                                     << " chi2rphi: " << tmp_trk_chi2rphi << " chi2rz: " << tmp_trk_chi2rz
                                     << " nstub: " << tmp_trk_nstub;
        if (tmp_trk_genuine)
          edm::LogVerbatim("Tracklet") << "    (is genuine)";
        if (tmp_trk_unknown)
          edm::LogVerbatim("Tracklet") << "    (is unknown)";
        if (tmp_trk_combinatoric)
          edm::LogVerbatim("Tracklet") << "    (is combinatoric)";
      }

      m_trk_pt->push_back(tmp_trk_pt);
      m_trk_eta->push_back(tmp_trk_eta);
      m_trk_phi->push_back(tmp_trk_phi);
      m_trk_phi_local->push_back(tmp_trk_phi_local);
      m_trk_z0->push_back(tmp_trk_z0);
      if (tmp_trk_nFitPars == 5)
        m_trk_d0->push_back(tmp_trk_d0);
      else
        m_trk_d0->push_back(999.);
      m_trk_chi2->push_back(tmp_trk_chi2);
      m_trk_chi2dof->push_back(tmp_trk_chi2dof);
      m_trk_chi2rphi->push_back(tmp_trk_chi2rphi);
      m_trk_chi2rz->push_back(tmp_trk_chi2rz);
      m_trk_bendchi2->push_back(tmp_trk_bendchi2);
      m_trk_MVA1->push_back(tmp_trk_MVA1);
      m_trk_MVA2->push_back(tmp_trk_MVA2);
      m_trk_nstub->push_back(tmp_trk_nstub);
      m_trk_dhits->push_back(tmp_trk_dhits);
      m_trk_lhits->push_back(tmp_trk_lhits);
      m_trk_seed->push_back(tmp_trk_seed);
      m_trk_hitpattern->push_back(tmp_trk_hitpattern);
      m_trk_phiSector->push_back(tmp_trk_phiSector);
      m_trk_genuine->push_back(tmp_trk_genuine);
      m_trk_loose->push_back(tmp_trk_loose);
      m_trk_unknown->push_back(tmp_trk_unknown);
      m_trk_combinatoric->push_back(tmp_trk_combinatoric);

      unsigned int tmp_InvR = l1track_ptr->getRinvBits();
      unsigned int tmp_TanL = l1track_ptr->getTanlBits();
      m_trk_word_InvR->push_back(tmp_InvR);
      m_trk_word_TanL->push_back(tmp_TanL);
      // ----------------------------------------------------------------------------------------------
      // for studying the fake rate
      // ----------------------------------------------------------------------------------------------
      edm::Ptr<TrackingParticle> my_tp = MCTruthTTTrackHandle->findTrackingParticlePtr(l1track_ptr);

      int myFake = 0;
      int myTP_pdgid = -999;
      float myTP_pt = -999;
      float myTP_eta = -999;
      float myTP_phi = -999;
      float myTP_z0 = -999;
      float myTP_dxy = -999;

      if (my_tp.isNull())
        myFake = 0;
      else {
        int tmp_eventid = my_tp->eventId().event();
        if (tmp_eventid > 0)
          myFake = 2;
        else
          myFake = 1;

        myTP_pdgid = my_tp->pdgId();
        myTP_pt = my_tp->p4().pt();
        myTP_eta = my_tp->p4().eta();
        myTP_phi = my_tp->p4().phi();
        myTP_z0 = my_tp->vertex().z();

        float myTP_x0 = my_tp->vertex().x();
        float myTP_y0 = my_tp->vertex().y();
        myTP_dxy = sqrt(myTP_x0 * myTP_x0 + myTP_y0 * myTP_y0);

        if (DebugMode) {
          edm::LogVerbatim("Tracklet") << "TP matched to track has pt = " << my_tp->p4().pt()
                                       << " eta = " << my_tp->momentum().eta() << " phi = " << my_tp->momentum().phi()
                                       << " z0 = " << my_tp->vertex().z() << " pdgid = " << my_tp->pdgId()
                                       << " dxy = " << myTP_dxy;
        }
      }

      m_trk_fake->push_back(myFake);
      m_trk_matchtp_pdgid->push_back(myTP_pdgid);
      m_trk_matchtp_pt->push_back(myTP_pt);
      m_trk_matchtp_eta->push_back(myTP_eta);
      m_trk_matchtp_phi->push_back(myTP_phi);
      m_trk_matchtp_z0->push_back(myTP_z0);
      m_trk_matchtp_dxy->push_back(myTP_dxy);

      m_trk_gtt_pt->push_back(l1track_ref->momentum().perp());
      m_trk_gtt_eta->push_back(l1track_ref->momentum().eta());
      m_trk_gtt_phi->push_back(l1track_ref->momentum().phi());
    }  //end track loop

  int this_l1GTTtrack = 0;
  for (iterL1Track = GTTTTTrackHandle->begin(); iterL1Track != GTTTTTrackHandle->end(); iterL1Track++) { //GTT Track Loop
    edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_> > l1track_ptr(GTTTTTrackHandle, this_l1GTTtrack);
    this_l1GTTtrack++;

    unsigned int tmp_valid = l1track_ptr->getValidBits();
    unsigned int tmp_InvR = l1track_ptr->getRinvBits();
    unsigned int tmp_Phi = l1track_ptr->getPhiBits();
    unsigned int tmp_TanL = l1track_ptr->getTanlBits();
    unsigned int tmp_Z0 = l1track_ptr->getZ0Bits();
    unsigned int tmp_D0 = l1track_ptr->getD0Bits();
    unsigned int tmp_chi2rphi = l1track_ptr->getChi2RPhiBits();
    unsigned int tmp_chi2rz = l1track_ptr->getChi2RZBits();
    unsigned int tmp_bendchi2 = l1track_ptr->getBendChi2Bits();
    unsigned int tmp_hitpattern = l1track_ptr->getHitPatternBits();
    unsigned int tmp_MVAquality = l1track_ptr->getMVAQualityBits();
    unsigned int tmp_MVAother = l1track_ptr->getMVAOtherBits();

    m_trk_word_valid->push_back(tmp_valid);
    m_trk_word_pT->push_back(tmp_InvR);
    m_trk_word_Phi->push_back(tmp_Phi);
    m_trk_word_eta->push_back(tmp_TanL);
    m_trk_word_Z0->push_back(tmp_Z0);
    m_trk_word_D0->push_back(tmp_D0);
    m_trk_word_chi2rphi->push_back(tmp_chi2rphi);
    m_trk_word_chi2rz->push_back(tmp_chi2rz);
    m_trk_word_bendchi2->push_back(tmp_bendchi2);
    m_trk_word_hitpattern->push_back(tmp_hitpattern);
    m_trk_word_MVAquality->push_back(tmp_MVAquality);
    m_trk_word_MVAother->push_back(tmp_MVAother);

  } //end GTT Track loop

  }    //end if SaveAllTracks

  // ----------------------------------------------------------------------------------------------
  // loop over tracking particles
  // ----------------------------------------------------------------------------------------------
  if (DebugMode)
    edm::LogVerbatim("Tracklet") << "\n Loop over tracking particles!";

  int this_tp = 0;
  std::vector<TrackingParticle>::const_iterator iterTP;
  for (iterTP = TrackingParticleHandle->begin(); iterTP != TrackingParticleHandle->end(); ++iterTP) {
    edm::Ptr<TrackingParticle> tp_ptr(TrackingParticleHandle, this_tp);
    this_tp++;

    int tmp_eventid = iterTP->eventId().event();
    if (MyProcess != 1 && tmp_eventid > 0)
      continue;  //only care about primary interaction

    float tmp_tp_pt = iterTP->pt();
    float tmp_tp_eta = iterTP->eta();
    float tmp_tp_phi = iterTP->phi();
    float tmp_tp_vz = iterTP->vz();
    float tmp_tp_vx = iterTP->vx();
    float tmp_tp_vy = iterTP->vy();
    int tmp_tp_pdgid = iterTP->pdgId();
    float tmp_tp_z0_prod = tmp_tp_vz;
    float tmp_tp_d0_prod = tmp_tp_vx * sin(tmp_tp_phi) - tmp_tp_vy * cos(tmp_tp_phi);

    if (MyProcess == 13 && abs(tmp_tp_pdgid) != 13)
      continue;
    if (MyProcess == 11 && abs(tmp_tp_pdgid) != 11)
      continue;
    if ((MyProcess == 6 || MyProcess == 15 || MyProcess == 211) && abs(tmp_tp_pdgid) != 211)
      continue;

    if (tmp_tp_pt < TP_minPt)
      continue;
    if (std::abs(tmp_tp_eta) > TP_maxEta)
      continue;

    // ----------------------------------------------------------------------------------------------
    // get d0/z0 propagated back to the IP

    float tmp_tp_t = tan(2.0 * atan(1.0) - 2.0 * atan(exp(-tmp_tp_eta)));
    float delx = -tmp_tp_vx;
    float dely = -tmp_tp_vy;

    float A = 0.01 * 0.5696;
    float Kmagnitude = A / tmp_tp_pt;
    float tmp_tp_charge = tp_ptr->charge();
    float K = Kmagnitude * tmp_tp_charge;
    float d = 0;
    float tmp_tp_x0p = delx - (d + 1. / (2. * K) * sin(tmp_tp_phi));
    float tmp_tp_y0p = dely + (d + 1. / (2. * K) * cos(tmp_tp_phi));
    float tmp_tp_rp = sqrt(tmp_tp_x0p * tmp_tp_x0p + tmp_tp_y0p * tmp_tp_y0p);
    float tmp_tp_d0 = tmp_tp_charge * tmp_tp_rp - (1. / (2. * K));
    tmp_tp_d0 = tmp_tp_d0 * (-1);  //fix d0 sign
    static double pi = 4.0 * atan(1.0);
    float delphi = tmp_tp_phi - atan2(-K * tmp_tp_x0p, K * tmp_tp_y0p);
    if (delphi < -pi)
      delphi += 2.0 * pi;
    if (delphi > pi)
      delphi -= 2.0 * pi;
    float tmp_tp_z0 = tmp_tp_vz + tmp_tp_t * delphi / (2.0 * K);
    // ----------------------------------------------------------------------------------------------

    if (std::abs(tmp_tp_z0) > TP_maxZ0)
      continue;

    // for pions in ttbar, only consider TPs coming from near the IP!
    float dxy = sqrt(tmp_tp_vx * tmp_tp_vx + tmp_tp_vy * tmp_tp_vy);
    float tmp_tp_dxy = dxy;
    if (MyProcess == 6 && (dxy > 1.0))
      continue;

    if (DebugMode )
      edm::LogVerbatim("Tracklet") << "Tracking particle, pt: " << tmp_tp_pt << " eta: " << tmp_tp_eta
                                   << " phi: " << tmp_tp_phi << " z0: " << tmp_tp_z0 << " d0: " << tmp_tp_d0
                                   << " z_prod: " << tmp_tp_z0_prod << " d_prod: " << tmp_tp_d0_prod
                                   << " pdgid: " << tmp_tp_pdgid << " eventID: " << iterTP->eventId().event()
                                   << " ttclusters " << MCTruthTTClusterHandle->findTTClusterRefs(tp_ptr).size()
                                   << " ttstubs " << MCTruthTTStubHandle->findTTStubRefs(tp_ptr).size() << " tttracks "
                                   << MCTruthTTTrackHandle->findTTTrackPtrs(tp_ptr).size();

    // ----------------------------------------------------------------------------------------------
    // only consider TPs associated with >= 1 cluster, or >= X stubs, or have stubs in >= X layers (configurable options)
    if (MCTruthTTClusterHandle->findTTClusterRefs(tp_ptr).empty()) {
      if (DebugMode)
        edm::LogVerbatim("Tracklet") << "No matching TTClusters for TP, continuing...";
      continue;
    }

    std::vector<edm::Ref<edmNew::DetSetVector<TTStub<Ref_Phase2TrackerDigi_> >, TTStub<Ref_Phase2TrackerDigi_> > >
        theStubRefs = MCTruthTTStubHandle->findTTStubRefs(tp_ptr);
    int nStubTP = (int)theStubRefs.size();

    // how many layers/disks have stubs?
    int hasStubInLayer[11] = {0};
    for (auto& theStubRef : theStubRefs) {
      DetId detid(theStubRef->getDetId());

      int layer = -1;
      if (detid.subdetId() == StripSubdetector::TOB) {
        layer = static_cast<int>(tTopo.layer(detid)) - 1;  //fill in array as entries 0-5
      } else if (detid.subdetId() == StripSubdetector::TID) {
        layer = static_cast<int>(tTopo.layer(detid)) + 5;  //fill in array as entries 6-10
      }

      //treat genuine stubs separately (==2 is genuine, ==1 is not)
      if (MCTruthTTStubHandle->findTrackingParticlePtr(theStubRef).isNull() && hasStubInLayer[layer] < 2)
        hasStubInLayer[layer] = 1;
      else
        hasStubInLayer[layer] = 2;
    }

    int nStubLayerTP = 0;
    int nStubLayerTP_g = 0;
    for (int isum : hasStubInLayer) {
      if (isum >= 1)
        nStubLayerTP += 1;
      if (isum == 2)
        nStubLayerTP_g += 1;
    }

    if (DebugMode)
      edm::LogVerbatim("Tracklet") << "TP is associated with " << nStubTP << " stubs, and has stubs in " << nStubLayerTP
                                   << " different layers/disks, and has GENUINE stubs in " << nStubLayerTP_g
                                   << " layers ";

    if (TP_minNStub > 0) {
      if (DebugMode)
        edm::LogVerbatim("Tracklet") << "Only consider TPs with >= " << TP_minNStub << " stubs";
      if (nStubTP < TP_minNStub) {
        if (DebugMode)
          edm::LogVerbatim("Tracklet") << "TP fails minimum nbr stubs requirement! Continuing...";
        continue;
      }
    }
    if (TP_minNStubLayer > 0) {
      if (DebugMode)
        edm::LogVerbatim("Tracklet") << "Only consider TPs with stubs in >= " << TP_minNStubLayer << " layers/disks";
      if (nStubLayerTP < TP_minNStubLayer) {
        if (DebugMode)
          edm::LogVerbatim("Tracklet") << "TP fails stubs in minimum nbr of layers/disks requirement! Continuing...";
        continue;
      }
    }

    m_tp_pt->push_back(tmp_tp_pt);
    m_tp_eta->push_back(tmp_tp_eta);
    m_tp_phi->push_back(tmp_tp_phi);
    m_tp_dxy->push_back(tmp_tp_dxy);
    m_tp_z0->push_back(tmp_tp_z0);
    m_tp_d0->push_back(tmp_tp_d0);
    m_tp_z0_prod->push_back(tmp_tp_z0_prod);
    m_tp_d0_prod->push_back(tmp_tp_d0_prod);
    m_tp_pdgid->push_back(tmp_tp_pdgid);
    m_tp_nstub->push_back(nStubTP);
    m_tp_eventid->push_back(tmp_eventid);
    m_tp_charge->push_back(tmp_tp_charge);

    // ----------------------------------------------------------------------------------------------
  }  //end loop tracking particles

  eventTree->Fill();
}  // end of analyze()

///////////////////////////
// DEFINE THIS AS A PLUG-IN
DEFINE_FWK_MODULE(L1TrackReducedMaker);



