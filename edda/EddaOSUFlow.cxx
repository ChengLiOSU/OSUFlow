
#include "EddaOSUFlow.h"
#include "OSUFlow.h"
#include "EddaDatasetProcessor.h"

void EddaFileReader::loadData(const char* fname, OSUFlow *osuflow)
{
	//default values. may extend in the future
	bool bStatic = true;

	float sMin[3], sMax[3];
	float dim[3];
	int min_t, max_t;
	DataMode mode = DataMode::RAW;
	float **data;

	EddaDatasetProcessor edp;
	data = edp.eddaDatasetInfo(fname, bStatic, sMin, sMax, dim, min_t, max_t);

	osuflow->LoadData(fname, sMin, sMax, dim, min_t, max_t, mode, data);

	//delete[] data; //should not delete stored pointers in data
}