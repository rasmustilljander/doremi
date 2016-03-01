#pragma once
#ifndef MAIN_H
#define MAIN_H
#endif

//CRUCIAL
#define _XM_NO_INTRINSICS_
#include <windows.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <DirectXMathVector.inl>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
//#include <SimpleMath.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
//using namespace DirectX::SimpleMath;



#include "Cam.h"
#include "MayaLoader.h"

class Main{
public:
	Main();
	Main(HINSTANCE hInstance, UINT scrW, UINT scrH);
	~Main();

private:
	bool Init();
	void InitVariables();
	bool InitWindow();
	bool InitDirect3D(HWND hWindow);
	void SetViewport();
	void CreateBuffers();
	void CreateShaders();

	int Run();
	void Update();
	void Render();

	void Dealloc();

	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
public:
	LRESULT MsgProc(HWND hwindow, UINT msg, WPARAM wParam, LPARAM lParam);
private:


	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	ID3D11RenderTargetView* gBackRufferRenderTargetView = nullptr;
	IDXGISwapChain* gSwapChain = nullptr;
	ID3D11DepthStencilView* gDepthStencilView = nullptr;
	//2D Textures
	ID3D11Texture2D* depthStencilBuffer = nullptr;
	//ENABLE/DISABLE DEPTH
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11DepthStencilState* m_depthStencilState;


	UINT screenWidth, screenHeight;
	DWORD windowStyle;

	D3D11_VIEWPORT viewport;
	HWND hWindow;
	HINSTANCE hInstance;

	//shaderstuff n mojs
	ID3D11SamplerState *wrap_Sampstate;
	ID3D11SamplerState *clamp_Sampstate;

	ID3D11InputLayout *defaultInputLayout;
	ID3D11VertexShader *defaultVS;
	ID3D11PixelShader *defaultPS;
	ID3D11VertexShader *basicVS;
	ID3D11PixelShader *basicPS;
	

	////Fps camera
	//Vector4 camPosition;
	//Vector4 camTarget;
	//Vector4 camUp;// = Vector4(0.0f, 1.0f, 0.0f, 0.0f);;
	//Vector4 DefaultForward = Vector4(0.0f, -1.0f, 0.0f, 0.0f);
	//Vector4 DefaultRight = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	//Vector4 camForward = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	//Vector4 camRight = Vector4(1.0f, 0.0f, 0.0f, 0.0f);

	//XMMATRIX camRotationMatrix;
	//XMMATRIX fpsCamLook;
	//XMMATRIX CamProjection;
	//XMMATRIX CamView;

	//float moveLeftRight = 0.0f;
	//float moveBackForward = 0.0f;
	//float camYaw = 0.0f;
	//float camPitch = 0.0f;
	//float speed = 0.0f;
	//float boost = 0.0f;
	//float zoom = 0.0f;

	//Camera fpsCam;


	//ID3D11Buffer* cWorldDataBuffer = nullptr; //constantbuffer!

	ID3D11Buffer* testVertexBuffer = nullptr;

	MayaLoader *mayaLoader = nullptr;
};



