#include "Application.h"
#include <string>

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

bool Application::HandleKeyboard(MSG msg)
{
	switch (msg.wParam)
	{
	case VK_UP:
		_cameraOrbitRadius = max(_cameraOrbitRadiusMin, _cameraOrbitRadius - (_cameraSpeed * 0.2f));
		return true;
		break;

	case VK_DOWN:
		_cameraOrbitRadius = min(_cameraOrbitRadiusMax, _cameraOrbitRadius + (_cameraSpeed * 0.2f));
		return true;
		break;

	case VK_RIGHT:
		_cameraOrbitAngleXZ -= _cameraSpeed;
		return true;
		break;

	case VK_LEFT:
		_cameraOrbitAngleXZ += _cameraSpeed;
		return true;
		break;
	}

	return false;
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
	_pCubeVertexBuffer = nullptr;
	_pCubeIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;

	DSLessEqual = nullptr;
	RSCullNone = nullptr;
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

	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\stone.dds", nullptr, &_pTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\floor.dds", nullptr, &_pGroundTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\blank.dds", nullptr, &_pBlankTextureRV);

    // Setup Camera
	_camera = new Camera(XMFLOAT3(0.0f, 2.0f, -1.0f), XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		(float)_renderWidth, (float)_renderHeight, 0.01f, 200.0f);
	_currentCamera = _camera;

	_carCamera = new Camera(XMFLOAT3(0.0f, 2.0f, -7.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		(float)_renderWidth, (float)_renderHeight, 0.01f, 200.0f);

	// Setup the scene's light
	basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	_windDir = Vector3(1.0f, 0.0f, 0.0f);
	_windChangeChance = 10.0f;
	_windStrength = 1.0f;
	_windChangeDelay = 5.0f;
	_windTimeWaited = 0.0f;

	InitGameObjects();

	return S_OK;
}

HRESULT Application::InitGameObjects()
{
	//Geometry
/////////////////////////////////////////////////////////////
	Geometry* cubeGeometry = new Geometry();
	cubeGeometry->indexBuffer = _pCubeIndexBuffer;
	cubeGeometry->vertexBuffer = _pCubeVertexBuffer;
	cubeGeometry->numberOfIndices = 36;
	cubeGeometry->vertexBufferOffset = 0;
	cubeGeometry->vertexBufferStride = sizeof(SimpleVertex);

	Geometry* skyboxGeometry = new Geometry();
	skyboxGeometry->indexBuffer = _pSkyboxIndexBuffer;
	skyboxGeometry->vertexBuffer = _pSkyboxVertexBuffer;
	skyboxGeometry->numberOfIndices = 36;
	skyboxGeometry->vertexBufferOffset = 0;
	skyboxGeometry->vertexBufferStride = sizeof(SimpleVertex);

	Geometry* planeGeometry = new Geometry();
	planeGeometry->indexBuffer = _pPlaneIndexBuffer;
	planeGeometry->vertexBuffer = _pPlaneVertexBuffer;
	planeGeometry->numberOfIndices = 6;
	planeGeometry->vertexBufferOffset = 0;
	planeGeometry->vertexBufferStride = sizeof(SimpleVertex);

	//Materials
/////////////////////////////////////////////////////////////////////////
	Material* shinyMaterial = new Material();
	shinyMaterial->ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial->specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial->specularPower = 10.0f;

	Material* noSpecMaterial = new Material();
	noSpecMaterial->ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial->specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial->specularPower = 0.0f;

	Material* lowLightMaterial = new Material();
	lowLightMaterial->ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	lowLightMaterial->diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	lowLightMaterial->specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	lowLightMaterial->specularPower = 0.0f;

	//OctaTree
/////////////////////////////////////////////////////////////////////////
	Appearance* appearance = new Appearance(cubeGeometry, noSpecMaterial);
	appearance->SetTextureRV(_pTextureRV);

	Transform* transform = new Transform(Vector3(16.0f), Vector3(), Vector3());
	
	ParticleModel* particleModel = new ParticleModel(transform, _cubeCenterMass, _cubeRect, _cubeVertices);
	
	GameObject* debugCube = new GameObject("DebugCube", appearance, transform, particleModel);

	Node* head = new Node(0, debugCube);
	_octaTree = new OctaTree(head, true, 4);

	//Floor Obj
///////////////////////////////////////////////////////////////////
	appearance = new Appearance(planeGeometry, noSpecMaterial);
	appearance->SetTextureRV(_pGroundTextureRV);

	transform = new Transform(Vector3(15.0f, 1.0f, 15.0f), Vector3(),Vector3());

	particleModel = new ParticleModel(transform, _planeCenterMass, _planeRect, _planeVertices);
	particleModel->SetMass(100.0f);

	GameObject* gameObject = new GameObject("Floor", appearance, transform, particleModel);
	_staticObjects.push_back(gameObject);
///////////////////////////////////////////////////////////////////
	
	//Skybox Obj
	appearance = new Appearance(skyboxGeometry, noSpecMaterial);

	transform = new Transform(Vector3(16.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3());

	particleModel = new ParticleModel(transform, _skyboxCenterMass, _skyboxRect, _skyboxVertices);

	gameObject = new GameObject("Skybox", appearance, transform, particleModel);
	//staticObjects.push_back(gameObject);
///////////////////////////////////////////////////////////////////

	//Cube Objects - NonStatic
	CarBody* carBody;
	for (unsigned int i = 0; i < 1; i++)
	{
		appearance = new Appearance(cubeGeometry, shinyMaterial);
		appearance->SetTextureRV(_pTextureRV);

		//transform = new Transform(Vector3(0.5f), Vector3(0.0f + (i * 0.5f), 3.0f + (i * 2.0f), 0.0f), Vector3(0.0f, 0.0f, 45.0f));
		transform = new Transform(Vector3(0.5f), Vector3(0.0f + (i * 1.5f), 1.51f, 3.0f), Vector3());
		//transform = new Transform(Vector3(0.5f), Vector3(0.0f, 0.9f, 3.0f), Vector3());
		
		carBody = new CarBody(transform, _cubeCenterMass, _cubeRect, _cubeVertices);
		carBody->DisableStatic();
		carBody->SetRadius(transform->GetScale().x + (transform->GetScale().x / 2));
		carBody->SetMass(50.0f);

		gameObject = new GameObject("Car" + std::to_string(i), appearance, transform, carBody);
		_movingObjects.push_back(gameObject);
	}

	if (!_movingObjects.empty())
		_carCamera->SetParent(_movingObjects.at(0));
	else
	{
		_carCamera = nullptr;
		delete _carCamera;
	}

	//Cube Objects - Static
	for (unsigned int i = 0; i < 8; i++)
	{
		appearance = new Appearance(cubeGeometry, shinyMaterial);
		appearance->SetTextureRV(_pTextureRV);

		transform = new Transform(Vector3(0.5f), Vector3(-5.0f + (i * 1.5f), 0.5f, 10.0f), Vector3());
		particleModel = new ParticleModel(transform, _cubeCenterMass, _cubeRect, _cubeVertices);
		particleModel->SetRadius(transform->GetScale().x + (transform->GetScale().x / 2));
		particleModel->SetMass(100.0f);

		gameObject = new GameObject("Static" + std::to_string(i), appearance, transform, particleModel);
		_staticObjects.push_back(gameObject);
	}

	_octaTree->CreateTree(_staticObjects);
///////////////////////////////////////////////////////////////////

	//Cube Objects - Particles
	_particleSystem = new ParticleSystem();

	appearance = new Appearance(cubeGeometry, lowLightMaterial);
	appearance->SetTextureRV(_pBlankTextureRV);
	appearance->SetBlendState(_pTransparency);
	appearance->SetBlendFactor(Vector3(0.2f));
	appearance->SetTransparency(true);

	//Fire Particles
	transform = new Transform(Vector3(0.05f), Vector3(5.0f, 0.07f, 3.0f), Vector3());
	particleModel = new ParticleModel(transform, _cubeCenterMass, _cubeRect, _cubeVertices);
	particleModel->SetRadius(transform->GetScale().x + (transform->GetScale().x / 2));
	particleModel->DisableStatic();
	particleModel->SetMass(0.1f);

	gameObject = new GameObject("Particle", appearance, transform, particleModel);

	FireParManager* fParManager = new FireParManager("FireParManager", gameObject, 180.0f, 20.0f, 500, false);
	_particleSystem->AddParticleManager(fParManager);

	//Snow Particles
	appearance = new Appearance(cubeGeometry, shinyMaterial);
	appearance->SetTextureRV(_pBlankTextureRV);

	transform = new Transform(Vector3(0.05f), Vector3(0.0f, 15.0f, 0.0f), Vector3());
	particleModel = new ParticleModel(transform, _cubeCenterMass, _cubeRect, _cubeVertices);
	particleModel->SetRadius(transform->GetScale().x + (transform->GetScale().x / 2));
	particleModel->DisableStatic();
	gameObject = new GameObject("Particle", appearance, transform, particleModel);

	SnowParManager* sParManager = new SnowParManager("SnowParManager", gameObject, 180.0f, 35.0f, 1500, false);
	_particleSystem->AddParticleManager(sParManager);

	//Rain Particles
	appearance = new Appearance(cubeGeometry, lowLightMaterial);
	appearance->SetTextureRV(_pBlankTextureRV);

	transform = new Transform(Vector3(0.005f, 0.1f, 0.005f), Vector3(0.0f, 15.0f, 0.0f), Vector3());
	particleModel = new ParticleModel(transform, _cubeCenterMass, _cubeRect, _cubeVertices);
	particleModel->SetRadius(transform->GetScale().y + (transform->GetScale().y / 2));
	particleModel->DisableStatic();
	gameObject = new GameObject("Particle", appearance, transform, particleModel);

	RainParManager* rParManager = new RainParManager("RainParManager", gameObject, 180.0f, 10.0f, 1000, false);
	_particleSystem->AddParticleManager(rParManager);
///////////////////////////////////////////////////////////////////

	//CleanUp
	cubeGeometry = nullptr;
	delete cubeGeometry;

	skyboxGeometry = nullptr;
	delete skyboxGeometry;

	planeGeometry = nullptr;
	delete planeGeometry;

	shinyMaterial = nullptr;
	delete planeGeometry;

	noSpecMaterial = nullptr;
	delete noSpecMaterial;

	head = nullptr;
	delete head;

	debugCube = nullptr;
	delete debugCube;

	fParManager = nullptr;
	delete fParManager;

	sParManager = nullptr;
	delete sParManager;

	gameObject = nullptr;
	delete gameObject;

	appearance = nullptr;
	delete appearance;

	transform = nullptr;
	delete transform;

	particleModel = nullptr;
	delete particleModel;

	carBody = nullptr;
	delete carBody;

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

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    };

	Rect3 rect = Rect3();
	float max = INT_MIN, min = INT_MAX;

	for (auto simpleVertex : vertices)
	{
		_cubeVertices.push_back(ToVec3(simpleVertex.PosL));

		float sum = 0;
		sum += simpleVertex.PosL.x;
		sum += simpleVertex.PosL.y;
		sum += simpleVertex.PosL.z;

		if (sum > max)
		{
			max = sum;
			rect.maxPoint = ToVec3(simpleVertex.PosL);
		}

		if (sum < min)
		{
			min = sum;
			rect.minPoint = ToVec3(simpleVertex.PosL);
		}
		_cubeCenterMass += simpleVertex.PosL;
	}
	_cubeRect = rect;
	_cubeCenterMass /= (float)std::size(vertices);

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pCubeVertexBuffer);

    if (FAILED(hr))
        return hr;

	// Create vertex buffer
	SimpleVertex skyboxVertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	rect = Rect3();
	max = INT_MIN, min = INT_MAX;

	for (auto simpleVertex : skyboxVertices)
	{
		_skyboxVertices.push_back(ToVec3(simpleVertex.PosL));

		float sum = 0;
		sum += simpleVertex.PosL.x;
		sum += simpleVertex.PosL.y;
		sum += simpleVertex.PosL.z;

		if (sum > max)
		{
			max = sum;
			rect.maxPoint = ToVec3(simpleVertex.PosL);
		}

		if (sum < min)
		{
			min = sum;
			rect.minPoint = ToVec3(simpleVertex.PosL);
		}
		_skyboxCenterMass += simpleVertex.PosL;
	}
	_skyboxRect = rect;
	_skyboxCenterMass /= (float)std::size(skyboxVertices);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = skyboxVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pSkyboxVertexBuffer);

	if (FAILED(hr))
		return hr;

	// Create vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	rect = Rect3();
	max = INT_MIN, min = INT_MAX;

	for (auto simpleVertex : planeVertices)
	{
		_planeVertices.push_back(ToVec3(simpleVertex.PosL));

		float sum = 0;
		sum += simpleVertex.PosL.x;
		sum += simpleVertex.PosL.y;
		sum += simpleVertex.PosL.z;

		if (sum > max)
		{
			max = sum;
			rect.maxPoint = ToVec3(simpleVertex.PosL);
		}

		if (sum < min)
		{
			min = sum;
			rect.minPoint = ToVec3(simpleVertex.PosL);
		}

		_planeCenterMass += simpleVertex.PosL;
	}
	_planeRect = rect;
	_planeCenterMass /= (float)std::size(planeVertices);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pCubeIndexBuffer);
	_pSkyboxIndexBuffer = _pCubeIndexBuffer;

    if (FAILED(hr))
        return hr;

	// Create plane index buffer
	WORD planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

	if (FAILED(hr))
		return hr;

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
    RECT rc = {0, 0, 960, 540};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
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

