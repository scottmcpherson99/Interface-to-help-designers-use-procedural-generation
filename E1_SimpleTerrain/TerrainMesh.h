#pragma once
#include "PlaneMesh.h"
#include <stdio.h>
#include <time.h>
class TerrainMesh :
	public PlaneMesh {
public:
	TerrainMesh( ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 128 );
	~TerrainMesh();

	void Resize( int newResolution );
	void Regenerate( ID3D11Device* device, ID3D11DeviceContext* deviceContext ); 
	void Flat();
	bool faultTutorial = false;
	bool particleDepositionTutorial = false;
	bool voronoiRegionTutorial = false;
	bool midPointDisplacementTutorial = false;
	bool mid_point_ = false;
	bool smooth = false;
	bool fault = false;
	bool voronoi = false;
	bool particleDeposition = false;
	int voronoiRegions = 3;
	int faultNumbers = 3;
	float faultValue = 5;
	int particleDepositionIterations = 1;
	int particlesDropped_ = 250;
	int particleDropX_ = 10;
	int particleDropY_ = 10;
	int cornerHeight = 30;
	float midPointDisplacementValue = 15;

	const inline int GetResolution(){ return resolution; }
private:
	void CreateBuffers( ID3D11Device* device, VertexType* vertices, unsigned long* indices );
	void BuildHeightMap();
	void BuildSmoothMap();
	void BuildFault(const int totalFaults, float faultValue);
	void BuildMidPointDisplacement();
	void BuildVoronoiRegions(const int voronoiRegions);
	void BuildParticleDeposition(int particlesDropped, int particleDropX, int particleDropY, int iterations);

	const float m_UVscale = 10.0f;			//Tile the UV map 10 times across the plane
	const float terrainSize = 100.0f;		//What is the width and height of our terrain
	float* heightMap;


};
