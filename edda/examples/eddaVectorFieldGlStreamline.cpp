/*
eddaStreamline:
The is a demo to show how to create an eddaVectorField object by an edda dataset. then a vtCStreamLine object is create by this eddaVectorField to compute streamlines to be rendered.

Creted by
Cheng Li (The Ohio State University)
June, 2016
*/

#include <list>
#include <iterator>

#include "gcb.h"
#include "LineRendererInOpenGL.h"

#include "EddaVectorField.h"
#include "FieldLine.h"

#include "edda.h"
#include "dataset/dataset.h"
#include "io/edda_reader.h"
#include "core/vector_matrix.h"

std::shared_ptr<edda::Dataset<edda::VECTOR3> > dataset; 
EddaVectorField *flowField;
vtCStreamLine* pStreamLine;

VECTOR3 minLen, maxLen;
float center[3], len[3];
CLineRendererInOpenGL cLineRenderer;
list<vtListSeedTrace*> sl_list;
list<VECTOR4> liv4Colors;


////////////////////////////////////////////////////////////////////////////
void compute_streamlines()
{
	LOG("");

	//create seeds
	edda::VECTOR3 minB, maxB;
	dataset->getGrid()->boundary(minB, maxB);
	unsigned int numSeeds[3] = {10, 10, 1 };
	int	nSeeds = numSeeds[0] * numSeeds[1] * numSeeds[2];
	VECTOR3 *seeds = new VECTOR3[nSeeds];
	for (int k = 0; k < numSeeds[2]; k++){
		for (int j = 0; j < numSeeds[1]; j++){
			for (int i = 0; i < numSeeds[0]; i++){
				int ind = k*numSeeds[1] * numSeeds[0] + j*numSeeds[0] + i;
				seeds[ind] = VECTOR3(
					minB[0] + (maxB[0] - minB[0])*1.0 / (numSeeds[0] + 1)*(i + 1),
					minB[1] + (maxB[1] - minB[1])*1.0 / (numSeeds[1] + 1)*(j + 1),
					minB[2] + (maxB[2] - minB[2])*1.0 / (numSeeds[2] + 1)*(k + 1));
			}
		}
	}
	for (int i = 0; i<nSeeds; i++)
		printf(" seed no. %d : [%f %f %f]\n", i, seeds[i][0],
		seeds[i][1], seeds[i][2]);


	//using the vtCStreamLine to compute the streamlines
	printf("compute streamlines..\n");
	unsigned int randomSeed = 0;
	pStreamLine->setBackwardTracing(false);
	pStreamLine->SetInitialStepSize(1);
	pStreamLine->SetMaxStepSize(5);
	pStreamLine->setMaxPoints(500);
	float currentT = 0.0;
	pStreamLine->setSeedPoints(seeds, nSeeds, currentT, NULL);
	sl_list.clear();
	pStreamLine->execute((void *)&currentT, sl_list, NULL);


	printf(" done integrations\n");
	printf("list size = %d\n", (int)sl_list.size());

	// ADD-BY-LEETEN 07/07/2010-BEGIN
	int n = sl_list.size(); // Jimmy added: list.size() takes O(n) time
	for (int i = 0; i < n; i++)
	{
		VECTOR4 v4Color;
		switch ((i / 2) % 7)
		{
		case 0: v4Color = VECTOR4(1.0f, 0.0f, 0.0f, 1.0f);	break;
		case 1: v4Color = VECTOR4(0.0f, 1.0f, 0.0f, 1.0f);	break;
		case 2: v4Color = VECTOR4(0.0f, 0.0f, 1.0f, 1.0f);	break;
		case 3: v4Color = VECTOR4(1.0f, 1.0f, 0.0f, 1.0f);	break;
		case 4: v4Color = VECTOR4(1.0f, 0.0f, 1.0f, 1.0f);	break;
		case 5: v4Color = VECTOR4(0.0f, 1.0f, 1.0f, 1.0f);	break;
		case 6: v4Color = VECTOR4(1.0f, 1.0f, 1.0f, 1.0f);	break;
		}
		liv4Colors.push_back(v4Color);
	}
	// ADD-BY-LEETEN 07/07/2010-END
	cLineRenderer._Update();
}

void draw_streamlines()
{
	glPushAttrib(
		GL_LIGHTING_BIT |
		0
		);

	cLineRenderer._Draw();

	glPopAttrib();
}