HRESULT Application::InitDevice()
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

	UINT sampleCount = 4;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _renderWidth;
    sd.BufferDesc.Height = _renderHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleCount;
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

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_renderWidth;
    vp.Height = (FLOAT)_renderHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _renderWidth;
	depthStencilDesc.Height = _renderHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Rasterizer
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CWcullMode);

	//WireFrame RState
	D3D11_RASTERIZER_DESC wfdesc;

	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	wfdesc.FrontCounterClockwise = true;

	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_pWireFrame);
	if (FAILED(hr))
		return hr;

	//pixel blending
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	hr = _pd3dDevice->CreateBlendState(&blendDesc, &_pTransparency);
	if (FAILED(hr))
		return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
	if (_pSamplerLinear) _pSamplerLinear->Release();

	if (_pTextureRV) _pTextureRV->Release();
	if (_pGroundTextureRV) _pGroundTextureRV->Release();
	if (_pBlankTextureRV) _pBlankTextureRV->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();

    if (_pCubeVertexBuffer) _pCubeVertexBuffer->Release();
    if (_pCubeIndexBuffer) _pCubeIndexBuffer->Release();
	if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
	if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();
	//if (_pSkyboxVertexBuffer) _pSkyboxVertexBuffer->Release();
	//if (_pSkyboxIndexBuffer) _pSkyboxIndexBuffer->Release();

    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	if (DSLessEqual) DSLessEqual->Release();
	if (RSCullNone) RSCullNone->Release();

	if (CCWcullMode) CCWcullMode->Release();
	if (CWcullMode) CWcullMode->Release();
	if (_pWireFrame) _pWireFrame->Release();
	if(_pTransparency) _pTransparency->Release();

	if (_camera) _camera->~Camera();
	if (_carCamera) _carCamera->~Camera();

	if(_octaTree) _octaTree->~OctaTree();
	if(_particleSystem) _particleSystem->~ParticleSystem();
	
	_movingObjects.clear();
	_staticObjects.clear();
}

