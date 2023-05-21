#include "Application.h"

#define NUMBEROFCUBES 4

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
	XMFLOAT3 cameraPosition = _camera->GetPosition();

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
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	CCWcullMode=nullptr;
	CWcullMode= nullptr;
	DSLessEqual = nullptr;
	RSCullNone = nullptr;
	 _WindowHeight = 0;
	 _WindowWidth = 0;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{

	HRESULT hr = 0;

    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
	if (!GetClientRect(_hWnd, &rc)) { return E_FAIL; }
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	
	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Assets\\Textures\\stone.dds", nullptr, &_pTextureRV);
	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Assets\\Textures\\floor.dds", nullptr, &_pGroundTextureRV);
	
	if (FAILED(hr))
	{
		return E_FAIL;
	}

    // Setup Camera
	XMFLOAT3 eye = XMFLOAT3(0.0f, 2.0f, -1.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 2.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_camera = new Camera(eye, at, up, (float)_renderWidth, (float)_renderHeight, 0.01f, 200.0f);

	// Setup the scene's light
	basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	shared_ptr<Geometry> donutGeometry = std::make_shared<Geometry>();
	objMeshData = OBJLoader::Load("Assets/3DModels/donut.obj", _pd3dDevice);

	donutGeometry->indexBuffer = objMeshData.IndexBuffer;
	donutGeometry->numberOfIndices = objMeshData.IndexCount;
	donutGeometry->vertexBuffer = objMeshData.VertexBuffer;
	donutGeometry->vertexBufferOffset = objMeshData.VBOffset;
	donutGeometry->vertexBufferStride = objMeshData.VBStride;

	shared_ptr<Geometry> sphereGeometry = std::make_shared<Geometry>();
	objMeshData = OBJLoader::Load("Assets/3DModels/sphere.obj", _pd3dDevice);
	sphereGeometry->indexBuffer = objMeshData.IndexBuffer;
	sphereGeometry->numberOfIndices = objMeshData.IndexCount;
	sphereGeometry->vertexBuffer = objMeshData.VertexBuffer;
	sphereGeometry->vertexBufferOffset = objMeshData.VBOffset;
	sphereGeometry->vertexBufferStride = objMeshData.VBStride;

	shared_ptr<Geometry> cubeGeometry = std::make_shared<Geometry>();
	cubeGeometry->indexBuffer = _pIndexBuffer;
	cubeGeometry->vertexBuffer = _pVertexBuffer;
	cubeGeometry->numberOfIndices = 36;
	cubeGeometry->vertexBufferOffset = 0;
	cubeGeometry->vertexBufferStride = sizeof(SimpleVertex);

	shared_ptr<Geometry> planeGeometry = std::make_shared<Geometry>();
	planeGeometry->indexBuffer = _pPlaneIndexBuffer;
	planeGeometry->vertexBuffer = _pPlaneVertexBuffer;
	planeGeometry->numberOfIndices = 6;
	planeGeometry->vertexBufferOffset = 0;
	planeGeometry->vertexBufferStride = sizeof(SimpleVertex);

	shared_ptr<Material> shinyMaterial = std::make_shared<Material>();
	shinyMaterial->ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial->specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial->specularPower = 10.0f;

	shared_ptr<Material> noSpecMaterial = std::make_shared<Material>();
	noSpecMaterial->ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial->specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial->specularPower = 0.0f;

	Transform* transform;

	transform = new Transform(Vector3(0.0f, -1.0f, 0.0f), MakeQFromEulerAngles(90,0,0), Vector3(15.0f, 15.0f, 1.0f));


	Appearance floorAppearance(planeGeometry, noSpecMaterial, _pGroundTextureRV);
	GameObject* gameObject = new GameObject("Floor", floorAppearance, transform);
	Vector3 normal = Vector3(0, 1, 0);
	Vector3 pointOnPlane = transform->GetPosition();
	float distance = _math.Dot(normal, pointOnPlane);
	Vector3 min = transform->GetPosition() - transform->GetScale() / 2;
	Vector3 max = transform->GetPosition() + transform->GetScale() / 2;
	PlaneCollider* collider = new PlaneCollider(transform, normal, distance,min,max);
	gameObject->GetPhysicsModel()->SetCollider(collider);


	_gameObjects.push_back(gameObject);

	Transform* gameObjectTransform;
	gameObject;

	for (auto i = 0; i < NUMBEROFCUBES; i++)
	{
		transform = new Transform();
		transform->SetScale(Vector3(0.4f, 0.4f, 0.4f));
		transform->SetPosition(Vector3(-3.0f + (i * 2.5f), 1.0f, 10.0f));
		Appearance sphereAppearance(sphereGeometry, shinyMaterial, _pTextureRV);
		gameObject = new GameObject("Cube " + to_string(i), sphereAppearance, transform);

		SphereCollider* collider = new SphereCollider(transform, 1.0f);

		gameObject->GetPhysicsModel()->ApplyGravity();

		gameObject->GetPhysicsModel()->SetCollider(collider);

		if (i == 1 || i == 3)
		{
			transform = new Transform();
			transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
			transform->SetPosition(Vector3(-3.0f + (i * 2.5f), 1.0f, 10.0f));
			Appearance cubeAppearance(cubeGeometry, shinyMaterial, _pTextureRV);
			gameObject = new GameObject("Cube " + to_string(i), cubeAppearance, transform);
			Vector3 min = Vector3(-1, -1, -1);
			Vector3 max = Vector3(1, 1, 1);
			AABBCollider* collider = new AABBCollider(transform, min, max);

			gameObject->GetPhysicsModel()->SetCollider(collider);
			gameObject->GetPhysicsModel()->ApplyGravity();
		}

		//gameObject->GetPhysicsModel()->ToggleGravity();
		_gameObjects.push_back(gameObject);
	}

	accumulator = 0.0f;

	transform = new Transform();
	transform->SetScale(Vector3(0.5f, 0.5f, 0.5f));
	transform->SetPosition(Vector3(-6.0f, 0.5f, 10.0f));

	Appearance donutAppearance(donutGeometry, shinyMaterial, _pTextureRV);
	gameObject = new GameObject("Donut", donutAppearance, transform);


	_gameObjects.push_back(gameObject);

	for (int i = 0; i < _gameObjects.size(); i++)
	{
		if (_gameObjects[i]->GetPhysicsModel()->IsCollideable())
		{
			float temp = _gameObjects[i]->GetPhysicsModel()->GetCollider()->GetColliderType();
			if (temp == 1.0f || temp == 2.0f)
			{
				//DebugPrintF("oops");
				_gameObjects[i]->GetPhysicsModel()->ToggleGravity(true);
			}
		}
	}

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The HLSL file cannot be compiled.  Error output to Visual Studio Console.", L"Error", MB_OK);
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
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The HLSL file cannot be compiled.  Error output to Visual Studio Console.", L"Error", MB_OK);
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

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	// Create vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 5.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

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
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

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

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
	if (_pSamplerLinear) _pSamplerLinear->Release();

	if (_pTextureRV) _pTextureRV->Release();

	if (_pGroundTextureRV) _pGroundTextureRV->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();

    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
	if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
	if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();

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

	if (_camera)
	{
		delete _camera;
	}

	for (auto gameObject : _gameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
		}
	}
}



