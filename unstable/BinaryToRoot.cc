#include "BinaryToRoot.h"

#include<iostream>

#include "time.h"
#include "stdlib.h"
#include "stdint.h"
#include "algorithm"

#include "TF1.h"
#include "TCanvas.h"
#include "TString.h"
using namespace std;
BinaryToRoot::BinaryToRoot(const char* binFile, std::streamsize sizeTime)
{
	InputFileName_=binFile;
	std::cout<<"BinaryToRoot started"<<std::endl;
	config_ = new Config(sizeTime);
	// ================ open the file stream ===============
	InputFileTest_ = new std::ifstream(InputFileName_.c_str(), std::ios::in | std::ios::binary);
	if(InputFileTest_->is_open() == true)
	{
		uint32_t dbHead =0;
		char c;
		int i=0;
		while (InputFileTest_->get(&c,sizeUint32))
		{ 
			i++;
		//	std::cout<<"Input: "<<i<<" : "<<c<<std::endl;
		}
	}

	InputFile_.open(InputFileName_.c_str(), std::ifstream::in);
	if(InputFile_.is_open() == false)
	{
	  std::cout << "Error input file: " << InputFileName_ <<" could not be opend."<< std::endl;
		exit(1);
	}
	std::string temp = binFile;
	tTemp_ = temp+".root";
	outFile_ = new TFile(tTemp_,"RECREATE");
	tree_ = new TTree("RawTree", "Raw data events");
	tree_->Branch("injCharge", &injCharge, "injCharge/F");
	tree_->Branch("delay", &delay, "delay/F");
	tree_->Branch("time", &time, "time/F");
	tree_->Branch("adcPH", adcPH, TString::Format("adcPH[%i]/i", nChannels)); // trick with the string to have the right array length
	tree_->Branch("temp", &temp, "temp/F");
	tree_->Branch("headers", headers, TString::Format("headers[%i]/i", totHeadBits)); // trick with the string to have the right array length
	
	allEvents = new TH2I("allEvents", "Raw PH all channels;Channel;Pulse height [ADC]", 256, -0.5, 255.5, 1024, -0.5, 1023.5);
}

BinaryToRoot::~BinaryToRoot()
{
	std::cout<<"BinaryToRoot closed"<<std::endl;
	outFile_->cd();
	tree_->Write();
	InputFile_.close();
	outFile_->Close();
}



