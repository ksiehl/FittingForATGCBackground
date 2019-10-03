#include <TTree.h>
#include <TChain.h>
#include <TBranch.h>
#include <TFile.h>
#include <TString.h>
#include <TSystem.h>
#include <TMath.h>
#include <iostream>

void merge_trees(std::vector<TString> files, TChain &chain)
{
	for(unsigned int i=0; i<files.size(); i++)
		chain.Add(files[i]);
}

void fill_tree_with_cuts(TTree &oldtree, TTree &tree, TString ch)
{
	Double_t jet_pt,jet_tau21_PUPPI,jet_mass_softdrop_PUPPI,W_pt,deltaR_LeptonWJet,deltaPhi_WJetMet,deltaPhi_WJetWlep,pfMET,METCUT;
	Int_t nbtag;
	Bool_t bit_HLT_Ele_45, bit_HLT_Ele_115;
	Bool_t passTrigger=true;
	METCUT	= ch=="ele" ? 110. : 40.;

	oldtree.SetBranchAddress("jet_pt",&jet_pt);
	oldtree.SetBranchAddress("jet_tau21_PUPPI",&jet_tau21_PUPPI);
	oldtree.SetBranchAddress("jet_mass_softdrop_PUPPI",&jet_mass_softdrop_PUPPI);
	oldtree.SetBranchAddress("W_pt",&W_pt);
	oldtree.SetBranchAddress("deltaR_LeptonWJet",&deltaR_LeptonWJet);
	oldtree.SetBranchAddress("deltaPhi_WJetMet",&deltaPhi_WJetMet);
	oldtree.SetBranchAddress("deltaPhi_WJetWlep",&deltaPhi_WJetWlep);
	oldtree.SetBranchAddress("nbtag",&nbtag);
	oldtree.SetBranchAddress("pfMET",&pfMET);
	if (ch=="ele"){
		oldtree.SetBranchAddress("bit_HLT_Ele_45",&bit_HLT_Ele_45);
		oldtree.SetBranchAddress("bit_HLT_Ele_115",&bit_HLT_Ele_115);
	}
	

	Long64_t nEntries	= oldtree.GetEntries();

	int used_events		= 0;

	for(unsigned int i=0; i<nEntries; i++)
	{
		oldtree.GetEntry(i);
		//if (ch=="ele") passTrigger=(bit_HLT_Ele_45 || bit_HLT_Ele_115);
		if(jet_pt>200. && jet_tau21_PUPPI<0.55 && jet_mass_softdrop_PUPPI<150. && jet_mass_softdrop_PUPPI>40. && W_pt>200. && fabs(deltaR_LeptonWJet)>TMath::Pi()/2 && fabs(deltaPhi_WJetMet)>2. && fabs(deltaPhi_WJetWlep)>2. && nbtag==0 && pfMET>METCUT && passTrigger)
		{
			tree.Fill();
			if(used_events%1000==0 && i!=0)
				std::cout<<used_events<<"/"<<i<<"/"<<nEntries<<", "<<(float)i/(float)nEntries*100<<"%"<<", used: "<<(float)used_events/(float)i*100<<"%"<<std::endl;
			used_events++;
		}
	}
	std::cout<<"Using "<<used_events<<" events of total "<<nEntries<<std::endl;
}

void save_tree(TString name, TTree &tree, TString ch)
{
	TString path	= "../background_output_storage/"+ch(0,2)+"/";
	TFile * fileOut	= TFile::Open(path+"tree_"+name+"_"+ch(0,2)+".root","recreate");
	tree.Write();
	fileOut->Close();
	std::cout<<"Saved "<<name<<" as "<<path+"tree_"+name+"_"+ch(0,2)+".root"<<std::endl;
}

