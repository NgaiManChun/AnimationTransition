// =======================================================
// MGCommon.cpp
// 
// MG（まんちゃんゲームズ）の共通関数など
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "MGCommon.h"
#include "renderer.h"
#include "drawTool.h"
#include "audioTool.h"
#include "resourceTool.h"
#include <vector>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>

namespace MG {
	static Renderer* g_renderer;
	static DrawTool* g_drawTool;
	static AudioTool* g_audioTool;
	static ResourceTool* g_resourceTool;
	static F4 g_screen;
	static int g_deltaTime;

	// 高速逆平方根
	float FastInverseSquareRoot(float value) {
		float half = 0.5f * value;
		int i = *(int*)&value;
		i = 0x5f3759df - (i >> 1);	// 魔法の定数
		value = *(float*)&i;
		value = value * (1.5f - half * value * value);
		return value;
	}

	float Dot(const F2& v0, const F2& v1) {
		return v0.x * v1.x + v0.y * v1.y;
	}

	float Dot(const F3& v0, const F3& v1) {
		return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
	}

	float Dot(const Quaternion& q0, const Quaternion& q1) {
		return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
	}

	F3 Cross(const F3& v0, const F3& v1) {
		return F3{
			-(v0.z * v1.y - v0.y * v1.z),
			-(v0.x * v1.z - v0.z * v1.x),
			-(v0.y * v1.x - v0.x * v1.y) 
		};
	}

	F2 Normalize(const F2& v) {
		float length = sqrtf((v.x * v.x) + (v.y * v.y));
		if (length > 0)	length = 1 / length;
		return {
		  v.x * length,
		  v.y * length
		};
	}

	F3 Normalize(const F3& v) {
		float length = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		if (length > 0)	length = 1 / length;
		return {
		  v.x * length,
		  v.y * length,
		  v.z * length
		};
	}

	F3 FastNormalize(const F3& v) {
		float length = FastInverseSquareRoot((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		if (length > 0)	length = 1 / length;
		return {
		  v.x * length,
		  v.y * length,
		  v.z * length
		};
	}

	Quaternion Normalize(const Quaternion& q) {
		float length = sqrtf((q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w));
		if (length > 0.0f) {
			float invLength = 1.0f / length;
			return {
			  q.x * invLength,
			  q.y * invLength,
			  q.z * invLength,
			  q.w * invLength
			};
		}
		return q;
	}
	
	bool Collision2D(const std::vector<F2>& shape, const F2& point) {
		for (int i = 0; i < shape.size(); i++) {
			F2 vp = Normalize(point - shape[i]);
			F2 v0 = Normalize(shape[(i + 1) % shape.size()] - shape[i]);
			F2 v1 = {};
			if (i > 0) {
				v1 = Normalize(shape[i - 1] - shape[i]);
			}
			else {
				v1 = Normalize(shape[shape.size() - 1] - shape[i]);
			}
			float d0 = Dot(vp, v0);
			float d1 = Dot(v0, v1);
			if (d0 < d1) {
				return false;
			}
		}
		return true;
	}
	
	bool Collision2D(const std::vector<F2>& shape0, const std::vector<F2>& shape1) {

		// 線分交差判定
		for (int i = 0; i < shape0.size(); i++) {
			for (int j = 0; j < shape1.size(); j++) {
				float d1 = Dot(shape1[j] - shape0[i], shape0[(i + 1) % shape0.size()] - shape0[i]);
				float d2 = Dot(shape1[(j + 1) % shape1.size()] - shape0[i], shape0[(i + 1) % shape0.size()] - shape0[i]);
				if (d1 * d2 <= 0) {
					return true;
				}
			}
		}

		// 内包判定
		std::vector<F2> shapes[2] = {
			shape0,
			shape1
		};
		for (int s = 0; s < 2; s++) {
			std::vector<F2>& shape0 = shapes[s];
			std::vector<F2>& shape1 = shapes[(s + 1) % 2];
			for (int i = 0; i < shape1.size(); i++) {
				if (Collision2D(shape0, shape1[i]))
					return true;
			}
		}
		return false;
	}

	float Distance(const F2& p0, const  F2& p1) {
		return sqrtf((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y));
	}

	float Distance(const F3& p0, const F3& p1) {
		return sqrtf((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y) + (p1.z - p0.z) * (p1.z - p0.z));
	}

	float DistanceSquare(const F2& p0, const  F2& p1) {
		return (p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y);
	}

	float DistanceSquare(const F3& p0, const F3& p1) {
		return (p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y) + (p1.z - p0.z) * (p1.z - p0.z);
	}

	float Lerp(const float& v0, const float& v1, float t)
	{
		return v0 * (1.0f - t) + v1 * t;
	}

	F3 Lerp(const F3& v0, const F3& v1, float t)
	{
		return v0 * (1.0f - t) + v1 * t;
	}

	Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t) {
		Quaternion result{
			(1 - t) * q1.x + t * q2.x,
			(1 - t) * q1.y + t * q2.y,
			(1 - t) * q1.z + t * q2.z,
			(1 - t) * q1.w + t * q2.w
		};
		result.Normalize();
		return result;
	}

	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
		float dotProduct = Dot(q0, q1);

		Quaternion q2Modified = q1;
		if (dotProduct < 0.0f) {
			dotProduct = -dotProduct;
			q2Modified.w = -q1.w;
			q2Modified.x = -q1.x;
			q2Modified.y = -q1.y;
			q2Modified.z = -q1.z;
		}

		// 内積がほぼ1の場合、LERPにフォールバック
		if (dotProduct > 1.0f - EPSILON) {
			Quaternion result;
			result.w = (1 - t) * q1.w + t * q2Modified.w;
			result.x = (1 - t) * q1.x + t * q2Modified.x;
			result.y = (1 - t) * q1.y + t * q2Modified.y;
			result.z = (1 - t) * q1.z + t * q2Modified.z;
			result.Normalize();
			return result;
		}

		// θ を計算
		float theta = acosf(dotProduct);
		float sinTheta = sqrtf(1.0f - dotProduct * dotProduct);

		// SLERP の補間
		float w1 = sinf((1.0f - t) * theta) / sinTheta;
		float w2 = sinf(t * theta) / sinTheta;

		return {
			w1 * q1.x + w2 * q2Modified.x,
			w1 * q1.y + w2 * q2Modified.y,
			w1 * q1.z + w2 * q2Modified.z,
			w1 * q1.w + w2 * q2Modified.w
		};
	}