///////////////////////////////////////////////////////////////////////////////
void
_KeyboardFunc(unsigned char ubKey, int iX, int iY)
{
	switch (ubKey)
	{
	case 's':
		compute_streamlines();
		glutPostRedisplay();
		break;

	case 'h':
	{
				int iHalo;
				cLineRenderer._GetInteger(CLineRenderer::ENABLE_HALO, &iHalo);
				iHalo = !iHalo;
				cLineRenderer._SetInteger(CLineRenderer::ENABLE_HALO, iHalo);
	}
		glutPostRedisplay();
		break;

	case 'l':
	{
				int iLighting;
				cLineRenderer._GetInteger(CLineRenderer::ENABLE_LIGHTING, &iLighting);
				iLighting = !iLighting;
				cLineRenderer._SetInteger(CLineRenderer::ENABLE_LIGHTING, iLighting);
	}

		glutPostRedisplay();
		break;

	}
}

void
_DisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render the scene
	draw_streamlines();

	// NOTE: Call glutSwapBuffers() at the end of your display function
	glutSwapBuffers();
}

void
init()
{
	LOG(printf("Initialize here."));
	glEnable(GL_DEPTH_TEST);

	// setup light 0
	static GLfloat pfLightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat pfLightColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, pfLightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, pfLightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, pfLightColor);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 4.0f);
	cLineRenderer._UpdateLighting();

	// ADD-BY-LEETEN 08/14/2010-BEGIN
	LOG(printf("The vector field is ready. Press key 's' to generate the primtives."));
	// ADD-BY-LEETEN 08/14/2010-END
}

void
quit()
{
	LOG(printf("Clean up here."));
}

int
main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("usage: eddaVectorFieldGlStreamline filename\n");
		return 0;
	}

	const char* fname = argv[1];
	//recommanded sample data: "OSUFlow_Source_Dir/edda/sample_data/SW6_PIV.vti";

	//load the edda dataset
	dataset = edda::loadEddaVector3Dataset(fname, "");

	//create the EddaVectorField using the edda dataset
	flowField = new EddaVectorField(dataset, 1);
	
	//create a vtCStreamLine to compute streamlines
	pStreamLine = new vtCStreamLine((CVectorField*)flowField);


	///////////////////////////////////////////////////////////////
	// when use GCB, it is still needed to initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_STENCIL);

	edda::VECTOR3 minB, maxB;
	dataset->getGrid()->boundary(minB, maxB);
	
	minLen[0] = minB[0]; minLen[1] = minB[1];  minLen[2] = minB[2];
	maxLen[0] = maxB[0]; maxLen[1] = maxB[1];  maxLen[2] = maxB[2];
	//  osuflow->SetBoundary(minB, maxB);  // set the boundary. just to test
	// the subsetting feature of OSUFlow
	printf(" volume boundary X: [%f %f] Y: [%f %f] Z: [%f %f]\n",
		minLen[0], maxLen[0], minLen[1], maxLen[1],
		minLen[2], maxLen[2]);

	center[0] = (minLen[0] + maxLen[0]) / 2.0;
	center[1] = (minLen[1] + maxLen[1]) / 2.0;
	center[2] = (minLen[2] + maxLen[2]) / 2.0;
	printf("center is at %f %f %f \n", center[0], center[1], center[2]);
	len[0] = maxLen[0] - minLen[0];
	len[1] = maxLen[1] - minLen[1];
	len[2] = maxLen[2] - minLen[2];

	///////////////////////////////////////////////////////////////
	cLineRenderer._SetBoundingBox(
		minLen[0], minLen[1], minLen[2],
		maxLen[0], maxLen[1], maxLen[2]);
	cLineRenderer._SetDataSource(&sl_list);
	// ADD-BY-LEETEN /2010-BEGIN
	cLineRenderer._SetColorSource(&liv4Colors);
	cLineRenderer._SetInteger(CLineRenderer::COLOR_SCHEME, CLineRenderer::CColorScheme::COLOR_PER_TRACE);
	// ADD-BY-LEETEN 07/07/2010-END

	///////////////////////////////////////////////////////////////
	glutCreateWindow("GCB Streamline");

	// specify the callbacks you really need. Except gcbInit() and gcbDisplayFunc(), other callbacks are optional
	gcbInit(init, quit);
	gcbDisplayFunc(_DisplayFunc);
	gcbKeyboardFunc(_KeyboardFunc);

	// enter the GLUT loop
	glutMainLoop();

	return 0;
}

