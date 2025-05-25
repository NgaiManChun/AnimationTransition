// =======================================================
// resourceToolDX.cpp
// 
// DirectX環境のリソース管理
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "resourceToolDX.h"
#include "rendererDX.h"
#include "audioToolDX.h"

namespace MG {
	constexpr const char* TEXTURE_WHITE = "asset\\texture\\white.png";

	// =======================================================
	// DirectX環境のテクスチャリソース
	// =======================================================

	TextureDX::TextureDX(const HASH key, ID3D11ShaderResourceView* resourceView, unsigned int width, unsigned int height)
		: resourceView(resourceView), Texture(key, width, height)
	{
	}


	// =======================================================
	// DirectX環境のオーディオリソース
	// =======================================================
	AudioDX::AudioDX(const HASH key, BYTE* soundData, WAVEFORMATEX waveFormatEX, int length, int playLength) :
		Audio(key), soundData(soundData), waveFormatEX(waveFormatEX), length(length), playLength(playLength) {}

	ModelDX::ModelDX(const HASH key) : Model(key) {}


	// =======================================================
	// DirectX環境のリソース管理クラス
	// =======================================================
	ResourceToolDX::ResourceToolDX(RendererDX* renderer) : renderer(renderer) {

		// Direct2D ファクトリを作成
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);