void make_trees(TString ch)
{
        //electrons only
	std::vector<TString> WJets_names;

	//both channels
	std::vector<TString> TTbar_names;
	std::vector<TString> STop_names;
	std::vector<TString> WW_names;
	std::vector<TString> WZ_names;
	std::vector<TString> data_names;

	//muon wjets require separate steps due to size
	std::vector<TString> WJets0_names;
	std::vector<TString> WJets1_names;
	std::vector<TString> WJets2_names;

	//only used for electron channel
	WJets_names.push_back("WJets_Ht100To200_"+ch+".root");
	WJets_names.push_back("WJets_Ht200To400_"+ch+".root");
	WJets_names.push_back("WJets_Ht400To600_"+ch+".root");
	WJets_names.push_back("WJets_Ht600To800_"+ch+".root");
	WJets_names.push_back("WJets_Ht800To1200_"+ch+".root");
	WJets_names.push_back("WJets_Ht1200To2500_"+ch+".root");
	WJets_names.push_back("WJets_Ht2500ToInf_"+ch+".root");
	//WJets_names.push_back("WJets_Pt100To250_"+ch+".root");
	//WJets_names.push_back("WJets_Pt250To400_"+ch+".root");
	//WJets_names.push_back("WJets_Pt400To600_"+ch+".root");
	//WJets_names.push_back("WJets_Pt600ToInf_"+ch+".root");

	//only used for muon channel
	WJets0_names.push_back("WJets_Ht100To200_"+ch+".root");
	WJets0_names.push_back("WJets_Ht200To400_"+ch+".root");
	WJets0_names.push_back("WJets_Ht400To600_"+ch+".root");
	WJets0_names.push_back("WJets_Ht2500ToInf_"+ch+".root");

	WJets1_names.push_back("WJets_Ht800To1200_"+ch+".root");
	WJets1_names.push_back("WJets_Ht1200To2500_"+ch+".root");

	WJets2_names.push_back("WJets_Ht600To800_"+ch+".root");
	
	//used for both channels
	TTbar_names.push_back("ttbar_"+ch+".root");

	STop_names.push_back("s-ch_"+ch+".root");
	STop_names.push_back("tW-ch-antitop_"+ch+".root");
	STop_names.push_back("tW-ch-top_"+ch+".root");
	STop_names.push_back("t-ch-antitop_"+ch+".root");
	STop_names.push_back("t-ch-top_"+ch+".root");

	WW_names.push_back("WW_"+ch+".root");
	
	WZ_names.push_back("WZ_"+ch+".root");

	data_names.push_back("data_"+ch+".root");


	TChain WJets_old("BasicTree");
	if (ch == "ele") merge_trees(WJets_names,WJets_old);
	TTree * WJets_new	= WJets_old.CloneTree(0);
	if (ch == "ele") std::cout<<"Reading W+jets..."<<std::endl;
	if (ch == "ele") fill_tree_with_cuts(WJets_old,*WJets_new,ch);

	TChain WJets0_old("BasicTree");
	if (ch == "mu") merge_trees(WJets0_names,WJets0_old);
	TTree * WJets0_new	= WJets0_old.CloneTree(0);
	if (ch == "mu") std::cout<<"Reading W+jets0..."<<std::endl;
	if (ch == "mu") fill_tree_with_cuts(WJets0_old,*WJets0_new,ch);

	TChain WJets1_old("BasicTree");
	if (ch == "mu") merge_trees(WJets1_names,WJets1_old);
	TTree * WJets1_new	= WJets1_old.CloneTree(0);
	if (ch == "mu") std::cout<<"Reading W+jets1..."<<std::endl;
	if (ch == "mu") fill_tree_with_cuts(WJets1_old,*WJets1_new,ch);

	TChain WJets2_old("BasicTree");
	if (ch == "mu") merge_trees(WJets2_names,WJets2_old);
	TTree * WJets2_new	= WJets2_old.CloneTree(0);
	if (ch == "mu") std::cout<<"Reading W+jets2..."<<std::endl;
	if (ch == "mu") fill_tree_with_cuts(WJets2_old,*WJets2_new,ch);

	std::cout<<"Reading TTbar..."<<std::endl;
	TChain TTbar_old("BasicTree");
	merge_trees(TTbar_names,TTbar_old);
	TTree * TTbar_new	= TTbar_old.CloneTree(0);
	fill_tree_with_cuts(TTbar_old,*TTbar_new,ch);	

	std::cout<<"Reading STop..."<<std::endl;
	TChain STop_old("BasicTree");
	merge_trees(STop_names,STop_old);
	TTree * STop_new	= STop_old.CloneTree(0);
	fill_tree_with_cuts(STop_old,*STop_new,ch);

	std::cout<<"Reading WW..."<<std::endl;
	TChain WW_old("BasicTree");
	merge_trees(WW_names,WW_old);
	TTree * WW_new		= WW_old.CloneTree(0);
	fill_tree_with_cuts(WW_old,*WW_new,ch);
	
	std::cout<<"Reading WZ..."<<std::endl;
	TChain WZ_old("BasicTree");
	merge_trees(WZ_names,WZ_old);
	TTree * WZ_new		= WZ_old.CloneTree(0);
	fill_tree_with_cuts(WZ_old,*WZ_new,ch);
	
	std::cout<<"Reading data..."<<std::endl;
	TChain data_old("treeDumper/BasicTree");
	merge_trees(data_names,data_old);
	TTree * data_new	= data_old.CloneTree(0);
	fill_tree_with_cuts(data_old,*data_new,ch);

	
	save_tree("TTbar",*TTbar_new,ch);
	save_tree("STop",*STop_new,ch);
	save_tree("WW",*WW_new,ch);
	save_tree("WZ",*WZ_new,ch);
	save_tree("data",*data_new,ch);

	if (ch == "ele") save_tree("WJets",*WJets_new,ch);
	
	if (ch == "mu")
	  {
	    save_tree("WJets0",*WJets0_new,ch);
	    save_tree("WJets1",*WJets1_new,ch);
	    save_tree("WJets2",*WJets2_new,ch);
	  }
}

void Modify_tree()
{
	std::cout<<"Reading electron trees"<<std::endl;
	make_trees("ele");
	std::cout<<"Reading muon trees"<<std::endl;
	make_trees("mu");
}

