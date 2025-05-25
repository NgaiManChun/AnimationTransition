// =======================================================
// resourceTool.cpp
// 
// リソース管理クラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "resourceTool.h"
#include <typeinfo>

namespace MG {
	Resource::Resource(const HASH key) : key(key)
	{
	}
	HASH Resource::GetKey() {
		return key;
	}
	HASH Texture::TYPE = typeid(Texture).hash_code();
	HASH Audio::TYPE = typeid(Audio).hash_code();
	HASH Model::TYPE = typeid(Model).hash_code();
	HASH Animation::TYPE = typeid(Animation).hash_code();
	HASH Texture::GetType() { return Texture::TYPE; }
	HASH Audio::GetType() { return Audio::TYPE; }
	
	HASH Model::GetType() { return Model::TYPE; }
	HASH Animation::GetType() { return Animation::TYPE; }

	Texture::Texture(const HASH key, unsigned int width, unsigned int height)
		: Resource(key), width(width), height(height) {}

	unsigned int Texture::GetWidth() const {
		return width;
	}

	unsigned int Texture::GetHeight() const {
		return height;
	}

	Audio::Audio(const HASH key) : Resource(key) {}
	Model::Model(const HASH key) : Resource(key) {}
	Animation::Animation(const HASH key) : Resource(key) {}

	void Animation::Apply(const std::string& nodeName, float frame, F3& size, F3& position, Quaternion& rotate)
	{
		ANIMATION_CHANNEL* animationChannel = nullptr;
		auto itr = modelNodeChannels.find(nodeName);
		if (itr != modelNodeChannels.end()) {
			animationChannel = itr->second;
		}
		else {
			return;
		}

		if (animationChannel->positionKeyNum) {
			VECTOR_KEY* minKey = animationChannel->positionKeys;
			VECTOR_KEY* maxKey = minKey;
			for (int i = 0; i < animationChannel->positionKeyNum; i++) {
				if (frame < animationChannel->positionKeys[i].frame) {
					maxKey = (animationChannel->positionKeys + i);
					break;
				}
				else {
					minKey = (animationChannel->positionKeys + i);
					maxKey = minKey;
				}
			}
			float d = (maxKey->frame - minKey->frame);
			if (d > 0.0f) {
				float t = (frame - minKey->frame) / d;
				position = minKey->vector * (1.0f - t) + maxKey->vector * t;
			}
			else {
				position = minKey->vector;
			}
		}

		if (animationChannel->scalingKeyNum) {
			VECTOR_KEY* minKey = animationChannel->scalingKeys;
			VECTOR_KEY* maxKey = minKey;
			for (int i = 0; i < animationChannel->scalingKeyNum; i++) {
				if (frame < animationChannel->scalingKeys[i].frame) {
					maxKey = (animationChannel->scalingKeys + i);
					break;
				}
				else {
					minKey = (animationChannel->scalingKeys + i);
					maxKey = minKey;
				}
			}
			float d = (maxKey->frame - minKey->frame);
			if (d > 0.0f) {
				float t = (frame - minKey->frame) / d;
				size = minKey->vector * (1.0f - t) + maxKey->vector * t;
			}
			else {
				size = minKey->vector;
			}
		}

		if (animationChannel->rotationKeyNum) {
			QUATERNION_KEY* minKey = animationChannel->rotationKeys;
			QUATERNION_KEY* maxKey = minKey;
			for (int i = 0; i < animationChannel->rotationKeyNum; i++) {
				if (frame < animationChannel->rotationKeys[i].frame) {
					maxKey = (animationChannel->rotationKeys + i);
					break;
				}
				else {
					minKey = (animationChannel->rotationKeys + i);
					maxKey = minKey;
				}
			}
			float d = (maxKey->frame - minKey->frame);
			if (d > 0.0f) {
				float t = (frame - minKey->frame) / d;
				rotate = Lerp(minKey->rotate, maxKey->rotate, t);
			}
			else {
				rotate = minKey->rotate;
			}
		}
	}


	// =======================================================
	// 指定したリソースをスコープを追加
	// =======================================================
	void ResourceTool::__AddScope(const HASH key, const string& scope)
	{
		if (std::find(__resources[key].scope.begin(), __resources[key].scope.end(), scope) == __resources[key].scope.end()) {
			__resources[key].scope.push_back(scope);
		}
	}