void Application::CollisionDetection(float deltaTime)
{
	std::vector<GameObject*> collisionObjs = _movingObjects;

	//add all collidable particles to collision check array
	for (BaseParManager* manager : _particleSystem->GetManagers())
	{
		for (Particle* par : manager->GetCollidableParticles())
		{
			collisionObjs.push_back(par->object);
		}
	}

	//check for collisions
	if (!collisionObjs.empty())
	{
		//Check Collision For moving objects
		for (unsigned int i = 0; i < collisionObjs.size(); i++)
		{
			std::vector<CollisionPair*> pairs = _octaTree->GetCollisionPairs(collisionObjs.at(i));
			for (auto pair : pairs)
			{
				CheckCollision(pair->firstObj, pair->secondObj, deltaTime);
			}
		}

		//Check Collision For moving objects
		for (unsigned int i = 0; i < collisionObjs.size() - 1; i++)
		{
			for (unsigned int j = i + 1; j < collisionObjs.size(); j++)
			{
				CheckCollision(collisionObjs.at(i), collisionObjs.at(j), deltaTime);
			}
		}
	}
}

bool Application::CheckCollision(GameObject* aObj, GameObject* bObj, float deltaTime)
{
	if (!aObj->GetParticleModel()->IsStatic() || !bObj->GetParticleModel()->IsStatic())
	{
		float aRadius = aObj->GetParticleModel()->GetRadius();
		float bRadius = bObj->GetParticleModel()->GetRadius();

		if (SphereCollision(aObj->GetTransform()->GetPosition(), aRadius, bObj->GetTransform()->GetPosition(), bRadius)
				|| aRadius == 0.0f || bRadius == 0.0f)
		{
			Rect3 aScaled = aObj->GetParticleModel()->GetAxisAlignedRect3World();
			Rect3 bScaled = bObj->GetParticleModel()->GetAxisAlignedRect3World();

			if (BoundingBoxCollision(aScaled, bScaled))
			{
				Rect3 a = aObj->GetParticleModel()->GetRect3Local();
				Rect3 b = bObj->GetParticleModel()->GetRect3Local();

				CollisionResults results;
				if ((results = RotatedBoundingBoxCollision(a, *aObj->GetTransform(), b, *bObj->GetTransform())).collided)
				{
					CollisionResolution(aObj, bObj, results, deltaTime);
					return true;
				}
			}
		}
	}
	return false;
}

