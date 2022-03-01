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
L1TRK_INST ="L1TrackJets" ### if not in input DIGRAW then we make them in the above step
process = cms.Process(L1TRK_INST)

L1TRKALGO = 'HYBRID'  #baseline, 4par fit
# L1TRKALGO = 'HYBRID_DISPLACED'  #extended, 5par fit
#L1TRKALGO = 'HYBRID_PROMPTANDDISP'

DISPLACED = ''

############################################################
# import standard configurations
############################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.Geometry.GeometryExtended2026D49Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2026D49_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

############################################################
# message logger
############################################################

# process.load("FWCore.MessageLogger.MessageLogger_cfi")
# process.MessageLogger = cms.Service("MessageLogger",
#     destinations   = cms.untracked.vstring('detailedInfo','critical','cout','cerr'),
#     critical = cms.untracked.PSet(threshold = cms.untracked.string('ERROR')),
#     detailedInfo = cms.untracked.PSet(threshold  = cms.untracked.string('INFO')),
#     cerr = cms.untracked.PSet(threshold  = cms.untracked.string('WARNING'))
# )

############################################################
# input and output
############################################################

#process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
# process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
# root://xrootd-cms.infn.it//store/relval/CMSSW_11_1_0_pre2/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU25ns_110X_mcRun4_realistic_v2_2026D49PU200-v1/20000/F7BF4AED-51F1-9D47-B86D-6C3DDA134AB9.root
#readFiles = cms.untracked.vstring(
    # '/store/relval/CMSSW_11_1_0_pre2/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU25ns_110X_mcRun4_realistic_v2_2026D49PU200-v1/20000/F7BF4AED-51F1-9D47-B86D-6C3DDA134AB9.root'
 #   '/store/mc/Phase2HLTTDRWinter20DIGI/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW/PU200_110X_mcRun4_realistic_v3-v2/110000/AC119ABB-457D-1942-A9F7-93C2962A9C5C.root'

    #'/store/relval/CMSSW_11_1_0_pre2/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU25ns_110X_mcRun4_realistic_v2_2026D49PU200-v1/20000/01816581-F04F-B849-83AA-38A348F9320B.root',
    #'/store/relval/CMSSW_11_1_0_pre2/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU25ns_110X_mcRun4_realistic_v2_2026D49PU200-v1/20000/075DCEC8-A2E9-7E48-A630-E1CBF89370A9.root',
    #'/store/relval/CMSSW_11_1_0_pre2/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU25ns_110X_mcRun4_realistic_v2_2026D49PU200-v1/20000/07EEAB60-C58E-6146-804B-6BA1B8E14875.root',
    #'/store/relval/CMSSW_11_1_0_pre2/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU25ns_110X_mcRun4_realistic_v2_2026D49PU200-v1/20000/09C2F689-B97E-6C44-94D2-C76445A4D703.root'
    # 'file:/afs/cern.ch/work/b/benjamin/Data/F7BF4AED-51F1-9D47-B86D-6C3DDA134AB9.root'
#)
inputFiles = []
options = VarParsing.VarParsing('analysis')
# specify number of events to process.
#options.register('Algo','FastHisto',VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,"Name of algo")
options.register('Events',100,VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "Number of Events to analyze" )
options.register('Output','Hist.root',VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,"Name of output histogram file")
options.register('Algorithm','FastHisto',VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,"Name of algo")


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


#process.TFileService = cms.Service("TFileService", fileName = cms.string('L1TObjNtuple_TTbar_NN.root'), closeFileFast = cms.untracked.bool(True))
process.TFileService = cms.Service("TFileService", fileName = cms.string(options.Output), closeFileFast = cms.untracked.bool(True))


############################################################
# L1 tracking: remake stubs?
############################################################

#process.load('L1Trigger.TrackTrigger.TrackTrigger_cff')
#from L1Trigger.TrackTrigger.TTStubAlgorithmRegister_cfi import *
#process.load("SimTracker.TrackTriggerAssociation.TrackTriggerAssociator_cff")

#from SimTracker.TrackTriggerAssociation.TTClusterAssociation_cfi import *
#TTClusterAssociatorFromPixelDigis.digiSimLinks = cms.InputTag("simSiPixelDigis","Tracker")

#process.TTClusterStub = cms.Path(process.TrackTriggerClustersStubs)
#process.TTClusterStubTruth = cms.Path(process.TrackTriggerAssociatorClustersStubs)


