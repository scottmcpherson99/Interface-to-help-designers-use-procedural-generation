#include "SampleTerrain.h"
#include <vector>
#include <iostream>
#include <chrono>

using namespace std;
SampleTerrain::SampleTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution) :
	PlaneMesh(device, deviceContext, lresolution)
{
	Resize(resolution);
	Regenerate(device, deviceContext);
}

//Cleanup the heightMap
SampleTerrain::~SampleTerrain() {
	delete[] heightMap;
	heightMap = 0;
}

void SampleTerrain::BuildHeightMap() {
	float height = 0.0f;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;

	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			heightMap[(j * resolution) + i] = 0;
		}
	}
}

void SampleTerrain::Resize(int newResolution) {
	resolution = newResolution;
	heightMap = new float[resolution * resolution];
	if (vertexBuffer != NULL) {
		vertexBuffer->Release();
	}
	vertexBuffer = NULL;
}

// Set up the heightmap and create or update the appropriate buffers
void SampleTerrain::Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {

	int beforetest = 1;
	int aftertest = 2;
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, height, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//Calculate and store the height values
	if (smooth == true)
	{
		BuildSmoothMap();
	}
	else if (fault == true)
	{
		BuildFault(faultNumbers);
	}
	else if (mid_point_ == true)
	{
		beforetest = 1;
		BuildMidPointDisplacement();
		aftertest = 2;
	}
	else if (voronoi == true)
	{
		BuildVoronoiRegions(voronoiRegions);
	}
	else if (particleDeposition == true)
	{
		BuildParticleDeposition(particlesDropped_, particleDropX_, particleDropY_);
	}
	else
	{
		BuildHeightMap();
	}
	// Calculate the number of vertices in the terrain mesh.
	// We share vertices in this mesh, so the vertex count is simply the terrain 'resolution'
	// and the index count is the number of resulting triangles * 3 OR the number of quads * 6
	vertexCount = resolution * resolution;

	indexCount = ((resolution - 1) * (resolution - 1)) * 6;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	index = 0;

	// UV coords.
	u = 0;
	v = 0;
	increment = m_UVscale / resolution;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;

	//Set up vertices
	for (j = 0; j < (resolution); j++) {
		for (i = 0; i < (resolution); i++) {
			positionX = (float)i * scale;
			positionZ = (float)(j)*scale;

			height = heightMap[index];
			vertices[index].position = XMFLOAT3(positionX, height, positionZ);
			vertices[index].texture = XMFLOAT2(u, v);

			u += increment;
			index++;
		}
		u = 0;
		v += increment;
	}

	//Set up index list
	index = 0;
	for (j = 0; j < (resolution - 1); j++) {
		for (i = 0; i < (resolution - 1); i++) {

			//Build index array
			indices[index] = (j * resolution) + i;
			indices[index + 1] = ((j + 1) * resolution) + (i + 1);
			indices[index + 2] = ((j + 1) * resolution) + i;

			indices[index + 3] = (j * resolution) + i;
			indices[index + 4] = (j * resolution) + (i + 1);
			indices[index + 5] = ((j + 1) * resolution) + (i + 1);
			index += 6;
		}
	}

	//Set up normals
	for (j = 0; j < (resolution - 1); j++) {
		for (i = 0; i < (resolution - 1); i++) {
			//Calculate the plane normals
			XMFLOAT3 a, b, c;	//Three corner vertices
			a = vertices[j * resolution + i].position;
			b = vertices[j * resolution + i + 1].position;
			c = vertices[(j + 1) * resolution + i].position;

			//Two edges
			XMFLOAT3 ab(c.x - a.x, c.y - a.y, c.z - a.z);
			XMFLOAT3 ac(b.x - a.x, b.y - a.y, b.z - a.z);

			//Calculate the cross product
			XMFLOAT3 cross;
			cross.x = ab.y * ac.z - ab.z * ac.y;
			cross.y = ab.z * ac.x - ab.x * ac.z;
			cross.z = ab.x * ac.y - ab.y * ac.x;
			float mag = (cross.x * cross.x) + (cross.y * cross.y) + (cross.z * cross.z);
			mag = sqrtf(mag);
			cross.x /= mag;
			cross.y /= mag;
			cross.z /= mag;
			vertices[j * resolution + i].normal = cross;
		}
	}

	//Smooth the normals by averaging the normals from the surrounding planes
	XMFLOAT3 smoothedNormal(0, 1, 0);
	for (j = 0; j < resolution; j++) {
		for (i = 0; i < resolution; i++) {
			smoothedNormal.x = 0;
			smoothedNormal.y = 0;
			smoothedNormal.z = 0;
			float count = 0;
			//Left planes
			if ((i - 1) >= 0) {
				//Top planes
				if ((j) < (resolution - 1)) {
					smoothedNormal.x += vertices[j * resolution + (i - 1)].normal.x;
					smoothedNormal.y += vertices[j * resolution + (i - 1)].normal.y;
					smoothedNormal.z += vertices[j * resolution + (i - 1)].normal.z;
					count++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothedNormal.x += vertices[(j - 1) * resolution + (i - 1)].normal.x;
					smoothedNormal.y += vertices[(j - 1) * resolution + (i - 1)].normal.y;
					smoothedNormal.z += vertices[(j - 1) * resolution + (i - 1)].normal.z;
					count++;
				}
			}
			//right planes
			if ((i) < (resolution - 1)) {

				//Top planes
				if ((j) < (resolution - 1)) {
					smoothedNormal.x += vertices[j * resolution + i].normal.x;
					smoothedNormal.y += vertices[j * resolution + i].normal.y;
					smoothedNormal.z += vertices[j * resolution + i].normal.z;
					count++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothedNormal.x += vertices[(j - 1) * resolution + i].normal.x;
					smoothedNormal.y += vertices[(j - 1) * resolution + i].normal.y;
					smoothedNormal.z += vertices[(j - 1) * resolution + i].normal.z;
					count++;
				}
			}
			smoothedNormal.x /= count;
			smoothedNormal.y /= count;
			smoothedNormal.z /= count;

			float mag = sqrt((smoothedNormal.x * smoothedNormal.x) + (smoothedNormal.y * smoothedNormal.y) + (smoothedNormal.z * smoothedNormal.z));
			smoothedNormal.x /= mag;
			smoothedNormal.y /= mag;
			smoothedNormal.z /= mag;

			vertices[j * resolution + i].normal = smoothedNormal;
		}
	}
	//If we've not yet created our dyanmic Vertex and Index buffers, do that now
	if (vertexBuffer == NULL) {
		CreateBuffers(device, vertices, indices);
	}
	else {
		//If we've already made our buffers, update the information
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		//  Disable GPU access to the vertex buffer data.
		deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		//  Update the vertex buffer here.
		memcpy(mappedResource.pData, vertices, sizeof(VertexType) * vertexCount);
		//  Reenable GPU access to the vertex buffer data.
		deviceContext->Unmap(vertexBuffer, 0);
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

void SampleTerrain::Flat()
{
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			heightMap[j * resolution + i] = 0;
		}
		}
}

