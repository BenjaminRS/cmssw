#include "TROOT.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TMath.h"
#include <TError.h>
#include "TSystem.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"
#include "TVectorT.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void SetPlotStyle();
void CMS_lumi( TCanvas* pad, int iPeriod, int iPosX );
void mySmallText(Double_t x, Double_t y, Color_t color, char* text);
TH1D* GetCumulative(TH1D* plot, int nevt);


void Plotter(TString type1, TString type2){

    SetPlotStyle();
    char text[500];

    gROOT->SetBatch();
    gErrorIgnoreLevel = kWarning;

    TChain* tree_FH = new TChain("L1TrackNtuple/eventTree");
    tree_FH->Add(type1 + ".root");

    TChain* tree_NN = new TChain("L1TrackNtuple/eventTree");
    tree_NN->Add(type2 + ".root");


    vector<float>* PVReco_FH;
    vector<float>* PVReco_NN;

    vector<float>* PVMC_FH;
    vector<float>* PVMC_NN;


    TBranch* b_PVReco_FH;
    TBranch* b_PVReco_NN;

    TBranch* b_PVMC_FH;
    TBranch* b_PVMC_NN;

    PVReco_FH = 0;
    PVReco_NN = 0;

    PVMC_FH = 0;
    PVMC_NN = 0;

    tree_FH->SetBranchAddress("pv_L1reco", &PVReco_FH, &b_PVReco_FH);
    tree_NN->SetBranchAddress("pv_L1reco", &PVReco_NN, &b_PVReco_NN);

    tree_FH->SetBranchAddress("pv_MC", &PVMC_FH, &b_PVMC_FH);
    tree_NN->SetBranchAddress("pv_MC", &PVMC_NN, &b_PVMC_NN);

    TH1F* h_PVReco_FH = new TH1F("PVReco_FH", ";z_{0}^{PV} [cm];#Events", 50, -15.0, 15.0);
    TH1F* h_PVReco_NN = new TH1F("PVReco_NN", ";z_{0}^{PV} [cm];#Events", 50, -15.0, 15.0);
    TH1F* h_PVMC      = new TH1F("PVMC",      ";z_{0}^{PV} [cm];#Events",      50, -15.0, 15.0);

    TH1F* h_FH_res      = new TH1F("FH_res",";z_{0}^{PV} Residual [cm];#Events", 50, -15.0, 15.0);
    TH1F* h_NN_res      = new TH1F("NN_res",";z_{0}^{PV} Residual [cm];#Events", 50, -15.0, 15.0);

    TH1F* h_FH_res_red      = new TH1F("FH_res_red",";z_{0}^{PV} Residual [cm];#Events", 50, -1.0, 1.0);
    TH1F* h_NN_res_red      = new TH1F("NN_res_red",";z_{0}^{PV} Residual [cm];#Events", 50, -1.0, 1.0);

    TH2F* h_NN_err_vs_PV      = new TH2F("NN_res_vs_PV",
                                        ";True PV z_{0} [cm];True PV z_{0} - Reco PV z_{0} NN [cm];#Events",
                                         60, -15, 15, 60,-15,15);
    TH2F* h_FH_err_vs_PV      = new TH2F("FH_res_vs_PV",
                                        ";True PV z_{0} [cm];True PV z_{0} - Reco PV z_{0} FH [cm];#Events",
                                         60, -15, 15, 60,-15,15);

    TH2F* h_NN_vs_PV      = new TH2F("NN_vs_PV",
                                     ";True PV z_{0} [cm];Reco PV z_{0} NN [cm];#Events",
                                      60, -15, 15, 60,-15,15);

    TH2F* h_FH_vs_PV      = new TH2F("FH_vs_PV",
                                     ";True PV z_{0} [cm];Reco PV z_{0} FH [cm];#Events",
                                      60, -15, 15, 60,-15,15);

    int num_thresholds = 100;

    TH2F* h_FH_vtx_efficiency = new TH2F("h_FH_vtx_efficiency",
                                      ";Threshold;True PV z_{0} [cm];#Events",
                                      num_thresholds, 0, 1, 50,-15,15);

    TH2F* h_NN_vtx_efficiency = new TH2F("h_NN_vtx_efficiency",
                                      ";Threshold;True PV z_{0} [cm];#Events",
                                      num_thresholds, 0, 1, 50,-15,15);


    vector<double> efficiency_thresholds;
    double threshold_10 = 0;

    for (int i = 0; i <num_thresholds; i++){
      efficiency_thresholds.push_back(threshold_10);
      threshold_10 += 1./num_thresholds;
    }


    int FHnevt = tree_FH->GetEntries();
    cout << "number of FH events = " << FHnevt << endl;

    int NNnevt = tree_NN->GetEntries();
    cout << "number of NN events = " << NNnevt << endl;



    for (int i = 0; i < FHnevt; i++) {

        tree_FH->GetEntry(i, 0);

        float PV = PVMC_FH->at(0);
        float err = PVMC_FH->at(0) - PVReco_FH->at(0);
        float FH = PVReco_FH->at(0);

        h_PVReco_FH->Fill(FH);
        h_PVMC->Fill(PV);
        h_FH_res->Fill(err);
        h_FH_res_red->Fill(err);

        h_FH_err_vs_PV->Fill(PV,err);
        h_FH_vs_PV->Fill(PV,FH);

        for (auto i : efficiency_thresholds){
          if (abs(err) <= i){
            h_FH_vtx_efficiency->Fill(i,PV);
            break;
          }
        }

    }

    for (int i = 0; i < NNnevt; i++) {

        tree_NN->GetEntry(i, 0);

        float PV = PVMC_NN->at(0);
        float err = PVMC_NN->at(0) - PVReco_NN->at(0);
        float NN = PVReco_NN->at(0);

        h_PVReco_NN->Fill(NN);
        h_NN_res->Fill(err);
        h_NN_res_red->Fill(err);

        h_NN_err_vs_PV->Fill(PV,err);
        h_NN_vs_PV->Fill(PV,NN);

        for (auto i : efficiency_thresholds){
          if (abs(err) <= i){
            h_NN_vtx_efficiency->Fill(i,PV);
            break;
          }
        }

    }



    TCanvas c;

    c.SetGridx();
    c.SetGridy();
   
    gSystem->mkdir("VtxPlots");
    TString DIR = "VtxPlots/";
    

    //Reconstructed Z0 Plots

    h_PVMC->SetLineColor(4);
    h_PVReco_NN->SetLineColor(1);
    h_PVReco_QNN->SetLineColor(8);
    h_PVReco_FH->SetLineColor(2);

    TLegend* l1 = new TLegend(0.2, 0.71, .3, .86);
    l1->SetBorderSize(0);
    l1->SetTextSize(0.03);
    l1->AddEntry(h_PVMC, "True", "l");
    l1->AddEntry(h_PVReco_NN, "NN", "l");
    l1->AddEntry(h_PVReco_QNN, "QNN", "l");
    l1->AddEntry(h_PVReco_FH, "FH", "l");

    h_PVMC->GetXaxis()->SetTitle("z_{0}^{PV} [cm]");

    float max = h_PVMC->GetMaximum();
    if (h_PVReco_NN->GetMaximum() > max)
        max = h_PVReco_NN->GetMaximum();
    else if (h_PVReco_FH->GetMaximum() > max)
        max = h_PVReco_FH->GetMaximum();
    h_PVMC->SetAxisRange(0,max*1.1, "Y");

    h_PVMC->Draw("");
    h_PVReco_NN->Draw("same");
    h_PVReco_QNN->Draw("same");
    h_PVReco_FH->Draw("same");
    l1->Draw("same");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"Reco_Z0.pdf");


    //Reconstructed Z0 Resolution Plot LogY

    float meanFH = h_FH_res->GetMean();
    float rmsFH = h_FH_res->GetRMS();

    float meanNN = h_NN_res->GetMean();
    float rmsNN = h_NN_res->GetRMS();

    float meanQNN = h_QNN_res->GetMean();
    float rmsQNN = h_QNN_res->GetRMS();

    gPad->SetLogy();

    h_FH_res->SetLineColor(1);
    h_NN_res->SetLineColor(2);
    h_QNN_res->SetLineColor(8);

    TLegend* l2 = new TLegend(0.2, 0.76, .3, .86);
    l2->SetBorderSize(0);
    l2->SetTextSize(0.03);
    l2->AddEntry(h_FH_res, "FH", "l");
    l2->AddEntry(h_NN_res, "NN", "l");
    l2->AddEntry(h_QNN_res, "QNN", "l");

    h_FH_res->Draw("");
    h_NN_res->Draw("same");
    h_QNN_res->Draw("same");
    l2->Draw("same");
    CMS_lumi( &c, 1, 11 );

    sprintf(text, "FH Mean: %.4f", meanFH);
    mySmallText(0.7, 0.86, 1, text);
    sprintf(text, "FH RMS: %.4f", rmsFH);
    mySmallText(0.7, 0.84, 1, text);
    sprintf(text, "NN Mean: %.4f", meanNN);
    mySmallText(0.7, 0.82, 2, text);
    sprintf(text, "NN RMS: %.4f", rmsNN);
    mySmallText(0.7, 0.80, 2, text);
    sprintf(text, "QNN Mean: %.4f", meanQNN);
    mySmallText(0.7, 0.78, 8, text);
    sprintf(text, "QNN RMS: %.4f", rmsQNN);
    mySmallText(0.7, 0.76, 8, text);
    sprintf(text, "Num Entries: %i", FHnevt);
    mySmallText(0.7, 0.74, 1, text);
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"Res_Z0_logY.pdf");

    //Reconstructed Z0 Resolution Plots

    gPad->SetLogy(0);

    h_FH_res_red->SetLineColor(1);
    h_NN_res_red->SetLineColor(2);
    h_QNN_res_red->SetLineColor(8);

    Double_t xq[2];
    Double_t yq_FH[2];
    Double_t yq_NN[2];
    Double_t yq_QNN[2];
    xq[0] = 0.32;
    xq[1] = 0.68;
    h_FH_res->GetQuantiles(2,yq_FH,xq);
    h_NN_res->GetQuantiles(2,yq_NN,xq);
    h_QNN_res->GetQuantiles(2,yq_QNN,xq);


    float max2 = h_FH_res_red->GetMaximum();
    if (h_NN_res_red->GetMaximum() > max2)
        max2 = h_NN_res_red->GetMaximum();
    h_FH_res_red->SetAxisRange(0,max2*1.1, "Y");

    TLegend* l3 = new TLegend(0.2, 0.76, .3, .86);
    l3->SetBorderSize(0);
    l3->SetTextSize(0.03);
    l3->AddEntry(h_FH_res_red, "FH", "l");
    l3->AddEntry(h_NN_res_red, "NN", "l");
    l3->AddEntry(h_QNN_res_red, "QNN", "l");


    h_FH_res_red->Draw("");
    h_NN_res_red->Draw("same");
    h_QNN_res_red->Draw("same");
    l3->Draw("same");
    CMS_lumi( &c, 1, 11 );
    sprintf(text, "FH Mean: %.4f", meanFH);
    mySmallText(0.7, 0.86, 1, text);
    sprintf(text, "FH Quartile: %.4f", yq_FH[1] - yq_FH[0]);
    mySmallText(0.7, 0.84, 1, text);
    sprintf(text, "NN Mean: %.4f", meanNN);
    mySmallText(0.7, 0.82, 2, text);
    sprintf(text, "NN Quartile: %.4f", yq_NN[1] - yq_NN[0]);
    mySmallText(0.7, 0.80, 2, text);
    sprintf(text, "QNN Mean: %.4f", meanQNN);
    mySmallText(0.7, 0.78, 8, text);
    sprintf(text, "QNN Quartile: %.4f", yq_QNN[1] - yq_QNN[0]);
    mySmallText(0.7, 0.76, 8, text);
    sprintf(text, "Num Entries: %i", FHnevt);
    mySmallText(0.7, 0.74, 1, text);
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"Res_Z0.pdf");


    //PV vs Reconstructed Z0 Resolution 2D plots

    h_NN_err_vs_PV->SetMinimum(0);
    h_NN_err_vs_PV->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"PV_vs_NN_Res_Z0.pdf");

    h_QNN_err_vs_PV->SetMinimum(0);
    h_QNN_err_vs_PV->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"PV_vs_QNN_Res_Z0.pdf");

    h_FH_err_vs_PV->SetMinimum(0);
    h_FH_err_vs_PV->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"PV_vs_FH_Res_Z0.pdf");

    //PV vs Reconstructed Z0 2D plots

    h_FH_vs_PV->SetMinimum(0);
    h_FH_vs_PV->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"PV_vs_FH_Z0.pdf");

    h_NN_vs_PV->SetMinimum(0);
    h_NN_vs_PV->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"PV_vs_NN_Z0.pdf");

    h_QNN_vs_PV->SetMinimum(0);
    h_QNN_vs_PV->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"PV_vs_QNN_Z0.pdf");

    h_NN_vtx_efficiency->SetMinimum(0);
    h_NN_vtx_efficiency->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"NN_vtx_efficiency.pdf");

    h_QNN_vtx_efficiency->SetMinimum(0);
    h_QNN_vtx_efficiency->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"QNN_vtx_efficiency.pdf");

    h_FH_vtx_efficiency->SetMinimum(0);
    h_FH_vtx_efficiency->Draw("colz");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"FH_vtx_efficiency.pdf");

    //PV efficiency 1D plots z0 scan

    float threshold = 30./256.;
    int bin_threshold = ceil((float)num_thresholds*threshold);

    TString s = TString::Itoa(bin_threshold,10);

    auto t = TString::Format("%g",threshold);

    TH1D* h_FH_vtx_efficiency_projection = h_FH_vtx_efficiency->ProjectionY("PY_FH",0,bin_threshold,"eo");
    TH1D* h_NN_vtx_efficiency_projection = h_NN_vtx_efficiency->ProjectionY("PY_NN",0,bin_threshold,"eo");
    TH1D* h_QNN_vtx_efficiency_projection = h_QNN_vtx_efficiency->ProjectionY("PY_QNN",0,bin_threshold,"eo");

    TH1F* h_FH_vtx_efficiency_threshold = (TH1F*)h_FH_vtx_efficiency_projection->Clone();
    TH1F* h_NN_vtx_efficiency_threshold = (TH1F*)h_NN_vtx_efficiency_projection->Clone();
    TH1F* h_QNN_vtx_efficiency_threshold = (TH1F*)h_QNN_vtx_efficiency_projection->Clone();

    h_FH_vtx_efficiency_threshold->Divide(h_FH_vtx_efficiency_threshold, h_PVMC, 1.0, 1.0, "B");
    h_NN_vtx_efficiency_threshold->Divide(h_NN_vtx_efficiency_threshold, h_PVMC, 1.0, 1.0, "B");
    h_QNN_vtx_efficiency_threshold->Divide(h_QNN_vtx_efficiency_threshold, h_PVMC, 1.0, 1.0, "B");

    h_FH_vtx_efficiency_threshold->GetYaxis()->SetTitle("Vertex Matching Efficiency, threshold = " + t);

    h_FH_vtx_efficiency_threshold->SetLineColor(1);
    h_NN_vtx_efficiency_threshold->SetLineColor(2);
    h_QNN_vtx_efficiency_threshold->SetLineColor(8);
    h_FH_vtx_efficiency_threshold->SetMarkerColor(1);
    h_NN_vtx_efficiency_threshold->SetMarkerColor(2);
    h_QNN_vtx_efficiency_threshold->SetMarkerColor(8);

    h_FH_vtx_efficiency_threshold->SetMarkerStyle(20);
    h_FH_vtx_efficiency_threshold->SetMarkerSize(0.7);
    h_NN_vtx_efficiency_threshold->SetMarkerStyle(21);
    h_NN_vtx_efficiency_threshold->SetMarkerSize(0.7);
    h_QNN_vtx_efficiency_threshold->SetMarkerStyle(21);
    h_QNN_vtx_efficiency_threshold->SetMarkerSize(0.7);

    TLegend* l4 = new TLegend(0.2, 0.76, .3, .86);
    l4->SetBorderSize(0);
    l4->SetTextSize(0.03);
    l4->AddEntry(h_FH_vtx_efficiency_threshold, "FH", "p");
    l4->AddEntry(h_NN_vtx_efficiency_threshold, "NN", "p");
    l4->AddEntry(h_QNN_vtx_efficiency_threshold, "QNN", "p");

    h_FH_vtx_efficiency_threshold->Draw("p");
    h_NN_vtx_efficiency_threshold->Draw("psame");
    h_QNN_vtx_efficiency_threshold->Draw("psame");
    l4->Draw("same");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"Vtx_finding_efficiency_zscan_"+s+".pdf");

    //PV efficency threshold scan


    TH1D* h_FH_vtx_efficiency_thres_projection = h_FH_vtx_efficiency->ProjectionX("PX_FH",0,-1,"eo");
    TH1D* h_NN_vtx_efficiency_thres_projection = h_NN_vtx_efficiency->ProjectionX("PX_NN",0,-1,"eo");
    TH1D* h_QNN_vtx_efficiency_thres_projection = h_QNN_vtx_efficiency->ProjectionX("PX_QNN",0,-1,"eo");

    TH1D* h_FH_vtx_efficiency_z0 = GetCumulative(h_FH_vtx_efficiency_thres_projection,FHnevt);
    TH1D* h_NN_vtx_efficiency_z0 = GetCumulative(h_NN_vtx_efficiency_thres_projection,NNnevt);
    TH1D* h_QNN_vtx_efficiency_z0 = GetCumulative(h_QNN_vtx_efficiency_thres_projection,QNNnevt);

    h_FH_vtx_efficiency_z0->GetYaxis()->SetTitle("Vertex Matching Efficiency");

    h_FH_vtx_efficiency_z0->SetLineColor(1);
    h_NN_vtx_efficiency_z0->SetLineColor(2);
    h_QNN_vtx_efficiency_z0->SetLineColor(8);
    h_FH_vtx_efficiency_z0->SetMarkerColor(1);
    h_NN_vtx_efficiency_z0->SetMarkerColor(2);
    h_QNN_vtx_efficiency_z0->SetMarkerColor(8);

    h_FH_vtx_efficiency_z0->SetMarkerStyle(20);
    h_FH_vtx_efficiency_z0->SetMarkerSize(0.7);
    h_NN_vtx_efficiency_z0->SetMarkerStyle(21);
    h_NN_vtx_efficiency_z0->SetMarkerSize(0.7);
    h_QNN_vtx_efficiency_z0->SetMarkerStyle(21);
    h_QNN_vtx_efficiency_z0->SetMarkerSize(0.7);

    TLegend* l5 = new TLegend(0.66, 0.2, .76, .3);
    l5->SetBorderSize(0);
    l5->SetTextSize(0.03);
    l5->AddEntry(h_FH_vtx_efficiency_z0, "FH", "p");
    l5->AddEntry(h_NN_vtx_efficiency_z0, "NN", "p");
    l5->AddEntry(h_QNN_vtx_efficiency_z0, "QNN", "p");

    h_FH_vtx_efficiency_z0->Draw("p");
    h_NN_vtx_efficiency_z0->Draw("psame");
    h_QNN_vtx_efficiency_z0->Draw("psame");
    l5->Draw("same");
    CMS_lumi( &c, 1, 11 );
    c.SaveAs(DIR+"Vtx_finding_efficiency_threshold_scan.pdf");

}