	// =======================================================
	// 指定したテクスチャをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseTexture(const std::string& path, const std::string& scope)
	{
		const HASH key = strToHash(path);
		if (__resources[key].resource && __resources[key].resource->GetType() == Texture::TYPE) {
			ReleaseResource(key, scope);
		}
	}


	// =======================================================
	// 指定したテクスチャをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseTexture(unsigned int resourceId, const std::string& scope)
	{
		ReleaseTexture("texture:" + std::to_string(resourceId), scope);
	}

	// =======================================================
	// 指定したスコープからすべてのテクスチャを解放
	// =======================================================
	void ResourceTool::ReleaseTexture(const std::string& scope)
	{
		for (const auto& pair : __resources) {
			if (pair.second.resource && pair.second.resource->GetType() == Texture::TYPE) {
				ReleaseResource(pair.first, scope);
			}
		}
	}


	// =======================================================
	// 指定したオーディオをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseAudio(const std::string& path, const std::string& scope)
	{
		const HASH key = strToHash(path);
		if (__resources[key].resource && __resources[key].resource->GetType() == Audio::TYPE) {
			ReleaseResource(key, scope);
		}
	}


	// =======================================================
	// 指定したオーディオをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseAudio(unsigned int resourceId, const std::string& scope)
	{
		ReleaseAudio("audio:" + std::to_string(resourceId), scope);
	}


	// =======================================================
	// 指定したスコープからすべてのオーディオを解放
	// =======================================================
	void ResourceTool::ReleaseAudio(const std::string& scope)
	{
		for (const auto& pair : __resources) {
			if (pair.second.resource && pair.second.resource->GetType() == Audio::TYPE) {
				ReleaseResource(pair.first, scope);
			}
		}
	}


	// =======================================================
	// 指定したモデルをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseModel(const std::string& path, const std::string& scope)
	{
		const HASH key = strToHash(path);
		if (__resources[key].resource && __resources[key].resource->GetType() == Model::TYPE) {
			ReleaseResource(key, scope);
		}
	}


	// =======================================================
	// 指定したスコープからすべてのモデルを解放
	// =======================================================
	void ResourceTool::ReleaseModel(const std::string& scope)
	{
		for (const auto& pair : __resources) {
			if (pair.second.resource && pair.second.resource->GetType() == Model::TYPE) {
				ReleaseResource(pair.first, scope);
			}
		}
	}

	void ResourceTool::ReleaseAnimation(const std::string& path, const std::string& scope)
	{
		const HASH key = strToHash(path);
		if (__resources[key].resource && __resources[key].resource->GetType() == Animation::TYPE) {
			ReleaseResource(key, scope);
		}
	}

	void ResourceTool::ReleaseAnimation(const std::string& scope)
	{
		for (const auto& pair : __resources) {
			if (pair.second.resource && pair.second.resource->GetType() == Animation::TYPE) {
				ReleaseResource(pair.first, scope);
			}
		}
	}


	// =======================================================
	// 指定したリソースをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseResource(const std::string& path, const std::string& scope)
	{
		ReleaseResource(strToHash(path), scope);
	}


	// =======================================================
	// 指定したリソースをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseResource(unsigned int resourceId, const std::string& scope)
	{
		ReleaseResource(std::to_string(resourceId), scope);
	}

	void ResourceTool::ReleaseResource(const HASH key, const std::string& scope)
	{
		if (__resources[key].resource) {
			auto it = std::find(__resources[key].scope.begin(), __resources[key].scope.end(), scope);
			if (it != __resources[key].scope.end()) {
				__resources[key].scope.erase(it);
			}

			if (__resources[key].scope.size() == 0) {
				__ReleaseResource(key);
			}
		}
	}


	// =======================================================
	// 指定したリソースをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseResource(Resource* resource, const std::string& scope)
	{
		if (resource) {
			ReleaseResource(resource->GetKey(), scope);
		}
	}


	// =======================================================
	// 指定したスコープからすべてのリソースを解放
	// =======================================================
	void ResourceTool::ReleaseResource(const std::string& scope)
	{
		for (const auto& pair : __resources) {
			ReleaseResource(pair.first, scope);
		}
	}


	// =======================================================
	// すべてのリソースを解放
	// =======================================================
	void ResourceTool::ReleaseAllResource()
	{
		for (const auto& pair : __resources) {
			if (pair.second.resource) {
				__ReleaseResource(pair.first);
			}
		}
		__resources.clear();
	}
	
}