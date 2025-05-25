// =======================================================
// collision.h
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/30
// =======================================================
#ifndef _COLLISION_H
#define _COLLISION_H

#include "MGDataType.h"

namespace MG {

	namespace Collision {

		class CollisionUnit;

		enum COLLISION_TYPE {
			COLLISION_TYPE_POINT,
			COLLISION_TYPE_AABB,
			COLLISION_TYPE_SPHERE,
			COLLISION_TYPE_BOX,
			COLLISION_TYPE_CAPSULE
		};

		struct COLLISION_UNIT {
			COLLISION_TYPE type;
			M4x4 transform;
			std::string name;
		};

		struct COLLISION_PAIR {
			const CollisionUnit* a;
			const CollisionUnit* b;
		};

		typedef CollisionUnit* (*DYNAMIC_COLLSION)();

		class Point;
		class AABB;
		class Sphere;
		class Box;
		class Capsule;
		class CollisionUnit {
		protected:
			std::string name;
			CollisionUnit(const std::string& name = "");
			CollisionUnit(const M4x4& transform = M4x4::TranslatingMatrix({}), const std::string& name = "");
		public:
			M4x4 transform = M4x4::TranslatingMatrix({});

			virtual void Translation(const F3& move) = 0;
			virtual void Scale(const F3& scale) = 0;
			virtual void Rotate(const Quaternion& rotate) = 0;
			virtual void Transform(const M4x4& matrix) = 0;
			virtual bool Overlap(const CollisionUnit* unit) const;
			virtual bool Overlap(const F3& p) const = 0;
			virtual bool Overlap(const Point& p) const = 0;
			virtual bool Overlap(const AABB& aabb) const = 0;
			virtual bool Overlap(const Sphere& sphere) const = 0;
			virtual bool Overlap(const Box& box) const = 0;
			virtual bool Overlap(const Capsule& capsule) const = 0;
			const std::string& GetName() const;

			static CollisionUnit* Create(COLLISION_TYPE type, const M4x4& transform, const std::string& name);
			static CollisionUnit* Create(CollisionUnit* collisionUnit);

		};

		class Point : public CollisionUnit {
		public:
			F3 position;
			Point(const F3& position = {}, const std::string& name = "");
			Point(const M4x4& transform, const std::string& name = "");
			void Translation(const F3& move) override;
			void Scale(const F3& scale) override;
			void Rotate(const Quaternion& rotate) override;
			void Transform(const M4x4& matrix) override;
			bool Overlap(const F3& p) const override;
			bool Overlap(const Point& p) const override;
			bool Overlap(const AABB& aabb) const override;
			bool Overlap(const Sphere& sphere) const override;
			bool Overlap(const Box& box) const override;
			bool Overlap(const Capsule& capsule) const override;

		};

		class AABB : public CollisionUnit {
		public:
			F3 center;
			F3 halfSize;
			AABB(const F3& center = {}, const F3& halfSize = {}, const std::string& name = "");
			AABB(const M4x4& transform, const std::string& name = "");
			void Translation(const F3& move) override;
			void Scale(const F3& scale) override;
			void Rotate(const Quaternion& rotate) override;
			void Transform(const M4x4& matrix) override;
			bool Overlap(const F3& p) const override;
			bool Overlap(const Point& p) const override;
			bool Overlap(const AABB& aabb) const override;
			bool Overlap(const Sphere& sphere) const override;
			bool Overlap(const Box& box) const override;
			bool Overlap(const Capsule& capsule) const override;
		};

		class Sphere : public CollisionUnit {
		public:
			//F3 center;
			//M4x4 transform = M4x4::TranslatingMatrix({});
			M4x4 invTransform = transform;
			Sphere(const F3& position = {}, const F3& scale = { 1.0f, 1.0f, 1.0f }, const Quaternion& rotate = Quaternion::Identity(), const std::string& name = "");
			Sphere(const M4x4& transform, const std::string& name = "");
			void Translation(const F3& move) override;
			void Scale(const F3& scale) override;
			void Rotate(const Quaternion& rotate) override;
			void Transform(const M4x4& matrix) override;
			bool Overlap(const F3& p) const override;
			bool Overlap(const Point& p) const override;
			bool Overlap(const AABB& aabb) const override;
			bool Overlap(const Sphere& sphere) const override;
			bool Overlap(const Box& box) const override;
			bool Overlap(const Capsule& capsule) const override;
		};