void Application::ToggleGravity(int objectNumber) 
{
	_gameObjects[objectNumber]->GetPhysicsModel()->ToggleGravity();
}


int times = 0;
std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point t2;

void Application::Update()
{
	if (times >= 60)
	{
		t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		times = 0;
		double seconds = time_span.count();
		//DebugPrintF("60 frames in %f seconds\n", seconds);
		t1 = std::chrono::high_resolution_clock::now();
	}
	times++;

    static DWORD dwTimeStart = 0;

    DWORD dwTimeCur = GetTickCount64();

    if (dwTimeStart == 0)
        dwTimeStart = dwTimeCur;

	float deltaTime = _timer.GetDeltaTime();
	accumulator += deltaTime;

	while (accumulator >= FPS60)
	{
		InputHandeling();
		CollisionHandeling();
		for (auto gameObject : _gameObjects)
		{
			gameObject->Update(FPS60);
		}
		accumulator -= FPS60;
	}


	// Update camera
	float angleAroundZ = XMConvertToRadians(_cameraOrbitAngleXZ);

	float x = _cameraOrbitRadius * cos(angleAroundZ);
	float z = _cameraOrbitRadius * sin(angleAroundZ);

	XMFLOAT3 cameraPos = _camera->GetPosition();
	cameraPos.x = x;
	cameraPos.z = z;

	_camera->SetPosition(cameraPos);
	_camera->Update();
}