void SetPlotStyle() {
  // from ATLAS plot style macro

  // use plain black on white colors
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetHistLineColor(1);

  gStyle->SetPalette(1);

  // set the paper & margin sizes
  gStyle->SetPaperSize(20, 26);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  gStyle->SetTitleXOffset(1.4);
  gStyle->SetTitleYOffset(1.4);

  // use large fonts
  gStyle->SetTextFont(42);
  gStyle->SetTextSize(0.03);
  gStyle->SetLabelFont(42, "x");
  gStyle->SetTitleFont(42, "x");
  gStyle->SetLabelFont(42, "y");
  gStyle->SetTitleFont(42, "y");
  gStyle->SetLabelFont(42, "z");
  gStyle->SetTitleFont(42, "z");
  gStyle->SetLabelSize(0.03, "x");
  gStyle->SetTitleSize(0.03, "x");
  gStyle->SetLabelSize(0.03, "y");
  gStyle->SetTitleSize(0.03, "y");
  gStyle->SetLabelSize(0.03, "z");
  gStyle->SetTitleSize(0.03, "z");

  // use bold lines and markers
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.);
  gStyle->SetHistLineWidth(2.);
  gStyle->SetLineStyleString(2, "[12 12]");

  gStyle->SetLegendBorderSize(1);
  gStyle->SetLegendFillColor(0);
  gStyle->SetLegendFont(42);
  gStyle->SetLegendTextSize(0.);

  // get rid of error bar caps
  gStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  gStyle->SetStatY(0.9);
  gStyle->SetStatX(0.9);
  gStyle->SetStatW(0.4);
  gStyle->SetStatH(0.5); 

}