	M4x4 Lerp(const M4x4& m0, const M4x4& m1, float t)
	{
		float s = 1.0f - t;
		return {
			m0._v00 * s + m1._v00 * t,		m0._v01 * s + m1._v01 * t,		m0._v02 * s + m1._v02 * t,		m0._v03 * s + m1._v03 * t,
			m0._v10 * s + m1._v10 * t,		m0._v11 * s + m1._v11 * t,		m0._v12 * s + m1._v12 * t,		m0._v13 * s + m1._v13 * t,
			m0._v20 * s + m1._v20 * t,		m0._v21 * s + m1._v21 * t,		m0._v22 * s + m1._v22 * t,		m0._v23 * s + m1._v23 * t,
			m0._v30 * s + m1._v30 * t,		m0._v31 * s + m1._v31 * t,		m0._v32 * s + m1._v32 * t,		m0._v33 * s + m1._v33 * t,
		};
	}

	F3 Rotate(const F3& v, const Quaternion& q) {
		Quaternion nq = Normalize(q);
		Quaternion conjugate = { -nq.x, -nq.y, -nq.z, nq.w };
		Quaternion rq = (nq * Quaternion{ v.x, v.y, v.z, 0 }) * conjugate;

		return { rq.x, rq.y, rq.z };
	}

	F3 Bezier(F3 p0, F3 p1, F3 p2, float t) {
		float f0 = (1.0f - t) * (1.0f - t);
		float f1 = 2.0f * (1.0f - t) * t;
		float f2 = t * t;
		return p0 * f0 + p1 * f1 + p2 * f2;
	}

