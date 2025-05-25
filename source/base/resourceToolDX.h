// =======================================================
// spriteToolDX.h
// 
// DirectX環境のリソース管理
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _RESOURCE_TOOL_DX_H
#define _RESOURCE_TOOL_DX_H

#include "resourceTool.h"
#include "rendererDX.h"
#include <xaudio2.h>
#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib,"d2d1.lib")    
#pragma comment(lib,"dwrite.lib")

namespace MG {
	// =======================================================
	// DirectX環境のテクスチャリソース
	// =======================================================
	class TextureDX : public Texture {
	public:
		TextureDX(const HASH key, ID3D11ShaderResourceView* resourceView, unsigned int width, unsigned int height);
		ID3D11ShaderResourceView* resourceView;
	};


	// =======================================================
	// DirectX環境のオーディオリソース
	// =======================================================
	class AudioDX : public Audio {
	public:
		AudioDX(const HASH key, BYTE* soundData, WAVEFORMATEX waveFormatEX, int length, int playLength);
		BYTE* soundData;
		WAVEFORMATEX waveFormatEX;
		int length;
		int playLength;
	};

	// =======================================================
	// DirectX環境のモデルリソース
	// =======================================================
	class ModelDX : public Model {
	public:

		std::map<MESH*, ID3D11Buffer*> vertexBuffers;
		std::map<MESH*, ID3D11Buffer*> boneWeightBuffers;
		std::map<MESH*, ID3D11Buffer*> indexBuffers;
		std::map<MESH*, std::vector<MESH_BONE>> meshBones;
		std::map<MODEL_NODE*, M4x4> nodeWorldTransforms;
		ModelDX(const HASH key);
	};


	// =======================================================
	// DirectX環境のリソース管理クラス
	// =======================================================
	class ResourceToolDX : public ResourceTool {
	protected:
		RendererDX* renderer;
		ID2D1Factory* d2dFactory;
		IDWriteFactory* dwriteFactory;
		void __ReleaseResource(const HASH key) override;
	public:
		ResourceToolDX(RendererDX* renderer);
		Texture* LoadTexture(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Texture* LoadTexture(unsigned int resourceId, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Texture* RenderText(const std::wstring& text, const FONT& font = {}, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Audio* LoadAudio(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Audio* LoadAudio(unsigned int resourceId, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Model* LoadModel(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Animation* LoadAnimation(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
	};

} // namespace MG

#endif