void CMS_lumi( TCanvas* pad, int iPeriod, int iPosX )
{   
  TString cmsText     = "CMS";
  float cmsTextFont   = 61;  // default is helvetic-bold

  bool writeExtraText = true;
  TString extraText   = "Phase-2 Simulation Preliminary";
  float extraTextFont = 52;  // default is helvetica-italics

  // text sizes and text offsets with respect to the top frame
  // in unit of the top margin size
  float lumiTextSize     = 0.6;
  float lumiTextOffset   = 0.2;
  float cmsTextSize      = 0.75;
  float cmsTextOffset    = 0.1;  // only used in outOfFrame version

  float relPosX    = 0.045;
  float relPosY    = 0.035;
  float relExtraDY = 1.2;

  // ratio of "CMS" and extra text size
  float extraOverCmsTextSize  = 0.76;

  TString lumi_13TeV = "20.1 fb^{-1}";
  TString lumi_8TeV  = "19.7 fb^{-1}";
  TString lumi_7TeV  = "5.1 fb^{-1}";
  TString lumi_sqrtS = "";

  bool drawLogo      = false;         
  bool outOfFrame    = true;
  if( iPosX/10==0 ) 
    {
      outOfFrame = true;
    }
  int alignY_=3;
  int alignX_=2;
  if( iPosX/10==0 ) alignX_=1;
  if( iPosX==0    ) alignX_=1;
  if( iPosX==0    ) alignY_=1;
  if( iPosX/10==1 ) alignX_=1;
  if( iPosX/10==2 ) alignX_=2;
  if( iPosX/10==3 ) alignX_=3;
  //if( iPosX == 0  ) relPosX = 0.12;
  int align_ = 10*alignX_ + alignY_;

  float H = pad->GetWh();
  float W = pad->GetWw();
  float l = pad->GetLeftMargin();
  float t = pad->GetTopMargin();
  float r = pad->GetRightMargin();
  float b = pad->GetBottomMargin();
  //  float e = 0.025;

  pad->cd();

  TString lumiText;
  if( iPeriod==1 )
    {
      lumiText = "14 TeV, 200 PU";
    }

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  float extraTextSize = extraOverCmsTextSize*cmsTextSize;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);

  if( outOfFrame )
    {
      latex.SetTextFont(cmsTextFont);
      latex.SetTextAlign(11); 
      latex.SetTextSize(cmsTextSize*t);    
      latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);
    }
  
  pad->cd();

  float posX_=0;
  if( iPosX%10<=1 )
    {
      posX_ =   l + relPosX*(1-l-r);
    }
  else if( iPosX%10==2 )
    {
      posX_ =  l + 0.5*(1-l-r);
    }
  else if( iPosX%10==3 )
    {
      posX_ =  1-r - relPosX*(1-l-r);
    }
  float posY_ = 1-t - relPosY*(1-t-b);
  if( !outOfFrame )
    {
      if( drawLogo )
        std::cout << "logo? " << std::endl;
      else
	{
	  latex.SetTextFont(cmsTextFont);
	  latex.SetTextSize(cmsTextSize*t);
	  latex.SetTextAlign(align_);
	  latex.DrawLatex(posX_, posY_, cmsText);
	  if( writeExtraText ) 
	    {
	      //latex.SetTextFont(extraTextFont);
	      //latex.SetTextAlign(align_);
	      //latex.SetTextSize(extraTextSize*t);
	      //latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, extraText);

        latex.SetTextFont(extraTextFont);
	      latex.SetTextAlign(11);
	      latex.SetTextSize(extraTextSize*t);
        latex.DrawLatex(l,1-t+lumiTextOffset*t - relExtraDY*cmsTextSize*t,extraText);
	    }
	}
    }
  else if( writeExtraText )
    {
	  posX_ =   l +0.06 ;
	  posY_ =   1-t+lumiTextOffset*t + relExtraDY*cmsTextSize*t - 0.022;
	
    latex.SetTextFont(extraTextFont);
    latex.SetTextSize(extraTextSize*t);
    latex.SetTextAlign(align_);
    latex.DrawLatex(posX_, posY_, extraText);      
    }
  return;
}