void Application::CollisionResolution(GameObject * a, GameObject * b, CollisionResults results, float deltaTime)
{
	//Debugging
	if (a->GetName() == "Car0" && b->GetName() == "Car1")
	{
		bool fake = true;
	}

	//1-0: absorb nothing - absorb all 
	float coefficient = 0.5f;
	float buffer = 0.05f;

	ParticleModel* tempA = a->GetParticleModel();
	ParticleModel* tempB = b->GetParticleModel();

	float massSum = tempA->GetMass() + tempB->GetMass();

	Vector3 massVelSum = (tempA->GetVelocity() * tempA->GetMass()) + 
							(tempB->GetVelocity() * tempB->GetMass());
	
	Vector3	finalVelA, finalVelB;

	//calculate A's finalVel
	if (!b->GetParticleModel()->IsStatic())
	{
		if (results.pen == 0)
			b->GetTransform()->SetPosition(b->GetTransform()->GetOldPosition());
		else
			b->GetTransform()->SetPosition(b->GetTransform()->GetPosition() - (Vector3(results.dirOfReflection) * results.pen));
		
		finalVelA = (massVelSum + (tempB->GetMass() * coefficient *
			(tempB->GetVelocity() - tempA->GetVelocity())) / massSum);
	}
	else
	{
		finalVelA = (tempA->GetVelocity() - (2 * Dot(tempA->GetVelocity(), results.dirOfReflection)) *
			results.dirOfReflection) * coefficient;
		//finalVelA = -(tempA->GetVelocity() / coefficient);

		if (finalVelA.x < buffer && finalVelA.x > -buffer)
		{
			finalVelA.x = 0.0f;
		}

		if (finalVelA.y < buffer && finalVelA.y > -buffer)
		{
			finalVelA.y = 0.0f;
		}

		if (finalVelA.z < buffer && finalVelA.z > -buffer)
		{
			finalVelA.z = 0.0f;
		}
	}

	//calculate B's finalVel
	if (!a->GetParticleModel()->IsStatic())
	{
		if(results.pen == 0)
			a->GetTransform()->SetPosition(a->GetTransform()->GetOldPosition());
		else
			a->GetTransform()->SetPosition(a->GetTransform()->GetPosition() - (Vector3(results.dirOfReflection * -1.0f) * results.pen));
		
		finalVelB = (massVelSum + (tempA->GetMass() * coefficient *
			(tempA->GetVelocity() - tempB->GetVelocity())) / massSum);
	}
	else
	{
		finalVelB = (tempB->GetVelocity() - (2 * Dot(tempB->GetVelocity(), results.dirOfReflection)) * 
			results.dirOfReflection) * coefficient;
		//finalVelB = -(tempB->GetVelocity() / coefficient);

		if (finalVelB.x < buffer && finalVelB.x > -buffer)
		{
			finalVelB.x = 0.0f;
		}

		if (finalVelB.y < buffer && finalVelB.y > -buffer)
		{
			finalVelB.y = 0.0f;
		}

		if (finalVelB.z < buffer && finalVelB.z > -buffer)
		{
			finalVelB.z = 0.0f;
		}
	}

	//A isn't static
	if (!a->GetParticleModel()->IsStatic())
	{
		finalVelA = Vec3Normalization(results.dirOfReflection) * finalVelA.Length();
		
		Vector3 force = finalVelA / deltaTime;
		force = finalVelA * a->GetParticleModel()->GetMass();
		
		//tempA->AddForce(force);
		tempA->SetVelocity(finalVelA);

		CarBody* temp1 = (CarBody*)a->GetParticleModel();
		if (temp1)
		{
			temp1->CalculateAngularTorque(finalVelA, results.pointOfContact, deltaTime);
		}
	}

	//B isn't static
	if(!b->GetParticleModel()->IsStatic())
	{
		finalVelB = (Vec3Normalization(results.dirOfReflection) * -1.0f) * finalVelB.Length();
		
		Vector3 force = finalVelA / deltaTime;
		force = finalVelA * a->GetParticleModel()->GetMass();

		//tempB->AddForce(force);
		tempB->SetVelocity(finalVelB);

		CarBody* temp2 = (CarBody*)b->GetParticleModel();
		if (temp2)
		{
			temp2->CalculateAngularTorque(finalVelB, results.pointOfContact, deltaTime);
		}
	}
}

