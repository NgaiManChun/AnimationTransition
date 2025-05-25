// =======================================================
// MGDataType.h
// 
// ã§í ÉfÅ[É^ç\ë¢ëÃíËã`Ç»Ç«
// 
// çÏé“ÅFÈ∞ï∂èrÅiÉKÉCÅ@É}ÉìÉ`ÉÖÉìÅjÅ@2024/11/09
// =======================================================
#ifndef _MG_DATA_TYPE_H
#define _MG_DATA_TYPE_H

#include "MGObject.h"
#include <math.h>

namespace MG {

	constexpr const char* FONT_YU_GOTHIC = "Yu Gothic";							// ü‡ÉSÉVÉbÉN
	constexpr const char* FONT_YU_MINCHO = "Yu Mincho";							// ü‡ñæí©
	constexpr const char* FONT_MEIRYO = "Meiryo";								// ÉÅÉCÉäÉI
	constexpr const char* FONT_MS_GOTHIC = "MS Gothic";							// MS ÉSÉVÉbÉN
	constexpr const char* FONT_MS_MINCHO = "MS Mincho";							// MS ñæí©
	constexpr const char* FONT_HIRAGINO_KAKU_GOTHIC = "Hiragino Kaku Gothic";	// ÉqÉâÉMÉmäpÉSÉVÉbÉN
	constexpr const char* FONT_HIRAGINO_MINCHO = "Hiragino Mincho";				// ÉqÉâÉMÉmñæí©
	constexpr const char* FONT_NOTO_SANS_CJK_JP = "Noto Sans CJK JP";			// Noto Sans CJK
	constexpr const char* FONT_NOTO_SERIF_CJK_JP = "Noto Serif CJK JP";			// Noto Serif CJK
	constexpr const char* FONT_SOURCE_HAN_SANS = "Source Han Sans";				// åπÉmäpÉSÉVÉbÉN

	static const float EPSILON = 1e-6f;

	class F2;
	class F3;
	class F4;
	class Quaternion;

	class F2 {
	public:
		float x;
		float y;

		F2 operator+(const F2& v) const;
		F2 operator-(const F2& v) const;
		F2 operator*(float v) const;
	};

	class F3 {
	public:
		float x;
		float y;
		float z;

		F3 operator+(const F3& v) const;
		F3 operator-(const F3& v) const;
		F3 operator*(const F3& v) const;
		F3 operator/(const F3& v) const;
		F3 operator*(float v) const;
		F3 operator/(float v) const;
		void operator+=(const F3& v);
		void operator-=(const F3& v);
		void operator*=(const F3& v);
		void operator/=(const F3& v);
		void operator*=(const float& v);
		void operator/=(const float& v);
		operator F2() const;
	};

	class F4 {
	public:
		float x;
		float y;
		float z;
		float w;

		F4 operator-(float v) const;
		F4 operator*(float v) const;
		F4 operator/(float v) const;
		void operator+=(const F4& v);
		void operator-=(const F4& v);
		operator F2() const;
		operator F3() const;
		operator Quaternion() const;
	};

	class Quaternion {
	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
		Quaternion(const F3& forward, const F3& upper = { 0.0f, 1.0f, 0.0f });
		Quaternion operator*(const Quaternion& q) const;
		void operator*=(const Quaternion& q);
		void Normalize();
		Quaternion Inverse() const;
		float AngleDifference(const Quaternion& q);
		F3 GetPitchYawRoll() const;

		static Quaternion Identity();
		static Quaternion AxisRadian(const F3& axis, float radian);
		static Quaternion AxisXRadian(float radian);
		static Quaternion AxisYRadian(float radian);
		static Quaternion AxisZRadian(float radian);
		static Quaternion AxisDegree(const F3& axis, float degree);
		static Quaternion AxisXDegree(float degree);
		static Quaternion AxisYDegree(float degree);
		static Quaternion AxisZDegree(float degree);
		static Quaternion PitchYawRoll(const F3& pyr);
	};

	struct M4x4 {
		float	_v00, _v01, _v02, _v03,
				_v10, _v11, _v12, _v13,
				_v20, _v21, _v22, _v23,
				_v30, _v31, _v32, _v33;

		static M4x4 ScalingMatrix(const F3& v) {
			return {
				v.x,	0.0f,	0.0f,	0.0f,
				0.0f,	v.y,	0.0f,	0.0f,
				0.0f,	0.0f,	v.z,	0.0f,
				0.0f,	0.0f,	0.0f,	1.0f
			};
		}

