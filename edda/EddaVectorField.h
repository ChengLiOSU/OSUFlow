#ifndef EDDA_VECTOR_FIELD
#define EDDA_VECTOR_FIELD

/*
This class create a vector field class that inherites CVectorField. Since several OSUFlow computing classes operate on a CVectorField member, setting the CVectorField member as this inheritated EddaVectorField object, gives the computing class the ability to retrieve attributes from the edda dataset.

A example is provided using the vtCStreamLine class. When creating an object of the vtCStreamLine, its vector field member is given as an EddaVectorField object.

Creted by
Cheng Li (The Ohio State University)
June, 2016
*/

#include "Field.h"
#include <memory>

#include "dataset/dataset.h"
#include "core/vector_matrix.h"


class EddaVectorField :public CVectorField
{
public:
	//currently works for steady flow and Regular Cartisan grid
	EddaVectorField(std::shared_ptr<edda::Dataset<edda::VECTOR3> > _eddaDataset, int timesteps, int min_t = 0);

	std::shared_ptr<edda::Dataset<edda::VECTOR3> > eddaDataset;

	int at_phys(const int fromCell, VECTOR3& pos, PointInfo& pInfo, const float t, VECTOR3& nodeData) override;
};


#endif //EDDA_VECTOR_FIELD
