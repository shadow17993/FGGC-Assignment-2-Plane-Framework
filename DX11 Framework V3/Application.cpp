#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
		return E_FAIL;
	}

	RECT rc;
	GetClientRect(_hWnd, &rc);
	_WindowWidth = rc.right - rc.left;
	_WindowHeight = rc.bottom - rc.top;

	if (FAILED(InitDevice()))
	{
		Cleanup();

		return E_FAIL;
	}

	CreateDDSTextureFromFile(_pd3dDevice, L"OBJ/Crate_COLOR.dds", nullptr, &_pTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"OBJ/PineTree_COLOR.dds", nullptr, &_pTextureQuad);
	CreateDDSTextureFromFile(_pd3dDevice, L"OBJ/Hercules_COLOR.dds", nullptr, &_pTexturePlane);

	cameraMain = new Camera({ 0.0f, 25.0f, 20.0f }, { 0.0f, 20.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, _WindowWidth, _WindowHeight, 0.01f, 200.0f);
	OriginalCam = new Camera({ 0.0f, 25.0f, 20.0f }, { 0.0f, 8.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, _WindowWidth, _WindowHeight, 0.01f, 200.0f);
	birdsEye = new Camera({ 0.0f, 60.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, _WindowWidth, _WindowHeight, 0.01f, 200.0f);
	SideView = new Camera({ 30.0f, 0.0f, -6.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, _WindowWidth, _WindowHeight, 0.01f, 200.0f);

	isWireframeOn = false;

	planeMesh = OBJLoader::Load("OBJ/Hercules.obj", _pd3dDevice, true);
	terrainMesh = OBJLoader::LoadTerrain(513, 513, 2, 2, true, _pd3dDevice);
	sphereMesh = OBJLoader::Load("OBJ/star.obj", _pd3dDevice, true);

	basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	Geometry planeGeometry;
	planeGeometry.indexBuffer = planeMesh.IndexBuffer;
	planeGeometry.vertexBuffer = planeMesh.VertexBuffer;;
	planeGeometry.numberOfIndices = planeMesh.IndexCount;
	planeGeometry.vertexBufferOffset = planeMesh.VBOffset;
	planeGeometry.vertexBufferStride = planeMesh.VBStride;

	Geometry terrainGeometry;
	terrainGeometry.indexBuffer = terrainMesh.IndexBuffer;
	terrainGeometry.vertexBuffer = terrainMesh.VertexBuffer;;
	terrainGeometry.numberOfIndices = terrainMesh.IndexCount;
	terrainGeometry.vertexBufferOffset = terrainMesh.VBOffset;
	terrainGeometry.vertexBufferStride = terrainMesh.VBStride;

	Geometry sphereGeometry;
	sphereGeometry.indexBuffer = sphereMesh.IndexBuffer;
	sphereGeometry.vertexBuffer = sphereMesh.VertexBuffer;;
	sphereGeometry.numberOfIndices = sphereMesh.IndexCount;
	sphereGeometry.vertexBufferOffset = sphereMesh.VBOffset;
	sphereGeometry.vertexBufferStride = sphereMesh.VBStride;

	Material shinyMaterial;
	shinyMaterial.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial.specularPower = 10.0f;

	Material noSpecMaterial;
	noSpecMaterial.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial.specularPower = 0.0f;

	Appearance* planeAppearance = new Appearance(planeGeometry, shinyMaterial);
	planeAppearance->SetTextureRV(_pTexturePlane);

	Appearance* terrainAppearance = new Appearance(terrainGeometry, shinyMaterial);
	terrainAppearance->SetTextureRV(_pTextureRV);

	Appearance* sphereAppearance = new Appearance(sphereGeometry, shinyMaterial);
	terrainAppearance->SetTextureRV(_pTextureRV);

	Transform * planeTransform = new Transform();
	planeTransform->SetScale(1.0f, 1.0f, 1.0f);
	planeTransform->SetPosition(cameraMain->GetPosition().x, cameraMain->GetPosition().y - 15.0f, cameraMain->GetPosition().z - 20.0f);

	Transform * terrainTransform = new Transform(nullptr, { -200, -30, -200 });

	ParticleModel * particleModel = new ParticleModel(planeTransform, true, { 0.0f, 0.0f, 0.00001f }, { 0.0f, 0.0f, 0.0f }, 50.0f);

	GameObject* planeObj = new GameObject("Plane", planeTransform, particleModel, planeAppearance);
	GameObject* terrainObj = new GameObject("Terrain", terrainTransform, terrainAppearance);

	_gameObjects.push_back(planeObj);
	_gameObjects.push_back(terrainObj);

	Transform * psTransform = new Transform(planeTransform);

	_ps = new ParticleSystem(psTransform, { 0.0f, 0.0f, 10.0f }, sphereAppearance);

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 1024, 768};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::InitDevice()
{
	HRESULT hr = S_OK;

	InitSwapChain();

	InitDepthStencilBuffer();

	InitViewport();

	InitShadersAndInputLayout();

	// Set index buffer
	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	InitConstantBuffer();

	// Create the Textures
	InitTexture();

	// Create Blend State
	InitBlendState();

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}


HRESULT Application::InitConstantBuffer()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitTexture()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);
	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinearPlane);
	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerQuad);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitSwapChain()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = _WindowWidth;
	sd.BufferDesc.Height = _WindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT Application::InitViewport()
{
	HRESULT hr = S_OK;

	// SetUp the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)_WindowWidth;
	vp.Height = (FLOAT)_WindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}


