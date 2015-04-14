import FWCore.ParameterSet.Config as cms

# Updating the config (PR #8356)
def customiseFor8356(process):
    listL3trackingModule = ['hltL3MuonsOIState','hltL3MuonsOIHit','hltL3MuonsIOHit','hltL3NoFiltersNoVtxMuonsOIState','hltL3NoFiltersNoVtxMuonsOIHit','hltL3NoFiltersNoVtxMuonsIOHit']
    for l3module in listL3trackingModule:
        if hasattr(process, l3module):
            if hasattr(getattr(process, l3module), 'GlbRefitterParameters'):
                setattr(getattr(process, l3module).GlbRefitterParameters, 'RefitFlag', cms.bool(True))
            if hasattr(getattr(process, l3module), 'L3TrajBuilderParameters'):
                if hasattr(getattr(process, l3module).L3TrajBuilderParameters, 'MuonTrackingRegionBuilder'):
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'Pt_fixed', cms.bool(False))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'Z_fixed', cms.bool(True))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'Phi_fixed', cms.bool(False))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'Eta_fixed', cms.bool(False))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'Pt_min', cms.double(1.5))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'DeltaZ', cms.double(15.9))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'DeltaEta', cms.double(0.2))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'DeltaPhi', cms.double(0.2))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'maxRegions', cms.int32(1))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'precise', cms.bool(True))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'OnDemand', cms.int32(-1))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'input', cms.InputTag('hltL2Muons','UpdatedAtVtx'))
                    setattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'MeasurementTrackerName', cms.InputTag("hltESPMeasurementTracker"))
                    delattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'DeltaZ_Region')
                    delattr(getattr(process, l3module).L3TrajBuilderParameters.MuonTrackingRegionBuilder, 'UseFixedRegion')

    listL3seedingModule = ['hltL3TrajSeedIOHit','hltL3NoFiltersNoVtxTrajSeedIOHit']
    for l3seeding in listL3seedingModule:
        if hasattr(process, l3seeding):
            if hasattr(getattr(process, l3seeding), 'MuonTrackingRegionBuilder'):
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'Pt_fixed', cms.bool(False))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'Z_fixed', cms.bool(True))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'Phi_fixed', cms.bool(False))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'Eta_fixed', cms.bool(False))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'Pt_min', cms.double(1.5))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'DeltaZ', cms.double(15.9))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'DeltaEta', cms.double(0.2))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'DeltaPhi', cms.double(0.2))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'maxRegions', cms.int32(1))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'precise', cms.bool(True))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'OnDemand', cms.int32(-1))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'input', cms.InputTag('hltL2Muons','UpdatedAtVtx'))
                setattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'MeasurementTrackerName', cms.InputTag("hltESPMeasurementTracker"))
                delattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'DeltaZ_Region')
                delattr(getattr(process, l3seeding).MuonTrackingRegionBuilder, 'UseFixedRegion')

    return process




# Simplified TrackerTopologyEP config (PR #7966)
def customiseFor7966(process):
    if hasattr(process, 'trackerTopology'):
        params = process.trackerTopology.parameterNames_()
        for param in params:
            delattr(process.trackerTopology, param)
        setattr(process.trackerTopology, 'appendToDataLabel', cms.string(""))
    return process


# Removal of 'upgradeGeometry' from TrackerDigiGeometryESModule (PR #7794)
def customiseFor7794(process):
    if hasattr(process, 'TrackerDigiGeometryESModule'):
        if hasattr(process.TrackerDigiGeometryESModule, 'trackerGeometryConstants'):
            if hasattr(process.TrackerDigiGeometryESModule.trackerGeometryConstants, 'upgradeGeometry'):
                delattr(process.TrackerDigiGeometryESModule.trackerGeometryConstants, 'upgradeGeometry')
    return process


# CMSSW version specific customizations
def customiseHLTforCMSSW(process,menuType="GRun",fastSim=False):
    import os
    cmsswVersion = os.environ['CMSSW_VERSION']

    if cmsswVersion >= "CMSSW_7_5":
        process = customiseFor7966(process)
        process = customiseFor7794(process)
        process = customiseFor8356(process)


    return process
