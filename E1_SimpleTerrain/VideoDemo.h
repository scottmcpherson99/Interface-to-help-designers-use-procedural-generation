#pragma once
#include "PlaneMesh.h"
#include <stdio.h>
#include <time.h>
class VideoDemo :
	public PlaneMesh {
public:
	VideoDemo(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 128);
	~VideoDemo();

	void Resize(int newResolution);
	void Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	bool mid_point_Tutorial = false;
	bool smooth = false;
	bool faultTutorial = false;
	bool voronoiTutorial = false;
	bool particleDepositionTutorial = false;
	int voronoiRegions = 4;
	int faultNumbers = 3;
	int particlesDropped_ = 250;
	int particleDropX_ = 1;
	int particleDropY_ = 1;
	int cornerHeight = 40;
	float faultXLoop, faulYLoop;
	float faultLoopNumber;
	float particleDepositionNumber;
	float midPointDisplacementNumber;
	float voronioRegionsNumber;
	float voronioXLoop, voronioYLoop;
	const inline int GetResolution() { return resolution; }
private:
	void CreateBuffers(ID3D11Device* device, VertexType* vertices, unsigned long* indices);
	void BuildHeightMap();
	void BuildSmoothMap();
	void BuildFault(const int totalFaults);
	void BuildMidPointDisplacement();
	void BuildVoronoiRegions(const int voronoiRegions);
	void BuildParticleDeposition(int particlesDropped, int particleDropX, int particleDropY);

	const float m_UVscale = 10.0f;			//Tile the UV map 10 times across the plane
	const float terrainSize = 100.0f;		//What is the width and height of our terrain
	float* heightMap;
	
	float totalDuration = 0;

};
