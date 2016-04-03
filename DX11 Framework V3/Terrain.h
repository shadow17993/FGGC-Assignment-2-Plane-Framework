#pragma once

#include <d3d11.h>
//#include <d3dx11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXMath.h>
#include "Structures.h"
#include <string>
#include <vector>


class Terrain
{

private:
	MeshData _meshData;

	XMFLOAT4X4 _world;

	XMFLOAT4X4 _scale;
	XMFLOAT4X4 _rotate;
	XMFLOAT4X4 _translate;

public:
	Terrain();
	virtual ~Terrain();

	XMFLOAT4X4 GetWorld() const { return _world; };

	void UpdateWorld();

	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetTranslation(float x, float y, float z);

	void Initialise(MeshData meshData);
	void Update(float elapsedTime);
	void Draw(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pImmediateContext);
	MeshData LoadTerrain(int row, int col, float dx, float dz, bool heightmap, ID3D11Device * pd3dDevice);
	vector<SimpleVertex> LoadHeightmap(string filename, vector<SimpleVertex> planeVertex, int col, int row);
};