		static M4x4 RotatingMatrix(const Quaternion& q) {
			return {
				1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z,
				2.0f * (q.x * q.y - q.z * q.w),
				2.0f * (q.x * q.z + q.y * q.w),
				0.0f,

				2.0f * (q.x * q.y + q.z * q.w),
				1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z,
				2.0f * (q.y * q.z - q.x * q.w),
				0.0f,

				2.0f * (q.x * q.z - q.y * q.w),
				2.0f * (q.y * q.z + q.x * q.w),
				1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y,
				0.0f,

				0.0f,
				0.0f,
				0.0f,
				1.0f
			};
		}

		static M4x4 RotatingMatrix(const F4& q) {
			return {
				1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z,
				2.0f * (q.x * q.y - q.z * q.w),
				2.0f * (q.x * q.z + q.y * q.w),
				0.0f,

				2.0f * (q.x * q.y + q.z * q.w),
				1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z,
				2.0f * (q.y * q.z - q.x * q.w),
				0.0f,

				2.0f * (q.x * q.z - q.y * q.w),
				2.0f * (q.y * q.z + q.x * q.w),
				1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y,
				0.0f,

				0.0f,
				0.0f,
				0.0f,
				1.0f
			};
		}

		static M4x4 TranslatingMatrix(const F3& v) {
			return {
				1.0f, 0.0f, 0.0f, v.x,
				0.0f, 1.0f, 0.0f, v.y,
				0.0f, 0.0f, 1.0f, v.z,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		M4x4 operator *(const M4x4& m) const {
			return {
				m._v00 * _v00 + m._v01 * _v10 + m._v02 * _v20 + m._v03 * _v30,    m._v00 * _v01 + m._v01 * _v11 + m._v02 * _v21 + m._v03 * _v31,    m._v00 * _v02 + m._v01 * _v12 + m._v02 * _v22 + m._v03 * _v32,    m._v00 * _v03 + m._v01 * _v13 + m._v02 * _v23 + m._v03 * _v33,
				m._v10 * _v00 + m._v11 * _v10 + m._v12 * _v20 + m._v13 * _v30,    m._v10 * _v01 + m._v11 * _v11 + m._v12 * _v21 + m._v13 * _v31,    m._v10 * _v02 + m._v11 * _v12 + m._v12 * _v22 + m._v13 * _v32,    m._v10 * _v03 + m._v11 * _v13 + m._v12 * _v23 + m._v13 * _v33,
				m._v20 * _v00 + m._v21 * _v10 + m._v22 * _v20 + m._v23 * _v30,    m._v20 * _v01 + m._v21 * _v11 + m._v22 * _v21 + m._v23 * _v31,    m._v20 * _v02 + m._v21 * _v12 + m._v22 * _v22 + m._v23 * _v32,    m._v20 * _v03 + m._v21 * _v13 + m._v22 * _v23 + m._v23 * _v33,
				m._v30 * _v00 + m._v31 * _v10 + m._v32 * _v20 + m._v33 * _v30,    m._v30 * _v01 + m._v31 * _v11 + m._v32 * _v21 + m._v33 * _v31,    m._v30 * _v02 + m._v31 * _v12 + m._v32 * _v22 + m._v33 * _v32,    m._v30 * _v03 + m._v31 * _v13 + m._v32 * _v23 + m._v33 * _v33,
			};
		}

		void operator *=(const M4x4& m) {
			_v00 = _v00 * m._v00 + _v01 * m._v10 + _v02 * m._v20 + _v03 * m._v30;
			_v01 = _v00 * m._v01 + _v01 * m._v11 + _v02 * m._v21 + _v03 * m._v31; 
			_v02 = _v00 * m._v02 + _v01 * m._v12 + _v02 * m._v22 + _v03 * m._v32;
			_v03 = _v00 * m._v03 + _v01 * m._v13 + _v02 * m._v23 + _v03 * m._v33;

			_v10 = _v10 * m._v00 + _v11 * m._v10 + _v12 * m._v20 + _v13 * m._v30;
			_v11 = _v10 * m._v01 + _v11 * m._v11 + _v12 * m._v21 + _v13 * m._v31;
			_v12 = _v10 * m._v02 + _v11 * m._v12 + _v12 * m._v22 + _v13 * m._v32;
			_v13 = _v10 * m._v03 + _v11 * m._v13 + _v12 * m._v23 + _v13 * m._v33;

			_v20 = _v20 * m._v00 + _v21 * m._v10 + _v22 * m._v20 + _v23 * m._v30;
			_v21 = _v20 * m._v01 + _v21 * m._v11 + _v22 * m._v21 + _v23 * m._v31;
			_v22 = _v20 * m._v02 + _v21 * m._v12 + _v22 * m._v22 + _v23 * m._v32;
			_v23 = _v20 * m._v03 + _v21 * m._v13 + _v22 * m._v23 + _v23 * m._v33;

			_v30 = _v30 * m._v00 + _v31 * m._v10 + _v32 * m._v20 + _v33 * m._v30;
			_v31 = _v30 * m._v01 + _v31 * m._v11 + _v32 * m._v21 + _v33 * m._v31;
			_v32 = _v30 * m._v02 + _v31 * m._v12 + _v32 * m._v22 + _v33 * m._v32;
			_v33 = _v30 * m._v03 + _v31 * m._v13 + _v32 * m._v23 + _v33 * m._v33;
		}
			
		F3 operator *(const F3& v) const {
			return {
				v.x * _v00 + v.y * _v01 + v.z * _v02 + _v03,
				v.x * _v10 + v.y * _v11 + v.z * _v12 + _v13,
				v.x * _v20 + v.y * _v21 + v.z * _v22 + _v23
			};
		}

		F3 TransformNormal(const F3& v) const {
			return {
				v.x * _v00 + v.y * _v01 + v.z * _v02,
				v.x * _v10 + v.y * _v11 + v.z * _v12,
				v.x * _v20 + v.y * _v21 + v.z * _v22
			};
		}

		M4x4 Inverse() const {

			// çsóÒéÆó]àˆéqï™â
			float det22_23 = _v22 * _v33 - _v23 * _v32;
			float det21_33 = _v21 * _v33 - _v23 * _v31;
			float det21_32 = _v21 * _v32 - _v22 * _v31;
			float det20_33 = _v20 * _v33 - _v23 * _v30;
			float det20_32 = _v20 * _v32 - _v22 * _v30;
			float det20_31 = _v20 * _v31 - _v21 * _v30;

			float detM00 = _v11 * det22_23 - _v12 * det21_33 + _v13 * det21_32;
			float detM01 = _v10 * det22_23 - _v12 * det20_33 + _v13 * det20_32;
			float detM02 = _v10 * det21_33 - _v11 * det20_33 + _v13 * det20_31;
			float detM03 = _v10 * det21_32 - _v11 * det20_32 + _v12 * det20_31;

			float determinant = _v00 * detM00 - _v01 * detM01 + _v02 * detM02 - _v03 * detM03;

			if (determinant == 0.0f) {
				return *this;
			}

			float det12_33 = _v12 * _v33 - _v13 * _v32;
			float det11_33 = _v11 * _v33 - _v13 * _v31;
			float det11_32 = _v11 * _v32 - _v12 * _v31;
			float det10_33 = _v10 * _v33 - _v13 * _v30;
			float det10_32 = _v10 * _v32 - _v12 * _v30;
			float det10_31 = _v10 * _v31 - _v11 * _v30;
			float det12_23 = _v12 * _v23 - _v13 * _v22;
			float det11_23 = _v11 * _v23 - _v13 * _v21;
			float det11_22 = _v11 * _v22 - _v12 * _v21;
			float det10_23 = _v10 * _v23 - _v13 * _v20;
			float det10_22 = _v10 * _v22 - _v12 * _v20;
			float det10_21 = _v10 * _v21 - _v11 * _v20;

			float detM10 = _v01 * det22_23 - _v02 * det21_33 + _v03 * det21_32;
			float detM11 = _v00 * det22_23 - _v02 * det20_33 + _v03 * det20_32;
			float detM12 = _v00 * det21_33 - _v01 * det20_33 + _v03 * det20_31;
			float detM13 = _v00 * det21_32 - _v01 * det20_32 + _v02 * det20_31;
			float detM20 = _v01 * det12_33 - _v02 * det11_33 + _v03 * det11_32;
			float detM21 = _v00 * det12_33 - _v02 * det10_33 + _v03 * det10_32;
			float detM22 = _v00 * det11_33 - _v01 * det10_33 + _v03 * det10_31;
			float detM23 = _v00 * det11_32 - _v01 * det10_32 + _v02 * det10_31;
			float detM30 = _v01 * det12_23 - _v02 * det11_23 + _v03 * det11_22;
			float detM31 = _v00 * det12_23 - _v02 * det10_23 + _v03 * det10_22;
			float detM32 = _v00 * det11_23 - _v01 * det10_23 + _v03 * det10_21;
			float detM33 = _v00 * det11_22 - _v01 * det10_22 + _v02 * det10_21;

			float invDet = 1.0f / determinant;

			return {
				 detM00 * invDet, -detM10 * invDet,  detM20 * invDet, -detM30 * invDet,
				-detM01 * invDet,  detM11 * invDet, -detM21 * invDet,  detM31 * invDet,
				 detM02 * invDet, -detM12 * invDet,  detM22 * invDet, -detM32 * invDet,
				-detM03 * invDet,  detM13 * invDet, -detM23 * invDet,  detM33 * invDet
			};

		}
	};

	struct VERTEX {
		F3 position;
		F3 normal;
		F4 diffuse;
		F2 texCoord;
	};

	struct VERTEX_BONE_WEIGHT {
		unsigned int boneIndexes[4] = {};
		float weights[4] = {};
	};

	struct BONE {
		M4x4 transform = {};
		const char* name = nullptr;
	};

	enum PRIMITIVE_TYPE {
		PRIMITIVE_TYPE_POINT,
		PRIMITIVE_TYPE_LINE,
		PRIMITIVE_TYPE_TRIANGLE,
		PRIMITIVE_TYPE_NONE
	};

	struct MESH {
		PRIMITIVE_TYPE primitiveType;
		unsigned int vertexNum;
		unsigned int vertexIndexNum;
		unsigned int boneNum;

		VERTEX* vertices;
		unsigned int* vertexIndexes;
		BONE* bones;
		VERTEX_BONE_WEIGHT* boneWeights;

		const char* textureStr;
	};

	struct MODEL_NODE {
		unsigned int childrenNum;
		unsigned int meshNum;
		MODEL_NODE* children;
		unsigned int* meshIndexes;
		F3 scale;
		F3 position;
		F4 rotate;
		const char* name;
		const char* instance;
	};

	struct MESH_BONE {
		M4x4 offset;
		M4x4 world;
		MODEL_NODE* node;
	};

	struct TEXTURE {
		unsigned int width;
		unsigned int height;

		unsigned char* data;
		const char* textureStr;
	};

	struct MODEL {
		unsigned int meshNum;
		unsigned int textureNum;
		MODEL_NODE* rootNode;
		MESH* meshes;
		TEXTURE* textures;
	};

	struct VECTOR_KEY {
		float frame;
		F3 vector;
	};

	struct QUATERNION_KEY {
		float frame;
		Quaternion rotate;
	};

	struct ANIMATION_CHANNEL {
		unsigned int positionKeyNum;
		unsigned int scalingKeyNum;
		unsigned int rotationKeyNum;


		VECTOR_KEY* positionKeys;
		VECTOR_KEY* scalingKeys;
		QUATERNION_KEY* rotationKeys;
		const char* nodeName;
	};

	struct ANIMATION {
		float frameRate;
		float frames;
		unsigned int channelNum;
		ANIMATION_CHANNEL* channels;
		const char* name;
	};

	struct MODEL_INSTANCE {
		const char* name;
		const char* instance;
		F3 scale;
		F3 position;
		F4 rotate;
	};

	struct ARRANGEMENT {
		unsigned int instanceNum;
		MODEL_INSTANCE* instances;
	};

	enum TOPOLOGY {
		TOPOLOGY_TRIANGLESTRIP,
		TOPOLOGY_TRIANGLELIST,
		TOPOLOGY_LINESTRIP,
		TOPOLOGY_LINELIST
	};

	enum FONT_WEIGHT {
		FONT_WEIGHT_THIN = 100,
		FONT_WEIGHT_EXTRA_LIGHT = 200,
		FONT_WEIGHT_ULTRA_LIGHT = 200,
		FONT_WEIGHT_LIGHT = 300,
		FONT_WEIGHT_SEMI_LIGHT = 350,
		FONT_WEIGHT_NORMAL = 400,
		FONT_WEIGHT_REGULAR = 400,
		FONT_WEIGHT_MEDIUM = 500,
		FONT_WEIGHT_DEMI_BOLD = 600,
		FONT_WEIGHT_SEMI_BOLD = 600,
		FONT_WEIGHT_BOLD = 700,
		FONT_WEIGHT_EXTRA_BOLD = 800,
		FONT_WEIGHT_ULTRA_BOLD = 800,
		FONT_WEIGHT_BLACK = 900,
		FONT_WEIGHT_HEAVY = 900,
		FONT_WEIGHT_EXTRA_BLACK = 950,
		FONT_WEIGHT_ULTRA_BLACK = 950
	};

	enum FONT_STLYE {
		FONT_STYLE_NORMAL = 0,
		FONT_STYLE_OBLIQUE,
		FONT_STYLE_ITALIC
	};

	struct FONT {
		float size = 32.0f;
		const char* fontName = FONT_YU_GOTHIC;
		FONT_WEIGHT weight = FONT_WEIGHT_NORMAL;
		FONT_STLYE style = FONT_STYLE_NORMAL;
	};

	F3 Max(const F3& a, const F3& b);
	F3 Min(const F3& a, const F3& b);

	MODEL* GetModelByMGObject(const MGObject& mgo);

	ANIMATION* GetAnimationByMGObject(const MGObject& mgo);

	ARRANGEMENT* GetArrangementByMGObject(const MGObject& mgo);

	class Animation;
	struct ANIMATION_APPLICANT {
		const Animation* animation = nullptr;
		const float currentFrame;
	};

} // namespace MG

#endif