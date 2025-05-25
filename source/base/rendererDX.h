// =======================================================
// rendererDX.h
// 
// DirectX環境のレンダラー
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// 2024/09/11 複数ブレンドステート追加
// =======================================================
#ifndef _RENDERER_DX_H
#define _RENDERER_DX_H

#include "renderer.h"

#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)

#include <DirectXMath.h>
using namespace DirectX;


//テクスチャサポートライブラリ
#include    "DirectXTex.h"
//デバッグビルドかリリースビルドでリンクするライブラリを選択する
#if _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")    //デバッグ用
#else
#pragma comment(lib, "DirectXTex_Release.lib")  //リリース用
#endif

// =======================================================
// ライブラリのリンク
// =======================================================
#pragma	comment (lib, "d3d11.lib")
#pragma	comment (lib, "d3dcompiler.lib")
#pragma	comment (lib, "winmm.lib")
#pragma	comment (lib, "dxguid.lib")
#pragma	comment (lib, "dinput8.lib")

#define TOXMFLOAT2(F2) XMFLOAT2(F2.x, F2.y)
#define TOXMFLOAT3(F3) XMFLOAT3(F3.x, F3.y, F3.z)
#define TOXMFLOAT4(F4) XMFLOAT4(F4.x, F4.y, F4.z, F4.w)

namespace MG {

	struct CONSTANT {
		XMFLOAT4X4 worldViewProjection;
		XMFLOAT2 uvOffset;
		XMFLOAT2 uvRange;
	};

	struct MATERIAL
	{
		XMFLOAT4 color;
	};

	struct LIGHT {
		XMFLOAT4 ambientLight;
		XMFLOAT4 driectLight;
		XMFLOAT4 driectLightLocal;
		
	};

	struct BONE_BUFFER {
		XMFLOAT4X4 boneOffsetMatrix[100];
		XMFLOAT4X4 boneWorldMatrix[100];
		
	};

	/*struct DX_BONE_VERTEX {
		XMFLOAT4 position;
		XMFLOAT4 normal;
		XMFLOAT4 diffuse;
		XMFLOAT2 texCoord;
		XMUINT4 boneIndexes;
		XMFLOAT4 boneWeights;
	};*/

	class RenderTargetDX : public RenderTarget {
	public:
		RenderTargetDX(
			ID3D11ShaderResourceView* shaderResourceView,
			ID3D11RenderTargetView* renderTargetView,
			ID3D11DepthStencilView* depthStencilView,
			Texture* texture
		);
		ID3D11ShaderResourceView* const shaderResourceView;
		ID3D11RenderTargetView* const renderTargetView;
		ID3D11DepthStencilView* const depthStencilView;
	};

	XMVECTOR ToXMVector(F3 v);

	class RendererDX : public Renderer {
	protected:
		float screenWidth;
		float screenHeight;
		float viewWidth;
		float viewHeight;

		ID3D11Device* m_device = NULL;
		ID3D11DeviceContext* m_context = NULL;
		IDXGISwapChain* m_swapChain = NULL;

		ID3D11RenderTargetView* m_renderTargetView = NULL;
		ID3D11DepthStencilView* m_depthStencilView = NULL;
		ID3D11RenderTargetView* m_currentRenderTargetView = NULL;
		ID3D11DepthStencilView* m_currentRepthStencilView = NULL;

		//ID3D11RasterizerState* m_rasterizerState = NULL;
		ID3D11DepthStencilState* m_depthState = NULL;
		ID3D11SamplerState* m_samplerState = NULL;

		ID3D11InputLayout* m_VertexLayout = NULL;
		ID3D11VertexShader* m_VertexShader = NULL;
		ID3D11InputLayout* m_BoneVertexLayout = NULL;
		ID3D11VertexShader* m_BoneVertexShader = NULL;
		ID3D11PixelShader* m_PixelShader = NULL;
		ID3D11PixelShader* m_PixelShaderPolygon = NULL;
		ID3D11PixelShader* m_PixelShaderNoLighting = NULL;
		

		ID3D11Buffer* m_ConstantBuffer = NULL;
		ID3D11Buffer* m_MaterialBuffer = NULL;
		ID3D11Buffer* m_ColorBuffer = NULL;
		ID3D11Buffer* m_LightBuffer = NULL;
		ID3D11Buffer* m_BoneBuffer = NULL;

		XMMATRIX m_WorldMatrix = {};
		XMMATRIX m_ViewMatrix = {};
		XMMATRIX m_ProjectionMatrix = {};

		XMFLOAT4 ambientLight = { 1.0f, 1.0f, 1.0f, 0.7f };
		XMFLOAT4 driectLight = { 1.0f, 1.0f, 1.0f, 0.3f };
		XMFLOAT4 driectLightPosition = { 5.0f, 5.0f, -0.5f, 1.0f };

		XMFLOAT2 uvOffset = { 0.0f, 0.0f };
		XMFLOAT2 uvRange = { 1.0f, 1.0f };


		float bFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ID3D11BlendState* bState[BLEND_STATE_MAX];
		ID3D11DepthStencilState* depthStates[DEPTH_STATE_MAX];
		ID3D11RasterizerState* rasterizerStates[RASTERIZER_STATE_MAX];


		std::list<RenderTargetDX*> renderTargets;
		RenderTargetDX* subRenderTarget;


	public:
		RendererDX(float screenWidth, float screenHeight);
		~RendererDX();
		HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
		void Uninit();
		
		void SetWorldMatrix(const XMMATRIX& WorldMatrix);
		void SetViewMatrix(const XMMATRIX& ViewMatrix);
		void SetProjectionMatrix(const XMMATRIX& ProjectionMatrix);
		void SetMaterial(const MATERIAL& Material);
		void Present(void);

		void SetBlendState(BLEND_STATE state) override;
		void SetDepthState(DEPTH_STATE state) override;
		void SetRasterizerState(RASTERIZER_STATE state) override;
		void SetViewport(float x, float y, float width, float height) override;
		void ApplyCamera(Camera* camera) override;
		void SetWorld(const F3& size, const Quaternion& rotate, const F3& position) override;
		void UpdateWorldViewProjection() override;
		void UpdateConstant() override;
		void UpdateLight() override;
		void SetAmbient(const F4& color) override;
		void SetLightPosition(const F3& position) override;
		void SetLightColor(const F4& color) override;
		void SetLight(const F3& position, const F4& color) override;
		void SetColor(const F4& color) override;
		void SetUVOffset(const F2& uvOffset) override;
		void SetUVRange(const F2& uvRange) override;
		void SetBones(MESH_BONE* bones, size_t size) override;
		void SetUseBone(bool enable) override;
		RenderTarget* CreateRenderTarget(unsigned int width = GetScreenWidth(), unsigned int height = GetScreenHeight()) override;
		void ReleaseRenderTarget(RenderTarget* renderTarget) override;
		void SetRenderTarget(RenderTarget* renderTarget) override;
		RenderTarget* GetSubRenderTarget() override;
		void ClearRenderTargetView(RenderTarget* renderTarget = nullptr, bool clearDepth = true) override;

		ID3D11Device* GetDevice(void);
		ID3D11DeviceContext* GetDeviceContext(void);
		IDXGISwapChain* GetSwapChain(void);
		

	};

} // namespace MG

#endif