// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "TerrainMesh.h"
#include "VideoDemo.h"
#include "SampleTerrain.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

class App1 : public BaseApplication
{
public:
	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();
	int gameState = 0;

protected:
	bool render();
	void gui();
	void visualFaultDemoGUI();
	void visualParticleDepositionDemoGUI();
	void visualMidPointDisplacementDemoGUI();
	void visualVoronioRegionsDemoGUI();
	void sampleTerrainsGUI();
	void checkTrainingComplete();
	void menuBar();
	void freeTerrainMenu();
	void mainMenu();
	void trainingMenu();
	void faultGUI();
	void faultText();
	void faultProsCons();
	void faultSampleTerrain(); 
	void midPointGUI();
	void midPointText();
	void midPointProsCons();
	void midPointSampleTerrain();
	void ParticleDepositionGUI();
	void ParticleDepositionText();
	void ParticleDepositionProsCons();
	void ParticleDepositionSampleTerrain();
	void VoronioRegionsGUI();
	void VoronioRegionsText();
	void VoronioRegionsProsCons();
	void VoronoiRegionsSampleTerrain();
private:
	LightShader* shader;
	TerrainMesh* m_Terrain;
	VideoDemo* videoDemo;
	Light* light;

	int terrainResolution = 128;

	bool voronoiMenu = false;
	bool faultMenu = false;
	bool midpointDisplacementMenu = false;
	bool particleDepositionMenu = false;
	bool terrainSliders = false;

	bool trainingComplete = false;
	bool faultComplete = false;
	bool particleDepositionComplete = false;
	bool voronoiRegionsComplete = false;
	bool midPointDisplacementComplete = false;

	bool initialiseCameraPosition = false;
	int screenWidth_;
	int screenHeight_;

	bool faultSection[4];
	int faultLine = 0;
	bool outputFaultTime = false;
	bool outputParticleDepoTime = false;
	
	bool midPointSection[4];
	int midPointLine = 0;
	
	int particleDepoIteration = 3;
	float particleDepocounter;
	bool ParticleDepositionSection[4];
	int ParticleDepositionLine = 0;


	bool VoronioRegionsSection[4];
	int VoronioRegionsLine = 0;
};

#endif