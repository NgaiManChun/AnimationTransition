#include "MGDataType.h"
// =======================================================
// MGDataType.cpp
// 
// 共通データ構造体定義など
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/09
// =======================================================
#include "MGDataType.h"
#include "MGCommon.h"
#include <string>
#include <fstream>
#include <list>

namespace MG {

	F2 F2::operator+(const F2& v) const {
		return {
			x + v.x,
			y + v.y
		};
	}
	F2 F2::operator-(const F2& v) const {
		return {
			x - v.x,
			y - v.y
		};
	}

	F2 F2::operator*(float v) const {
		return {
			x * v,
			y * v
		};
	}

	F3 F3::operator+(const F3& v) const {
		return {
			x + v.x,
			y + v.y,
			z + v.z
		};
	}
	F3 F3::operator-(const F3& v) const {
		return {
			x - v.x,
			y - v.y,
			z - v.z
		};
	}
	F3 F3::operator*(const F3& v) const {
		return {
			x * v.x,
			y * v.y,
			z * v.z
		};
	}
	F3 F3::operator/(const F3& v) const {
		return {
			x / v.x,
			y / v.y,
			z / v.z
		};
	}
	F3 F3::operator*(float v) const {
		return {
			x * v,
			y * v,
			z * v
		};
	}
	F3 F3::operator/(float v) const {
		return {
			x / v,
			y / v,
			z / v
		};
	}
	void F3::operator+=(const F3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}
	void F3::operator-=(const F3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	void F3::operator*=(const F3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}
	void F3::operator/=(const F3& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}
	void F3::operator*=(const float& v) {
		x *= v;
		y *= v;
		z *= v;
	}
	void F3::operator/=(const float& v) {
		x /= v;
		y /= v;
		z /= v;
	}

	F3::operator F2() const {
		return {
			x, y
		};
	}

	F4 F4::operator-(float v) const {
		return {
			x - v,
			y - v,
			z - v,
			w - v
		};
	}

	F4 F4::operator*(float v) const {
		return {
			x * v,
			y * v,
			z * v,
			w * v
		};
	}

	F4 F4::operator/(float v) const {
		return {
			x / v,
			y / v,
			z / v,
			w / v
		};
	}

	void F4::operator+=(const F4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

	void F4::operator-=(const F4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
	}

	F4::operator F2() const {
		return { x, y };
	}

	F4::operator F3() const {
		return { x, y, z };
	}

	F4::operator Quaternion() const {
		return { x, y, z, w };
	}

	Quaternion::Quaternion(float x, float y, float z, float w): x(x), y(y), z(z), w(w)
	{
	}

	Quaternion::Quaternion(const F3& forward, const F3& upper)
	{
		F3 f = MG::Normalize(forward);
		F3 r = MG::Normalize(Cross(upper, f));
		F3 u = Cross(f, r);

		float trace = r.x + u.y + f.z;

		if (trace > 0) {
			float scalar = std::sqrt(trace + 1.0f) * 2.0f;
			x = -(f.y - u.z) / scalar;
			y = -(r.z - f.x) / scalar;
			z = -(u.x - r.y) / scalar;
			w = scalar * 0.25f;
		}
		else if ((r.x > u.y) && (r.x > f.z)) {
			float scalar = std::sqrt(1.0f + r.x - u.y - f.z) * 2.0f;
			x = -scalar * 0.25f;
			y = -(r.y + u.x) / scalar;
			z = -(r.z + f.x) / scalar;
			w = (f.y - u.z) / scalar;
		}
		else if (u.y > f.z) {
			float scalar = std::sqrt(1.0f + u.y - r.x - f.z) * 2.0f;
			x = -(u.x + r.y) / scalar;
			y = -0.25f * scalar;
			z = -(u.z + f.y) / scalar;
			w = (r.z - f.x) / scalar;
		}
		else {
			float scalar = std::sqrt(1.0f + f.z - r.x - u.y) * 2.0f;
			x = -(f.x + r.z) / scalar;
			y = -(f.y + u.z) / scalar;
			z = -0.25f * scalar;
			w = (u.x - r.y) / scalar;
		}
	}

	Quaternion Quaternion::operator*(const Quaternion& q) const {
		// 左から適用
		return {
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w,
			w * q.w - x * q.x - y * q.y - z * q.z
		};
	}

	void Quaternion::operator*=(const Quaternion& q) {
		*this = (*this) * q;
	}

	void Quaternion::Normalize() {
		float length = 1.0f / sqrtf(w * w + x * x + y * y + z * z);
		x *= length;
		y *= length;
		z *= length;
		w *= length;
	}

	Quaternion Quaternion::Inverse() const {
		return { -x, -y, -z, w };
	}

	float Quaternion::AngleDifference(const Quaternion& q) {
		// q_rel = q2 * q1^(-1)
		Quaternion conjugate = { -x, -y, -z, w };
		Quaternion q_rel = q * conjugate;

		// 角度差を計算 (2 * acos(|q_rel.w|))
		float angle = 2.0f * acosf(fabsf(q_rel.w));
		return angle;
	}

	F3 Quaternion::GetPitchYawRoll() const {

		float R00 = 2.0f * w * w + x * x - 1.0f; 
		float R01 = 2.0f * x * y - 2.0f * z * w; 
		float R02 = 2.0f * x * z + 2.0f * y * w;
		float R10 = 2.0f * x * y + 2.0f * z * w;
		float R11 = 2.0f * w * w + 2.0f * y * y - 1.0f;
		float R12 = 2.0f * y * z - 2.0f * x * w;
		float R20 = 2.0f * x * z - 2.0f * y * w;
		float R21 = 2.0f * y * z + 2.0f * x * w;
		float R22 = 2.0f * w * w + 2.0f * z * z - 1.0f;

		float pitch = std::asinf(-R12);
		float yaw;
		float roll;
		if (std::abs(pitch) > 0.9999f) {
			yaw = atanf(R02 / R22);
			roll = atanf(R10 / R11);
		}
		else {
			yaw = atanf(-(R20 / R00));
			roll = 0.0f;
		}
		return { pitch, yaw, roll };
	}

	Quaternion Quaternion::Identity() {
		static Quaternion identity{ 0.0f, 0.0f, 0.0f, 1.0f };
		return identity;
	}

	Quaternion Quaternion::AxisRadian(const F3& axis, float radian) {
		float v = sinf(radian * 0.5f);
		return {
			axis.x * v,
			axis.y * v,
			axis.z * v,
			cosf(radian * 0.5f)
		};
	}

	Quaternion Quaternion::AxisXRadian(float radian) {
		return AxisRadian({ 1.0f, 0.0f, 0.0f }, radian);  // 左手系：反転
	}

	Quaternion Quaternion::AxisYRadian(float radian) {
		return AxisRadian({ 0.0f, 1.0f, 0.0f }, radian);  // 左手系：反転
	}

	Quaternion Quaternion::AxisZRadian(float radian) {
		return AxisRadian({ 0.0f, 0.0f, 1.0f }, radian);  // 左手系：反転
	}

	Quaternion Quaternion::AxisDegree(const F3& axis, float degree) {
		return AxisRadian(axis, degree * PI / 180.0f);
	}

	Quaternion Quaternion::AxisXDegree(float degree) {
		return AxisXRadian(degree * PI / 180.0f);
	}

	Quaternion Quaternion::AxisYDegree(float degree) {
		return AxisYRadian(degree * PI / 180.0f);
	}

	Quaternion Quaternion::AxisZDegree(float degree) {
		return AxisZRadian(degree * PI / 180.0f);
	}

	Quaternion Quaternion::PitchYawRoll(const F3& pyr) {
		F3 PYR = pyr;
		while (PYR.x > PI) PYR.x -= 2.0f * PI;
		while (PYR.x < -PI) PYR.x += 2.0f * PI;
		while (PYR.y > PI) PYR.y -= 2.0f * PI;
		while (PYR.y < -PI) PYR.y += 2.0f * PI;
		while (PYR.z > PI) PYR.z -= 2.0f * PI;
		while (PYR.z < -PI) PYR.z += 2.0f * PI;

		return MG::Normalize(
			Quaternion::AxisYRadian(PYR.y) *
			Quaternion::AxisXRadian(PYR.x) *
			Quaternion::AxisZRadian(PYR.z)
		);
	}

	

	F3 Max(const F3& a, const F3& b) {
		return {
			std::max(a.x, b.x),
			std::max(a.y, b.y),
			std::max(a.z, b.z)
		};
	}

	F3 Min(const F3& a, const F3& b) {
		return {
			std::min(a.x, b.x),
			std::min(a.y, b.y),
			std::min(a.z, b.z)
		};
	}

	void _GetModelNodeList(MODEL_NODE* node, std::list<MODEL_NODE*>& list) {
		for (unsigned int i = 0; i < node->childrenNum; i++) {
			list.push_back((node->children + i));
		}
		for (unsigned int i = 0; i < node->childrenNum; i++) {
			_GetModelNodeList((node->children + i), list);
		}
	}

	MODEL_NODE* _LoadModelNodeData(MODEL_NODE* currentNode, MODEL_NODE* nextNode) {
		if (currentNode->childrenNum) {
			currentNode->children = nextNode;
			nextNode += currentNode->childrenNum;
		}
		else {
			currentNode->children = nullptr;
		}
		for (int i = 0; i < currentNode->childrenNum; i++) {
			nextNode = _LoadModelNodeData((currentNode->children + i), nextNode);
		}
		return nextNode;
	}

	MODEL* GetModelByMGObject(const MGObject& mgo) {
		char* current = mgo.data;

		MODEL* model = (MODEL*)current;
		current += sizeof(MODEL);

		model->meshes = (MESH*)current;
		current += sizeof(MESH) * model->meshNum;

		for (int i = 0; i < model->meshNum; i++) {


			model->meshes[i].vertices = (VERTEX*)current;
			current += sizeof(VERTEX) * model->meshes[i].vertexNum;
			if (model->meshes[i].boneNum > 0) {
				model->meshes[i].boneWeights = (VERTEX_BONE_WEIGHT*)current;
				current += sizeof(VERTEX_BONE_WEIGHT) * model->meshes[i].vertexNum;
			}

			model->meshes[i].vertexIndexes = (unsigned int*)current;
			current += sizeof(unsigned int) * model->meshes[i].vertexIndexNum;

			model->meshes[i].textureStr = (const char*)current;
			current += std::strlen(current) + 1;

			if (model->meshes[i].boneNum > 0) {
				model->meshes[i].bones = (BONE*)current;
				current += sizeof(BONE) * model->meshes[i].boneNum;
				for (int b = 0; b < model->meshes[i].boneNum; b++) {
					model->meshes[i].bones[b].name = (const char*)current;
					current += std::strlen(current) + 1;
				}
			}
			
		}

		model->textures = (TEXTURE*)current;
		current += sizeof(TEXTURE) * model->textureNum;

		for (int i = 0; i < model->textureNum; i++) {
			model->textures[i].data = (unsigned char*)current;
			current += (model->textures[i].height)?sizeof(unsigned char) * model->textures[i].width * model->textures[i].height * 4 : model->textures[i].width;

			model->textures[i].textureStr = (const char*)current;
			current += std::strlen(current) + 1;
		}

		MODEL_NODE* rootNode = (MODEL_NODE*)current;
		_LoadModelNodeData(rootNode, (rootNode + 1));
		std::list<MODEL_NODE*> list;
		list.push_back(rootNode);
		_GetModelNodeList(rootNode, list);
		current += sizeof(MODEL_NODE) * list.size();
		for(auto itr = list.begin(); itr != list.end(); itr++){

			MODEL_NODE* node = *itr;
			
			if (node->meshNum) {
				node->meshIndexes = (unsigned int*)current;
				current += sizeof(unsigned int) * node->meshNum;
			}
			else {
				node->meshIndexes = nullptr;
			}

			node->name = (const char*)current;
			current += std::strlen(current) + 1;

			node->instance = (const char*)current;
			current += std::strlen(current) + 1;
		}
		model->rootNode = rootNode;
		return model;
	}

	ANIMATION* GetAnimationByMGObject(const MGObject& mgo) {
		char* current = mgo.data;

		ANIMATION* animation = (ANIMATION*)current;
		current += sizeof(ANIMATION);

		animation->name = current;
		current += strlen(animation->name) + 1;

		animation->channels = (ANIMATION_CHANNEL*)current;
		current += sizeof(ANIMATION_CHANNEL) * animation->channelNum;

		for (int i = 0; i < animation->channelNum; i++) {

			ANIMATION_CHANNEL& animationChannel = animation->channels[i];

			animationChannel.positionKeys = (VECTOR_KEY*)current;
			current += sizeof(VECTOR_KEY) * animationChannel.positionKeyNum;

			animationChannel.scalingKeys = (VECTOR_KEY*)current;
			current += sizeof(VECTOR_KEY) * animationChannel.scalingKeyNum;

			animationChannel.rotationKeys = (QUATERNION_KEY*)current;
			current += sizeof(QUATERNION_KEY) * animationChannel.rotationKeyNum;

			animationChannel.nodeName = current;
			current += strlen(animationChannel.nodeName) + 1;
		}
		return animation;
	}

	ARRANGEMENT* GetArrangementByMGObject(const MGObject& mgo)
	{
		char* current = mgo.data;
		ARRANGEMENT* arrangement = (ARRANGEMENT*)current;
		current += sizeof(ARRANGEMENT);

		arrangement->instances = (MODEL_INSTANCE*)current;
		current += sizeof(MODEL_INSTANCE) * arrangement->instanceNum;

		for (int i = 0; i < arrangement->instanceNum; i++) {
			arrangement->instances[i].name = current;
			current += strlen(arrangement->instances[i].name) + 1;

			arrangement->instances[i].instance = current;
			current += strlen(arrangement->instances[i].instance) + 1;
		}

		return arrangement;
	}
	
}

