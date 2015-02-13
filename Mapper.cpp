#include "Mapper.h"
#include <cmath>
#include "GlobalConstants.h"
Mapper::Mapper(){

}
/*
Mapping: 4x40 per fec, 4x8 per sampa..

1 - 2 - 3 - 4.....



*/
/*PadAddr Mapper::getPadFromAddr(int channel, int sampa, int fec){

	int sampaLowerLeft = (constants::PAD_PER_ROW * constants::ROW_PER_SAMPA * sampa) + 1;
	int lowerLeftPad = sampaLowerLeft + (fec * 4);
	int sampaRow = (sampa * 8) + 1;


	int channelRowNumber = ceil((channel + 1)/4.0);
	int channelColumnNumber = (channel % 4);

	int padRow =  sampaRow + channelRowNumber - 1;
	//pad not calculated correctly. FIX THIS ASAP!
	int pad = lowerLeftPad + ((channelRowNumber-1) * 48) + channelColumnNumber;

	return PadAddr(pad, padRow);

}

void Mapper::padToAddrMapping(int* channel, int* sampa, int* fec, PadAddr pad){
	int padNr = pad.getPad();
	int padRow = pad.getPadRow();

	int tempFec = ceil(padNr/48);

}*/

int Mapper::getSampaChannel(uint16_t hw){
	std::ifstream iff("altro-sampa-mapping.data");
	std::string line;

	while(!iff.eof()){
		std::getline(iff, line);
		std::cout << "Line: " << line;
		int currentHw = std::stoi(line.substr(0, line.find(" ")));

		if(currentHw == hw){
			return std::stoi(line.substr(line.find(" ")));
		}
	}
	iff.close();
	return -1;
}

unsigned char Mapper::getBranch(uint16_t data){
	return (data >> 11);
}

unsigned char Mapper::getFec(uint16_t data){
	return (data >> 7) & 0xf;
}

unsigned char Mapper::getAltro(uint16_t data){
	return (data >> 4) & 7;
}

unsigned char Mapper::getChannel(uint16_t data){
	return data & 0xf;
}
