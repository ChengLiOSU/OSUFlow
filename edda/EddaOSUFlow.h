#ifndef EDDA_OSUFLOW
#define EDDA_OSUFLOW

/*
This class load the edda file into an OSUFlow object. Currently support loading each vector component as a sample result

Creted by
Cheng Li (The Ohio State University)
June, 2016
*/

class OSUFlow;
class EddaFileReader
{
public:
	void loadData(const char* fname, OSUFlow *osuflow);
};


#endif //EDDA_OSUFLOW