void SampleTerrain::BuildFault(const int totalFaults)
{


	int edge[3];
	int edge2[3];
	int point_[2];
	int axis_;
	float new_value[3];
	int counter;

	for (int loop = totalFaults; loop > 0; loop--)
	{	//pick two points on the edge of the terrain
		for (int i = 0; i < 2; i++)
		{

			axis_ = rand() % 4;

			switch (axis_)
			{
			case 0:
				edge[i] = 0;
				edge2[i] = rand() % 129;
				break;
			case 1:
				edge[i] = 128;
				edge2[i] = rand() % 129;
				break;
			case 2:
				edge[i] = rand() % 129;
				edge2[i] = 0;
				break;
			case 3:
				edge[i] = rand() % 129;
				edge2[i] = 128;
				break;
			}
		}

		edge[2] = edge[1] - edge[0];
		edge2[2] = edge2[1] - edge2[0];

		counter = rand() % 2;

		for (int j = 0; j < (resolution); j++) {
			for (int i = 0; i < (resolution); i++) {

				point_[0] = j - edge[0];
				point_[1] = i - edge2[0];

				//perform cross products on line
				new_value[1] = -((edge[2] * point_[1]) - (point_[0] * edge2[2]));


				if (new_value[1] >= 0)
				{
					if (counter == 0)
					{
						heightMap[(j * resolution) + i] = heightMap[(j * resolution) + i] + loop;
					}
					else if (counter == 1)
					{
						heightMap[(j * resolution) + i] = heightMap[(j * resolution) + i] - loop;
					}
				}

				else if (new_value[1] < 0)
				{
					if (counter == 0)
					{
						heightMap[(j * resolution) + i] = heightMap[(j * resolution) + i] - loop;
					}
					else if (counter == 1)
					{
						heightMap[(j * resolution) + i] = heightMap[(j * resolution) + i] + loop;
					}
				}
			}
		}
	}

}

