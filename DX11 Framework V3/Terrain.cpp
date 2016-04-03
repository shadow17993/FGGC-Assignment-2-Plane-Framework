#include "Terrain.h"
#include <vector>
#include <iostream>
#include <fstream>


Terrain::Terrain()
{
}


Terrain::~Terrain()
{
}

void Terrain::Initialise(MeshData meshData)
{
	_meshData = meshData;

	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	XMStoreFloat4x4(&_scale, XMMatrixIdentity());
	XMStoreFloat4x4(&_rotate, XMMatrixIdentity());
	XMStoreFloat4x4(&_translate, XMMatrixIdentity());
}

void Terrain::SetScale(float x, float y, float z)
{
	XMStoreFloat4x4(&_scale, XMMatrixScaling(x, y, z));
}

void Terrain::SetRotation(float x, float y, float z)
{
	XMStoreFloat4x4(&_rotate, XMMatrixRotationX(x) * XMMatrixRotationY(y) * XMMatrixRotationZ(z));
}

void Terrain::SetTranslation(float x, float y, float z)
{
	XMStoreFloat4x4(&_translate, XMMatrixTranslation(x, y, z));
}

void Terrain::UpdateWorld()
{
	XMMATRIX scale = XMLoadFloat4x4(&_scale);
	XMMATRIX rotate = XMLoadFloat4x4(&_rotate);
	XMMATRIX translate = XMLoadFloat4x4(&_translate);

	XMStoreFloat4x4(&_world, scale * rotate * translate);
}

void Terrain::Update(float elapsedTime)
{
	// TODO: Add GameObject logic

}

void Terrain::Draw(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pImmediateContext)
{
	// NOTE: We are assuming that the constant buffers and all other draw setup has already taken place

	// Set vertex and index buffers
	pImmediateContext->IASetVertexBuffers(0, 1, &_meshData.VertexBuffer, &_meshData.VBStride, &_meshData.VBOffset);
	pImmediateContext->IASetIndexBuffer(_meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pImmediateContext->DrawIndexed(_meshData.IndexCount, 0, 0);
}

MeshData Terrain::LoadTerrain(int row, int col, float dx, float dz, bool heightmap, ID3D11Device * pd3dDevice)
{
	MeshData _meshData;
	vector<SimpleVertex> planeVertex;
	vector<WORD> planeIndex;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			planeVertex.push_back(
			{
				XMFLOAT3((j + 1) * dx - 1, 0.0f, (i*dz)),
				XMFLOAT3((j + 1) * dx - 1, 0.0f, (i*dz)),
				XMFLOAT2((1.0f / row) * (i + 1), (1.0f / col) * (j + 1))
			});
		}
	}

	if (heightmap)
	{
		planeVertex = LoadHeightmap("OBJ/Heightmap 513x513.raw", planeVertex, row, col);
	}

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			planeIndex.push_back(i * (col + 1) + j);
			planeIndex.push_back(i * (col + 1) + j + 1);
			planeIndex.push_back((i + 1) * (col + 1) + j);
			planeIndex.push_back((i + 1) * (col + 1) + j);
			planeIndex.push_back(i * (col + 1) + j + 1);
			planeIndex.push_back((i + 1) * (col + 1) + j + 1);
		}
	}

	// Gets Vertices from Vector to Array
	SimpleVertex* finalVerts = new SimpleVertex[planeVertex.size()];
	unsigned int numMeshVertices = planeVertex.size();
	for (unsigned int i = 0; i < numMeshVertices; ++i)
	{
		finalVerts[i] = planeVertex[i];
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * planeVertex.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = finalVerts;

	pd3dDevice->CreateBuffer(&bd, &InitData, &_meshData.VertexBuffer);

	_meshData.VBOffset = 0;
	_meshData.VBStride = sizeof(SimpleVertex);


	// Gets Indices from Vector to Array
	unsigned short* indicesArray = new unsigned short[planeIndex.size()];
	unsigned int numMeshIndices = planeIndex.size();
	for (unsigned int i = 0; i < numMeshIndices; ++i)
	{
		indicesArray[i] = planeIndex[i];
	}

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * planeIndex.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indicesArray;
	pd3dDevice->CreateBuffer(&bd, &InitData, &_meshData.IndexBuffer);

	_meshData.IndexCount = planeIndex.size();

	return _meshData;
}

vector<SimpleVertex> Terrain::LoadHeightmap(string filename, vector<SimpleVertex> planeVertex, int width, int height)
{
	UINT HeightMapWidth = width;
	UINT HeightMapHeight = width;
	string HeightMapName = filename;
	vector<float> heightMap;

	vector<unsigned char> in(HeightMapWidth * HeightMapHeight);

	std::ifstream inFile;
	inFile.open(filename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		inFile.close();
	}

	heightMap.resize(HeightMapWidth * HeightMapHeight, 0);
	for (UINT i = 0; i < HeightMapWidth * HeightMapHeight; ++i)
	{
		planeVertex[i].Pos.y = (in[i] / 255.0f) * 50;
		planeVertex[i].Normal.y = planeVertex[i].Pos.y;
	}

	return planeVertex;
}