process.load("L1Trigger.TrackFindingTracklet.L1HybridEmulationTracks_cff")
process.load("L1Trigger.L1TTrackMatch.L1TrackJetProducer_cfi")
process.load("L1Trigger.L1TTrackMatch.L1GTTInputProducer_cfi")
process.load("L1Trigger.L1TTrackMatch.L1TrackJetEmulationProducer_cfi")
process.load("L1Trigger.L1TTrackMatch.L1TrackFastJetProducer_cfi")
process.load("L1Trigger.L1TTrackMatch.L1TrackerEtMissProducer_cfi")
process.load("L1Trigger.L1TTrackMatch.L1TkHTMissProducer_cfi")


# HYBRID: prompt tracking
if (L1TRKALGO == 'HYBRID'):
    process.TTTracksEmulation = cms.Path(process.L1HybridTracks)
    process.TTTracksEmulationWithTruth = cms.Path(process.L1HybridTracksWithAssociators)
    #process.pL1TrackJets = cms.Path(process.L1TrackJets)
    #process.pL1TrackFastJets=cms.Path(process.L1TrackFastJets)
    process.pL1GTTInput = cms.Path(process.L1GTTInputProducer)
    #process.pL1TrackJetsEmulation = cms.Path(process.L1TrackJetsEmulation)
    #process.pTkMET = cms.Path(process.L1TrackerEtMiss)
    #process.pTkMHT = cms.Path(process.L1TrackerHTMiss)
    DISPLACED = 'Prompt'

# HYBRID: extended tracking
elif (L1TRKALGO == 'HYBRID_DISPLACED'):
    process.TTTracksEmulation = cms.Path(process.L1ExtendedHybridTracks)
    process.TTTracksEmulationWithTruth = cms.Path(process.L1ExtendedHybridTracksWithAssociators)
    process.pL1TrackJets = cms.Path(process.L1TrackJetsExtended)
    process.pL1TrackFastJets = cms.Path(process.L1TrackFastJetsExtended)
    process.pL1GTTInput = cms.Path(process.L1GTTInputProducerExtended)
    process.pL1TrackJetsEmulation = cms.Path(process.L1TrackJetsExtendedEmulation)
    process.pTkMET = cms.Path(process.L1TrackerEtMissExtended)
    process.pTkMHT = cms.Path(process.L1TrackerHTMissExtended)
    DISPLACED = 'Displaced'#

# HYBRID: extended tracking
elif (L1TRKALGO == 'HYBRID_PROMPTANDDISP'):
    process.TTTracksEmulation = cms.Path(process.L1PromptExtendedHybridTracks)
    process.TTTracksEmulationWithTruth = cms.Path(process.L1PromptExtendedHybridTracksWithAssociators)
    process.pL1TrackJets = cms.Path(process.L1TrackJets*process.L1TrackJetsExtended)
    process.pL1TrackFastJets = cms.Path(process.L1TrackFastJets*process.L1TrackFastJetsExtended)
    process.pL1GTTInput = cms.Path(process.L1GTTInputProducer*process.L1GTTInputProducerExtended)
    process.pL1TrackJetsEmulation = cms.Path(process.L1TrackJetsEmulation*process.L1TrackJetsExtendedEmulation)
    process.pTkMET = cms.Path(process.L1TrackerEtMiss*process.L1TrackerEtMissExtended)
    process.pTkMHT = cms.Path(process.L1TrackerHTMiss*process.L1TrackerHTMissExtended)
    DISPLACED = 'Both'


############################################################
# Primary vertex
############################################################
process.load('L1Trigger.VertexFinder.VertexProducer_cff')
process.VertexProducer.VertexReconstruction.Algorithm = cms.string(options.Algorithm)
process.pPV = cms.Path(process.VertexProducer)



############################################################
# Define the track ntuple process, MyProcess is the (unsigned) PDGID corresponding to the process which is run
# e.g. single electron/positron = 11
#      single pion+/pion- = 211
#      single muon+/muon- = 13
#      pions in jets = 6
#      taus = 15
#      all TPs = 1
############################################################

