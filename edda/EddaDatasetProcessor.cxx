#include "EddaDatasetProcessor.h"
#include <memory>
#include <string>

#include "edda.h"
#include "dataset/dataset.h"
#include "io/edda_reader.h"
#include "core/vector_matrix.h"

using namespace std;

using namespace edda;

float ** EddaDatasetProcessor::eddaDatasetInfo(const char* fname, bool bStatic, float *sMin, float *sMax,
	float *dim, int &min_t, int &max_t)
{

	shared_ptr<Dataset<VECTOR3> > dataset = loadEddaVector3Dataset(fname, "");

	VECTOR3 minB, maxB;
	dataset->getGrid()->boundary(minB, maxB);
	sMin[0] = minB[0];
	sMin[1] = minB[1];
	sMin[2] = minB[2];
	sMax[0] = maxB[0];
	sMax[1] = maxB[1];
	sMax[2] = maxB[2];

	int *m_nDimension = dataset->getDimension();
	dim[0] = m_nDimension[0];
	dim[1] = m_nDimension[1];
	dim[2] = m_nDimension[2];

	float** ppData;
	if (bStatic) {

		int numTimesteps = max_t - min_t + 1;
		ppData = new float*[numTimesteps];

		min_t = 0;
		max_t = 0;

		/// Only for structured grids.
		float *pData = new float[m_nDimension[0] * m_nDimension[1] * m_nDimension[2] * 3];
		for (int k = 0; k < m_nDimension[2]; k++){
			for (int j = 0; j < m_nDimension[1]; j++){
				for (int i = 0; i < m_nDimension[0]; i++){

					int ind = k*m_nDimension[1] * m_nDimension[0] + j*m_nDimension[0] + i;
					std::vector<dist::Variant> vvari = (dataset->getArray())->getVector(ind);
					pData[3 * ind] = boost::get<float>(vvari[0]);//or dist::getSample(vvari[0]);
					pData[3 * ind + 1] = boost::get<float>(vvari[1]);
					pData[3 * ind + 2] = boost::get<float>(vvari[2]);

					////may add when mean is implemented in later version of edda
					//pData[3 * ind] = dist::getMean(vvari[0]);
					//pData[3 * ind + 1] = dist::getMean(vvari[1]);
					//pData[3 * ind + 2] = dist::getMean(vvari[2]);
				}
			}
		}
		ppData[min_t] = pData;
	}
	else {
		////TODO later
		ppData = NULL;
	}

	return ppData;

	return NULL;
}