		// DirectWrite ファクトリを作成
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwriteFactory));

	}


	// =======================================================
	// リソース解放
	// =======================================================
	void ResourceToolDX::__ReleaseResource(const HASH key)
	{
		size_t type = __resources[key].resource->GetType();
		if (type == Texture::TYPE) {
			TextureDX* texture = (TextureDX*)__resources[key].resource;
			texture->resourceView->Release();
			texture->resourceView = nullptr;
			delete texture;
		}
		else if (type == Audio::TYPE) {
			AudioDX* audio = (AudioDX*)__resources[key].resource;
			delete[] audio->soundData;
			audio->soundData = nullptr;
			delete audio;
		}
		else if (type == Model::TYPE) {
			ModelDX* model = (ModelDX*)__resources[key].resource;
			for (int i = 0; i < model->rawModel->meshNum; i++) {
				MESH* mesh = (model->rawModel->meshes + i);
				if (model->vertexBuffers[mesh]) {
					model->vertexBuffers[mesh]->Release();
					model->vertexBuffers[mesh] = nullptr;
				}
				if (model->indexBuffers[mesh]) {
					model->indexBuffers[mesh]->Release();
					model->indexBuffers[mesh] = nullptr;
				}
				if (model->boneWeightBuffers[mesh]) {
					model->boneWeightBuffers[mesh]->Release();
					model->boneWeightBuffers[mesh] = nullptr;
				}
				
			}
			ReleaseTexture(std::to_string(key));
			model->vertexBuffers.clear();
			model->indexBuffers.clear();
			model->boneWeightBuffers.clear();
			model->meshBones.clear();
			model->meshTextures.clear();
			model->rawModel;
			delete model->rawModel;
			delete model;
		}
		else if (type == Animation::TYPE) {
			Animation* animation = (Animation*)__resources[key].resource;
			animation->modelNodeChannels.clear();
			delete animation->rawAnimation;
			delete animation;
		}

		__resources[key].resource = nullptr;
	}


	// =======================================================
	// テクスチャリソース取得
	// =======================================================
	Texture* ResourceToolDX::LoadTexture(const string& path, const string& scope)
	{
		const HASH key = strToHash(path);
		if (!__resources[key].resource) {
			ID3D11ShaderResourceView* resourceView;
			TexMetadata metadata;
			ScratchImage image;

			std::vector<wchar_t> buffer(path.size() + 1);
			std::mbstowcs(buffer.data(), path.c_str(), path.size() + 1);
			LoadFromWICFile(buffer.data(), WIC_FLAGS_NONE, &metadata, image);

			// 読み込んだ画像データをDirectXへわたしてテクスチャとして管理させる
			CreateShaderResourceView(renderer->GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &resourceView);
			// なんか失敗した場合に警告を出す
			assert(resourceView);
			__resources[key].resource = new TextureDX(key, resourceView, metadata.width, metadata.height);
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == Texture::TYPE) {
			__AddScope(key, scope);
			return (Texture*)__resources[key].resource;
		}
		return nullptr;
	}


	// =======================================================
	// テクスチャリソース取得、リソースID指定バージョン
	// =======================================================
	Texture* ResourceToolDX::LoadTexture(unsigned int resourceId, const string& scope)
	{
		//string key = "tex_" + std::to_string(resourceId);
		const HASH key = strToHash("texture:" + std::to_string(resourceId));
		if (!__resources[key].resource) {
			ID3D11ShaderResourceView* resourceView;
			TexMetadata metadata;
			ScratchImage image;

			HRSRC hrs = FindResourceW(nullptr, MAKEINTRESOURCEW(resourceId), L"TEXTURE");
			if (!hrs) { return nullptr; }
			HANDLE ptr = LoadResource(nullptr, hrs);
			if (!ptr) { return nullptr; }

			DWORD size = SizeofResource(nullptr, hrs);
			LoadFromWICMemory(ptr, size, WIC_FLAGS_NONE, &metadata, image);

			// 読み込んだ画像データをDirectXへわたしてテクスチャとして管理させる
			CreateShaderResourceView(renderer->GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &resourceView);
			// なんか失敗した場合に警告を出す
			assert(resourceView);
			__resources[key].resource = new TextureDX(key, resourceView, metadata.width, metadata.height);
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == Texture::TYPE) {
			__AddScope(key, scope);
			return (Texture*)__resources[key].resource;
		}
		return nullptr;
	}

	Texture* ResourceToolDX::RenderText(const std::wstring& text, const FONT& font, const std::string& scope)
	{
		std::hash<float> floatHash;
		std::string hashData = "text:";
		hashData += std::to_string(wstrToHash(text));
		hashData += std::to_string(strToHash(font.fontName));
		hashData += std::to_string(floatHash(font.size));
		hashData += std::to_string(floatHash(font.style));
		hashData += std::to_string(floatHash(font.weight));
		HASH key = strToHash(hashData);
		if (!__resources[key].resource) {
			//std::wstring wstr = StringToWString(text);
			const wchar_t* wcstr = text.c_str();

			ID3D11Texture2D* texture2D;
			ID3D11ShaderResourceView* resourceView = nullptr;
			IDXGISurface* dxgiSurface = nullptr;
			ID2D1RenderTarget* renderTarget = nullptr;

			IDWriteTextFormat* textFormat = nullptr;
			IDWriteTextLayout* textLayout = nullptr;
			ID2D1SolidColorBrush* solidBrush = nullptr;

			std::wstring fontName = StringToWString(font.fontName);

			// テキストフォーマット作成
			dwriteFactory->CreateTextFormat(
				fontName.c_str(),
				nullptr,
				(DWRITE_FONT_WEIGHT)font.weight,
				(DWRITE_FONT_STYLE)font.style,
				DWRITE_FONT_STRETCH_NORMAL,
				font.size,
				L"ja-jp",
				&textFormat
			);
			textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			
			F2 screenSize = GetScreenSize();

			// テキストレイアウト作成
			dwriteFactory->CreateTextLayout(wcstr, wcslen(wcstr), textFormat, screenSize.x, screenSize.y, &textLayout);

			// 大きさ計測
			DWRITE_TEXT_METRICS textMetrics;
			textLayout->GetMetrics(&textMetrics);

			// テクスチャ作成
			D3D11_TEXTURE2D_DESC texDesc = {};
			texDesc.Width = textMetrics.width;
			texDesc.Height = textMetrics.height;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			renderer->GetDevice()->CreateTexture2D(&texDesc, nullptr, &texture2D);
			if (texture2D) {
				renderer->GetDevice()->CreateShaderResourceView(texture2D, NULL, &resourceView);

				// レンダーターゲットの作成
				D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
					D2D1_RENDER_TARGET_TYPE_DEFAULT,
					D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
					0, 0
				);
				texture2D->QueryInterface(&dxgiSurface);
				d2dFactory->CreateDxgiSurfaceRenderTarget(dxgiSurface, &props, &renderTarget);

				// ブラシ作成
				renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &solidBrush);

				renderTarget->BeginDraw();

				renderTarget->Clear();

				renderTarget->DrawTextLayout({ 0, 0 }, textLayout, solidBrush);

				renderTarget->EndDraw();

				__resources[key].resource = new TextureDX(key, resourceView, textMetrics.width, textMetrics.height);
			}
			

			textFormat->Release();
			textLayout->Release();
			if (solidBrush) {
				solidBrush->Release();
			}
			
			if (renderTarget) {
				renderTarget->Release();
			}
			if (dxgiSurface) {
				dxgiSurface->Release();
			}
			if (texture2D) {
				texture2D->Release();
			}
			
			
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == Texture::TYPE) {
			__AddScope(key, scope);
			return (Texture*)__resources[key].resource;
		}
		return nullptr;
	}

	// =======================================================
	// オーディオデータ読み込み
	// =======================================================
	AudioDX* __LoadAudio(const HASH key, HMMIO hmmio)
	{
		WAVEFORMATEX wfx = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else
		{
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		BYTE* soundData;
		int length;
		int playLength;

		buflen = datachunkinfo.cksize;
		soundData = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)soundData, buflen);

		length = readlen;
		playLength = readlen / wfx.nBlockAlign;


		mmioClose(hmmio, 0);

		return new AudioDX(key, soundData, wfx, length, playLength);
	}


	// =======================================================
	// オーディオリソース取得
	// =======================================================
	Audio* ResourceToolDX::LoadAudio(const std::string& path, const std::string& scope)
	{
		const HASH key = strToHash(path);
		if (!__resources[key].resource) {
			HMMIO hmmio = NULL;
			MMIOINFO mmioinfo = { 0 };
			hmmio = mmioOpen((LPSTR)path.data(), &mmioinfo, MMIO_READ);
			assert(hmmio);
			__resources[key].resource = __LoadAudio(key, hmmio);
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == Audio::TYPE) {
			__AddScope(key, scope);
			return (Audio*)__resources[key].resource;
		}
		return nullptr;
	}


	// =======================================================
	// オーディオリソース取得
	// =======================================================
	Audio* ResourceToolDX::LoadAudio(unsigned int resourceId, const std::string& scope)
	{
		//string key = "aud_" + std::to_string(resourceId);
		const HASH key = strToHash("audio:" + std::to_string(resourceId));
		if (!__resources[key].resource) {
			// リソースを見つける
			HRSRC hrs = FindResourceW(nullptr, MAKEINTRESOURCEW(resourceId), L"AUDIO");
			if (!hrs) { return nullptr; }

			// リソースをロード
			HGLOBAL ptr = LoadResource(nullptr, hrs);
			if (!ptr) { return nullptr; }

			// リソースをロックしてメモリアドレスを取得
			LPVOID pLockedResource = LockResource(ptr);
			DWORD resourceSize = SizeofResource(nullptr, hrs);
			if (!pLockedResource || resourceSize == 0) {
				return nullptr;
			}

			// メモリを MMIO ハンドルとして扱う
			MMIOINFO mmioInfo = {};
			mmioInfo.pchBuffer = static_cast<HPSTR>(pLockedResource);
			mmioInfo.fccIOProc = FOURCC_MEM;
			mmioInfo.cchBuffer = resourceSize;

			HMMIO hmmio = mmioOpen(nullptr, &mmioInfo, MMIO_READ);
			assert(hmmio);

			__resources[key].resource = __LoadAudio(key, hmmio);
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == Audio::TYPE) {
			__AddScope(key, scope);
			return (Audio*)__resources[key].resource;
		}
		return nullptr;
	}

	// =======================================================
	// モデルリソース取得
	// =======================================================
	Model* ResourceToolDX::LoadModel(const std::string& path, const std::string& scope)
	{
		const HASH key = strToHash(path);
		std::string subScope = std::to_string(key);
		if (!__resources[key].resource) {
			MGObject mgo = LoadMGO(path.c_str());
			MODEL* rawModel = GetModelByMGObject(mgo);
			ModelDX* model = new ModelDX(key);
			model->rawModel = rawModel;
			for (int i = 0; i < rawModel->textureNum; i++) {
				TEXTURE& texture = rawModel->textures[i];
				const HASH textureKey = strToHash(texture.textureStr);
				if (!__resources[textureKey].resource) {
					ID3D11ShaderResourceView* resourceView;
					TexMetadata metadata;
					ScratchImage image;
					size_t size = (texture.height) ? sizeof(unsigned char) * texture.width * texture.height * 4 : texture.width;
					LoadFromWICMemory(texture.data, size, WIC_FLAGS_NONE, &metadata, image);
					CreateShaderResourceView(renderer->GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &resourceView);
					assert(resourceView);
					texture.width = metadata.width;
					texture.height = metadata.height;
					__resources[textureKey].resource = new TextureDX(textureKey, resourceView, metadata.width, metadata.height);
				}
				if (__resources[textureKey].resource && __resources[textureKey].resource->GetType() == Texture::TYPE) {
					__AddScope(textureKey, subScope);
				}
			}

			ID3D11Device* pDevice = renderer->GetDevice();

			for (int i = 0; i < rawModel->meshNum; i++) {
				MESH* mesh = (rawModel->meshes + i);

				// 頂点バッファ生成
				{
					D3D11_BUFFER_DESC bd = {};
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.ByteWidth = sizeof(VERTEX) * mesh->vertexNum;
					bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bd.CPUAccessFlags = 0;

					D3D11_SUBRESOURCE_DATA data;
					data.pSysMem = mesh->vertices;
					data.SysMemPitch = 0;
					data.SysMemSlicePitch = 0;

					ID3D11Buffer* vertexBuffer;
					pDevice->CreateBuffer(&bd, &data, &vertexBuffer);

					model->vertexBuffers[mesh] = vertexBuffer;
				}

				if (mesh->boneNum > 0) {

					D3D11_BUFFER_DESC bd = {};
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.ByteWidth = sizeof(VERTEX_BONE_WEIGHT) * mesh->vertexNum;
					bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bd.CPUAccessFlags = 0;

					D3D11_SUBRESOURCE_DATA data;
					data.pSysMem = mesh->boneWeights;
					data.SysMemPitch = 0;
					data.SysMemSlicePitch = 0;

					ID3D11Buffer* boneWeightBuffer;
					pDevice->CreateBuffer(&bd, &data, &boneWeightBuffer);
					model->boneWeightBuffers[mesh] = boneWeightBuffer;
				}

				{
					LoadNodeWorldTransforms(model->rawModel->rootNode, M4x4::TranslatingMatrix({}), model->nodeWorldTransforms);
				}

				std::unordered_map<std::string, MODEL_NODE*> nameNodeMap;

				for (auto& keyPair : model->nodeWorldTransforms) {
					nameNodeMap[keyPair.first->name] = keyPair.first;
				}

				// ボーン
				{
					model->meshBones[mesh].reserve(mesh->boneNum);
					for (int b = 0; b < mesh->boneNum; b++) {
						MODEL_NODE* node = nameNodeMap[mesh->bones[b].name];
						model->meshBones[mesh].push_back({
							mesh->bones[b].transform,
							model->nodeWorldTransforms[node],
							node
						});
					}
				}
				
				// インデックバッファ生成
				{
					D3D11_BUFFER_DESC bd = {};
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.ByteWidth = sizeof(unsigned int) * mesh->vertexIndexNum;
					bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bd.CPUAccessFlags = 0;

					D3D11_SUBRESOURCE_DATA data;
					data.pSysMem = mesh->vertexIndexes;
					data.SysMemPitch = 0;
					data.SysMemSlicePitch = 0;

					ID3D11Buffer* indexBuffer;
					pDevice->CreateBuffer(&bd, &data, &indexBuffer);

					model->indexBuffers[mesh] = indexBuffer;
				}

				if (strcmp(mesh->textureStr, "")) {
					model->meshTextures[mesh] = LoadTexture(mesh->textureStr, subScope);
				}
				else {
					model->meshTextures[mesh] = LoadTexture(TEXTURE_WHITE, subScope);
				}
			}
			
			__resources[key].resource = model;
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == Model::TYPE) {
			__AddScope(key, scope);
			return (Model*)__resources[key].resource;
		}
		return nullptr;
	}

	Animation* ResourceToolDX::LoadAnimation(const std::string& path, const std::string& scope)
	{
		const HASH key = strToHash(path);
		if (!__resources[key].resource) {
			MGObject mgo = LoadMGO(path.c_str());
			ANIMATION* rawAnimation = GetAnimationByMGObject(mgo);
			Animation* animation = new Animation(key);
			animation->rawAnimation = rawAnimation;

			for (int i = 0; i < rawAnimation->channelNum; i++) {
				const char* nodeName = rawAnimation->channels[i].nodeName;
				animation->modelNodeChannels[nodeName] = (rawAnimation->channels + i);
			}
			

			__resources[key].resource = animation;
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == Animation::TYPE) {
			__AddScope(key, scope);
			return (Animation*)__resources[key].resource;
		}
		return nullptr;
	}


} // namespace MG