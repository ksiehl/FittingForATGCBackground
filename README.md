BACKGROUND ESTIMATION
========

```
This script creates the Pdfs for the background contributions and extracts the alpha function for the W+jets background.
NOTE: This is only gauranteed to work in SLC6

Setup Instructions
------------------

# Make sure you have the correct architecture
export SCRAM_ARCH=slc6_amd64_gcc472

# Setup CMSSW in ~/private/FittingForATGC/Bacground/
cmsrel CMSSW_5_3_32
cd CMSSW_5_3_32/src
cmsenv

# Clone the repository
git clone git@github.com:ksiehl/FittingForATGCBackground.git

# Build the project
scram b -j 20

# Combine the originally created background trees
cd /afs/cern.ch/work/k/ksiehl/public/ansar_project/
mkdir background_output_storage
cd background_output_storage
mkdir el
mkdir mu
cd ../ntuple_output_storage/
root -l /afs/cern.ch/work/k/ksiehl/public/ansar_project/BCKGRND_STEP/CMSSW_5_3_32/src/FittingForATGCBackground/Modify_tree.cc

# hadd the muon wjets together
cd ../background_output_storage/mu/
hadd tree_WJets_mu.root tree_WJets?_mu.root
rm tree_WJets?_mu.root # this step can be skipped if you're worried about hadding errors
cd -

# Move the new trees
mv ../background_output_storage/ /afs/cern.ch/work/k/ksiehl//public/ansar_project/BCKGRND_STEP/CMSSW_5_3_32/src/FittingForATGCBackground/InputTrees/

# New PDFs can be added in PDFs/HWWLVJRooPdfs.cxx, which has to be compiled in ROOT using (in this order)
# But before compiling the c scripts, we have to tell ROOT where Roofit headers are since CMS builds RooFit not as a part of ROOT.

cd PDF/
root -b
gSystem->AddIncludePath("-I/cvmfs/cms.cern.ch/slc6_amd64_gcc472/lcg/roofit/5.32.03-cms/include/");

.L PdfDiagonalizer.cc+
.L Util.cxx+
.L hyperg_2F1.c+
.L HWWLVJRooPdfs.cxx+

.q
cd -

# Run the main script; this produces the background (and SM signal) fitting functions, as well as the alpha transfer functions and stores them in workspaces and datasets
python prepare_bkg_oneCat_angle.py -b --channel el --readtrees --starangle CUTVALUE

python prepare_bkg_oneCat_angle.py -b --channel mu --readtrees --starangle CUTVALUE
-b: batch mode
-c: channel (el or mu)
--readtrees: read the TTrees and save to RooDataHists (only needed once per channel)
--hi: Specify upper mWV limit, useful for cutoff limits

# Plots are saved in plots_{channel}_HPV_900_4500
# The workspace is saved in cards_{channel}_HPV_900_4500
# The workspace has to be copied to the Input folder in the directory of the signal estimation script

---------------------