void BinaryToRoot::ReadFile()
{
	// =================================== file header =======================================
	time_t date;
	int32_t type;
	uint32_t headerLength;
	char* header;
	int version;
	
	InputFile_.read((char*) &date, config_->getSizeTime());
	InputFile_.read((char*) &type, sizeInt32);
	InputFile_.read((char*) &headerLength, sizeInt32);
	
	header = new char[headerLength];
	InputFile_.read(header, headerLength);
	
	runHeader_ = header;
	runType_ = type;
	if(header[0] != 'v' && header[0] != 'V') version = 0;
	else  version = atoi(&header[1]);
	
	std::cout << "====================== File header ====================" << std::endl;
	
	std::cout <<  "Time = " << ctime(&date);
	std::cout <<  "Type = " << type << " -> ";
	switch(type)
	{
		case 1:
			std::cout << "calibration run" << std::endl;
			break;
		case 2:
			std::cout << "laser sync. run" << std::endl;
			break;
		case 3:
			std::cout << "laser run" << std::endl;
			break;
		case 4:
			std::cout << "radioactive source run" << std::endl;
			break;
		case 5:
			std::cout << "pedestal run" << std::endl;
			break;
		default:
			std::cout << "unknown run type" << std::endl;
	}
	std::cout <<  "Header = " << header << std::endl;
	std::cout <<  "Version = " << version << std::endl;
/*	InputFile_.clear();
	InputFile_.seekg(0,ios::beg);
	if(InputFile_.is_open())
	{
	  while (getline (InputFile_,line_) )
	  {
	  //  std::cout<<line_<<std::endl;
	  }
	}
	InputFile_.clear();
	InputFile_.seekg(0,ios::beg);
	*/
  // matteo code
  // =============================== pedestals and noise =============================================
  double ped[nChannels];
	double noise[nChannels];
	
	InputFile_.read((char*) &ped, sizeDouble * nChannels);
	InputFile_.read((char*) &noise, sizeDouble * nChannels);
	
	// ============================= data blocks ===============================================================
	
	uint32_t dbHead = 0;
	unsigned int dbType;
	bool isUserDb;
	uint32_t dbSize;
	char* dataBlock;
	int evtCount = 0;
	
	double scanValue;
	uint32_t rawTime;
	uint16_t rawTemp;
	uint16_t tempAdcPH[nChannels]; // temporary data storage to have conversion from uint16_t to UInt_t
	uint16_t tempHead[totHeadBits]; // see above, but for the headers
	
	if(version == 0)
	{
		std::cout << "Version is 0 => Data are not going to be readen (so it is in the alibava code)" << std::endl;
		InputFile_.close();
		return;
	}
	
	if(version == 1)
	{
		std::cout << "Version is 1 => The program has no implementation for these data, the alibava code contains one" << std::endl;
		InputFile_.close();
		return;
	}
	
	do // loop to read the file to the end
    {
			try
			{
				do // find the data block header
	    {
				InputFile_.read((char*) &dbHead, sizeUint32);
				if(!InputFile_.good())
				{
					std::cout << "File stream not good while looking for data block header" << std::endl;
					throw 1;
				}
			} while((dbHead & 0xffff0000) != 0xcafe0000); // find the data block header
			}
			
			catch(int a)
			{
				std::cout << "Exception catched: " << a << std::endl;
				break;
			}
			
			dbType = dbHead & 0xfff;
			isUserDb = dbHead & 0x1000;
			
			InputFile_.read((char*) &dbSize, sizeUint32);
			
			switch(dbType) // if the data block is not data is just readen and trown away
	{
				case NewFile:
					dataBlock = new char[dbSize];
					InputFile_.read(dataBlock, dbSize);
					delete dataBlock;
					break;
				case StartOfRun:
					dataBlock = new char[dbSize];
					InputFile_.read(dataBlock, dbSize);
					delete dataBlock;
					break;
				case Data:
					if(isUserDb) // user data, structure not known
	    {
				dataBlock = new char[dbSize];
				InputFile_.read(dataBlock, dbSize);
				delete dataBlock;
				break;
			}
			
			InputFile_.read((char*) &scanValue, sizeDouble);
			injCharge = (double) (int(scanValue) & 0xff);
			injCharge *= 1024;
			delay = (double) (int(scanValue) >> 16);
			
			InputFile_.read((char*) &rawTime, sizeUint32);
			InputFile_.read((char*) &rawTemp, sizeUint16);
			
			time = convertTime((uint32_t)rawTime);
			temp = convertTemp((uint16_t)rawTemp);
			
			for(int ichip = 0; ichip < 2; ++ichip)
			{
				InputFile_.read((char*) &tempHead[ichip * (totHeadBits / 2)], (totHeadBits / 2) * sizeUint16);
				InputFile_.read((char*) &tempAdcPH[ichip * (nChannels / 2)], (nChannels / 2) * sizeUint16);
			}
			
			for(int ichan = 0; ichan < nChannels; ++ichan)
			{
				adcPH[ichan] = tempAdcPH[ichan];
				allEvents->Fill(ichan, adcPH[ichan]);
			}
			
			for(int iBit = 0; iBit < totHeadBits; ++iBit)
				headers[iBit] = tempHead[iBit];
			
			evtCount++;
			tree_->Fill();
			break;
				case CheckPoint:
					dataBlock = new char[dbSize];
					InputFile_.read(dataBlock, dbSize);
					delete dataBlock;
					break;
				case EndOfRun:
					dataBlock = new char[dbSize];
					InputFile_.read(dataBlock, dbSize);
					delete dataBlock;
					break;
				default:
					std::cout << "Data block type unknown" << std::endl;
	}
		} while(InputFile_.good());
		
		std::cout << "==================================================" << std::endl;
		std::cout << "Total events = " << evtCount << std::endl;
		
		delete header;
		
		return;
}

Float_t BinaryToRoot::convertTime(uint32_t rawT)
{
	int16_t ipart = (rawT & 0xffff0000) >> 16;
	if(ipart < 0) ipart *= -1;
	uint16_t fpart = rawT & 0xffff;
	return 100 * (ipart + fpart / 65535.);
}

Float_t BinaryToRoot::convertTemp(uint16_t rawT)
{
	return 0.12 * rawT - 39.8;
}