HRESULT Application::InitDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;


	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);


	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	return S_OK;
}

HRESULT Application::InitBlendState()
{
	HRESULT hr = S_OK;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable =				true;
	rtbd.BlendOp =					D3D11_BLEND_OP_ADD;
	rtbd.BlendOpAlpha =				D3D11_BLEND_OP_ADD;
	rtbd.DestBlend =				D3D11_BLEND_BLEND_FACTOR;
	rtbd.DestBlendAlpha =			D3D11_BLEND_ZERO;
	rtbd.RenderTargetWriteMask =	D3D10_COLOR_WRITE_ENABLE_ALL;
	rtbd.SrcBlend =					D3D11_BLEND_SRC_COLOR;
	rtbd.SrcBlendAlpha =			D3D11_BLEND_ONE;

	blendDesc.AlphaToCoverageEnable =	false;
	blendDesc.RenderTarget[0] =			rtbd;

	hr = _pd3dDevice->CreateBlendState(&blendDesc, &_transparency);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitRenderState()
{
	HRESULT hr = S_OK;

	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));

	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	_pd3dDevice->CreateRasterizerState(&wfdesc, &_wireframe);
	_pImmediateContext->RSSetState(_wireframe);

	return hr;
}



HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}


void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
	/*if (_pVertexBuffer2) _pVertexBuffer2->Release();
	if (_pVertexBuffer3) _pVertexBuffer3->Release();*/
    if (_pIndexBuffer) _pIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_transparency) _transparency->Release();
	//if (_wireframe) _wireframe->Release();
}

void Application::Update()
{
    // Update our time
    /*static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.025f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }*/

	float t = 0.01667;

	getInput(t);

	for (auto gameobjects : _gameObjects)
	{
		gameobjects->Update(t);
	}

	_ps->Update(t);
}

