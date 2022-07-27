#include "L1Trigger/VertexFinder/interface/VertexProducer.h"
#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
#include "DataFormats/L1Trigger/interface/Vertex.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "L1Trigger/VertexFinder/interface/AlgoSettings.h"
#include "L1Trigger/VertexFinder/interface/RecoVertex.h"
#include "L1Trigger/VertexFinder/interface/VertexFinder.h"

using namespace l1tVertexFinder;
using namespace std;

VertexProducer::VertexProducer(const edm::ParameterSet& iConfig)
    : l1TracksToken_(consumes<TTTrackCollectionView>(iConfig.getParameter<edm::InputTag>("l1TracksInputTag"))),
      tTopoToken(esConsumes<TrackerTopology, TrackerTopologyRcd>()),
      outputCollectionName_(iConfig.getParameter<std::string>("l1VertexCollectionName")),
      ttTrackMCTruthToken_(consumes<TTTrackAssociationMap<Ref_Phase2TrackerDigi_> >(iConfig.getParameter<edm::InputTag>("mcTruthTrackInputTag"))),
      settings_(AlgoSettings(iConfig)) {
  // Get configuration parameters

  switch (settings_.vx_algo()) {
    case Algorithm::fastHisto:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using the fastHisto binning algorithm";
      break;
    case Algorithm::fastHistoEmulation:
      edm::LogInfo("VertexProducer")
          << "VertexProducer::Finding vertices using the emulation version of the fastHisto binning algorithm";
      break;
    case Algorithm::fastHistoLooseAssociation:
      edm::LogInfo("VertexProducer")
          << "VertexProducer::Finding vertices using the fastHistoLooseAssociation binning algorithm";
      break;
    case Algorithm::GapClustering:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using a gap clustering algorithm";
      break;
    case Algorithm::agglomerativeHierarchical:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using a Simple Merge Clustering algorithm";
      break;
    case Algorithm::DBSCAN:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using a DBSCAN algorithm";
      break;
    case Algorithm::PVR:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using a PVR algorithm";
      break;
    case Algorithm::adaptiveVertexReconstruction:
      edm::LogInfo("VertexProducer")
          << "VertexProducer::Finding vertices using an AdaptiveVertexReconstruction algorithm";
      break;
    case Algorithm::HPV:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using a Highest Pt Vertex algorithm";
      break;
    case Algorithm::Kmeans:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using a kmeans algorithm";
      break;
    case Algorithm::Generator:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using ** GENERATOR ** vertex (average of TP z0s)";
      break;
    case Algorithm::NN:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using the Neural Network algorithm";
      break;
    case Algorithm::NNEmulation:
      edm::LogInfo("VertexProducer") << "VertexProducer::Finding vertices using the Neural Network Emulation";
      break;
  }

  //--- Define EDM output to be written to file (if required)
  if ((settings_.vx_algo() == Algorithm::fastHistoEmulation ) | (settings_.vx_algo() == Algorithm::NNEmulation ))  {
    produces<l1t::VertexWordCollection>(outputCollectionName_ + "Emulation");
  } else {
    produces<l1t::VertexCollection>(outputCollectionName_);
  }

    if (settings_.vx_algo() == Algorithm::NN) {
    // load graphs, create a new session and add the graphDef
    std::cout << "loading cnn trk weight graph from " << settings_.vx_cnn_trkw_graph() << std::endl;
    cnnTrkGraph_ = tensorflow::loadGraphDef(settings_.vx_cnn_trkw_graph());
    cnnTrkSesh_ = tensorflow::createSession(cnnTrkGraph_);

    std::cout << "loading cnn pv z0 graph from " << settings_.vx_cnn_pvz0_graph() << std::endl;
    cnnPVZ0Graph_ = tensorflow::loadGraphDef(settings_.vx_cnn_pvz0_graph());
    cnnPVZ0Sesh_ = tensorflow::createSession(cnnPVZ0Graph_);

    std::cout << "loading cnn association graph from " << settings_.vx_cnn_graph() << std::endl;
    cnnAssGraph_ = tensorflow::loadGraphDef(settings_.vx_cnn_graph());
    cnnAssSesh_ = tensorflow::createSession(cnnAssGraph_);
  }

    if (settings_.vx_algo() == Algorithm::NNEmulation) {
    // load graphs, create a new session and add the graphDef
    std::cout << "loading cnn trk weight graph from " << settings_.vx_cnn_trkw_graph() << std::endl;
    cnnTrkGraph_ = tensorflow::loadGraphDef(settings_.vx_cnn_trkw_graph());
    cnnTrkSesh_ = tensorflow::createSession(cnnTrkGraph_);

    std::cout << "loading cnn pv z0 graph from " << settings_.vx_cnn_pvz0_graph() << std::endl;
    cnnPVZ0Graph_ = tensorflow::loadGraphDef(settings_.vx_cnn_pvz0_graph());
    cnnPVZ0Sesh_ = tensorflow::createSession(cnnPVZ0Graph_);

    std::cout << "loading cnn association graph from " << settings_.vx_cnn_graph() << std::endl;
    cnnAssGraph_ = tensorflow::loadGraphDef(settings_.vx_cnn_graph());
    cnnAssSesh_ = tensorflow::createSession(cnnAssGraph_);
  }
}

void VertexProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  edm::Handle<TTTrackCollectionView> l1TracksHandle;
  iEvent.getByToken(l1TracksToken_, l1TracksHandle);

  std::vector<l1tVertexFinder::L1Track> l1Tracks;
  l1Tracks.reserve(l1TracksHandle->size());
  if (settings_.debug() > 1) {
    edm::LogInfo("VertexProducer") << "produce::Processing " << l1TracksHandle->size() << " tracks";
  }
  for (const auto& track : l1TracksHandle->ptrs()) {
    auto l1track = L1Track(track);
    // Check the minimum pT of the tracks
    // This is left here because it represents the smallest pT to be sent by the track finding boards
    // This has less to do with the algorithms than the constraints of what will be sent to the vertexing algorithm
    if (l1track.pt() >= settings_.vx_TrackMinPt()) {
      l1Tracks.push_back(l1track);
    } else {
      if (settings_.debug() > 2) {
        edm::LogInfo("VertexProducer") << "produce::Removing track with too low of a pt (" << l1track.pt() << ")\n"
                                       << "         word = " << l1track.getTTTrackPtr()->getTrackWord().to_string(2);
      }
    }
  }
  if (settings_.debug() > 1) {
    edm::LogInfo("VertexProducer") << "produce::Processing " << l1Tracks.size() << " tracks after minimum pt cut of"
                                   << settings_.vx_TrackMinPt() << " GeV";
  }

  VertexFinder vf(l1Tracks, settings_);

  switch (settings_.vx_algo()) {
    case Algorithm::fastHisto: {
      const TrackerTopology& tTopo = iSetup.getData(tTopoToken);
      vf.fastHisto(&tTopo);
      break;
    }
    case Algorithm::fastHistoEmulation:
      vf.fastHistoEmulation();
      break;
    case Algorithm::fastHistoLooseAssociation:
      vf.fastHistoLooseAssociation();
      break;
    case Algorithm::GapClustering:
      vf.GapClustering();
      break;
    case Algorithm::agglomerativeHierarchical:
      vf.agglomerativeHierarchicalClustering();
      break;
    case Algorithm::DBSCAN:
      vf.DBSCAN();
      break;
    case Algorithm::PVR:
      vf.PVR();
      break;
    case Algorithm::adaptiveVertexReconstruction:
      vf.adaptiveVertexReconstruction();
      break;
    case Algorithm::HPV:
      vf.HPV();
      break;
    case Algorithm::Kmeans:
      vf.Kmeans();
      break;
    case Algorithm::Generator: {
      std::vector<const L1Track*> pvTracks;
      for (const auto& track : l1Tracks) {
        edm::Handle<TTTrackAssociationMap<Ref_Phase2TrackerDigi_> > MCTruthTTTrackHandle;
        iEvent.getByToken(ttTrackMCTruthToken_, MCTruthTTTrackHandle);
        edm::Ptr<TrackingParticle> tpMatch = MCTruthTTTrackHandle->findTrackingParticlePtr(track.getTTTrackPtr());
        if (tpMatch.isNull())
          continue;
        if (tpMatch->eventId().event() == 0)
          pvTracks.push_back(&track);
      }
      vf.Generator(pvTracks);
      break;
    }
    case Algorithm::NN:
      vf.CNNPVZ0Algorithm(cnnTrkSesh_, cnnPVZ0Sesh_, cnnAssSesh_);
      break;

    case Algorithm::NNEmulation:
      vf.CNNPVZ0Emulation(cnnTrkSesh_, cnnPVZ0Sesh_, cnnAssSesh_);
      break;
  }

  vf.sortVerticesInPt();
  vf.findPrimaryVertex();

  // //=== Store output EDM track and hardware stub collections.
  if ((settings_.vx_algo() == Algorithm::fastHistoEmulation) | (settings_.vx_algo() == Algorithm::NNEmulation)) {
    std::unique_ptr<l1t::VertexWordCollection> product_emulation =
        std::make_unique<l1t::VertexWordCollection>(vf.verticesEmulation().begin(), vf.verticesEmulation().end());
    iEvent.put(std::move(product_emulation), outputCollectionName_ + "Emulation");
  } else {
    std::unique_ptr<l1t::VertexCollection> product(new std::vector<l1t::Vertex>());
    for (const auto& vtx : vf.vertices()) {
      product->emplace_back(vtx.vertex());
    }
    iEvent.put(std::move(product), outputCollectionName_);
  }
}
DEFINE_FWK_MODULE(VertexProducer);
