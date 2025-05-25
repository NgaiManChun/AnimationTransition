// =======================================================
// resourceTool.h
// 
// リソース管理クラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _RESOURCE_TOOL_H
#define _RESOURCE_TOOL_H

#include "MGCommon.h"
#include "MGDataType.h"

namespace MG {
	//constexpr const char* RESOURCE_SCOPE_GOBAL = "gobal";

	typedef unsigned long long HASH;

	struct RESOURCE_NOTE {
		Resource* resource = 0;
		std::list<string> scope;
	};

	class Resource {
	private:
		const HASH key;
	public:
		Resource(const HASH key);
		virtual HASH GetType() = 0;
		HASH GetKey();
	};

	class Texture : public Resource {
	private:
		unsigned int width = 0;
		unsigned int height = 0;
	public:
		static HASH TYPE;
		Texture(const HASH key, unsigned int width, unsigned int height);
		HASH GetType() override;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
	};

	class Audio : public Resource {
	public:
		static HASH TYPE;
		Audio(const HASH key);
		HASH GetType() override;
	};

	class Model : public Resource {
	public:
		static HASH TYPE;
		MODEL* rawModel;
		std::map<MESH*, Texture*> meshTextures;

		Model(const HASH key);
		HASH GetType() override;
	};

	class Animation : public Resource {
	public:
		static HASH TYPE;
		ANIMATION* rawAnimation;
		std::unordered_map<std::string, ANIMATION_CHANNEL*> modelNodeChannels;

		Animation(const HASH key);
		HASH GetType() override;
		void Apply(const std::string& nodeName, float frame, F3& size, F3& position, Quaternion& rotate);
	};

	class ResourceTool {
	protected:
		std::hash<std::string> strToHash{};
		std::hash<std::wstring> wstrToHash{};
		map<HASH, RESOURCE_NOTE> __resources;
		virtual void __ReleaseResource(const HASH key) = 0;
		void __AddScope(const HASH key, const string& scope);
	public:
		virtual Texture* LoadTexture(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		virtual Texture* LoadTexture(unsigned int resourceId, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		virtual Texture* RenderText(const std::wstring& text, const FONT& font = {}, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		void ReleaseTexture(const std::string& path, const std::string& scope);
		void ReleaseTexture(unsigned int resourceId, const std::string& scope);
		void ReleaseTexture(const std::string& scope);

		virtual Audio* LoadAudio(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		virtual Audio* LoadAudio(unsigned int resourceId, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		void ReleaseAudio(const std::string& path, const std::string& scope);
		void ReleaseAudio(unsigned int resourceId, const std::string& scope);
		void ReleaseAudio(const std::string& scope);

		virtual Model* LoadModel(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		void ReleaseModel(const std::string& path, const std::string& scope);
		void ReleaseModel(const std::string& scope);

		virtual Animation* LoadAnimation(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		void ReleaseAnimation(const std::string& path, const std::string& scope);
		void ReleaseAnimation(const std::string& scope);

		void ReleaseResource(const std::string& path, const std::string& scope);
		void ReleaseResource(unsigned int resourceId, const std::string& scope);
		void ReleaseResource(const HASH key, const std::string& scope);
		void ReleaseResource(Resource* resource, const std::string& scope);
		void ReleaseResource(const std::string& scope);
		void ReleaseAllResource();
	};

} // namespace MG

#endif