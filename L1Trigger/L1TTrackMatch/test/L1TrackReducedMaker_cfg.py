############################################################
# define basic process
############################################################

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import FWCore.ParameterSet.VarParsing as VarParsing

import os

############################################################
# edit options here
############################################################
L1TRK_INST ="ReducedNtupleMaker" ### if not in input DIGRAW then we make them in the above step
process = cms.Process(L1TRK_INST)

############################################################
# import standard configurations
############################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.Geometry.GeometryExtended2026D77Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2026D77_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.INFO.limit = cms.untracked.int32(0) # default: 0

############################################################
# input and output
############################################################

inputFiles = []
options = VarParsing.VarParsing('analysis')
# specify number of events to process.
#options.register('Algo','FastHisto',VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,"Name of algo")
options.register('Events',10,VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "Number of Events to analyze" )
options.register('Output','Hist.root',VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,"Name of output histogram file")

options.parseArguments()
for filePath in options.inputFiles:
    if filePath.endswith(".root"):
        inputFiles.append(filePath)
    else:
        inputFiles += FileUtils.loadListFromFile(filePath)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.Events) )

secFiles = cms.untracked.vstring()

process.source = cms.Source ("PoolSource",
                            fileNames = cms.untracked.vstring(inputFiles),
                            secondaryFileNames = secFiles,
                            duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
                            )

process.source.inputCommands = cms.untracked.vstring("keep *","drop l1tTkPrimaryVertexs_L1TkPrimaryVertex__*")

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.Output), closeFileFast = cms.untracked.bool(True))


############################################################
# L1 tracking: remake stubs?
############################################################

process.load('L1Trigger.TrackTrigger.TrackTrigger_cff')
from L1Trigger.TrackTrigger.TTStubAlgorithmRegister_cfi import *
process.load("SimTracker.TrackTriggerAssociation.TrackTriggerAssociator_cff")

from SimTracker.TrackTriggerAssociation.TTClusterAssociation_cfi import *
TTClusterAssociatorFromPixelDigis.digiSimLinks = cms.InputTag("simSiPixelDigis","Tracker")

process.TTClusterStub = cms.Path(process.TrackTriggerClustersStubs)
process.TTClusterStubTruth = cms.Path(process.TrackTriggerAssociatorClustersStubs)


# DTC emulation
process.load('L1Trigger.TrackerDTC.ProducerES_cff')
process.load('L1Trigger.TrackerDTC.ProducerED_cff')
process.dtc = cms.Path(process.TrackerDTCProducer)#*process.TrackerDTCAnalyzer)

process.load("L1Trigger.TrackFindingTracklet.L1HybridEmulationTracks_cff")
process.load("L1Trigger.L1TTrackMatch.L1GTTInputProducer_cfi")


process.TTTracksEmu = cms.Path(process.L1HybridTracks)
process.TTTracksEmuWithTruth = cms.Path(process.L1HybridTracksWithAssociators)
process.pL1GTTInput = cms.Path(process.L1GTTInputProducer)

############################################################
# Define the track ntuple process, MyProcess is the (unsigned) PDGID corresponding to the process which is run
# e.g. single electron/positron = 11
#      single pion+/pion- = 211
#      single muon+/muon- = 13
#      pions in jets = 6
#      taus = 15
#      all TPs = 1
############################################################

process.L1TrackNtuple = cms.EDAnalyzer('L1TrackReducedMaker',
        MyProcess = cms.int32(1),
        DebugMode = cms.bool(False),      # printout lots of debug statements
        SaveAllTracks = cms.bool(True),  # save *all* L1 tracks, not just truth matched to primary particle
        SaveStubs = cms.bool(False),      # save some info for *all* stubs
        L1Tk_minNStub = cms.int32(4),     # L1 tracks with >= 4 stubs
        TP_minNStub = cms.int32(4),       # require TP to have >= X number of stubs associated with it
        TP_minNStubLayer = cms.int32(4),  # require TP to have stubs in >= X layers/disks
        TP_minPt = cms.double(2.0),       # only save TPs with pt > X GeV
        TP_maxEta = cms.double(2.5),      # only save TPs with |eta| < X
        TP_maxZ0 = cms.double(15.0),      # only save TPs with |z0| < X cm
        L1TrackInputTag = cms.InputTag("TTTracksFromTrackletEmulation", "Level1TTTracks"),                                                      # TTTracks, prompt
        MCTruthTrackInputTag = cms.InputTag("TTTrackAssociatorFromPixelDigis", "Level1TTTracks"),                                               # MCTruth track, prompt
        L1GTTTrackInputTag = cms.InputTag("L1GTTInputProducer","Level1TTTracksConverted"),                                                      # TTTracks, prompt, GTT converted                      # TTTracks, prompt, emulation, selected
        L1StubInputTag = cms.InputTag("TTStubsFromPhase2TrackerDigis","StubAccepted"),
        MCTruthClusterInputTag = cms.InputTag("TTClusterAssociatorFromPixelDigis", "ClusterAccepted"),
        MCTruthStubInputTag = cms.InputTag("TTStubAssociatorFromPixelDigis", "StubAccepted"),
        TrackingParticleInputTag = cms.InputTag("mix", "MergedTrackTruth"),
        GenVertexInputTag = cms.InputTag("generatorSmeared","","SIM"),
)

process.ntuple = cms.Path(process.L1TrackNtuple)

process.out = cms.OutputModule( "PoolOutputModule",
                                fastCloning = cms.untracked.bool( False ),
                                fileName = cms.untracked.string("test.root" )
		               )
process.pOut = cms.EndPath(process.out)


# use this if you want to re-run the stub making
# process.schedule = cms.Schedule(process.TTClusterStub,process.TTClusterStubTruth,process.TTTracksEmuWithTruth,process.ntuple)

# use this if cluster/stub associators not available
# process.schedule = cms.Schedule(process.TTClusterStubTruth,process.TTTracksEmuWithTruth,process.ntuple)

process.schedule = cms.Schedule(process.TTClusterStub, process.TTClusterStubTruth, process.dtc, process.TTTracksEmuWithTruth, process.pL1GTTInput, process.ntuple)
