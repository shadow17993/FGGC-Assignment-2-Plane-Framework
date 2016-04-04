#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <dinput.h>
#include "resource.h"
#include "DDSTextureLoader.h"
#include "Camera.h"
#include "GameObject.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "Terrain.h"
#include "Appearance.h"
#include "Transform.h"
#include "ParticleSystem.h"

using namespace DirectX;

class Application
{
private:
	HINSTANCE					_hInst;
	HWND						_hWnd;
	D3D_DRIVER_TYPE				_driverType;
	D3D_FEATURE_LEVEL			_featureLevel;
	ID3D11Device*				_pd3dDevice;
	ID3D11DeviceContext*		_pImmediateContext;
	IDXGISwapChain*				_pSwapChain;
	ID3D11RenderTargetView*		_pRenderTargetView;
	ID3D11VertexShader*			_pVertexShader;
	ID3D11PixelShader*			_pPixelShader;
	ID3D11InputLayout*			_pVertexLayout;

	ID3D11Buffer*				_pVertexBuffer;
	ID3D11Buffer*				_pIndexBuffer;
	ID3D11Buffer*				_pConstantBuffer;

	ID3D11DepthStencilView*		_depthStencilView;
	ID3D11Texture2D*			_depthStencilBuffer;
	ID3D11RasterizerState*		_wireframe;
	ID3D11ShaderResourceView*	_pTextureRV = nullptr;
	ID3D11ShaderResourceView*	_pTexturePlane = nullptr;
	ID3D11SamplerState*			_pSamplerLinear = nullptr;
	ID3D11SamplerState*			_pSamplerLinearPlane = nullptr;
	ID3D11ShaderResourceView*	_pTextureQuad = nullptr;
	ID3D11SamplerState*			_pSamplerQuad = nullptr;
	ID3D11BlendState*			_transparency;

	XMFLOAT4X4					_view;
	XMFLOAT4X4					_projection;
	XMFLOAT4X4					_world;
	
	MeshData					terrainMesh;
	MeshData					planeMesh;
	MeshData					sphereMesh;

	Terrain*					terrainObj;

	vector<GameObject*>			_gameObjects;
	ParticleSystem*				_ps;


	XMFLOAT3 lightDirection = { 0.25f, 0.5f, -1.0f };					// Light direction from surface (XYZ)
	XMFLOAT4 diffuseMaterial = { 0.8f, 0.5f, 0.5f, 1.0f };				// Diffuse material properties (RGBA)
	XMFLOAT4 diffuseLight = { 1.0f, 1.0f, 1.0f, 0.5f };					// Diffuse light colour (RGBA)
	XMFLOAT4 AmbientMaterial = { 0.2f, 0.2f, 0.2f, 1.0f };
	XMFLOAT4 AmbientLight = { 0.5f, 0.5f, 0.5f, 1.0f };
	XMFLOAT4 specularMtrl = { 0.8f, 0.8f, 0.8f, 1.0f };
	XMFLOAT4 specularLight = { 0.5f, 0.5f, 0.5f, 1.0f };
	float specularPower = 10.0f;
	XMFLOAT3 eyePos = { 0.0f, 1.0f, -6.0f };
	XMFLOAT4 fogColour = {0.5f, 0.5f, 0.5f, 1.0f};
	float fogRange = 10.0f;
	float fogStart = 0.0f;

	UINT _WindowHeight;
	UINT _WindowWidth;

	Camera* cameraMain;
	Camera* OriginalCam;
	Camera* birdsEye;
	Camera* SideView;

	bool isWireframeOn;

	float angle;
	float rotation;

	Light basicLight;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitConstantBuffer();
	HRESULT InitTexture();
	HRESULT InitDepthStencilBuffer();
	HRESULT InitSwapChain();
	HRESULT InitViewport();
	HRESULT InitBlendState();
	HRESULT InitRenderState();
	void getInput(float t);

	void Update();
	void Draw();
};