void Application::Update(float deltaTime)
{
	//Turn on/off particle managers
	///////////////////////////////////////////////////
	if (GetAsyncKeyState(VK_NUMPAD1) & 1)
	{
		FireParManager* temp = (FireParManager*)_particleSystem->GetManager("FireParManager");
		if (temp)
		{
			if (temp->IsActive())
			{
				temp->DisableManager();
				temp->Reset();
			}
			else
				temp->EnableManager();
		}
	}

	if (GetAsyncKeyState(VK_NUMPAD2) & 1)
	{
		SnowParManager* temp = (SnowParManager*)_particleSystem->GetManager("SnowParManager");
		if (temp)
		{
			if (temp->IsActive())
			{
				temp->DisableManager();
				temp->Reset();
			}
			else
				temp->EnableManager();
		}
	}

	if (GetAsyncKeyState(VK_NUMPAD3) & 1)
	{
		RainParManager* temp = (RainParManager*)_particleSystem->GetManager("RainParManager");
		if (temp)
		{
			if (temp->IsActive())
			{
				temp->DisableManager();
				temp->Reset();
			}
			else
				temp->EnableManager();
		}
	}
	///////////////////////////////////////////////////

	if (GetAsyncKeyState('O'))
	{
		CarBody* temp = (CarBody*)_movingObjects.at(0)->GetParticleModel();
		if (temp)
			temp->SetLinearThrust(Vector3(0.1f, 0.0f, 0.0f));
		//_gameObjects.at(1)->GetParticleModel()->SetForwardThrust({ 30, 0, 0 });1111111111111111111111111111
		//_gameObjects.at(1)->Update(_deltaTime);
	}

	if (GetAsyncKeyState('P'))
	{
		CarBody* temp = (CarBody*)_movingObjects.at(1)->GetParticleModel();
		//_gameObjects.at(2)->GetParticleModel()->SetUpThrust({ 0, 0, 0 });

		if(temp)
			temp->SetLinearThrust(Vector3(-0.15f, 0.0f, 0.0f));
		//_gameObjects.at(2)->GetParticleModel()->SetForwardThrust({ -30, 0, 0 });
		//_gameObjects.at(2)->Update(_deltaTime);
	}

	if (GetAsyncKeyState('K'))
	{
		for (auto gameObject : _movingObjects)
		{
			CarBody* temp = (CarBody*)_movingObjects.at(1)->GetParticleModel();

			if (temp)
				temp->SetLinearThrust(Vector3());

			//_gameObjects.at(i)->GetTransform()->SetPosition(Vector3());
			gameObject->GetParticleModel()->SetVelocity(Vector3());
			gameObject->GetParticleModel()->SetAcceleration(Vector3());
		}
	}

	//apply wind
	_windTimeWaited += deltaTime;
	if (_windTimeWaited >= _windChangeDelay)
	{
		int randChance = rand() % 100;
		if (randChance <= _windChangeChance)
		{
			_windTimeWaited = 0.0f;
			OutputDebugStringA("change");
			_windStrength = rand() % 10 + 2;
			_windDir = Vector3(RandomClamped(), 0.0f, RandomClamped());
		}
		else
			_windTimeWaited -= 2.0f;
	}

	//Pole pole = Pole(_movingObjects.at(0), _movingObjects.at(1));

	// Update objects
	for (auto gameObject : _movingObjects)
	{
		if (!GetAsyncKeyState('Q'))
			//gameObject->GetParticleModel()->AddForce(_windDir * _windStrength);

		gameObject->Update(deltaTime);
	}

	if (GetAsyncKeyState('Z'))
	{
		Spring spring = Spring(_movingObjects.at(0), _movingObjects.at(1), 5.0f, 10.0f);
		spring.Update(deltaTime);
	}

	//pole.Update(deltaTime);

	for (auto gameObject : _staticObjects)
	{
		gameObject->Update(deltaTime);
	}

	_particleSystem->UpdateParManagers(deltaTime);

	CollisionDetection(deltaTime);

	// Update camera
	float angleAroundZ = XMConvertToRadians(_cameraOrbitAngleXZ);

	float x = _cameraOrbitRadius * cos(angleAroundZ);
	float z = _cameraOrbitRadius * sin(angleAroundZ);

	XMFLOAT3 cameraPos = _camera->GetPosition();
	cameraPos.x = x;
	cameraPos.z = z;

	_camera->SetPosition(cameraPos);
	_camera->Update();

	_currentCamera = _camera;
	if (GetAsyncKeyState('M'))
	{
		_carCamera->Update();
		_currentCamera = _carCamera;
	}
}

