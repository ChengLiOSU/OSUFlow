/*
 * StreamlineSimpleVTKUnstructured.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: Chun-Ming Chen
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <iterator>

#include "OSUFlowVTK.h"
#include "vtkDataSet.h"
#include "vtkStructuredGrid.h"
#include "vtkMultiBlockPLOT3DReader.h"
#include "vtkMultiBlockDataSet.h"
#include <vtkInterpolatedVelocityField.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkFunctionSet.h>
#include <vtkImageInterpolator.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkCellLocatorInterpolatedVelocityField.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

using namespace std;

#define PARAVIEW_DATA_ROOT "/home/jchen/project/ParaViewData/"

#if 0
vtkDataSet *getData_ex2()
{
	vtkExodusIIReader *reader = vtkExodusIIReader::New();
	reader->SetFileName(PARAVIEW_DATA_ROOT "Data/disk_out_ref.ex2");
	reader->GenerateObjectIdCellArrayOn();

	reader->Update();

	vtkMultiBlockDataSet *mbdata = reader->GetOutput();
	if (mbdata==0) {
		printf("multiblock data error\n");
	}

	vtkDataSet *data;
	printf("Number of blocks=%d\n", mbdata->GetNumberOfBlocks());

	for (int i=0; i<mbdata->GetNumberOfBlocks(); i++)
	{
		mbdata->GetBlock(i)->Print(cout);
	}


	data = vtkUnstructuredGrid::SafeDownCast( vtkMultiBlockDataSet::SafeDownCast( mbdata->GetBlock(0) )->GetBlock(0) );
	data->Print(cout);


	//reader->Delete();

	// test
	vtkCellLocatorInterpolatedVelocityField *interpolator = vtkCellLocatorInterpolatedVelocityField::New();
	interpolator->AddDataSet(data);
	double vel[3];
	double coords[4] = {-5,-5,-10,0};
	for (int k=0; k<3; k++)
	{
		bool success = ((vtkFunctionSet *)interpolator)->FunctionValues(coords, vel);
		printf("querry success = %d, vec = %lf %lf %lf\n", success, vel[0], vel[1], vel[2]);
	}


	reader->Delete();
	return data;
}
#endif
vtkDataSet *getData_vtu()
{
	vtkXMLUnstructuredGridReader *reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName("/home/jchen/project/data/disk/disk_0_0.vtu");
	reader->Update();

	vtkUnstructuredGrid *data = reader->GetOutput();
	if (data==0) {
		printf("data error\n");
	}
	data->Print(cout);

	data->GetPointData()->Print(cout);
	int r = data->GetPointData()->SetActiveAttribute("V", vtkDataSetAttributes::VECTORS);
	printf("index=%d\n", r);

	//reader->Delete();
	// test
	vtkCellLocatorInterpolatedVelocityField *interpolator = vtkCellLocatorInterpolatedVelocityField::New();
	interpolator->AddDataSet(data);
	double vel[3];
	double coords[4] = {0,0,0,0};
	for (int k=0; k<3; k++)
	{
		bool success = ((vtkFunctionSet *)interpolator)->FunctionValues(coords, vel);
		printf("querry success = %d, vec = %lf %lf %lf\n", success, vel[0], vel[1], vel[2]);
	}

	reader->Delete();
	return data;
}

int main()
{
	//vtkSmartPointer<vtkDataSet> data = getData_plot3d();
	//vtkSmartPointer<vtkDataSet> data = getData_vts();
	//vtkSmartPointer<vtkDataSet> data = getData_vti(); // NOT WORKING : image and polydata are interpolated differently
	vtkSmartPointer<vtkDataSet> data = getData_vtu();


	OSUFlowVTK *osuflow = new OSUFlowVTK;
	osuflow->setData(data);

	// gen seeds
	VECTOR3 minB, maxB;
	osuflow->Boundary(minB, maxB);
	printf("bounds: %lf %lf %lf %lf %lf %lf\n", minB[0], maxB[0], minB[1], maxB[1], minB[2], maxB[2]);

	osuflow->SetRandomSeedPoints(&minB[0], &maxB[0], 100);


	// openmp
#ifdef _OPENMP
	osuflow->initOpenMP(8);
#endif

	// init results
	list<vtListSeedTrace*> list;
	osuflow->SetIntegrationParams(1, 5);
	osuflow->GenStreamLines(list , BACKWARD_AND_FORWARD, 50, 0);
	printf(" done integrations\n");
	printf("list size = %d\n", (int)list.size());

	std::list<vtListSeedTrace*>::iterator pIter;


	pIter = list.begin();
	for (; pIter!=list.end(); pIter++) {
		vtListSeedTrace *trace = *pIter;
		std::list<VECTOR3*>::iterator pnIter;
		pnIter = trace->begin();
		for (; pnIter!=trace->end(); pnIter++) {
		  VECTOR3 p = **pnIter;
		  printf(" %f %f %f ", p[0], p[1], p[2]);
		}
		printf("\n");
	}

	delete osuflow;
}