void mySmallText(Double_t x, Double_t y, Color_t color, char* text) {
  Double_t tsize = 0.02;
  TLatex l;
  l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextColor(color);
  l.DrawLatex(x, y, text);
}

TH1D* GetCumulative(TH1D* plot,int nevt) {
  std::string newName = Form("cumulative_%s", plot->GetName());
  TH1D* temp = (TH1D*)plot->Clone(newName.c_str());
  temp->SetDirectory(0);
  for (int i = 0; i < plot->GetNbinsX() + 1; i++) {
    double content(0.0), error2(0.0);
    for (int j = 0; j < i; j++) {
      content += plot->GetBinContent(j);
      error2 += plot->GetBinError(j) * plot->GetBinError(j);
    }
    temp->SetBinContent(i, content/nevt);
    temp->SetBinError(i, TMath::Sqrt(error2/(nevt*nevt)));
  }
  return temp;
}

vector<TH1F*> CalcCM(char* name,vector<float> True, vector<float> Pred, int num_threshold){
  vector<TH1F*> return_vector;
  TH1F* TP = new TH1F("True Postives",";Threshold;#Tracks",num_threshold, 0.0,1.0);
  TH1F* P = new TH1F("Total Postives",";Threshold;#Tracks",num_threshold, 0.0,1.0);
  TH1F* TN = new TH1F ("True Negatives",";Threshold;#Tracks",num_threshold, 0.0,1.0);
  TH1F* N = new TH1F("Total Negatives",";Threshold;#Tracks",num_threshold, 0.0,1.0);

  for (int i = 0; i < num_threshold; i++){
    float dt = (float)i / (num_threshold - 1);
    for (int j = 0; j < (int)True.size(); j++){
      if (True.at(j) == 1.){
        P->Fill(dt);
        if (Pred.at(j) > dt)
          TP->Fill(dt);
      }
      else {
        N->Fill(dt);
        if (Pred.at(j) <= dt)
          TN->Fill(dt);
      }
    }
  }

  return_vector.push_back(TP);
  return_vector.push_back(P);
  return_vector.push_back(TN);
  return_vector.push_back(N);

  return return_vector;
}

