#ifndef EDDA_DATASET_PROCESSOR
#define EDDA_DATASET_PROCESSOR

/*
Since there are multiple naming conflict between OSUFlow and Edda, this class is designed to take Edda functions but no OSUFlow functions. Other parts of the project OSUFlowEdda should take OSUFlow functions but no Edda functions.
*/

class EddaDatasetProcessor{
public:
	float ** eddaDatasetInfo(const char* fname, bool bStatic, float *sMin, float *sMax,
		float *dim, int &min_t, int &max_t);
};

#endif