	F4 HSV2RGB(float h, float s, float v, float a) {
		float r = v;
		float g = v;
		float b = v;
		if (s > 0.0f) {
			h *= 6.0f;
			int i = (int)h;
			float f = h - (float)i;
			switch (i) {
			default:
			case 0:
				g *= 1 - s * (1 - f);
				b *= 1 - s;
				break;
			case 1:
				r *= 1 - s * f;
				b *= 1 - s;
				break;
			case 2:
				r *= 1 - s;
				b *= 1 - s * (1 - f);
				break;
			case 3:
				r *= 1 - s;
				g *= 1 - s * f;
				break;
			case 4:
				r *= 1 - s * (1 - f);
				g *= 1 - s;
				break;
			case 5:
				g *= 1 - s;
				b *= 1 - s * f;
				break;
			}
		}
		return { r, g, b, a };
	}

	void ApplyAnimation(ANIMATION_CHANNEL* animationChannel, float frame, F3* size, F3* position, Quaternion* rotate) {
		if (animationChannel->positionKeyNum && position) {
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
				*position = minKey->vector * (1.0f - t) + maxKey->vector * t;
			}
			else {
				*position = minKey->vector;
			}
		}

		if (animationChannel->scalingKeyNum && size) {
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
				*size = minKey->vector * (1.0f - t) + maxKey->vector * t;
			}
			else {
				*size = minKey->vector;
			}
		}

		if (animationChannel->rotationKeyNum && rotate) {
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
				*rotate = Lerp(minKey->rotate, maxKey->rotate, t);
			}
			else {
				*rotate = minKey->rotate;
			}
		}
	}

	void LoadNodeWorldTransforms(MODEL_NODE* currentNode, const M4x4& worldTransform, std::map<MODEL_NODE*, M4x4>& nodeWorldTransforms) {
		nodeWorldTransforms[currentNode] = (M4x4::ScalingMatrix(currentNode->scale) * M4x4::RotatingMatrix(currentNode->rotate) * M4x4::TranslatingMatrix(currentNode->position)) * worldTransform;
		for (int i = 0; i < currentNode->childrenNum; i++) {
			LoadNodeWorldTransforms(&currentNode->children[i], nodeWorldTransforms[currentNode], nodeWorldTransforms);
		}
	}

	void LoadNodeWorldTransforms(MODEL_NODE* currentNode, const M4x4& worldTransform, std::map<MODEL_NODE*, M4x4>& nodeWorldTransforms, const std::vector<ANIMATION_APPLICANT>& animationApplicants) {

		F3 size = currentNode->scale;
		F3 position = currentNode->position;
		Quaternion rotate = { currentNode->rotate.x, currentNode->rotate.y, currentNode->rotate.z, currentNode->rotate.w };
		for (const ANIMATION_APPLICANT& applicant : animationApplicants) {
			auto itr = applicant.animation->modelNodeChannels.find(currentNode->name);
			if (itr != applicant.animation->modelNodeChannels.end()) {
				ANIMATION_CHANNEL* animationChannel = itr->second;
				ApplyAnimation(animationChannel, applicant.currentFrame, &size, &position, &rotate);
				break;
			}
		}
		nodeWorldTransforms[currentNode] = M4x4::ScalingMatrix(size) * M4x4::RotatingMatrix(rotate) * M4x4::TranslatingMatrix(position) * worldTransform;

		for (int i = 0; i < currentNode->childrenNum; i++) {
			LoadNodeWorldTransforms(&currentNode->children[i], nodeWorldTransforms[currentNode], nodeWorldTransforms, animationApplicants);
		}
	}

	void LoadNodeWorldTransforms(MODEL_NODE* currentNode, const M4x4& worldTransform, std::map<MODEL_NODE*, M4x4>& nodeWorldTransforms,
		const std::vector<ANIMATION_APPLICANT>& animationSet0, const std::vector<ANIMATION_APPLICANT>& animationSet1, float t)
	{
		F3 size0 = currentNode->scale;
		F3 position0 = currentNode->position;
		Quaternion rotate0 = { currentNode->rotate.x, currentNode->rotate.y, currentNode->rotate.z, currentNode->rotate.w };
		F3 size1 = currentNode->scale;
		F3 position1 = currentNode->position;
		Quaternion rotate1 = { currentNode->rotate.x, currentNode->rotate.y, currentNode->rotate.z, currentNode->rotate.w };
		for (const ANIMATION_APPLICANT& applicant : animationSet0) {
			auto itr = applicant.animation->modelNodeChannels.find(currentNode->name);
			if (itr != applicant.animation->modelNodeChannels.end()) {
				ANIMATION_CHANNEL* animationChannel = itr->second;
				ApplyAnimation(animationChannel, applicant.currentFrame, &size0, &position0, &rotate0);
				break;
			}
		}
		for (const ANIMATION_APPLICANT& applicant : animationSet1) {
			auto itr = applicant.animation->modelNodeChannels.find(currentNode->name);
			if (itr != applicant.animation->modelNodeChannels.end()) {
				ANIMATION_CHANNEL* animationChannel = itr->second;
				ApplyAnimation(animationChannel, applicant.currentFrame, &size1, &position1, &rotate1);
				break;
			}
		}
		nodeWorldTransforms[currentNode] = M4x4::ScalingMatrix(Lerp(size0, size1, t)) * M4x4::RotatingMatrix(Lerp(rotate0, rotate1, t)) * M4x4::TranslatingMatrix(Lerp(position0, position1, t)) * worldTransform;

		for (int i = 0; i < currentNode->childrenNum; i++) {
			LoadNodeWorldTransforms(&currentNode->children[i], nodeWorldTransforms[currentNode], nodeWorldTransforms, animationSet0, animationSet1, t);
		}
	}

	MODEL_NODE* FindNodeByName(MODEL_NODE* root, const std::string& name)
	{
		if (!name.compare(root->name)) {
			return root;
		}
		for (int i = 0; i < root->childrenNum; i++) {
			MODEL_NODE* result = FindNodeByName(&root->children[i], name);
			if (result) {
				return result;
			}
		}
		return nullptr;
	}


	void SetRenderer(Renderer* renderer)
	{
		g_renderer = renderer;
	}

	Renderer* GetRenderer()
	{
		return g_renderer;
	}

	void SetDrawTool(DrawTool* drawTool)
	{
		g_drawTool = drawTool;
	}

	DrawTool* GetDrawTool()
	{
		return g_drawTool;
	}

	void SetAudioTool(AudioTool* audioTool)
	{
		g_audioTool = audioTool;
	}

	AudioTool* GetAudioTool()
	{
		return g_audioTool;
	}

	void SetResourceTool(ResourceTool* resourceTool)
	{
		g_resourceTool = resourceTool;
	}

	ResourceTool* GetResourceTool()
	{
		return g_resourceTool;
	}

	void SetScreen(float width, float height, float centerX, float centerY)
	{
		g_screen = {
			width,
			height,
			centerX,
			centerY
		};
	}

	F2 GetScreenSize()
	{
		return { g_screen.x, g_screen.y };
	}

	F2 GetScreenCenter()
	{
		return { g_screen.z, g_screen.w };
	}

	float GetScreenWidth()
	{
		return g_screen.x;
	}

	float GetScreenHeight()
	{
		return g_screen.y;
	}

	float GetScreenCenterX()
	{
		return g_screen.z;
	}

	float GetScreenCenterY()
	{
		return g_screen.w;
	}

	void SetDeltaTime(int deltaTime)
	{
		g_deltaTime = deltaTime;
	}

	int GetDeltaTime()
	{
		return g_deltaTime;
	}

	std::wstring StringToWString(const std::string& str) {
#ifdef _WINDOWS_
		// 変換後のバッファサイズを取得
		int sizeNeeded = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
		if (sizeNeeded == 0) {
			return L""; // エラー処理
		}

		// バッファを確保して変換
		std::wstring wstr(sizeNeeded, 0);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], sizeNeeded);

		// null 終端のための余分な文字を削除
		wstr.resize(sizeNeeded - 1);
		return wstr;
