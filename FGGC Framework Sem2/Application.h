#ifndef APPLICATION_H
#define APPLICATION_H

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include "Camera.h"
#include "Common.h"
#include "Collision.h"

/*
//#include <SpriteFont.h>
#include "CommonStates.h"
//#include "DDSTextureLoader.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
*/

#include "GameObject.h"
#include "ParticleSystem.h"
#include "BaseParManager.h"
#include "FireParManager.h"
#include "SnowParManager.h"
#include "RainParManager.h"
#include "OctaTree.h"
#include "Spring.h"
#include "Pole.h"

using namespace DirectX;

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

	Rect3					_cubeRect;
	Vector3					_cubeCenterMass;
	std::vector<Vector3>	_cubeVertices;
	ID3D11Buffer*           _pCubeVertexBuffer;
	ID3D11Buffer*           _pCubeIndexBuffer;

	Rect3					_skyboxRect;
	Vector3					_skyboxCenterMass;
	std::vector<Vector3>	_skyboxVertices;
	ID3D11Buffer*           _pSkyboxVertexBuffer;
	ID3D11Buffer*           _pSkyboxIndexBuffer;

	Rect3					_planeRect;
	Vector3					_planeCenterMass;
	std::vector<Vector3>	_planeVertices;
	ID3D11Buffer*           _pPlaneVertexBuffer;
	ID3D11Buffer*           _pPlaneIndexBuffer;

	ID3D11Buffer*           _pConstantBuffer;

	ID3D11DepthStencilView* _depthStencilView = nullptr;
	ID3D11Texture2D* _depthStencilBuffer = nullptr;

	ID3D11ShaderResourceView * _pTextureRV = nullptr;
	ID3D11ShaderResourceView * _pGroundTextureRV = nullptr;
	ID3D11ShaderResourceView * _pBlankTextureRV = nullptr;

	ID3D11SamplerState * _pSamplerLinear = nullptr;

	Light basicLight;

	OctaTree* _octaTree;
	ParticleSystem* _particleSystem;
	std::vector<GameObject *> _movingObjects;
	std::vector<GameObject*> _staticObjects;

	Camera* _camera;
	Camera* _carCamera;
	Camera* _currentCamera;

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

	ID3D11RasterizerState* _pWireFrame;

	ID3D11BlendState* _pTransparency;
	
	Vector3 _windDir;
	float _windStrength, _windChangeChance;
	float _windTimeWaited, _windChangeDelay;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
	HRESULT InitGameObjects();

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	bool HandleKeyboard(MSG msg);

	void CollisionDetection(float deltaTime);
	bool CheckCollision(GameObject* a, GameObject* b, float deltaTime);
	void CollisionResolution(GameObject* a, GameObject* b, CollisionResults results, float deltaTime);

	void Update(float deltaTime);
	void Draw(float deltaTime);
};
#endif