vector<TVectorT<double>> CalcROC(TH1F* XHist, TH1F* YHist){
    vector<TVectorT<double>> return_vector;

    int num_threshold = XHist->GetNbinsX();

    TVectorT<double> ROC_X(num_threshold);
    TVectorT<double> ROC_Y(num_threshold);
    TVectorT<double> ROC_Xerr(num_threshold);
    TVectorT<double> ROC_Yerr(num_threshold);
    TVectorT<double> AUC_ROC(2);


    for (int i=0; i< (num_threshold - 1); i++){
      ROC_X[i]= (XHist->GetBinContent(i));
      ROC_Y[i]= (YHist->GetBinContent(i));
      ROC_Xerr[i] = (XHist->GetBinError(i));
      ROC_Yerr[i]= (YHist->GetBinError(i));
      AUC_ROC[0] += (XHist->GetBinContent(i + 1) - XHist->GetBinContent( i ) )*(0.5*(YHist->GetBinContent(i) + YHist->GetBinContent( i + 1) ));
      AUC_ROC[1] +=   XHist->GetBinError(i+1)*(0.5*(YHist->GetBinContent(i) + YHist->GetBinContent( i + 1)))
                    - XHist->GetBinError(i)*(0.5*(YHist->GetBinContent(i) + YHist->GetBinContent( i + 1)))
                    + YHist->GetBinError(i)*(0.5*(XHist->GetBinContent(i + 1) - XHist->GetBinContent( i )))
                    + YHist->GetBinError(i+1)*(0.5*(XHist->GetBinContent(i + 1) - XHist->GetBinContent( i )));
    }

    ROC_X[num_threshold-1] = (XHist->GetBinContent(num_threshold-1));
    ROC_Y[num_threshold-1] = (YHist->GetBinContent(num_threshold-1));
    ROC_Xerr[num_threshold-1] = (XHist->GetBinError(num_threshold-1));
    ROC_Yerr[num_threshold-1] = (YHist->GetBinError(num_threshold-1));

    return_vector.push_back(ROC_X);
    return_vector.push_back(ROC_Y);
    return_vector.push_back(ROC_Xerr);
    return_vector.push_back(ROC_Yerr);
    return_vector.push_back(AUC_ROC);

    return return_vector;

}
