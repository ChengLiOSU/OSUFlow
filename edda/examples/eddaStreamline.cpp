/*

gcbStreamline:
The is a demo to show how to generate streamlines by OSUFlow and how to render the generated streamlines.

Creted by
Abon Chaudhuri and Teng-Yok Lee (The Ohio State University)
May, 2010

*/

#include <list>
#include <iterator>

#include "gcb.h"
#include "OSUFlow.h"
#include "LineRendererInOpenGL.h"

#include "EddaOSUFlow.h"

OSUFlow *osuflow;
char *szVecFilePath;
VECTOR3 minLen, maxLen;
float center[3], len[3];
CLineRendererInOpenGL cLineRenderer;
list<vtListSeedTrace*> sl_list;
list<VECTOR4> liv4Colors;




////////////////////////////////////////////////////////////////////////////
void compute_streamlines()
{
	LOG("");

	float from[3], to[3];

	from[0] = minLen[0];   from[1] = minLen[1];   from[2] = minLen[2];
	to[0] = maxLen[0];   to[1] = maxLen[1];   to[2] = maxLen[2];

	printf("generating seeds...\n");
	osuflow->SetRandomSeedPoints(from, to, 200);
	int nSeeds;
	VECTOR3* seeds = osuflow->GetSeeds(nSeeds);
	for (int i = 0; i<nSeeds; i++)
		printf(" seed no. %d : [%f %f %f]\n", i, seeds[i][0],
		seeds[i][1], seeds[i][2]);

	sl_list.clear();

	printf("compute streamlines..\n");
	osuflow->SetIntegrationParams(1, 5);
	osuflow->GenStreamLines(sl_list, BACKWARD_AND_FORWARD, 500, 0);
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

		// ADD-BY-LEETEN 09/29/2012-BEGIN
	case 'S':
	{
				VECTOR3 v3Min, v3Max;
				osuflow->Boundary(v3Min, v3Max);
				float pfDomainMin[4];
				float pfDomainMax[4];
				for (size_t d = 0; d < 3; d++)
				{
					pfDomainMin[d] = v3Min[d];
					pfDomainMax[d] = v3Max[d];
				}
				pfDomainMin[3] = 0.0f;
				pfDomainMax[3] = 0.0f;

				char szFilename[1024];
				strcpy(szFilename, szVecFilePath);
				strcat(szFilename, ".trace");

				OSUFlow::WriteFlowlines(
					pfDomainMin,
					pfDomainMax,
					&sl_list,
					NULL,
					szFilename);
				LOG(printf("Save the streamlines to %s", szFilename));
	}
		break;
		// ADD-BY-LEETEN 09/29/2012-END

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
	string filename;
	//filename = argv[1];
	filename = "D:/ProjectSource/OSUFlow/edda/sample_data/SW6_PIV.vti";
	
	///////////////////////////////////////////////////////////////
	// when use GCB, it is still needed to initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_STENCIL);



	///////////////////////////////////////////////////////////////
	// initialize OSU flow
	osuflow = new OSUFlow();
	
	// load the scalar field
	//LOG(printf("read file %s\n", argv[1]));

	EddaFileReader efr;
	efr.loadData(filename.c_str(), osuflow);


	szVecFilePath = argv[1];	// ADD-BY-LEETEN 09/29/2012

	// comptue the bounding box of the streamlines 
	VECTOR3 minB, maxB;
	osuflow->Boundary(minLen, maxLen); // get the boundary 
	minB[0] = minLen[0]; minB[1] = minLen[1];  minB[2] = minLen[2];
	maxB[0] = maxLen[0]; maxB[1] = maxLen[1];  maxB[2] = maxLen[2];
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