#endif
		return L"";
	}

	std::string FixedFloatString(float value, unsigned int digits) 
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(digits) << value;
		return oss.str();
	}

	void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
		size_t pos = 0;
		while ((pos = str.find(from, pos)) != std::wstring::npos) {
			str.replace(pos, from.length(), to);
			pos += to.length();
		}
	}

	Texture* LoadTexture(const std::string& path, const std::string& scope)
	{
		if (g_resourceTool) {
			return g_resourceTool->LoadTexture(path, scope);
		}
		return nullptr;
	}

	Texture* RenderText(const std::wstring& text, const FONT& font, const std::string& scope)
	{
		if (g_resourceTool) {
			return g_resourceTool->RenderText(text, font, scope);
		}
		return nullptr;
	}

	void ReleaseTexture(const std::string& path, const std::string& scope)
	{
		g_resourceTool->ReleaseTexture(path, scope);
	}

	void ReleaseTexture(const std::string& scope)
	{
		g_resourceTool->ReleaseTexture(scope);
	}

	Audio* LoadAudio(const std::string& path, const std::string& scope)
	{
		if (g_resourceTool) {
			return g_resourceTool->LoadAudio(path, scope);
		}
		return nullptr;
	}
	void ReleaseAudio(const std::string& path, const std::string& scope)
	{
		g_resourceTool->ReleaseAudio(path, scope);
	}

	void ReleaseAudio(const std::string& scope)
	{
		g_resourceTool->ReleaseAudio(scope);
	}

	Model* LoadModel(const std::string& path, const std::string& scope)
	{
		if (g_resourceTool) {
			return g_resourceTool->LoadModel(path, scope);
		}
		return nullptr;
	}

	void ReleaseModel(const std::string& path, const std::string& scope)
	{
		g_resourceTool->ReleaseModel(path, scope);
	}

	void ReleaseModel(const std::string& scope)
	{
		g_resourceTool->ReleaseModel(scope);
	}

	Animation* LoadAnimation(const std::string& path, const std::string& scope)
	{
		return g_resourceTool->LoadAnimation(path, scope);
	}

	void ReleaseAnimation(const std::string& path, const std::string& scope)
	{
		g_resourceTool->ReleaseAnimation(path, scope);
	}

	void ReleaseAnimation(const std::string& scope)
	{
		g_resourceTool->ReleaseAnimation(scope);
	}

	void ReleaseResource(Resource* resource, const std::string& scope)
	{
		g_resourceTool->ReleaseResource(resource, scope);
	}

	void ReleaseResource(const std::string& scope)
	{
		g_resourceTool->ReleaseResource(scope);
	}

	void DrawQuad(const Texture* texture, const F3& position, const F3& size, const Quaternion& rotate, const F4& color, const F2& uvOffset, const F2& uvRange
	)
	{
		if (g_drawTool) {
			g_drawTool->DrawQuad(texture, position, size, rotate, color, uvOffset, uvRange);
		}
	}

	void DrawCube(const Texture* texture, const F3& position, const F3& size, const Quaternion& rotate, const F4& color)
	{
		if (g_drawTool) {
			g_drawTool->DrawCube(texture, position, size, rotate, color);
		}
	}

	void DrawModel(const Model* model, const F3& position, const F3& size, const Quaternion& rotate, const F4& color, const F2& uvOffset, const F2& uvRange)
	{
		if (g_drawTool) {
			g_drawTool->DrawModel(model, position, size, rotate, color, uvOffset, uvRange);
		}
	}

	void DrawModel(const Model* model, const std::vector<ANIMATION_APPLICANT>& animationApplicants, const F3& position, const F3& size, const Quaternion& rotate, const F4& color)
	{
		if (g_drawTool) {
			g_drawTool->DrawModel(model, animationApplicants, position, size, rotate, color);
		}
	}

	void DrawModel(const Model* model, const std::vector<ANIMATION_APPLICANT>& animationApplicants0, const std::vector<ANIMATION_APPLICANT>& animationApplicants1, const float animTransitionT, const F3& position, const F3& size, const Quaternion& rotate, const F4& color)
	{
		if (g_drawTool) {
			g_drawTool->DrawModel(model, animationApplicants0, animationApplicants1, animTransitionT, position, size, rotate, color);
		}
	}

	void DrawModel(const Model* model, const std::map<MODEL_NODE*, M4x4>& transforms, const F4& color)
	{
		if (g_drawTool) {
			g_drawTool->DrawModel(model, transforms, color);
		}
	}

	void DrawPolygon(const Texture* texture, const VERTEX* vertices, size_t length, TOPOLOGY topology, const F3& position, const F3& size, const Quaternion& rotate, const F4& color, const F2& uvOffset, const F2& uvRange)
	{
		if (g_drawTool) {
			g_drawTool->DrawPolygon(texture, vertices, length, topology, position, size, rotate, color, uvOffset, uvRange);
		}
	}

}