void Application::Draw()
{
    //
    // Clear buffers
    //


	_timer.Tick();

	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
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

	XMFLOAT4X4 viewAsFloats = _camera->GetView();
	XMFLOAT4X4 projectionAsFloats = _camera->GetProjection();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);
	
	cb.light = basicLight;
	cb.EyePosW = _camera->GetPosition();

	// Render all scene objects
	for (auto gameObject : _gameObjects)
	{
		// Get render material
		shared_ptr<Material> material = gameObject->GetAppearance().GetMaterial();


		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetTransform()->GetWorldMatrix());
		  
		// Set texture
		if (gameObject->GetAppearance().HasTexture())
		{
			ID3D11ShaderResourceView* textureRV = gameObject->GetAppearance().GetTextureRV();

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

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}

bool isMKeyPressed = false;  
bool isXKeyPressed = false; 

void Application::InputHandeling()
{
	int objNum = 1;
	if (GetAsyncKeyState('1')) _controlSelection = 0.0f;
	if (GetAsyncKeyState('2'))_controlSelection = 1.0f;
	if (_controlSelection == 1.0f) 
	{
		objNum += 2;
	}
	if (GetAsyncKeyState('S'))
	{
		move(objNum, 1);
	}
	if (GetAsyncKeyState('W'))
	{
		move(objNum, 2);
	}
	if (GetAsyncKeyState('A'))
	{
		move(objNum, 3);
	}
	if (GetAsyncKeyState('D'))
	{
		move(objNum, 4);
	}
	if (GetAsyncKeyState('X'))
	{
		if (!isMKeyPressed)
		{
			move(objNum, 5);
			isMKeyPressed = true;
		}
	}
	else
	{
		isMKeyPressed = false;
	}
	if (GetAsyncKeyState('K'))
	{
		move(objNum+1, 1);
	}
	if (GetAsyncKeyState('I'))
	{
		move(objNum+1, 2);
	}
	if (GetAsyncKeyState('J'))
	{
		move(objNum+1, 3);
	}
	if (GetAsyncKeyState('L'))
	{
		move(objNum+1, 4);
	}
	if (GetAsyncKeyState('M'))
	{
		if (!isMKeyPressed)
		{
			move(objNum+1, 5);
			isMKeyPressed = true;
		}
	}
	else
	{
		isMKeyPressed = false;
	}

}

