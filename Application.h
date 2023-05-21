#pragma once

#include <windows.h>
#include <d3d11_4.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include "Loaders\DDSTextureLoader.h"
#include "Loaders\OBJLoader.h"
#include "resource.h"
#include "Camera.h"
#include "Structures.h"
#include "GameObject.h"
#include "Vector3.h"
#include "Transform.h"
#include "Timer.h"
#include "Debug.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "AABBCollider.h"
#include "Math.h"
#include "Vector3.h"
#include <thread>
#include "Quaternion.h"
#include <d3dcompiler.h>
#include <D3Dcompiler.h>
#include "RigidBody.h"

#define FPS60 1.0f/60.0f // Fixed frame rate of 60 fps

using namespace DirectX;
using namespace std;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;

	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;

	ID3D11Buffer*           _pPlaneVertexBuffer;
	ID3D11Buffer*           _pPlaneIndexBuffer;

	ID3D11Buffer*           _pConstantBuffer;

	ID3D11DepthStencilView* _depthStencilView = nullptr;
	ID3D11Texture2D* _depthStencilBuffer = nullptr;

	ID3D11ShaderResourceView * _pTextureRV = nullptr;
	ID3D11ShaderResourceView * _pGroundTextureRV = nullptr;

	ID3D11SamplerState * _pSamplerLinear = nullptr;

	MeshData objMeshData;

	Light basicLight;

	vector<GameObject *> _gameObjects;

	Camera * _camera = nullptr;
	float _cameraOrbitRadius = 7.0f;
	float _cameraOrbitRadiusMin = 2.0f;
	float _cameraOrbitRadiusMax = 50.0f;
	float _cameraOrbitAngleXZ = -90.0f;
	float _cameraSpeed = 2.0f;

	UINT _WindowHeight;
	UINT _WindowWidth;

	// Render dimensions - Change here to alter screen resolution
	UINT _renderHeight = 1080;
	UINT _renderWidth = 1920;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

private:
	Timer _timer;

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	void move(int objectNumber, int dir);

	void ToggleGravity(int objectNumber);

	float accumulator = 0;

	Math _math = Math();

	int _controlSelection = 0;
public:

	Timer& GetTimer() { return _timer; }

	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	bool HandleKeyboard(MSG msg);

	void Update();
	void Draw();

	void InputHandeling();
	void CollisionHandeling();
};

