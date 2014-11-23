#include "Config.h"

#include "iostream"
#include "time.h"
#include "stdlib.h"
#include "stdint.h"
#include "algorithm"

#include "TF1.h"
#include "TCanvas.h"
#include "TString.h"
Config::Config(std::streamsize sizeTime)
{
  sizeTime_ =sizeTime;
  std::cout<<"Config started"<<std::endl;
}

Config::~Config()
{
  std::cout<<"Config closed"<<std::endl;
}

