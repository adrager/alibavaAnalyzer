#ifndef Config_H
#define Config_H
#include "TFile.h"
#include "TTree.h"
#include "TH2I.h"

#include "fstream"
#include "string"
#include "stdint.h"


class Config
{ 
  static const std::streamsize sizeInt32 = 4;
  static const std::streamsize sizeUint32 = 4;
   // left as variable since is architecture dependent -> must be specified in the config file
protected:
  
public:
  Config(std::streamsize sizeTime);
  ~Config();
  std::streamsize getSizeTime(){return sizeTime_;}
  
private:
  std::streamsize sizeTime_;
};
#endif //#ifndef BINARYDATA_HH