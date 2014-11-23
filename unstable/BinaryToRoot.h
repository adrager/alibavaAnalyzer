 #ifndef BinaryDataToRoot_H
 #define BinaryDataToRoot_H
 
 /*
	* Class to read the binary data from the alibava
	* It calls the other run specific classes (or functions)
	* while reading the file
	*/
 
 #include "TFile.h"
 #include "TTree.h"
 #include "TH2I.h"
 #include "Config.h"
 #include "fstream"
 #include <sstream>
 #include <cstdio> 
 #include "istream"
 #include "string"
 #include "stdint.h"
 const int nChannels = 256;
 const int totHeadBits = 32;
 
 class BinaryToRoot
 { 
	 static const std::streamsize sizeInt32 = 4;
	 static const std::streamsize sizeUint32 = 4;
	 static const std::streamsize sizeInt16 = 2;
	 static const std::streamsize sizeUint16 = 2;
	 static const std::streamsize sizeChar = 1;
	 static const std::streamsize sizeDouble = 8;

	 std::streamsize sizeTime_; // left as variable since is architecture dependent -> must be specified in the config file
	 
	 enum BlockType {NewFile = 0, StartOfRun, Data, CheckPoint, EndOfRun};
 protected:
	 Float_t convertTime(uint32_t rawT);
	 Float_t convertTemp(uint16_t rawT);
 public:
	 BinaryToRoot(const char* InFileName,std::streamsize sizeTime);
	 ~BinaryToRoot();
	 
	 void ReadFile();
 private:
	 std::ifstream InputFile_;
	 std::ifstream* InputFileTest_;
	 std::string InputFileName_;
	 Config::Config* config_;
	 TFile* outFile_;
	 TTree* tree_;
	 TString tTemp_;
	 Float_t time; // event time (phase between trigger and clock)
	 Float_t temp; // chip temperature
	 
	 TH2I* allEvents;
	 UInt_t adcPH[nChannels];
	 UInt_t headers[totHeadBits]; // headers of the chips
	 
	 std::string runHeader_, line_;
	 int runType_;
	 
	 Float_t injCharge; // charge injection for calibration
	 Float_t delay; // delay for laser delay scan
	 

 };
 
 #endif //#ifndef BINARYDATA_HH
 