void Application::getInput(float t)
{
	// Move Plane Downwards
	if (GetAsyncKeyState('S') & 0x8000)
	{
		/*cameraMain->SetPosition(cameraMain->GetPosition().x, cameraMain->GetPosition().y - 0.1f, cameraMain->GetPosition().z);
		cameraMain->SetLookAt(cameraMain->GetLookAt().x, cameraMain->GetLookAt().y - 0.1f, cameraMain->GetLookAt().z);
		cameraMain->Update();*/
		/*birdsEye->SetPosition(birdsEye->GetPosition().x, birdsEye->GetPosition().y - 0.1f, birdsEye->GetPosition().z);
		birdsEye->SetLookAt(birdsEye->GetLookAt().x, birdsEye->GetLookAt().y - 0.1f, birdsEye->GetLookAt().z);
		birdsEye->Update();
		SideView->SetPosition(SideView->GetPosition().x, SideView->GetPosition().y - 0.1f, SideView->GetPosition().z);
		SideView->SetLookAt(SideView->GetLookAt().x, SideView->GetLookAt().y - 0.1f, SideView->GetLookAt().z);
		SideView->Update();*/
	}
	else if (GetAsyncKeyState('W') & 0x8000)// Move Plane Upwards
	{
		/*cameraMain->SetPosition(cameraMain->GetPosition().x, cameraMain->GetPosition().y + 0.1f, cameraMain->GetPosition().z);
		cameraMain->SetLookAt(cameraMain->GetLookAt().x, cameraMain->GetLookAt().y + 0.1f, cameraMain->GetLookAt().z);
		cameraMain->Update();*/
		/*birdsEye->SetPosition(birdsEye->GetPosition().x, birdsEye->GetPosition().y + 0.1f, birdsEye->GetPosition().z);
		birdsEye->SetLookAt(birdsEye->GetLookAt().x, birdsEye->GetLookAt().y + 0.1f, birdsEye->GetLookAt().z);
		birdsEye->Update();
		SideView->SetPosition(SideView->GetPosition().x, SideView->GetPosition().y + 0.1f, SideView->GetPosition().z);
		SideView->SetLookAt(SideView->GetLookAt().x, SideView->GetLookAt().y + 0.1f, SideView->GetLookAt().z);
		SideView->Update();*/
	}

	// Move Plane Right
	if (GetAsyncKeyState('D') & 0x8000)
	{
		/*cameraMain->SetPosition(cameraMain->GetPosition().x - 0.1f, cameraMain->GetPosition().y, cameraMain->GetPosition().z);
		cameraMain->SetLookAt(cameraMain->GetLookAt().x - 0.1f, cameraMain->GetLookAt().y, cameraMain->GetLookAt().z);
		cameraMain->Update();*/
		/*birdsEye->SetPosition(birdsEye->GetPosition().x - 0.1f, birdsEye->GetPosition().y, birdsEye->GetPosition().z);
		birdsEye->SetLookAt(birdsEye->GetLookAt().x - 0.1f, birdsEye->GetLookAt().y, birdsEye->GetLookAt().z);
		birdsEye->Update();
		SideView->SetPosition(SideView->GetPosition().x - 0.1f, SideView->GetPosition().y, SideView->GetPosition().z);
		SideView->SetLookAt(SideView->GetLookAt().x - 0.1f, SideView->GetLookAt().y, SideView->GetLookAt().z);
		SideView->Update();*/
	}
	else if (GetAsyncKeyState('A') & 0x8000)// Move Plane Left
	{
		/*cameraMain->SetPosition(cameraMain->GetPosition().x + 0.1f, cameraMain->GetPosition().y, cameraMain->GetPosition().z);
		cameraMain->SetLookAt(cameraMain->GetLookAt().x + 0.1f, cameraMain->GetLookAt().y, cameraMain->GetLookAt().z);
		cameraMain->Update();*/
		/*birdsEye->SetPosition(birdsEye->GetPosition().x + 0.1f, birdsEye->GetPosition().y, birdsEye->GetPosition().z);
		birdsEye->SetLookAt(birdsEye->GetLookAt().x + 0.1f, birdsEye->GetLookAt().y, birdsEye->GetLookAt().z);
		birdsEye->Update();
		SideView->SetPosition(SideView->GetPosition().x + 0.1f, SideView->GetPosition().y, SideView->GetPosition().z);
		SideView->SetLookAt(SideView->GetLookAt().x + 0.1f, SideView->GetLookAt().y, SideView->GetLookAt().z);
		SideView->Update();*/
	}

	/*if (GetAsyncKeyState('Q') & 0x8000)
	{
		cameraMain->SetPosition(cameraMain->GetPosition().x, cameraMain->GetPosition().y, cameraMain->GetPosition().z - 0.1f);
		cameraMain->SetLookAt(cameraMain->GetLookAt().x, cameraMain->GetLookAt().y, cameraMain->GetLookAt().z - 0.1f);
		cameraMain->Update();
		birdsEye->SetPosition(birdsEye->GetPosition().x, birdsEye->GetPosition().y, birdsEye->GetPosition().z - 0.1f);
		birdsEye->SetLookAt(birdsEye->GetLookAt().x, birdsEye->GetLookAt().y, birdsEye->GetLookAt().z - 0.1f);
		birdsEye->Update();
		SideView->SetPosition(SideView->GetPosition().x, SideView->GetPosition().y, SideView->GetPosition().z - 0.1f);
		SideView->SetLookAt(SideView->GetLookAt().x, SideView->GetLookAt().y, SideView->GetLookAt().z - 0.1f);
		SideView->Update();
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		cameraMain->SetPosition(cameraMain->GetPosition().x, cameraMain->GetPosition().y, cameraMain->GetPosition().z + 0.1f);
		cameraMain->SetLookAt(cameraMain->GetLookAt().x, cameraMain->GetLookAt().y, cameraMain->GetLookAt().z + 0.1f);
		cameraMain->Update();
		birdsEye->SetPosition(birdsEye->GetPosition().x, birdsEye->GetPosition().y, birdsEye->GetPosition().z + 0.1f);
		birdsEye->SetLookAt(birdsEye->GetLookAt().x, birdsEye->GetLookAt().y, birdsEye->GetLookAt().z + 0.1f);
		birdsEye->Update();
		SideView->SetPosition(SideView->GetPosition().x, SideView->GetPosition().y, SideView->GetPosition().z + 0.1f);
		SideView->SetLookAt(SideView->GetLookAt().x, SideView->GetLookAt().y, SideView->GetLookAt().z + 0.1f);
		SideView->Update();
	}*/

	if (GetAsyncKeyState('Y') & 0x8000)
	{
		cameraMain->SetLookAt(cameraMain->GetLookAt().x, cameraMain->GetLookAt().y + 0.01f, cameraMain->GetLookAt().z);
		cameraMain->Update();
	}

	if (GetAsyncKeyState('H') & 0x8000)
	{
		cameraMain->SetLookAt(cameraMain->GetLookAt().x, cameraMain->GetLookAt().y - 0.01f, cameraMain->GetLookAt().z);
		cameraMain->Update();
	}

	if (GetAsyncKeyState('Z') & 0x8000)
	{
		/*cameraMain->RotateY(0.01f);
		cameraMain->Update();*/
	}

	if (GetAsyncKeyState('G') & 0x8000)
	{
		cameraMain->SetLookAt(cameraMain->GetLookAt().x - 0.01f, cameraMain->GetLookAt().y, cameraMain->GetLookAt().z);
		cameraMain->Update();
	}

	if (GetAsyncKeyState('J') & 0x8000)
	{
		cameraMain->SetLookAt(cameraMain->GetLookAt().x + 0.01f, cameraMain->GetLookAt().y, cameraMain->GetLookAt().z);
		cameraMain->Update();
	}

	if (GetAsyncKeyState('1') & 0x8000)
	{
		cameraMain->SetPosition(OriginalCam->GetPosition().x, OriginalCam->GetPosition().y, OriginalCam->GetPosition().z);
		cameraMain->SetLookAt(OriginalCam->GetLookAt().x, OriginalCam->GetLookAt().y, OriginalCam->GetLookAt().z);
		cameraMain->SetUp(OriginalCam->GetUp().x, OriginalCam->GetUp().y, OriginalCam->GetUp().z);
		cameraMain->Update();
	}

	if (GetAsyncKeyState('2') & 0x8000)
	{
		cameraMain->SetPosition(birdsEye->GetPosition().x, birdsEye->GetPosition().y, birdsEye->GetPosition().z);
		cameraMain->SetLookAt(birdsEye->GetLookAt().x, birdsEye->GetLookAt().y, birdsEye->GetLookAt().z);
		cameraMain->SetUp(birdsEye->GetUp().x, birdsEye->GetUp().y, birdsEye->GetUp().z);
		cameraMain->Update();
	}

	if (GetAsyncKeyState('3') & 0x8000)
	{
		cameraMain->SetPosition(SideView->GetPosition().x, SideView->GetPosition().y, SideView->GetPosition().z);
		cameraMain->SetLookAt(SideView->GetLookAt().x, SideView->GetLookAt().y, SideView->GetLookAt().z);
		cameraMain->SetUp(SideView->GetUp().x, SideView->GetUp().y, SideView->GetUp().z);
		cameraMain->Update();
	}

	if (GetAsyncKeyState('9') & 0x8000)
	{
		if (!isWireframeOn)
		{
			isWireframeOn = true;
			InitRenderState();
		}
		else
		{
			isWireframeOn = false;
		}
	}

	eyePos = { cameraMain->GetLookAt().x, cameraMain->GetLookAt().y, cameraMain->GetLookAt().z };
	
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
	float ClearColor[4] = { 0.8f, 0.8f, 1.0f, 1.0f }; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //
    // Renders a triangle
    //
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

	ConstantBuffer cb;

	XMFLOAT4X4 viewAsFloats = cameraMain->GetView();
	XMFLOAT4X4 projectionAsFloats = cameraMain->GetProjection();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);

	cb.light = basicLight;
	cb.EyePosW = cameraMain->GetPosition();

	for (auto gameObject : _gameObjects)
	{
		// Get render material
		Material material = gameObject->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material.ambient;
		cb.surface.DiffuseMtrl = material.diffuse;
		cb.surface.SpecularMtrl = material.specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetTransform()->GetWorldMatrix());

		// Set texture
		if (gameObject->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView * textureRV = gameObject->GetAppearance()->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		// Draw object
		gameObject->Draw(_pImmediateContext);
	}

	cb.World = XMMatrixIdentity();
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_ps->Draw(_pImmediateContext);

	for (auto particles : _ps->getParticles())
	{
		// Get render material
		Material material = particles->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material.ambient;
		cb.surface.DiffuseMtrl = material.diffuse;
		cb.surface.SpecularMtrl = material.specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(particles->GetTransform()->GetWorldMatrix());

		// Set texture
		if (particles->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView * textureRV = particles->GetAppearance()->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		// Draw object
		particles->Draw(_pImmediateContext);
	}

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}