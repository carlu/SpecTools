// Quick sort to import ascii spe file produced by ORTEC Maestro to root spectrum
// Accepts a list of files on the command line, converts to ROOT TH1Fs, then writes all
// of them to a single root file.
//
// Carl Unsworth Sept 11
//
// To Compile:
// g++ Spe2Root.C --std=c++0x -o Spe2Root -O2 -Wall `root-config --cflags --libs`
//
// To Run:
// ./Spe2Root [filename]

// C/C++ libraries:
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

// ROOT libraries:
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>

#define MAX_BINS 65536
#define MAX_SPECTRA 256
#define MAX_CHARS 1024

int main(int argc, char **argv){

   // Input
   ifstream Infile;  // Input file
   string InfileName;// Input file name

   string str;       // string to read input file into
   // Internal
   int FirstBin, LastBin, i, Bin, NumBins;  
   int Data[MAX_BINS];
   // Output
   TH1F *Spectra[MAX_SPECTRA];
   TFile *Outfile = 0;
   string SpecName, SpecTitle, OutfileName;

   OutfileName = "SpectraOut.root";
   
   if(argc>1) {
      cout << "Input files are: "  << endl;
      for(i=0;i<argc;i++) {
         cout << argv[i] << endl;
      }   
   }
   else {
      cout << "No Input Spectra Specified" << endl;
      return 0;
   }
   
   // Open output file
   Outfile = new TFile(OutfileName.c_str(),"RECREATE");  
   
   // Now loop input files
   for(i=1;i<argc;i++) {
      // Get filename and open 
      InfileName = argv[i];
      Infile.open(InfileName.c_str());
      SpecName = InfileName.substr(0,InfileName.length()-4);

      if(Infile) {
         cout << InfileName << " opened!" << endl;
      }
      else {
         cout << "Unable to open " << InfileName << "! - Skipping" << endl;
         continue;
      }

      // Clear data from last spectrum
      memset(&Data,0,(MAX_BINS*sizeof(int)));
      // Loop header in spe file.
      while(!Infile.eof()) {
         getline(Infile,str);
         if(str.compare(0,6,"$DATA:")==0) {
            cout << "Found start of data: " << str << endl;
            break;
         }
      }     

      // Loop data and get bin content

      // First line tells us values for first and last bin
      getline(Infile,str);
      sscanf(str.c_str(),"%d %d",&FirstBin,&LastBin);
      cout << FirstBin << " " << LastBin << endl;

      // After that we get bin contents one line at a time, counting bins as we go
      NumBins = 0;
      while(!Infile.eof()) {
         getline(Infile,str);
         if(str.compare(0,5,"$ROI:")==0) {
            // Have we reached the end of the data?
            break;
         }
         // ...if not the end then grab the next bin
         sscanf(str.c_str(),"%d",&Data[NumBins]);
         NumBins += 1;
      }

      // Create histo with correct bins
      SpecTitle = SpecName;
      Spectra[i] = new TH1F(SpecName.c_str(),SpecTitle.c_str(),NumBins,FirstBin,LastBin);

      // Fill histo with data
      for (Bin=0;Bin<NumBins;Bin++) {
         Spectra[i]->SetBinContent(Bin,Data[Bin]);
      }

      // Write spectrum and close input file
      Spectra[i]->Write();
      Infile.close();
   }

   // Finished.
   Outfile->Close();
   return 0;
}
