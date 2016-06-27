#ifndef EDDA_OSUFLOW
#define EDDA_OSUFLOW

class OSUFlow;
class EddaFileReader{
public:
	void loadData(const char* fname, OSUFlow *osuflow);
};


#endif //EDDA_OSUFLOW