void Application::move(int objectNumber, int dir)
{
	float speed = 25.0f;
	switch (dir)
	{
	case 1:
		_gameObjects[objectNumber]->GetPhysicsModel()->AddForce(Vector3(0.0f, 0.0f, -speed));
		break;
	case 2:
		_gameObjects[objectNumber]->GetPhysicsModel()->AddForce(Vector3(0.0f, 0.f, speed));
		break;
	case 3:
		_gameObjects[objectNumber]->GetPhysicsModel()->AddForce(Vector3(-speed, 0.0f, 0.0f));
		break;
	case 4:
		_gameObjects[objectNumber]->GetPhysicsModel()->AddForce(Vector3(speed, 0.0f, 0.0f));
		break;
	case 5:
		if (_gameObjects[objectNumber]->GetPhysicsModel()->GetCollider()->IsGrounded())
		{
			_gameObjects[objectNumber]->GetPhysicsModel()->ToggleGravity(false);
			_gameObjects[objectNumber]->GetPhysicsModel()->GetCollider()->SetGrounded(false);
			_gameObjects[objectNumber]->GetPhysicsModel()->ApplyImpulse(Vector3(0.0f, 10.0f, 0.0f));
		}
		break;
	default:
		break;
	}
}

void Application::CollisionHandeling() 
{
	for (int i = 0; i < _gameObjects.size(); i++)
	{
		for (int k = i + 1; k < _gameObjects.size(); k++)
		{

			if (_gameObjects[i]->GetPhysicsModel()->IsCollideable() && _gameObjects[k]->GetPhysicsModel()->IsCollideable())
			{

				Collider* collider1 = _gameObjects[i]->GetPhysicsModel()->GetCollider();
				Collider* collider2 = _gameObjects[k]->GetPhysicsModel()->GetCollider();

				if (collider1->GetColliderType() == 3.0f && collider2->GetColliderType() == 1.0f)
				{
					float distanceToGround = _math.Dot(collider1->GetNormal(), collider2->GetPosition()) - collider1->GetDistance();
					if (distanceToGround >= 0.1f)
					{
						// Sphere is not on the ground
						_gameObjects[k]->GetPhysicsModel()->ToggleGravity(true);
						collider2->SetGrounded(false);
						//DebugPrintF("\nnot ground");
					}
					else
					{
						// Sphere is on the ground
						_gameObjects[k]->GetPhysicsModel()->ToggleGravity(false);
						collider2->SetGrounded(true);
						//DebugPrintF("\nground");
					}

				}
				if (collider1->GetColliderType() == 3.0f && collider2->GetColliderType() == 2.0f)
				{
					Vector3 aMin = collider2->GetPosition() + collider2->GetMin();
					Vector3 aMax = collider2->GetPosition() + collider2->GetMax();

					Vector3 corners[4] = {
						Vector3(aMin.x, aMin.y, aMin.z),
						Vector3(aMin.x, aMin.y, aMax.z),
						Vector3(aMax.x, aMin.y, aMin.z),
						Vector3(aMax.x, aMin.y, aMax.z)
					};

					float distanceToGround = 0.0f;

					for (int i = 0; i < 4; i++)
					{
						distanceToGround = -_math.Dot(collider1->GetNormal(), corners[i]) - collider1->GetDistance() - 1;
						if (distanceToGround <= 0.1f)
						{
							_gameObjects[k]->GetPhysicsModel()->ToggleGravity(true);
							collider2->SetGrounded(false);
							//DebugPrintF("\nnot ground");
						}
						else
						{
							_gameObjects[k]->GetPhysicsModel()->ToggleGravity(false);
							collider2->SetGrounded(true);
							//DebugPrintF("\nground");
						}
					}
				}
				//}
				if (collider1->CollidesWith(*collider2))
				{
					//sphere colliders
					if (collider1->GetColliderType() == 1.0f && collider2->GetColliderType() == 1.0f)
					{
						//DebugPrintF("sphere and sphere");
						Vector3 position1 = _gameObjects[i]->GetTransform()->GetPosition();
						Vector3 position2 = _gameObjects[k]->GetTransform()->GetPosition();
						float radius1 = _gameObjects[i]->GetPhysicsModel()->GetCollider()->GetRadius();
						float radius2 = _gameObjects[k]->GetPhysicsModel()->GetCollider()->GetRadius();

						Vector3 collisionNormal = position1 - position2;
						float distance = _math.Length(collisionNormal);
						float penetrationDepth = distance - radius1 - radius2;

						float inverseMass1 = 1 / _gameObjects[i]->GetPhysicsModel()->GetMass();
						float inverseMass2 = 1 / _gameObjects[k]->GetPhysicsModel()->GetMass();

						if (penetrationDepth < 0.0f) {
							// Objects are overlapping
							collisionNormal.Normalize();

							Vector3 correction = collisionNormal * penetrationDepth;
							correction *= (inverseMass1 / (inverseMass1 + inverseMass2));

							position1 -= correction * inverseMass1;
							position2 += correction * inverseMass2;

							_gameObjects[i]->GetTransform()->SetPosition(position1);
							_gameObjects[k]->GetTransform()->SetPosition(position2);

							// Now you can apply the impulse as before
							float restitution = 0.8f;
							Vector3 relativeVelocity = _gameObjects[i]->GetPhysicsModel()->GetVelocity() - _gameObjects[k]->GetPhysicsModel()->GetVelocity();
							if (_math.Dot(collisionNormal, relativeVelocity) < 0.0f)
							{
								float vj = -(1 + restitution) * _math.Dot(collisionNormal, relativeVelocity);
								float inverseMass1 = 1 / _gameObjects[i]->GetPhysicsModel()->GetMass();
								float inverseMass2 = 1 / _gameObjects[k]->GetPhysicsModel()->GetMass();
								float j = vj / (inverseMass1 + inverseMass2);
								Vector3 impulse1 = inverseMass1 * j * collisionNormal;
								Vector3 impulse2 = -inverseMass2 * j * collisionNormal;
								_gameObjects[i]->GetPhysicsModel()->ApplyImpulse(impulse1);
								_gameObjects[k]->GetPhysicsModel()->ApplyImpulse(impulse2);
							}
						}
					}
					//cube colliders
					if (collider1->GetColliderType() == 2.0f && collider2->GetColliderType() == 2.0f)
					{
						//DebugPrintF("cube and cube");
						Vector3 pos1 = collider1->GetPosition();
						Vector3 aMin = pos1 + collider1->GetMin();
						Vector3 aMax = pos1 + collider1->GetMax();
						//DebugPrintF("\ncube 1: pos %f %f %f \nmin  %f %f %f \nmax  %f %f %f", pos1.x, pos1.y, pos1.z, aMin.x, aMin.y, aMin.z, aMax.x, aMax.y, aMax.z);
						Vector3 pos2 = collider2->GetPosition();
						Vector3 bMin = pos2 + collider2->GetMin();
						Vector3 bMax = pos2 + collider2->GetMax();
						//DebugPrintF("\ncube 1: pos %f %f %f\n min  %f %f %f\n max  %f %f %f", pos2.x, pos2.y, pos2.z, bMin.x, bMin.y, bMin.z, bMax.x, bMax.y, bMax.z);
						float xOverlap = min(aMax.x, bMax.x) - max(aMin.x, bMin.x);
						float yOverlap = min(aMax.y, bMax.y) - max(aMin.y, bMin.y);
						float zOverlap = min(aMax.z, bMax.z) - max(aMin.z, bMin.z);

						Vector3 collisionNormal = Vector3(0, 0, 0);

						//DebugPrintF("\ncollision norm %f %f %f", collisionNormal.x, collisionNormal.y, collisionNormal.z);

						if (xOverlap < yOverlap && xOverlap < zOverlap)
						{
							if (aMin.x < bMin.x) collisionNormal = Vector3(-1.0f, 0.0f, 0.0f);
							else collisionNormal = Vector3(1.0f, 0.0f, 0.0f);
						}
						else if (yOverlap < zOverlap)
						{
							if (aMin.y < bMin.y) collisionNormal = Vector3(0.0f, -1.0f, 0.0f);
							else collisionNormal = Vector3(0.0f, 1.0f, 0.0f);
						}
						else
						{
							if (aMin.z < bMin.z) collisionNormal = Vector3(0.0f, 0.0f, -1.0f);
							else collisionNormal = Vector3(0.0f, 0.0f, 1.0f);
						}

						float penetrationDepth = _math.Min(xOverlap, _math.Min(yOverlap, zOverlap));


						//DebugPrintF("\npenetrationDepth %f", penetrationDepth);

						float inverseMass1 = 1 / _gameObjects[i]->GetPhysicsModel()->GetMass();
						float inverseMass2 = 1 / _gameObjects[k]->GetPhysicsModel()->GetMass();

						if (penetrationDepth > 0.0f) {
							Vector3 correction = collisionNormal * penetrationDepth;
							correction *= (inverseMass1 / (inverseMass1 + inverseMass2));
							DebugPrintF("\n%f %f %f", correction.x, correction.y, correction.z);
							Vector3 position1 = _gameObjects[i]->GetTransform()->GetPosition();
							Vector3 position2 = _gameObjects[k]->GetTransform()->GetPosition();

							position1 += correction * inverseMass1;
							position2 -= correction * inverseMass2;

							_gameObjects[i]->GetTransform()->SetPosition(position1);
							_gameObjects[k]->GetTransform()->SetPosition(position2);
						}

						float restitution = 0.6f;
						Vector3 relativeVelocity = _gameObjects[i]->GetPhysicsModel()->GetVelocity() - _gameObjects[k]->GetPhysicsModel()->GetVelocity();
						float relativeSpeedAlongNormal = _math.Dot(relativeVelocity, collisionNormal);
						if (relativeSpeedAlongNormal < 0.0f)
						{
							float totalInverseMass = _gameObjects[i]->GetPhysicsModel()->GetInverseMass() + _gameObjects[k]->GetPhysicsModel()->GetInverseMass();
							if (totalInverseMass <= 0.0f) return;
							float impulseMagnitude = -(1.0f + restitution) * relativeSpeedAlongNormal / totalInverseMass;
							Vector3 impulse = impulseMagnitude * collisionNormal;
							_gameObjects[i]->GetPhysicsModel()->ApplyImpulse(impulse);
							_gameObjects[k]->GetPhysicsModel()->ApplyImpulse(-impulse);
						}
					}
					//sphere and cube
					if ((collider1->GetColliderType() == 1.0f && collider2->GetColliderType() == 2.0f) ||
						(collider1->GetColliderType() == 2.0f && collider2->GetColliderType() == 1.0f))
					{
						//DebugPrintF("sphere and cube\n");

						GameObject* sphere = nullptr;
						GameObject* cube = nullptr;

						if (collider1->GetColliderType() == 1.0f) {
							sphere = _gameObjects[i];
							cube = _gameObjects[k];
						}
						else {
							sphere = _gameObjects[k];
							cube = _gameObjects[i];
						}

						collider1 = sphere->GetPhysicsModel()->GetCollider();
						collider2 = cube->GetPhysicsModel()->GetCollider();

						Vector3 aMin = cube->GetTransform()->GetPosition() + collider2->GetMin();
						Vector3 aMax = cube->GetTransform()->GetPosition() + collider2->GetMax();

						Vector3 closestPoint = _math.Clamp(collider1->GetPosition(), aMin, aMax);

						Vector3 collisionNormal = collider1->GetPosition() - closestPoint;
						collisionNormal = _math.Normalise(collisionNormal);

						Vector3 relativeVelocity = sphere->GetPhysicsModel()->GetVelocity() - cube->GetPhysicsModel()->GetVelocity();

						if (_math.Dot(collisionNormal, relativeVelocity) <= 0.0f)
						{
							float sphereInverseMass = 1 / sphere->GetPhysicsModel()->GetMass();
							float aabbInverseMass = 1 / cube->GetPhysicsModel()->GetMass();

							float penetrationDepth = collider1->GetRadius() - _math.Distance(collider1->GetPosition(), closestPoint);

							if (penetrationDepth > 0.0f) {
								const float percent = 0.2f;
								const float slop = 0.01f;
								Vector3 correction = max(penetrationDepth - slop, 0.0f) / (sphereInverseMass + aabbInverseMass) * percent * collisionNormal;

								Vector3 position1 = cube->GetTransform()->GetPosition();
								Vector3 position2 = sphere->GetTransform()->GetPosition();

								position1 -= correction * sphereInverseMass;
								position2 += correction * aabbInverseMass;

								cube->GetTransform()->SetPosition(position1);
								sphere->GetTransform()->SetPosition(position2);
							}

							float restitution = 0.8f;


							float impulseScalar = -(1 + restitution) * _math.Dot(collisionNormal, relativeVelocity) / (sphereInverseMass + aabbInverseMass);

							Vector3 sphereImpulse = impulseScalar * sphereInverseMass * collisionNormal;
							Vector3 aabbImpulse = -impulseScalar * aabbInverseMass * collisionNormal;

							sphere->GetPhysicsModel()->ApplyImpulse(sphereImpulse);
							cube->GetPhysicsModel()->ApplyImpulse(aabbImpulse);


						}
					}
					if (collider1->GetColliderType() == 3.0f && collider2->GetColliderType() == 1.0f)
					{
						Vector3 relativeVelocity = _gameObjects[k]->GetPhysicsModel()->GetVelocity();

						float dotProduct = _math.Dot(relativeVelocity, collider1->GetNormal());
						Vector3 reflection = relativeVelocity - 2.0f * dotProduct * collider1->GetNormal();

						float dampingFactor = 0.2f;
						Vector3 newVelocity = dampingFactor * reflection;
						//DebugPrintF("\n%f %f %f",newVelocity.x, newVelocity.y, newVelocity.z);
						Vector3 temp = _gameObjects[k]->GetPhysicsModel()->GetVelocity();
						temp.y = newVelocity.y;
						_gameObjects[k]->GetPhysicsModel()->SetVelocity(temp);


						float penetrationDepth = -(_math.Dot((collider2->GetPosition() - collider1->GetPosition()), collider1->GetNormal()) - collider2->GetRadius());
						//DebugPrintF("\n%f", penetrationDepth);
						if (penetrationDepth > 0.0f)
						{
							penetrationDepth = std::ceil(penetrationDepth * 100) / 100;
							//DebugPrintF("\n%f", penetrationDepth);
							Vector3 correction = penetrationDepth * collider1->GetNormal();
							Vector3 newPosition = collider2->GetPosition() + correction;
							_gameObjects[k]->GetTransform()->SetPosition(newPosition);
						}

						collider2->SetGrounded(true);
					}
					if (collider1->GetColliderType() == 3.0f && collider2->GetColliderType() == 2.0f)
					{
						Vector3 aMin = collider2->GetPosition() + collider2->GetMin();
						Vector3 aMax = collider2->GetPosition() + collider2->GetMax();

						Vector3 corners[4] = {
							Vector3(aMin.x, aMin.y, aMin.z),
							Vector3(aMin.x, aMin.y, aMax.z),
							Vector3(aMax.x, aMin.y, aMin.z),
							Vector3(aMax.x, aMin.y, aMax.z)
						};

						float distance = 0.0f;

						for (int i = 0; i < 4; i++)
						{
							distance = -_math.Dot(collider1->GetNormal(), corners[i]) - collider1->GetDistance()-1;
							if (distance >= 0.00) 
							{
								break;
							}
						}

						Vector3 correction = Vector3(0, distance, 0);
						Vector3 newPosition = collider2->GetPosition() + correction;
						_gameObjects[k]->GetTransform()->SetPosition(newPosition);
						collider2->SetGrounded(true);
					}
				}
			}
		}
	}
}