//Smooth the terrain to blend it together
void SampleTerrain::BuildSmoothMap()
{
	float height = 0.0f;
	int counter = 0;
	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;

	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			counter = 0;
			if ((i - 1) > 0)
			{
				if ((j - 1) > 0)
				{
					height = heightMap[((j - 1) * resolution) + (i - 1)];
					counter++;
				}
			}
			if (i - 1 > 0)
			{
				if (j + 1 < resolution)
				{
					height += heightMap[((j + 1) * resolution) + (i - 1)];
					counter++;
				}
			}
			if (i + 1 < resolution)
			{
				if (j - 1 > 0)
				{
					height += heightMap[((j - 1) * resolution) + (i + 1)];
					counter++;
				}
			}
			if (i + 1 < resolution)
			{
				if (j + 1 < resolution)
				{
					height += heightMap[((j + 1) * resolution) + (i + 1)];
					counter++;
				}
			}

			if (counter != 0)
			{
				height = height / counter;
			}


			heightMap[(j * resolution) + i] = (heightMap[(j * resolution) + i] + height) / 3.3;
		}
	}
}

void SampleTerrain::BuildMidPointDisplacement()
{
	//declare variables
	int min_corner_X = 0;
	int max_corner_X = 128;
	int min_corner_Y = 0;
	int max_corner_Y = 128;
	int rowSize = 128;
	float divideCorners = 1;
	float averageCorners = 0;
	int counter;
	int increase_decrease;
	bool iterationComplete = false;
	float increase_height = midPointDisplacementValue;

	//initialise first four corners
	heightMap[min_corner_X + rowSize * min_corner_Y] = (rand() % cornerHeight);
	heightMap[max_corner_X + rowSize * min_corner_Y] = (rand() % cornerHeight);
	heightMap[min_corner_X + rowSize * max_corner_Y] = (rand() % cornerHeight);
	heightMap[max_corner_X + rowSize * max_corner_Y] = (rand() % cornerHeight);

	for (int i = 1; i < 128; i *= 2)
	{
		max_corner_X = 128 / divideCorners;
		max_corner_Y = 128 / divideCorners;
		min_corner_X = 0;
		min_corner_Y = 0;
		iterationComplete = false;
		while (iterationComplete == false)
		{
			//initial square step
			averageCorners = 0;
			averageCorners += heightMap[min_corner_X + rowSize * min_corner_Y];
			averageCorners += heightMap[max_corner_X + rowSize * min_corner_Y];
			averageCorners += heightMap[min_corner_X + rowSize * max_corner_Y];
			averageCorners += heightMap[max_corner_X + rowSize * max_corner_Y];

			increase_decrease = rand() % 2 + 1;
			if (increase_decrease == 1)
			{
				heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * ((max_corner_Y + min_corner_Y) / 2)] = (averageCorners / 4) + increase_height;
			}
			else if (increase_decrease == 2)
			{
				heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * ((max_corner_Y + min_corner_Y) / 2)] = (averageCorners / 4) - increase_height;
			}
			if (max_corner_X == 128)
			{
				if (max_corner_Y == 128)
				{
					iterationComplete = true;
					increase_height /= 1.25;
				}
				else
				{
					min_corner_Y += 128 / divideCorners;
					max_corner_Y += 128 / divideCorners;
					min_corner_X = 0;
					max_corner_X = 128 / divideCorners;
				}
			}
			else
			{
				min_corner_X += 128 / divideCorners;
				max_corner_X += 128 / divideCorners;
			}
		}


		max_corner_X = 128 / divideCorners;
		max_corner_Y = 128 / divideCorners;
		min_corner_X = 0;
		min_corner_Y = 0;
		iterationComplete = false;
		while (iterationComplete == false)
		{
			//initial diamond step
			//iteration 1
			averageCorners = 0;
			averageCorners += heightMap[min_corner_X + rowSize * min_corner_Y];
			averageCorners += heightMap[max_corner_X + rowSize * min_corner_Y];
			averageCorners += heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * ((max_corner_Y + min_corner_Y) / 2)];
			if (min_corner_Y != 0)
			{
				//averageCorners += heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * (((max_corner_Y + min_corner_Y) / 2) - (max_corner_Y/2))];
				counter = 3;
			}
			else
			{
				counter = 3;
			}

			increase_decrease = rand() % 2 + 1;
			if (increase_decrease == 1)
			{
				heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * min_corner_Y] = (averageCorners / counter) + increase_height;
			}
			else if (increase_decrease == 2)
			{
				heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * min_corner_Y] = (averageCorners / counter) - increase_height;
			}


			//iteration 2
			averageCorners = 0;
			averageCorners += heightMap[min_corner_X + rowSize * min_corner_Y];
			averageCorners += heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * ((max_corner_Y + min_corner_Y) / 2)];
			averageCorners += heightMap[min_corner_X + rowSize * max_corner_Y];
			if (min_corner_X != 0)
			{
				//averageCorners += heightMap[(((max_corner_X + min_corner_X) / 2) - (max_corner_X / 2)) + rowSize * ((max_corner_Y + min_corner_Y) / 2)];
				counter = 3;
			}
			else
			{
				counter = 3;
			}
			increase_decrease = rand() % 2 + 1;
			if (increase_decrease == 1)
			{
				heightMap[min_corner_X + rowSize * ((max_corner_Y + min_corner_Y) / 2)] = (averageCorners / counter) + increase_height;
			}
			else if (increase_decrease == 2)
			{
				heightMap[min_corner_X + rowSize * ((max_corner_Y + min_corner_Y) / 2)] = (averageCorners / counter) - increase_height;
			}


			//iteration 3
			averageCorners = 0;
			averageCorners += heightMap[min_corner_X + rowSize * max_corner_Y];
			averageCorners += heightMap[max_corner_X + rowSize * max_corner_Y];
			averageCorners += heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * ((max_corner_Y + min_corner_Y) / 2)];
			if (max_corner_Y != 8)
			{
				//averageCorners += heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * (((max_corner_Y + min_corner_Y) / 2) + (max_corner_Y / 2))];
				counter = 3;
			}
			else
			{
				counter = 3;
			}
			increase_decrease = rand() % 2 + 1;
			if (increase_decrease == 1)
			{
				heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * max_corner_Y] = (averageCorners / counter) + increase_height;
			}
			else if (increase_decrease == 2)
			{
				heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * max_corner_Y] = (averageCorners / counter) - increase_height;
			}


			//iteration 4
			averageCorners = 0;
			averageCorners += heightMap[max_corner_X + rowSize * min_corner_Y];
			averageCorners += heightMap[max_corner_X + rowSize * max_corner_Y];
			averageCorners += heightMap[((max_corner_X + min_corner_X) / 2) + rowSize * ((max_corner_Y + min_corner_Y) / 2)];
			if (max_corner_X != 8)
			{
				//averageCorners += heightMap[(((max_corner_X + min_corner_X) / 2) + (max_corner_X / 2)) + rowSize * ((max_corner_Y + min_corner_Y) / 2)];
				counter = 3;
			}
			else
			{
				counter = 3;
			}

			increase_decrease = rand() % 2 + 1;
			if (increase_decrease == 1)
			{
				heightMap[max_corner_X + rowSize * ((max_corner_Y + min_corner_Y) / 2)] = (averageCorners / counter) + increase_height;
			}
			else if (increase_decrease == 2)
			{
				heightMap[max_corner_X + rowSize * ((max_corner_Y + min_corner_Y) / 2)] = (averageCorners / counter) - increase_height;
			}

			if (max_corner_X == 128)
			{
				if (max_corner_Y == 128)
				{
					iterationComplete = true;
					increase_height /= 1.25;
				}
				else
				{
					min_corner_Y += 128 / divideCorners;
					max_corner_Y += 128 / divideCorners;
					min_corner_X = 0;
					max_corner_X = 128 / divideCorners;
				}
			}
			else
			{
				min_corner_X += 128 / divideCorners;
				max_corner_X += 128 / divideCorners;
			}
		}
		divideCorners *= 2;
	}
	min_corner_X == 1;


}

