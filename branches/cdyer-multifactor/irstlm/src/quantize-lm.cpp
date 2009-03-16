/******************************************************************************
IrstLM: IRST Language Model Toolkit, compile LM
Copyright (C) 2006 Marcello Federico, ITC-irst Trento, Italy

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

******************************************************************************/

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>

#include "math.h"
#include "lmtable.h"

//----------------------------------------------------------------------
//  Special type and global variable for the BIN CLUSTERING algorithm
//      
//      
//----------------------------------------------------------------------

typedef struct{
  double pt;
  int idx;
  short code;
}BinEntry;


int cmpBinEntry(const void* a,const void* b){
  if (*(double *)a > *(double*)b)
    return 1;
  else if (*(double *)a < *(double*)b)
    return -1;
  else
    return 0;
}

BinEntry* bintable=NULL;

//----------------------------------------------------------------------
//  Global entry points
//----------------------------------------------------------------------

int parseWords(char *sentence, char **words, int max);

int ComputeCluster(int nc, double* cl,int N,double* Pts);

//----------------------------------------------------------------------
//  Global parameters (some are set in getArgs())
//----------------------------------------------------------------------

int       k      = 256;   // number of centers
const int MAXLEV = 11;    //maximum n-gram size

//----------------------------------------------------------------------
//  Main program
//----------------------------------------------------------------------

void usage(const char *msg = 0) {
  if (msg) { std::cerr << msg << std::endl; }
  std::cerr << "Usage: quantize-lm input-file.lm [output-file.qlm]" << std::endl;
  if (!msg) std::cerr << std::endl
    << "  quantize-lm reads a standard LM file in ARPA format and produces" << std::endl
    << "  a version of it with quantized probabilities and back-off weights"<< std::endl
    << "  that the IRST LMtoolkit can compile." << std::endl;
  }


int main(int argc, const char **argv)
{

  //Process Parameters 
  
  if (argc < 2) { usage(); exit(1); }
  std::vector<std::string> files;
  for (int i=1; i < argc; i++) {
    std::string opt = argv[i];
    files.push_back(opt);
  }
  if (files.size() > 2) { usage("Too many arguments"); exit(1); }
  if (files.size() < 1) { usage("Please specify a LM file to read from"); exit(1); }
  
  
  std::string infile = files[0];
  if (files.size() == 1) {
    std::string::size_type p = infile.rfind('/');
    if (p != std::string::npos && ((p+1) < infile.size())) {
      files.push_back(infile.substr(p+1) + ".qlm");
    } else {
      files.push_back(infile + ".qlm");
    }
  }
  
  
  std::cout << "Reading " << infile << "..." << std::endl;
  
  std::fstream inp(infile.c_str());
  if (!inp.good()) {
    std::cerr << "Failed to open " << infile << "!\n";
    exit(1);
  }
  
  std::string outfile = files[1];
  std::ofstream out(outfile.c_str());
  std::cout << "Writing " << outfile << "..." << std::endl;

  int nPts = 0;  // actual number of points
  
  // *** Read ARPA FILE ** 
  
  int numNgrams[MAXLEV + 1]; /* # n-grams for each order */
  int Order,MaxOrder;
  int n;
  
  float logprob,logbow, logten=log(10.0);
  
  double* dataPts=NULL;
  double* centersP=NULL; double* centersB=NULL;
  
  int* mapP=NULL; int* mapB=NULL;
  
  int centers=k;
  streampos iposition;
  
  out << "qARPA\n"; //print output header
  
  for (int i=1;i<=MAXLEV;i++) numNgrams[i]=0;
  
  char line[1024];
  
  while (inp.getline(line,1024)){
    
    bool backslash = (line[0] == '\\');
    
    if (sscanf(line, "ngram %d=%d", &Order, &n) == 2) {
      numNgrams[Order] = n;
      MaxOrder=Order;
    }
    
    if (backslash && sscanf(line, "\\%d-grams", &Order) == 1) {
      
      out << line << "\n";
      cerr << "-- Start processing of " << Order << "-grams\n";
      assert(Order <= MAXLEV);
      
      int N=numNgrams[Order];
      centers=k;
      if (Order==1) centers=256; // always use 256 centers
      
      char* words[MAXLEV+3];
      dataPts=new double[N]; // allocate data   
      
      iposition=inp.tellg();
      
      for (nPts=0;nPts<N;nPts++){
        inp.getline(line,1024);
        int howmany = parseWords(line, words, Order + 3);
        assert(howmany == Order+2 || howmany == Order+1);
        sscanf(words[0],"%f",&logprob);
        dataPts[nPts]=exp(logprob * logten);
      }
      
      cerr << "quantizing " << N << " probabilities\n";
      
      centersP=new double[centers];
      mapP=new int[N];
      
      ComputeCluster(centers,centersP,N,dataPts);
      
      
      assert(bintable !=NULL);
      for (int p=0;p<N;p++){
        mapP[bintable[p].idx]=bintable[p].code;
      }
      
      if (Order<MaxOrder){
        
        inp.seekg(iposition);
        
        for (nPts=0;nPts<N;nPts++){
          inp.getline(line,1024);
          int howmany = parseWords(line, words, Order + 3);
          if (howmany==Order+2) //backoff is written
            sscanf(words[Order+1],"%f",&logbow);
          else
            logbow=0; // backoff is implicit
          dataPts[nPts]=exp(logbow * logten);
        }
        
        centersB=new double[centers];
        mapB=new int[N];
        
        cerr << "quantizing " << N << " backoff weights\n";
        ComputeCluster(centers,centersB,N,dataPts);
        
        assert(bintable !=NULL);
        for (int p=0;p<N;p++){
          mapB[bintable[p].idx]=bintable[p].code;
        }
        
      }
      
      inp.seekg(iposition);
      
      out << centers << "\n";
      for (nPts=0;nPts<centers;nPts++){
        out << log(centersP[nPts])/logten;
        if (Order<MaxOrder) out << " " << log(centersB[nPts])/logten;
        out << "\n";
      }
      
      for (nPts=0;nPts<numNgrams[Order];nPts++){
        
        inp.getline(line,1024);
        
        parseWords(line, words, Order + 3);
        
        out << mapP[nPts];
        
        for (int i=1;i<=Order;i++) out << "\t" << words[i];
        
        if (Order < MaxOrder) out << "\t" << mapB[nPts];
        
        out << "\n";
        
      }
            
      if (mapP){delete [] mapP;mapP=NULL;}
      if (mapB){delete [] mapB;mapB=NULL;}
      
      if (centersP){delete [] centersP; centersP=NULL;}
      if (centersB){delete [] centersB; centersB=NULL;}
      
      delete [] dataPts;
      
      continue;
      
      
    }
    
    out << line << "\n";
  }
  
  cerr << "---- done\n";
  
  out.flush();
  inp.flush();
  
  out.close();
  inp.close();
  
}

