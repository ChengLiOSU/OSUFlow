#include "EddaVectorField.h"
#include "common.h"

EddaVectorField::EddaVectorField(std::shared_ptr<edda::Dataset<edda::VECTOR3> > _eddaDataset, int timesteps, int min_t)
{
	CVectorField();

	eddaDataset = _eddaDataset;

	//case: RegularCartesianGrid
	int *m_nDimension = eddaDataset->getDimension();
	//RegularCartesianGrid*
	m_pGrid = new RegularCartesianGrid(m_nDimension[0], m_nDimension[1], m_nDimension[2]);

	edda::VECTOR3 minB, maxB;
	eddaDataset->getGrid()->boundary(minB, maxB);
	VECTOR3 min_b, max_b;
	min_b[0] = minB[0];
	min_b[1] = minB[1];
	min_b[2] = minB[2];
	max_b[0] = maxB[0];
	max_b[1] = maxB[1];
	max_b[2] = maxB[2];
	m_pGrid->SetBoundary(min_b, max_b);

	/*
	edda::VECTOR4 realMinB, realMaxB;
	eddaDataset->getGrid()->realBoundary(realMinB, realMaxB);
	VECTOR4 realMin_b, realMax_b;
	realMin_b[0] = realMinB[0];
	realMin_b[1] = realMinB[1];
	realMin_b[2] = realMinB[2];
	realMin_b[3] = realMinB[3];
	realMax_b[0] = realMaxB[0];
	realMax_b[1] = realMaxB[1];
	realMax_b[2] = realMaxB[2];
	realMax_b[3] = realMaxB[3];
	m_pGrid->SetRealBoundary(realMin_b, realMax_b);
	*/

	
	m_nTimeSteps = timesteps;
	m_bIsNormalized = false;
	m_MinT = min_t;  
	m_MaxT = min_t + timesteps - 1;
}

int EddaVectorField::at_phys(const int fromCell,
	VECTOR3& pos,
	PointInfo& pInfo,
	const float t, VECTOR3& nodeData)
{
	vector<VECTOR3> vNodeData;

	const edda::VECTOR3 eddaPos(pos[0], pos[1], pos[2]);
	edda::VECTOR3 output;
	edda::ReturnStatus rs = eddaDataset->at_phys(eddaPos, output);
	
	nodeData[0] = output[0];
	nodeData[1] = output[1];
	nodeData[2] = output[2];

	if (rs == edda::ReturnStatus::SUCCESS)
		return 1;
	else
		return -1;
}