		struct FACE {
			F3* points[4];
			F3 normal;
		};

		class Box : public CollisionUnit {
		public:
			F3 center = {};
			F3 points[8] = {
				{  0.5f,	 0.5f,  0.5f }, // 右上奥	0
				{  0.5f,	 0.5f, -0.5f }, // 右上手前 1
				{  0.5f,	-0.5f,  0.5f }, // 右下奥	2 
				{  0.5f,	-0.5f, -0.5f }, // 右下手前 3
				{ -0.5f,	 0.5f,  0.5f }, // 左上奥	4
				{ -0.5f,	 0.5f, -0.5f }, // 左上手前 5
				{ -0.5f,	-0.5f,  0.5f }, // 左下奥	6
				{ -0.5f,	-0.5f, -0.5f }, // 左下手前 7
			};
			FACE faces[6] = {
				{ { points + 5, points + 1, points + 3, points + 7 }, {  0.0f,  0.0f, -1.0f } }, // 前 0
				{ { points + 1, points + 0, points + 2, points + 3 }, {  1.0f,  0.0f,  0.0f } }, // 右 1
				{ { points + 0, points + 4, points + 6, points + 2 }, {  0.0f,  0.0f,  1.0f } }, // 後 2
				{ { points + 4, points + 5, points + 7, points + 6 }, { -1.0f,  0.0f,  0.0f } }, // 左 3
				{ { points + 4, points + 0, points + 1, points + 5 }, {  0.0f,  1.0f,  0.0f } }, // 上 4
				{ { points + 3, points + 7, points + 6, points + 2 }, {  0.0f, -1.0f,  0.0f } }, // 下 5
			};
			FACE* relatedFaces[8][3] = {
				{ faces + 1, faces + 4, faces + 2 }, // 右上奥	
				{ faces + 1, faces + 4, faces + 0 }, // 右上手前
				{ faces + 1, faces + 5, faces + 2 }, // 右下奥	
				{ faces + 1, faces + 5, faces + 0 }, // 右下手前
				{ faces + 3, faces + 4, faces + 2 }, // 左上奥	
				{ faces + 3, faces + 4, faces + 0 }, // 左上手前
				{ faces + 3, faces + 5, faces + 2 }, // 左下奥	
				{ faces + 3, faces + 5, faces + 0 }, // 左下手前
			};
			//M4x4 transform = M4x4::TranslatingMatrix({});

			Box(const F3& position = {}, const F3& scale = { 1.0f, 1.0f, 1.0f }, const Quaternion& rotate = Quaternion::Identity(), const std::string& name = "");
			Box(const M4x4& transform, const std::string& name = "");
			Box(const Box& box);
			void Translation(const F3& move) override;
			void Scale(const F3& scale) override;
			void Rotate(const Quaternion& rotate) override;
			void Transform(const M4x4& matrix) override;
			bool Overlap(const F3& p) const override;
			bool Overlap(const Sphere& sphere) const override;
			bool Overlap(const Point& p) const override;
			bool Overlap(const AABB& aabb) const override;
			bool Overlap(const Box& box) const override;
			bool Overlap(const Capsule& capsule) const override;
			F3 GetNormal(const F3& position);
			void operator =(const Box& src);
		};

		class Capsule : public CollisionUnit {
		public:
			/*F3 center = {};
			float height;
			float radus;*/
			M4x4 invTransform = transform;
			Capsule(const F3& position = {}, const F3& scale = { 1.0f, 1.0f, 1.0f }, const Quaternion& rotate = Quaternion::Identity(), const std::string& name = "");
			Capsule(const M4x4& transform, const std::string& name = "");
			void Translation(const F3& move) override;
			void Scale(const F3& scale) override;
			void Rotate(const Quaternion& rotate) override;
			void Transform(const M4x4& matrix) override;
			bool Overlap(const F3& p) const override;
			bool Overlap(const Sphere& sphere) const override;
			bool Overlap(const Point& p) const override;
			bool Overlap(const AABB& aabb) const override;
			bool Overlap(const Box& box) const override;
			bool Overlap(const Capsule& capsule) const override;
		};


	} // namespace Collision
} // namespace MG

#endif