process.L1TrackNtuple = cms.EDAnalyzer('L1TrackObjectNtupleMaker',
        MyProcess = cms.int32(1),
        DebugMode = cms.bool(False),      # printout lots of debug statements
        SaveAllTracks = cms.bool(True),   # save *all* L1 tracks, not just truth matched to primary particle
        SaveStubs = cms.bool(False),      # save some info for *all* stubs
        Displaced = cms.string(DISPLACED),# "Prompt", "Displaced", "Both"
        L1Tk_minNStub = cms.int32(4),     # L1 tracks with >= 4 stubs
        TP_minNStub = cms.int32(4),       # require TP to have >= X number of stubs associated with it
        TP_minNStubLayer = cms.int32(4),  # require TP to have stubs in >= X layers/disks
        TP_minPt = cms.double(2.0),       # only save TPs with pt > X GeV
        TP_maxEta = cms.double(2.5),      # only save TPs with |eta| < X
        TP_maxZ0 = cms.double(15.0),      # only save TPs with |z0| < X cm
        L1TrackInputTag = cms.InputTag("TTTracksFromTrackletEmulation", "Level1TTTracks"), # TTTracks, prompt
        L1GTTTrackInputTag = cms.InputTag("L1GTTInputProducer","Level1TTTracksConverted"), # GTT converted TTTracks, prompt
	    L1TrackExtendedInputTag = cms.InputTag("TTTracksFromExtendedTrackletEmulation", "Level1TTTracks"), # TTTracks, extended
        MCTruthTrackInputTag = cms.InputTag("TTTrackAssociatorFromPixelDigis", "Level1TTTracks"),  # MCTruth track, prompt
        MCTruthTrackExtendedInputTag = cms.InputTag("TTTrackAssociatorFromPixelDigisExtended", "Level1TTTracks"),  # MCTruth track, extended
        L1StubInputTag = cms.InputTag("TTStubsFromPhase2TrackerDigis","StubAccepted"),
        MCTruthClusterInputTag = cms.InputTag("TTClusterAssociatorFromPixelDigis", "ClusterAccepted"),
        MCTruthStubInputTag = cms.InputTag("TTStubAssociatorFromPixelDigis", "StubAccepted"),
        TrackingParticleInputTag = cms.InputTag("mix", "MergedTrackTruth"),
        TrackingVertexInputTag = cms.InputTag("mix", "MergedTrackTruth"),
        ## tracking in jets stuff (--> requires AK4 genjet collection present!)
        TrackingInJets = cms.bool(False),
        GenJetInputTag = cms.InputTag("ak4GenJets", ""),
        ##track jets and track MET
        SaveTrackJets = cms.bool(False),
	    SaveTrackJetsEmulation = cms.bool(False),
        SaveTrackMET = cms.bool(False),
        TrackFastJetsInputTag = cms.InputTag("L1TrackFastJets","L1TrackFastJets"),
        TrackFastJetsExtendedInputTag = cms.InputTag("L1TrackFastJetsExtended","L1TrackFastJetsExtended"),
        TrackJetsInputTag = cms.InputTag("L1TrackJets", "L1TrackJets"),
	    TrackJetsExtendedInputTag=cms.InputTag("L1TrackJetsExtended", "L1TrackJetsExtended"),
	    TrackJetsEmulationInputTag = cms.InputTag("L1TrackJetsEmulation","L1TrackJets"),
        TrackJetsExtendedEmulationInputTag = cms.InputTag("L1TrackJetsExtendedEmulation","L1TrackJetsExtended"),
        TrackMETInputTag = cms.InputTag("L1TrackerEtMiss","L1TrackerEtMiss","L1TrackJets"),
        TrackMETExtendedInputTag = cms.InputTag("L1TrackerEtMissExtended","L1TrackerEtMissExtended"),
        TrackMHTInputTag = cms.InputTag("L1TrackerHTMiss","L1TrackerHTMiss","L1TrackJets"),
        TrackMHTExtendedInputTag = cms.InputTag("L1TrackerHTMissExtended","L1TrackerHTMiss"),
        GenParticleInputTag = cms.InputTag("genParticles",""),
	    GenVertexInputTag = cms.InputTag("generatorSmeared","","SIM"),
        RecoVertexInputTag=cms.InputTag("VertexProducer", process.VertexProducer.l1VertexCollectionName.value()),
)

process.ntuple = cms.Path(process.L1TrackNtuple)

process.out = cms.OutputModule( "PoolOutputModule",
                                fastCloning = cms.untracked.bool( False ),
                                fileName = cms.untracked.string("test.root" )
		               )
process.pOut = cms.EndPath(process.out)



# use this if you want to re-run the stub making
# process.schedule = cms.Schedule(process.TTClusterStub,process.TTClusterStubTruth,process.TTTracksEmulationWithTruth,process.ntuple)

# use this if cluster/stub associators not available
# process.schedule = cms.Schedule(process.TTClusterStubTruth,process.TTTracksEmulationWithTruth,process.ntuple)

#process.schedule = cms.Schedule(process.pPV, process.pL1TrackJets, process.pL1TrackFastJets, process.pTkMET, process.pTkMHT,process.pOut)
#process.schedule = cms.Schedule(process.pPV, process.pL1TrackJets, process.pL1TrackFastJets, process.pTkMET, process.pTkMHT, process.ntuple)
#process.schedule = cms.Schedule(process.ntuple)
process.schedule = cms.Schedule(process.TTTracksEmulationWithTruth, process.pL1GTTInput, process.pPV,process.ntuple)
