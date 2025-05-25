// =======================================================
// MGCommon.h
// 
// MG（まんちゃんゲームズ）の共通関数など
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _MG_COMMON_H
#define _MG_COMMON_H

#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>
#include "commonVariable.h"
#include "MGDataType.h"

#define IS_TYPE(obj, type) (typeid(obj) == typeid(type))

namespace MG {
	static constexpr const char* RESOURCE_SCOPE_GOBAL = "gobal";
	static constexpr const float PI = 3.14159274f;
	using std::string;
	using std::list;
	using std::map;

	class Renderer;
	class DrawTool;
	class AudioTool;
	class ResourceTool;

	class Resource;
	class Texture;
	class Audio;
	class AudioPlayer;
	class Model;
	class Animation;

	class Scene;
	class SceneTransition;
	class GameObject;

	float FastInverseSquareRoot(float value);
	float Dot(const F2& v0, const F2& v1);
	float Dot(const F3& v0, const F3& v1);
	float Dot(const Quaternion& q0, const Quaternion& q1);
	F3 Cross(const F3& v0, const F3& v1);
	F2 Normalize(const F2& v);
	F3 Normalize(const F3& v);
	F3 FastNormalize(const F3& v);
	Quaternion Normalize(const Quaternion& q);
	bool Collision2D(const std::vector<F2>& shape, const F2& point);
	bool Collision2D(const std::vector<F2>& shape0, const std::vector<F2>& shape1);
	float Distance(const F2& p0, const  F2& p1);
	float Distance(const F3& p0, const F3& p1);
	float DistanceSquare(const F2& p0, const  F2& p1);
	float DistanceSquare(const F3& p0, const F3& p1);
	float Lerp(const float& v0, const float& v1, float t);
	F3 Lerp(const F3& v0, const F3& v1, float t);
	Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);
	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
	M4x4 Lerp(const M4x4& m0, const M4x4& m1, float t);
	F3 Rotate(const F3& v, const Quaternion& q);
	F3 Bezier(F3 p0, F3 p1, F3 p2, float t);
	F4 HSV2RGB(float h, float s, float v, float a = 1.0f);

	void ApplyAnimation(ANIMATION_CHANNEL* animationChannel, float frame, F3* size, F3* position, Quaternion* rotate);
	void LoadNodeWorldTransforms(MODEL_NODE* currentNode, const M4x4& worldTransform, std::map<MODEL_NODE*, M4x4>& nodeWorldTransforms);
	void LoadNodeWorldTransforms(MODEL_NODE* currentNode, const M4x4& worldTransform, std::map<MODEL_NODE*, M4x4>& nodeWorldTransforms, 
		const std::vector<ANIMATION_APPLICANT>& animationApplicants);
	void LoadNodeWorldTransforms(MODEL_NODE* currentNode, const M4x4& worldTransform, std::map<MODEL_NODE*, M4x4>& nodeWorldTransforms, 
		const std::vector<ANIMATION_APPLICANT>& animationSet0, const std::vector<ANIMATION_APPLICANT>& animationSet1, float t);
	MODEL_NODE* FindNodeByName(MODEL_NODE* root, const std::string& name);

	void SetRenderer(Renderer* renderer);
	Renderer* GetRenderer();
	void SetDrawTool(DrawTool* drawTool);
	DrawTool* GetDrawTool();
	void SetAudioTool(AudioTool* audioTool);
	AudioTool* GetAudioTool();
	void SetResourceTool(ResourceTool* resourceTool);
	ResourceTool* GetResourceTool();

	void SetScreen(float width, float height, float centerX, float centerY);
	F2 GetScreenSize();
	F2 GetScreenCenter();
	float GetScreenWidth();
	float GetScreenHeight();
	float GetScreenCenterX();
	float GetScreenCenterY();
	void SetDeltaTime(int deltaTime);
	int GetDeltaTime();
	std::wstring StringToWString(const std::string& str);
	std::string FixedFloatString(float value, unsigned int digits);
	void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to);
	

	Texture* LoadTexture(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL);
	Texture* RenderText(const std::wstring& text, const FONT& font = {}, const std::string& scope = RESOURCE_SCOPE_GOBAL);
	void ReleaseTexture(const std::string& path, const std::string& scope);
	void ReleaseTexture(const std::string& scope);

	Audio* LoadAudio(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL);
	void ReleaseAudio(const std::string& path, const std::string& scope);
	void ReleaseAudio(const std::string& scope);

	Model* LoadModel(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL);
	void ReleaseModel(const std::string& path, const std::string& scope);
	void ReleaseModel(const std::string& scope);

	Animation* LoadAnimation(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL);
	void ReleaseAnimation(const std::string& path, const std::string& scope);
	void ReleaseAnimation(const std::string& scope);

	void ReleaseResource(Resource* resource, const std::string& scope);
	void ReleaseResource(const std::string& scope);

	
	void DrawQuad(const Texture* texture, const F3& position, const F3& size,
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
		const F2& uvOffset = { 0.0f, 0.0f },
		const F2& uvRange = { 1.0f, 1.0f }
	);
	void DrawCube(const Texture* texture, const F3& position, const F3& size,
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

	void DrawModel(const Model* model,
		const F3& position = { 0.0f, 0.0f, 0.0f },
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
		const F2& uvOffset = { 0.0f, 0.0f },
		const F2& uvRange = { 1.0f, 1.0f }
	);

	void DrawModel(const Model* model,
		const std::vector<ANIMATION_APPLICANT>& animationApplicants,
		const F3& position = { 0.0f, 0.0f, 0.0f },
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

	void DrawModel(const Model* model,
		const std::vector<ANIMATION_APPLICANT>& animationApplicants0,
		const std::vector<ANIMATION_APPLICANT>& animationApplicants1,
		const float animTransitionT,
		const F3& position = { 0.0f, 0.0f, 0.0f },
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

	void DrawModel(
		const Model* model,
		const std::map<MODEL_NODE*, M4x4>& transforms,
		const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

	void DrawPolygon(const Texture* texture, const VERTEX* vertices, size_t length,
		TOPOLOGY topology = TOPOLOGY_TRIANGLESTRIP,
		const F3& position = { 0.0f, 0.0f, 0.0f },
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
		const F2& uvOffset = { 0.0f, 0.0f },
		const F2& uvRange = { 1.0f, 1.0f }
	);


} // namespace MG

#endif