void SampleTerrain::BuildVoronoiRegions(const int voronoiRegions)
{

	std::vector<int> xZone_;
	std::vector<int> yZone_;
	int distance;
	int projDistance;

	for (int i = 0; i < voronoiRegions; i++)
	{
		xZone_.push_back(rand() % 128);
		yZone_.push_back(rand() % 128);
	}

	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			distance = 1000;
			for (int iteration = 0; iteration < voronoiRegions; iteration++)
			{
				projDistance = sqrt(pow(i - xZone_.at(iteration), 2) + pow(j - yZone_.at(iteration), 2));
				if (distance > projDistance)
				{
					distance = projDistance;
					heightMap[(j * resolution) + i] = iteration * 2;


				}
			}
		}
	}

}

void SampleTerrain::BuildParticleDeposition(int particlesDropped, int particleDropX, int particleDropY)
{
	for (int i = 0; i < 1; i++)
	{
		chrono::steady_clock::time_point start = chrono::steady_clock::now();
		const int Range = voronoiRegions;
		int spawnPointX = (rand() % 129);
		int spawnPointY = (rand() % 129);
		if (spawnPointX > 115)
		{
			spawnPointX = 115;
		}
		if (spawnPointX < 15)
		{
			spawnPointX = 15;
		}
		if (spawnPointY > 115)
		{
			spawnPointY = 115;
		}

		if (spawnPointY < 15)
		{
			spawnPointY = 15;
		}
		for (int i = 0; i < particlesDropped; i++)
		{
			int dropPointX = spawnPointX + (rand() % particleDropX) - particleDropX / 2;
			int dropPointY = spawnPointY + (rand() % particleDropY) - particleDropY / 2;

			if (dropPointX < 1)
			{
				dropPointX = 1;
			}

			if (dropPointY < 1)
			{
				dropPointY = 1;
			}

			if (heightMap[dropPointX * 128 + dropPointY] > heightMap[(dropPointX + 1) * 128 + dropPointY])
			{
				heightMap[(dropPointX + 1) * 128 + dropPointY] += 1;
			}
			else if (heightMap[dropPointX * 128 + dropPointY] > heightMap[(dropPointX - 1) * 128 + dropPointY])
			{
				heightMap[(dropPointX - 1) * 128 + dropPointY] += 1;
			}
			else if (heightMap[dropPointX * 128 + dropPointY] > heightMap[dropPointX * 128 + (dropPointY + 1)])
			{
				heightMap[dropPointX * 128 + (dropPointY + 1)] += 1;
			}
			else if (heightMap[dropPointX * 128 + dropPointY] > heightMap[dropPointX * 128 + (dropPointY - 1)])
			{
				heightMap[dropPointX * 128 + (dropPointY - 1)] += 1;
			}
			else if (heightMap[dropPointX * 128 + dropPointY] > heightMap[(dropPointX + 1) * 128 + (dropPointY + 1)])
			{
				heightMap[(dropPointX + 1) * 128 + (dropPointY + 1)] += 1;
			}
			else if (heightMap[dropPointX * 128 + dropPointY] > heightMap[(dropPointX + 1) * 128 + (dropPointY - 1)])
			{
				heightMap[(dropPointX + 1) * 128 + (dropPointY - 1)] += 1;
			}
			else if (heightMap[dropPointX * 128 + dropPointY] > heightMap[(dropPointX - 1) * 128 + (dropPointY + 1)])
			{
				heightMap[(dropPointX - 1) * 128 + (dropPointY + 1)] += 1;
			}
			else if (heightMap[dropPointX * 128 + dropPointY] > heightMap[(dropPointX - 1) * 128 + (dropPointY - 1)])
			{
				heightMap[(dropPointX - 1) * 128 + (dropPointY - 1)] += 1;
			}
			else
			{
				heightMap[dropPointX * 128 + dropPointY] += 1;
			}

		}
		Range == Range;
		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		std::chrono::duration<float> duration = end - start;
		totalDuration = duration.count();
	}
}


//Create the vertex and index buffers that will be passed along to the graphics card for rendering
void SampleTerrain::CreateBuffers(ID3D11Device* device, VertexType* vertices, unsigned long* indices) {

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the dyanmic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}