void Application::Draw(float deltaTime)
{
    //
    // Clear buffers
    //

	float ClearColor[4] = { 0.3f, 0.6f, 0.8f, 1.0f }; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //
    // Setup buffers and render scene
    //

	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    ConstantBuffer cb;

	XMFLOAT4X4 viewAsFloats = _currentCamera->GetView();
	XMFLOAT4X4 projectionAsFloats = _currentCamera->GetProjection();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);
	
	cb.light = basicLight;
	cb.EyePosW = _currentCamera->GetPosition();

	_pImmediateContext->RSSetState(CWcullMode);
	
	//moving objects
	for (auto gameObject : _movingObjects)
	{
		// Get render material
		Material* material = gameObject->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

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

	//draw static objects
	for (auto gameObject : _staticObjects)
	{
		// Get render material
		Material* material = gameObject->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

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

	//draw particleSystem Objects
	_particleSystem->DrawParManagers(_pImmediateContext, _pConstantBuffer, cb);

	_pImmediateContext->RSSetState(_pWireFrame);
	
	//_octaTree->DrawOctaTree(_pImmediateContext, _pConstantBuffer, cb, _octaTree->GetHead(), deltaTime);
	_octaTree->DrawOctaTreeLeafs(_pImmediateContext, _pConstantBuffer, cb, _octaTree->GetHead(), deltaTime, _octaTree->GetHeight(_octaTree->GetHead()));

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}