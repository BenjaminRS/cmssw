import FWCore.ParameterSet.Config as cms
import RecoLocalCalo.HcalRecProducers.HBHEMethod3Parameters_cfi as method3

horeco = cms.EDProducer(
    "HcalHitReconstructor",
    correctionPhaseNS = cms.double(13.0),
    digiLabel = cms.InputTag("hcalDigis"),
    Subdetector = cms.string('HO'),
    correctForPhaseContainment = cms.bool(True),
    correctForTimeslew = cms.bool(True),
    dropZSmarkedPassed = cms.bool(True),
    firstSample = cms.int32(4),
    samplesToAdd = cms.int32(4),
    tsFromDB = cms.bool(True),
    recoParamsFromDB = cms.bool(True),
    useLeakCorrection = cms.bool(False),
    dataOOTCorrectionName = cms.string(""),
    dataOOTCorrectionCategory = cms.string("Data"),
    mcOOTCorrectionName = cms.string(""),
    mcOOTCorrectionCategory = cms.string("MC"),
    puCorrMethod = cms.int32(0),

    # Set time slice for first digi to be stored in aux word
    # (HO uses time slices 4-7)
    firstAuxTS = cms.int32(4),

    #Tags for calculating status flags
    correctTiming = cms.bool(True),
    setNoiseFlags = cms.bool(True),
    setHSCPFlags  = cms.bool(True), # HSCP not implemented for horeco; this boolean does nothing
    setSaturationFlags = cms.bool(True),
    setTimingTrustFlags = cms.bool(False), # timing flags currently only implemented for HF
    setPulseShapeFlags = cms.bool(False),  # not yet defined for HO
    setNegativeFlags          = cms.bool(False),  # only in HBHE
    saturationParameters=  cms.PSet(maxADCvalue=cms.int32(127)),
    # Configuration parameters for Method 3
    pedestalSubtractionType = cms.int32(method3.pedestalSubtractionType),
    pedestalUpperLimit      = cms.double(method3.pedestalUpperLimit),
    timeSlewParsType        = cms.int32(method3.timeSlewParsType),
    timeSlewPars            = cms.vdouble(method3.timeSlewPars),
    respCorrM3              = cms.double(method3.respCorrM3)
) # horeco


