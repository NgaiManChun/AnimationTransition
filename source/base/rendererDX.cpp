// =======================================================
// rendererDX.cpp
// 
// DirectX環境のレンダラー
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// 2024/09/11 複数ブレンドステート追加
// =======================================================
#include "rendererDX.h"
#include "resourceToolDX.h"

namespace MG {

	RendererDX::RendererDX(float screenWidth, float screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight), viewWidth(screenWidth), viewHeight(screenHeight){}
	RendererDX::~RendererDX() {}
	ID3D11Device* RendererDX::GetDevice(void) { return m_device; }
	ID3D11DeviceContext* RendererDX::GetDeviceContext(void) { return m_context; }
	IDXGISwapChain* RendererDX::GetSwapChain(void) { return m_swapChain; }

	HRESULT RendererDX::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow) {

		// デバイス、スワップチェーン、コンテキスト生成
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			swapChainDesc.BufferCount = 1;
			swapChainDesc.BufferDesc.Width = screenWidth;
			swapChainDesc.BufferDesc.Height = screenHeight;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow = hWnd;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.Windowed = TRUE;

			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

			HRESULT hr = D3D11CreateDeviceAndSwapChain(
				NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				D3D11_CREATE_DEVICE_BGRA_SUPPORT,
				NULL,
				0,
				D3D11_SDK_VERSION,
				&swapChainDesc,
				&m_swapChain,
				&m_device,
				&featureLevel,
				&m_context
			);
			if (FAILED(hr)) return hr;
		}

		// レンダーターゲットビュー生成
		{
			ID3D11Texture2D* pBackBuffer = NULL;
			m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			m_device->CreateRenderTargetView(pBackBuffer, NULL, &m_renderTargetView);
			
			pBackBuffer->Release();
		}

		// デプスビュー生成
		{
			ID3D11Texture2D* texture = NULL;
			D3D11_TEXTURE2D_DESC texture2DDesc;
			ZeroMemory(&texture2DDesc, sizeof(texture2DDesc));
			texture2DDesc.Width = screenWidth;
			texture2DDesc.Height = screenHeight;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			texture2DDesc.SampleDesc.Count = 1;
			texture2DDesc.SampleDesc.Quality = 0;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			texture2DDesc.CPUAccessFlags = 0;
			texture2DDesc.MiscFlags = 0;
			m_device->CreateTexture2D(&texture2DDesc, NULL, &texture);

			//ステンシルターゲット作成
			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
			depthStencilViewDesc.Format = texture2DDesc.Format;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Flags = 0;
			m_device->CreateDepthStencilView(texture, &depthStencilViewDesc, &m_depthStencilView);
			texture->Release();
		}

		// サブレンダーターゲット作成
		subRenderTarget = (RenderTargetDX*)CreateRenderTarget();

		// ビューポート設定
		{
			D3D11_VIEWPORT viewport;
			viewport.Width = (FLOAT)screenWidth;
			viewport.Height = (FLOAT)screenHeight;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			m_context->RSSetViewports(1, &viewport);
		}

		// ラスタライザステート設定
		{
			D3D11_RASTERIZER_DESC rasterizerDesc;
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.MultisampleEnable = FALSE;
			m_device->CreateRasterizerState(&rasterizerDesc, &rasterizerStates[RASTERIZER_STATE_CULL_NONE]);

			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			m_device->CreateRasterizerState(&rasterizerDesc, &rasterizerStates[RASTERIZER_STATE_CULL_BACK]);

			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			m_device->CreateRasterizerState(&rasterizerDesc, &rasterizerStates[RASTERIZER_STATE_WIREFRAME]);

			SetRasterizerState(RASTERIZER_STATE_CULL_NONE);
		}

		// ブレンドステート設定
		{
			D3D11_BLEND_DESC belendDesc;
			ZeroMemory(&belendDesc, sizeof(belendDesc));
			belendDesc.AlphaToCoverageEnable = FALSE;
			belendDesc.IndependentBlendEnable = TRUE;
			belendDesc.RenderTarget[0].BlendEnable = TRUE;
			belendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			belendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			belendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			belendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			belendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			// 無効
			belendDesc.RenderTarget[0].BlendEnable = FALSE;
			m_device->CreateBlendState(&belendDesc, &bState[BLEND_STATE_NONE]);

			// αブレンド
			belendDesc.RenderTarget[0].BlendEnable = TRUE;
			m_device->CreateBlendState(&belendDesc, &bState[BLEND_STATE_ALPHA]);

			// 加算合成
			belendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			belendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			m_device->CreateBlendState(&belendDesc, &bState[BLEND_STATE_ADD]);

			// 減算合成
			belendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			belendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			m_device->CreateBlendState(&belendDesc, &bState[BLEND_STATE_SUB]);

			// くりぬき
			belendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			belendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			belendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			belendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			m_device->CreateBlendState(&belendDesc, &bState[BLEND_STATE_DEST_OUT]);
			
			SetBlendState(BLEND_STATE_ALPHA);

		}

		// 深度ステンシルステート設定
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.StencilEnable = FALSE;

			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
			m_device->CreateDepthStencilState(&depthStencilDesc, &depthStates[DEPTH_STATE_ENABLE]);

			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			m_device->CreateDepthStencilState(&depthStencilDesc, &depthStates[DEPTH_STATE_DRAW_ALL]);
			
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
			m_device->CreateDepthStencilState(&depthStencilDesc, &depthStates[DEPTH_STATE_NO_WRITE]);

			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			m_device->CreateDepthStencilState(&depthStencilDesc, &depthStates[DEPTH_STATE_NO_WRITE_DRAW_ALL]);

			m_context->OMSetDepthStencilState(depthStates[DEPTH_STATE_ENABLE], NULL);
		}

		// サンプラーステート設定
		{
			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			m_device->CreateSamplerState(&samplerDesc, &m_samplerState);
			m_context->PSSetSamplers(0, 1, &m_samplerState);
		}

		

		// 頂点シェーダコンパイル・生成
		{

			ID3DBlob* pErrorBlob;
			ID3DBlob* pVSBlob = NULL;
			HRESULT hr = D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "VertexShaderPolygon", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, &pErrorBlob);
			if (FAILED(hr))
			{
				MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
			}

			m_device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_VertexShader);

			// 入力レイアウト生成
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			UINT numElements = ARRAYSIZE(layout);

			m_device->CreateInputLayout(layout,
				numElements,
				pVSBlob->GetBufferPointer(),
				pVSBlob->GetBufferSize(),
				&m_VertexLayout);

			pVSBlob->Release();
		}


		{

			ID3DBlob* pErrorBlob;
			ID3DBlob* pVSBlob = NULL;
			HRESULT hr = D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "BoneVertexShaderPolygon", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, &pErrorBlob);
			if (FAILED(hr))
			{
				MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
			}

			m_device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_BoneVertexShader);


			// 入力レイアウト生成
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",			0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_UINT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDWEIGHT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			UINT numElements = ARRAYSIZE(layout);

			m_device->CreateInputLayout(layout,
				numElements,
				pVSBlob->GetBufferPointer(),
				pVSBlob->GetBufferSize(),
				&m_BoneVertexLayout);

			pVSBlob->Release();
		}

		// ピクセルシェーダコンパイル・生成
		{
			ID3DBlob* pErrorBlob;
			ID3DBlob* pPSBlob = NULL;
			HRESULT hr = D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "PixelShaderTexture", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &pErrorBlob);
			if (FAILED(hr))
			{
				MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
			}

			m_device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_PixelShader);

			pPSBlob->Release();
		}
		{
			ID3DBlob* pErrorBlob;
			ID3DBlob* pPSBlob = NULL;
			HRESULT hr = D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "PixelShaderPolygon", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &pErrorBlob);
			if (FAILED(hr))
			{
				MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
			}

			m_device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_PixelShaderPolygon);

			pPSBlob->Release();
		}
		{
			ID3DBlob* pErrorBlob;
			ID3DBlob* pPSBlob = NULL;
			HRESULT hr = D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "PixelShaderNoLighting", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &pErrorBlob);
			if (FAILED(hr))
			{
				MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
			}

			m_device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_PixelShaderNoLighting);

			pPSBlob->Release();
		}


		// 定数バッファ生成
		D3D11_BUFFER_DESC hBufferDesc;
		{
			hBufferDesc.ByteWidth = sizeof(CONSTANT);
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			m_device->CreateBuffer(&hBufferDesc, NULL, &m_ConstantBuffer);
			m_context->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
		}

		{
			hBufferDesc.ByteWidth = sizeof(MATERIAL);
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			m_device->CreateBuffer(&hBufferDesc, NULL, &m_MaterialBuffer);
			m_context->VSSetConstantBuffers(1, 1, &m_MaterialBuffer);
		}
		{
			D3D11_BUFFER_DESC hBufferDesc = {};
			hBufferDesc.ByteWidth = sizeof(LIGHT);
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			GetDevice()->CreateBuffer(&hBufferDesc, NULL, &m_LightBuffer);
			GetDeviceContext()->PSSetConstantBuffers(2, 1, &m_LightBuffer);
		}
		{
			D3D11_BUFFER_DESC hBufferDesc = {};
			hBufferDesc.ByteWidth = sizeof(BONE_BUFFER);
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			GetDevice()->CreateBuffer(&hBufferDesc, NULL, &m_BoneBuffer);
			GetDeviceContext()->VSSetConstantBuffers(3, 1, &m_BoneBuffer);
		}

		// 入力レイアウト設定
		m_context->IASetInputLayout(m_VertexLayout);

		// シェーダ設定
		m_context->VSSetShader(m_VertexShader, NULL, 0);
		m_context->PSSetShader(m_PixelShader, NULL, 0);

		// 行列の初期化
		m_ViewMatrix = XMMatrixLookAtLH({ 0.0f, 0.0f, -10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), screenWidth / screenHeight, 0.01f, 1000.0f);
		m_WorldMatrix = XMMatrixIdentity();
		UpdateWorldViewProjection();
		return S_OK;

	}

	void RendererDX::Uninit()
	{
		//m_rasterizerState->Release();
		for (int i = 0; i < BLEND_STATE_MAX; i++) {
			bState[i]->Release();
		}
		for (int i = 0; i < DEPTH_STATE_MAX; i++) {
			depthStates[i]->Release();
		}
		for (int i = 0; i < RASTERIZER_STATE_MAX; i++) {
			rasterizerStates[i]->Release();
		}
		m_samplerState->Release();

		m_ConstantBuffer->Release();
		m_MaterialBuffer->Release();
		m_LightBuffer->Release();
		m_BoneBuffer->Release();

		m_VertexLayout->Release();
		m_VertexShader->Release();
		m_BoneVertexLayout->Release();
		m_BoneVertexShader->Release();
		m_PixelShader->Release();
		m_PixelShaderPolygon->Release();
		m_PixelShaderNoLighting->Release();

		/*m_tempResourceView->Release();
		m_tempRenderTargetView->Release();*/

		m_renderTargetView->Release();
		m_depthStencilView->Release();

		m_swapChain->Release();
		m_context->Release();
		m_device->Release();

		for (auto renderTarget : renderTargets) {
			delete renderTarget->texture;
			if (renderTarget->shaderResourceView) {
				renderTarget->shaderResourceView->Release();
				//renderTarget->shaderResourceView = nullptr;
			}
			if (renderTarget->renderTargetView) {
				renderTarget->renderTargetView->Release();
				//renderTarget->renderTargetView = nullptr;
			}
			if (renderTarget->depthStencilView) {
				renderTarget->depthStencilView->Release();
				//renderTarget->depthStencilView = nullptr;
			}
			delete renderTarget;
		}
		renderTargets.clear();
		m_currentRenderTargetView = nullptr;
		m_currentRepthStencilView = nullptr;
	}

	void RendererDX::UpdateWorldViewProjection() {
		XMMATRIX worldViewProjection = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;

		XMFLOAT4X4 worldViewProjectionF4x4;
		XMStoreFloat4x4(&worldViewProjectionF4x4, XMMatrixTranspose(worldViewProjection));

		CONSTANT constant = {
			worldViewProjectionF4x4,
			uvOffset,
			uvRange
		};

		m_context->UpdateSubresource(m_ConstantBuffer, 0, NULL, &constant, 0, 0);
	}

	void RendererDX::UpdateConstant() {
		XMMATRIX worldViewProjection = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;

		XMFLOAT4X4 worldViewProjectionF4x4;
		XMStoreFloat4x4(&worldViewProjectionF4x4, XMMatrixTranspose(worldViewProjection));

		CONSTANT constant = {
			worldViewProjectionF4x4,
			uvOffset,
			uvRange
		};

		m_context->UpdateSubresource(m_ConstantBuffer, 0, NULL, &constant, 0, 0);
	}

	void RendererDX::UpdateLight()
	{
		LIGHT light = {
			ambientLight,
			driectLight,
			driectLightPosition
		};

		XMVECTOR driectLightLocal = { driectLightPosition.x, driectLightPosition.y, driectLightPosition.z, driectLightPosition.w };
		driectLightLocal = XMVector3TransformNormal(driectLightLocal, XMMatrixInverse(nullptr, m_WorldMatrix));
		XMStoreFloat4(&light.driectLightLocal, driectLightLocal);

		m_context->UpdateSubresource(m_LightBuffer, 0, NULL, &light, 0, 0);
	}

	void RendererDX::SetAmbient(const F4& color)
	{
		ambientLight = { color.x, color.y, color.z, color.w};
	}

	void RendererDX::SetLightPosition(const F3& position)
	{
		driectLightPosition = { position.x, position.y, position.z, 1.0f };
	}

	void RendererDX::SetLightColor(const F4& color)
	{
		driectLight = { color.x, color.y, color.z, color.w };
	}

	void RendererDX::SetLight(const F3& position, const F4& color)
	{
		driectLight = { color.x, color.y, color.z, color.w };
		driectLightPosition = { position.x, position.y, position.z, 1.0f };
	}

	void RendererDX::SetColor(const F4& color)
	{
		MATERIAL material = { XMFLOAT4(color.x, color.y, color.z, color.w) };
		SetMaterial(material);
	}

	void RendererDX::SetUVOffset(const F2& uvOffset)
	{
		this->uvOffset = { uvOffset.x, uvOffset.y };
	}

	void RendererDX::SetUVRange(const F2& uvRange)
	{
		this->uvRange = { uvRange.x, uvRange.y };
	}

	void RendererDX::SetBones(MESH_BONE* bones, size_t size)
	{
		BONE_BUFFER boneBuff = {
		};
		/*for (int i = 0; i < 100; i++) {
			XMStoreFloat4x4(&boneBuff.boneMatrix[i], (
				XMMATRIX(
					1.0f, 0.0f, 0.0f, 10.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				)
			));
		}*/
		//XMMatrixTranspose
		for (unsigned int i = 0; i < size; i++) {
			M4x4& offset = bones[i].offset;
			M4x4& world = bones[i].world;
			XMStoreFloat4x4(&boneBuff.boneOffsetMatrix[i], (XMMATRIX(
				offset._v00, offset._v10, offset._v20, offset._v30,
				offset._v01, offset._v11, offset._v21, offset._v31,
				offset._v02, offset._v12, offset._v22, offset._v32,
				offset._v03, offset._v13, offset._v23, offset._v33
			)));
			XMStoreFloat4x4(&boneBuff.boneWorldMatrix[i], (XMMATRIX(
				world._v00, world._v10, world._v20, world._v30,
				world._v01, world._v11, world._v21, world._v31,
				world._v02, world._v12, world._v22, world._v32,
				world._v03, world._v13, world._v23, world._v33
			)));
		}
		
		m_context->UpdateSubresource(m_BoneBuffer, 0, NULL, &boneBuff, 0, 0);
	}

	void RendererDX::SetUseBone(bool enable)
	{
		if (enable) {
			m_context->IASetInputLayout(m_BoneVertexLayout);
			m_context->VSSetShader(m_BoneVertexShader, NULL, 0);
		}
		else {
			m_context->IASetInputLayout(m_VertexLayout);
			m_context->VSSetShader(m_VertexShader, NULL, 0);
		}
		
	}

	void RendererDX::SetWorldMatrix(const XMMATRIX& WorldMatrix)
	{
		m_WorldMatrix = WorldMatrix;
	}

	void RendererDX::SetViewMatrix(const XMMATRIX& ViewMatrix)
	{
		m_ViewMatrix = ViewMatrix;
	}

	void RendererDX::SetProjectionMatrix(const XMMATRIX& ProjectionMatrix)
	{
		m_ProjectionMatrix = ProjectionMatrix;
	}

	void RendererDX::SetMaterial(const MATERIAL& Material)
	{
		m_context->UpdateSubresource(m_MaterialBuffer, 0, NULL, &Material, 0, 0);
	}

	void RendererDX::Present()
	{
		m_swapChain->Present(0, 0);
	}

	void RendererDX::SetViewport(float x, float y, float width, float height)
	{
		D3D11_VIEWPORT vp;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = x - width * 0.5f;
		vp.TopLeftY = y - height * 0.5f;
		m_context->RSSetViewports(1, &vp);
	}

	void RendererDX::ApplyCamera(Camera* camera)
	{
		if (camera) {
			if (typeid(*camera) == typeid(Camera3D)) {
				Camera3D* camera3D = (Camera3D*)camera;
				F3 cameraPosition = camera3D->GetPosition();
				F3 lookAt = cameraPosition + camera3D->GetFront();
				F3 cameraUpper = camera3D->GetUpper();
				F2 screenSize = GetScreenSize();
				XMMATRIX view = XMMatrixLookAtLH(
					ToXMVector(cameraPosition),
					ToXMVector(lookAt),
					ToXMVector(cameraUpper));
				XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(camera3D->GetAngle()), viewWidth / viewHeight, camera3D->GetNear(), camera3D->GetFar());
				SetViewMatrix(view);
				SetProjectionMatrix(perspective);
				SetDepthState(DEPTH_STATE_ENABLE);
				m_context->PSSetShader(m_PixelShader, NULL, 0);
			}
			else if (typeid(*camera) == typeid(Camera2D)) {
				Camera2D* camera2D = (Camera2D*)camera;
				
				XMMATRIX projection = XMMatrixOrthographicLH(viewWidth, viewHeight, camera2D->GetNear(), camera2D->GetFar());
				XMMATRIX view = XMMatrixIdentity();
				F2 offset = camera2D->GetOffset();
				projection *= XMMatrixTranslation(offset.x / viewWidth, offset.y / viewHeight, 0.0f);
				SetViewMatrix(view);
				SetProjectionMatrix(projection);
				SetDepthState(DEPTH_STATE_NO_WRITE_DRAW_ALL);
				m_context->PSSetShader(m_PixelShaderNoLighting, NULL, 0);
			}
			m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}
	}

	void RendererDX::SetDepthState(DEPTH_STATE state)
	{
		m_context->OMSetDepthStencilState(depthStates[state], NULL);
	}

	void RendererDX::SetRasterizerState(RASTERIZER_STATE state)
	{
		m_context->RSSetState(rasterizerStates[state]);
	}

	void RendererDX::SetWorld(const F3& size, const Quaternion& rotate, const F3& position)
	{
		SetWorldMatrix(
			XMMatrixScaling(size.x, size.y, size.z) *
			XMMatrixRotationQuaternion({ rotate.x, rotate.y, rotate.z, rotate.w }) *
			XMMatrixTranslation(position.x, position.y, position.z)
		);
	}

	void RendererDX::SetBlendState(BLEND_STATE state)
	{
		m_context->OMSetBlendState(bState[state], bFactor, 0xffffffff);
	}

	RenderTarget* RendererDX::CreateRenderTarget(unsigned int width, unsigned int height) {

		ID3D11ShaderResourceView* resourceView = NULL;
		ID3D11RenderTargetView* renderTargetView = NULL;
		ID3D11DepthStencilView* depthStencilView = NULL;

		// テクスチャ生成
		{
			ID3D11Texture2D* texture;
			D3D11_TEXTURE2D_DESC texture2DDesc;
			ZeroMemory(&texture2DDesc, sizeof(texture2DDesc));
			texture2DDesc.Width = width;
			texture2DDesc.Height = height;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture2DDesc.SampleDesc.Count = 1;
			texture2DDesc.SampleDesc.Quality = 0;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			texture2DDesc.SampleDesc.Count = 1;
			m_device->CreateTexture2D(&texture2DDesc, NULL, &texture);
			m_device->CreateShaderResourceView(texture, NULL, &resourceView);
			m_device->CreateRenderTargetView(texture, NULL, &renderTargetView);
			texture->Release();
		}

		// デプスビュー生成
		{
			ID3D11Texture2D* texture = NULL;
			D3D11_TEXTURE2D_DESC texture2DDesc;
			ZeroMemory(&texture2DDesc, sizeof(texture2DDesc));
			texture2DDesc.Width = width;
			texture2DDesc.Height = height;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			texture2DDesc.SampleDesc.Count = 1;
			texture2DDesc.SampleDesc.Quality = 0;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			texture2DDesc.CPUAccessFlags = 0;
			texture2DDesc.MiscFlags = 0;
			m_device->CreateTexture2D(&texture2DDesc, NULL, &texture);

			//ステンシルターゲット作成
			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
			depthStencilViewDesc.Format = texture2DDesc.Format;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Flags = 0;
			m_device->CreateDepthStencilView(texture, &depthStencilViewDesc, &depthStencilView);
			texture->Release();
		}
		RenderTargetDX* renderTarget = new RenderTargetDX(
			resourceView, 
			renderTargetView, 
			depthStencilView, 
			new TextureDX(0, resourceView, width, height)
		);
		renderTargets.push_back(renderTarget);
		return renderTarget;
	}

	void RendererDX::ReleaseRenderTarget(RenderTarget* renderTarget)
	{
		if (renderTarget) {
			RenderTargetDX* renderTargetDX = (RenderTargetDX*)renderTarget;
			if (renderTargetDX) {
				delete renderTargetDX->texture;
				if (renderTargetDX->shaderResourceView) {
					renderTargetDX->shaderResourceView->Release();
				}
				if (renderTargetDX->renderTargetView) {
					renderTargetDX->renderTargetView->Release();
				}
				if (renderTargetDX->depthStencilView) {
					renderTargetDX->depthStencilView->Release();
				}
				renderTargets.remove(renderTargetDX);
			}
			delete renderTarget;
		}
	}

	void RendererDX::SetRenderTarget(RenderTarget* renderTarget)
	{
		if (renderTarget) {
			RenderTargetDX* renderTargetDX = (RenderTargetDX*)renderTarget;
			m_context->OMSetRenderTargets(1, &renderTargetDX->renderTargetView, renderTargetDX->depthStencilView);
			m_currentRenderTargetView = renderTargetDX->renderTargetView;
			m_currentRepthStencilView = renderTargetDX->depthStencilView;
			viewWidth = renderTarget->texture->GetWidth();
			viewHeight = renderTarget->texture->GetHeight();
		}
		else {
			m_context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
			m_currentRenderTargetView = m_renderTargetView;
			m_currentRepthStencilView = m_depthStencilView;
			viewWidth = screenWidth;
			viewHeight = screenHeight;

		}
		
	}

	RenderTarget* RendererDX::GetSubRenderTarget()
	{
		return subRenderTarget;
	}

	void RendererDX::ClearRenderTargetView(RenderTarget* renderTarget, bool clearDepth)
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		if (renderTarget) {
			RenderTargetDX* renderTargetDX = (RenderTargetDX*)renderTarget;
			m_context->ClearRenderTargetView(renderTargetDX->renderTargetView, ClearColor);
			if (clearDepth) {
				m_context->ClearDepthStencilView(renderTargetDX->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
			}
			
		}
		else {
			m_context->ClearRenderTargetView(m_renderTargetView, ClearColor);
			if (clearDepth) {
				m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
			}
		}
		
	}

	XMVECTOR ToXMVector(F3 v)
	{
		return XMVECTOR{ v.x, v.y, v.z };
	}

	RenderTargetDX::RenderTargetDX(
		ID3D11ShaderResourceView* shaderResourceView, 
		ID3D11RenderTargetView* renderTargetView, 
		ID3D11DepthStencilView* depthStencilView,
		Texture* texture
	) : shaderResourceView(shaderResourceView), renderTargetView(renderTargetView), depthStencilView(depthStencilView), RenderTarget(texture)
	{
	}

} // namespace MG