// Compute Clusters

int ComputeCluster(int centers,double* ctrs,int N,double* dataPts){
  
  
  //cerr << "\nExecuting Clutering Algorithm:  k=" << centers<< "\n";
  
  if (bintable) delete [] bintable;
  
  bintable=new BinEntry[N];
  for (int i=0;i<N;i++){
    bintable[i].pt=dataPts[i];
    bintable[i].idx=i;
    bintable[i].code=0;
  }
  
  //cout << "start sort \n";
  qsort(bintable,N,sizeof(BinEntry),cmpBinEntry);
  
  int different=1;
  
  for (int i=1;i<N;i++)
    if (bintable[i].pt!=bintable[i-1].pt)
      different++;
  
  int interval=different/centers;
  if (interval==0) interval++;
  
  int* population=new int[centers];    
  int* species=new int[centers];    
  
  //cout << " Different entries=" << different 
  //     << " Total Entries=" << N << " Bin Size=" << interval << "\n";
  
  for (int i=0;i<centers;i++){
    population[i]=species[i]=0;
    ctrs[i]=0.0;
  }
  
  // initial values
  bintable[0].code=0;    
  population[0]=1;
  species[0]=1;
  
  int currcode=0;    
  different=1;
  
  for (int i=1;i<N;i++){
    
    if ((bintable[i].pt!=bintable[i-1].pt)){
      different++;
      if ((different % interval) == 0)
        if ((currcode+1) < centers 
            && 
            population[currcode]>0){
                currcode++;
        }
    }
      
      if (bintable[i].pt == bintable[i-1].pt)
        bintable[i].code=bintable[i-1].code;
      else{
        bintable[i].code=currcode;
        species[currcode]++;
      }
      
      population[bintable[i].code]++;
      
      assert(bintable[i].code < centers);
      
      ctrs[bintable[i].code]+=bintable[i].pt;
      
  }
    
    
    for (int i=0;i<centers;i++){
      if (population[i]>0)
        ctrs[i]/=(float)population[i];
      //cout << i << " population " << population[i] << " species " << species[i] <<"\n";
    }
    
    cout.flush();
    
    delete [] population;
    
    return 1;
    
}

//----------------------------------------------------------------------
//  Reading/Printing utilities
//      readPt - read a point from input stream into data storage
//              at position i.  Returns false on error or EOF.
//      printPt - prints a points to output file
//----------------------------------------------------------------------


int parseWords(char *sentence, char **words, int max)
{
  char *word;
  int i = 0;
  
  char *const wordSeparators = " \t\r\n";
  
  for (word = strtok(sentence, wordSeparators);
       i < max && word != 0;
       i++, word = strtok(0, wordSeparators))
  {
    words[i] = word;
  }
  if (i < max) {
    words[i] = 0;
  }
  
  return i;
}

