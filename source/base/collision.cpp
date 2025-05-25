// =======================================================
// collision.cpp
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/30
// =======================================================
#include "MGCommon.h"
#include "collision.h"
#include <typeinfo>

#define OVERLAP_AABB_POINT(aabb, p) \
abs(aabb.center.x - p.x) <= aabb.halfSize.x && \
abs(aabb.center.y - p.y) <= aabb.halfSize.y && \
abs(aabb.center.z - p.z) <= aabb.halfSize.z

#define OVERLAP_SPHERE_POINT(p0, p1, r) \
(p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y) + (p1.z - p0.z) * (p1.z - p0.z) <= r * r

namespace MG {
	namespace Collision {

		Point::Point(const F3& position, const std::string& name) : position(position), CollisionUnit(name)
		{
		}

		Point::Point(const M4x4& transform, const std::string& name) : CollisionUnit(transform, name)
		{
			position = transform * F3{};
		}

		void Point::Translation(const F3& move)
		{
			position += move;
		}

		void Point::Scale(const F3& scale)
		{
			position *= scale;
		}

		void Point::Rotate(const Quaternion& rotate)
		{
			position = MG::Rotate(position, rotate);
		}

		void Point::Transform(const M4x4& matrix)
		{
			position = matrix * position;
		}

		bool Point::Overlap(const F3& p1) const
		{
			const Point& p0 = *this;
			return
				p0.position.x == p1.x &&
				p0.position.y == p1.y &&
				p0.position.z == p1.z;
		}

		bool Point::Overlap(const Point& p1) const
		{
			const Point& p0 = *this;
			return
				p0.position.x == p1.position.x &&
				p0.position.y == p1.position.y &&
				p0.position.z == p1.position.z;
		}

		bool Point::Overlap(const AABB& aabb) const
		{
			const Point& p = *this;
			return OVERLAP_AABB_POINT(aabb, p.position);
		}

		bool Point::Overlap(const Sphere& sphere) const
		{
			return sphere.Overlap(position);
		}

		bool Point::Overlap(const Box& box) const
		{
			return box.Overlap(*this);
		}

		bool Point::Overlap(const Capsule& capsule) const
		{
			return capsule.Overlap(*this);
		}

		AABB::AABB(const F3& center, const F3& halfSize, const std::string& name) : center(center), halfSize(halfSize), CollisionUnit(name)
		{
		}

		AABB::AABB(const M4x4& transform, const std::string& name) : CollisionUnit(transform, name)
		{
			center = transform * F3{};
			halfSize = transform * F3{ 0.5f, 0.5f, 0.5f };
			halfSize = { abs(halfSize.x), abs(halfSize.y), abs(halfSize.z) };
		}

		void AABB::Translation(const F3& move)
		{
			center += move;
		}

		void AABB::Scale(const F3& scale)
		{
			center *= scale;
			halfSize *= scale;
			halfSize = { abs(halfSize.x), abs(halfSize.y), abs(halfSize.z) };
		}

		void AABB::Rotate(const Quaternion& rotate)
		{
			center = MG::Rotate(center, rotate);
			halfSize = MG::Rotate(halfSize, rotate);
			halfSize = { abs(halfSize.x), abs(halfSize.y), abs(halfSize.z) };
		}

		void AABB::Transform(const M4x4& matrix)
		{
			center = matrix * center;
			halfSize = matrix * halfSize;
			halfSize = { abs(halfSize.x), abs(halfSize.y), abs(halfSize.z) };
		}

		bool AABB::Overlap(const F3& p) const
		{
			const AABB& aabb = *this;
			return OVERLAP_AABB_POINT(aabb, p);
		}

		bool AABB::Overlap(const Point& p) const
		{
			const AABB& aabb = *this;
			return OVERLAP_AABB_POINT(aabb, p.position);
		}

		bool AABB::Overlap(const AABB& aabb1) const
		{
			const AABB& aabb0 = *this;
			return
				abs(aabb0.center.x - aabb1.center.x) <= aabb0.halfSize.x + aabb1.halfSize.x && \
				abs(aabb0.center.y - aabb1.center.y) <= aabb0.halfSize.y + aabb1.halfSize.y && \
				abs(aabb0.center.z - aabb1.center.z) <= aabb0.halfSize.z + aabb1.halfSize.z;
		}

		bool AABB::Overlap(const Sphere& sphere) const
		{
			return sphere.Overlap(*this);
		}

		bool AABB::Overlap(const Box& box) const
		{
			return false;
		}

		bool AABB::Overlap(const Capsule& capsule) const
		{
			return capsule.Overlap(*this);
		}

		Sphere::Sphere(const F3& position, const F3& scale, const Quaternion& rotate, const std::string& name) : CollisionUnit(name)
		{
			transform = M4x4::TranslatingMatrix(position) * M4x4::RotatingMatrix(rotate) * M4x4::ScalingMatrix(scale);
			invTransform = transform.Inverse();
		}

		Sphere::Sphere(const M4x4& transform, const std::string& name) : invTransform(transform.Inverse()), CollisionUnit(transform, name)
		{
		}

		void Sphere::Translation(const F3& move)
		{
			transform = transform * M4x4::TranslatingMatrix(move);
			invTransform = transform.Inverse();
		}

		void Sphere::Scale(const F3& scale)
		{
			transform = transform * M4x4::ScalingMatrix(scale);
			invTransform = transform.Inverse();
		}

		void Sphere::Rotate(const Quaternion& rotate)
		{
			transform = transform * M4x4::RotatingMatrix(rotate);
			invTransform = transform.Inverse();
		}

		void Sphere::Transform(const M4x4& matrix)
		{
			transform = transform * matrix;
			invTransform = transform.Inverse();
		}

		bool Sphere::Overlap(const F3& p) const
		{
			F3 pTrans = invTransform * p;
			return pTrans.x * pTrans.x + pTrans.y * pTrans.y + pTrans.z * pTrans.z <= 1;
		}

		bool Sphere::Overlap(const Point& p) const
		{
			F3 pTrans = invTransform * p.position;
			return pTrans.x * pTrans.x + pTrans.y * pTrans.y + pTrans.z * pTrans.z <= 1;
		}

		bool Sphere::Overlap(const AABB& aabb) const
		{
			F3 center = transform * F3{};
			const F3 p{
				std::max(aabb.center.x - aabb.halfSize.x, std::min(center.x, aabb.center.x + aabb.halfSize.x)),
				std::max(aabb.center.y - aabb.halfSize.y, std::min(center.y, aabb.center.y + aabb.halfSize.y)),
				std::max(aabb.center.z - aabb.halfSize.z, std::min(center.z, aabb.center.z + aabb.halfSize.z))
			};
			F3 pTrans = invTransform * p;
			return pTrans.x * pTrans.x + pTrans.y * pTrans.y + pTrans.z * pTrans.z <= 1;
		}

		bool Sphere::Overlap(const Sphere& sphere) const
		{

			Sphere other = sphere;
			other.Transform(invTransform);


			F3 p = other.transform * F3{};
			float d = p.x * p.x + p.y * p.y + p.z * p.z;
			if (d <= 1) return true;
			p = other.invTransform * F3{};
			d = p.x * p.x + p.y * p.y + p.z * p.z;
			if (d <= 1) return true;

			float offset = 0.5f;
			float offsetT = 0.5f;
			float offsetP = 0.5f;
			float nearestT = 0.0f;
			float nearestP = 0.0f;
			float nearestD = -1.0f;
			int count = 20;
			while (count) {
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						float theta = nearestT + i * offset * 2.0f * PI;
						float phi = nearestP + j * offset * 2.0f * PI;
						F3 p = other.transform * F3{
							sinf(phi) * cosf(theta),
							sinf(phi) * sinf(theta),
							cosf(phi)
						};
						float d = p.x * p.x + p.y * p.y + p.z * p.z;
						if (d <= 1) return true;
						if (d < nearestD || nearestD == -1.0f) {
							nearestD = d;
							nearestT = theta;
							nearestP = phi;
						}
					}
				}
				offset *= 0.75f;
				count--;
			}
			return false;
		}

		bool Sphere::Overlap(const Box& box) const
		{
			return box.Overlap(*this);
		}

		bool Sphere::Overlap(const Capsule& capsule) const
		{
			return capsule.Overlap(*this);
		}

		CollisionUnit::CollisionUnit(const std::string& name) : name(name)
		{
		}

		CollisionUnit::CollisionUnit(const M4x4& transform, const std::string& name) : transform(transform), name(name)
		{
		}

		bool CollisionUnit::Overlap(const CollisionUnit* unit) const
		{
			if (typeid(*unit) == typeid(Point)) {
				return Overlap(*((Point*)unit));
			}
			else if (typeid(*unit) == typeid(AABB)) {
				return Overlap(*((AABB*)unit));
			}
			else if (typeid(*unit) == typeid(Sphere)) {
				return Overlap(*((Sphere*)unit));
			}
			else if (typeid(*unit) == typeid(Box)) {
				return Overlap(*((Box*)unit));
			}
			else if (typeid(*unit) == typeid(Capsule)) {
				return Overlap(*((Capsule*)unit));
			}
			return false;
		}

		const std::string& CollisionUnit::GetName() const
		{
			return name;
		}

		CollisionUnit* CollisionUnit::Create(COLLISION_TYPE type, const M4x4& transform, const std::string& name) {
			if (type == COLLISION_TYPE_POINT) {
				return (CollisionUnit*) new Point(transform, name);
			}
			else if (type == COLLISION_TYPE_AABB) {
				return (CollisionUnit*) new AABB(transform, name);
			}
			else if (type == COLLISION_TYPE_SPHERE) {
				return (CollisionUnit*) new Sphere(transform, name);
			}
			else if (type == COLLISION_TYPE_BOX) {
				return (CollisionUnit*) new Box(transform, name);
			}
			else if (type == COLLISION_TYPE_CAPSULE) {
				return (CollisionUnit*) new Capsule(transform, name);
			}
			return (CollisionUnit*) new Box(transform, name);
		}

		CollisionUnit* CollisionUnit::Create(CollisionUnit* collisionUnit)
		{
			if (typeid(*collisionUnit) == typeid(Point)) {
				return (CollisionUnit*) new Point(*(Point*)collisionUnit);
			}
			else if (typeid(*collisionUnit) == typeid(AABB)) {
				return (CollisionUnit*) new AABB(*(AABB*)collisionUnit);
			}
			else if (typeid(*collisionUnit) == typeid(Sphere)) {
				return (CollisionUnit*) new Sphere(*(Sphere*)collisionUnit);
			}
			else if (typeid(*collisionUnit) == typeid(Box)) {
				return (CollisionUnit*) new Box(*(Box*)collisionUnit);
			}
			else if (typeid(*collisionUnit) == typeid(Capsule)) {
				return (CollisionUnit*) new Capsule(*(Capsule*)collisionUnit);
			}
			return nullptr;
		}

		Box::Box(const F3& position, const F3& scale, const Quaternion& rotate, const std::string& name) : CollisionUnit(name) {
			center = position;
			for (int i = 0; i < 8; i++) {
				points[i] = MG::Rotate(points[i] * scale, rotate) + position;
			}
			for (int i = 0; i < 6; i++) {
				faces[i].normal = MG::Rotate(faces[i].normal, rotate);
			}
		}

		Box::Box(const M4x4& transform, const std::string& name) : CollisionUnit(transform, name)
		{
			center = transform * center;
			for (int i = 0; i < 8; i++) {
				points[i] = transform * points[i];
			}
			for (int i = 0; i < 6; i++) {
				faces[i].normal = Normalize(transform.TransformNormal(faces[i].normal));
			}
		}

		Box::Box(const Box& src) : CollisionUnit(src.transform, src.name) {
			*this = src;
		}

		void Box::Translation(const F3& move) {
			transform = transform * M4x4::TranslatingMatrix(move);
			center += move;
			for (int i = 0; i < 8; i++) {
				points[i] += move;
			}
		}

		void Box::Scale(const F3& scale) {
			transform = transform * M4x4::ScalingMatrix(scale);
			center *= scale;
			for (int i = 0; i < 8; i++) {
				points[i] *= scale;
			}
			for (int i = 0; i < 6; i++) {
				faces[i].normal = Normalize(faces[i].normal * scale);
			}
		}

		void Box::Rotate(const Quaternion& rotate) {
			transform = transform * M4x4::RotatingMatrix(rotate);
			center = MG::Rotate(center, rotate);
			for (int i = 0; i < 8; i++) {
				points[i] = MG::Rotate(points[i], rotate);
			}
			for (int i = 0; i < 6; i++) {
				faces[i].normal = Normalize(MG::Rotate(faces[i].normal, rotate));
			}
		}

		void Box::Transform(const M4x4& matrix)
		{
			transform = transform * matrix;
			center = matrix * center;
			for (int i = 0; i < 8; i++) {
				points[i] = matrix * points[i];
			}
			for (int i = 0; i < 6; i++) {
				faces[i].normal = Normalize(matrix.TransformNormal(faces[i].normal));
			}
		}

		bool Box::Overlap(const F3& p) const {
			for (int i = 0; i < 6; i++) {
				if (Dot(faces[i].normal, *(faces[i].points[0]) - p) < 0) return false;
			}
			return true;
		}

		bool Box::Overlap(const Sphere& sphere) const {

			F3 min = center, max = center;
			for (int i = 0; i < 8; i++) {
				min = Min(min, points[i]);
				max = Max(max, points[i]);
			}
			AABB aabb(center, (max - min) * 0.5f);
			if (!sphere.Overlap(AABB(center, (max - min) * 0.5f))) return false;

			if (Overlap(sphere.transform * F3{})) return true;

			Box boxTrans(*this);
			boxTrans.Transform(sphere.invTransform);
			/*Box box;
			for (int i = 0; i < 6; i++) {
				const FACE& face = boxTrans.faces[i];
				const F3& N = face.normal;
				F3 p = face.normal * -1.0f;
				if (boxTrans.Overlap(p)) return true;
			}
			return false;*/
			{
				/*F3 n0 = boxTrans.points[5] - boxTrans.points[1];
				F3 v0 = F3{} - boxTrans.points[1];*/

				/*points + 5, points + 1, points + 3, points + 7
				points + 1, points + 0, points + 2, points + 3
				points + 0, points + 4, points + 6, points + 2
				points + 4, points + 5, points + 7, points + 6*/
			}


			for (int i = 0; i < 6; i++) {
				const FACE& face = boxTrans.faces[i];
				const F3& N = face.normal;
				const F3& A = *face.points[0];
				const F3& C = { 0.0f, 0.0f, 0.0f };
				float d = -(N.x * A.x + N.y * A.y + N.z * A.z);
				float D = N.x * C.x + N.y * C.x + N.z * C.z + d;
				if (fabsf(D) <= 1.0f) {
					F3 P = C - (N * D);
					if (boxTrans.Overlap(P)) return true;
					for (int p = 0; p < 4; p++) {
						F3 v0 = *face.points[p] - *face.points[(p + 1) % 4];
						F3 v1 = P - *face.points[(p + 1) % 4];
						float t = Dot(v0, v1) / pow(Distance({}, v0), 2);
						if (t < 0) {
							t = 0.0f;
						}
						else if (t > 1.0f) {
							t = 1.0f;
						}
						F3 Q = *face.points[(p + 1) % 4] + v0 * t;
						if (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z <= 1.0f) return true;
						//if (sphere.Overlap(Q)) return true;
					}
				}
			}

			return false;
		}

		bool Box::Overlap(const Point& p) const {
			return Overlap(p.position);
		}

		bool Box::Overlap(const AABB& aabb) const {
			return false;
		}

		bool Box::Overlap(const Box& box) const {
			const Box* boxes[2] = { this, &box };
			F3 max0 = center, max1 = box.center, min0 = center, min1 = box.center;
			for (int i = 0; i < 8; i++) {
				if (boxes[0]->Overlap(boxes[1]->points[i])) return true;
				if (boxes[1]->Overlap(boxes[0]->points[i])) return true;
				max0 = Max(max0, boxes[0]->points[i]);
				max1 = Max(max1, boxes[1]->points[i]);
				min0 = Min(min0, boxes[0]->points[i]);
				min1 = Min(min1, boxes[1]->points[i]);
			}

			// AABBを作って簡易的なチェック
			AABB aabb0(center, (max0 - min0) * 0.5f);
			AABB aabb1(center, (max1 - min1) * 0.5f);
			if (!aabb0.Overlap(aabb1)) return false;

			for (int b = 0; b < 2; b++) {
				for (int f = 0; f < 4; f++) {
					for (int p = 0; p < 3; p++) {
						const F3& A = *boxes[b]->faces[f].points[p];
						const F3& B = *boxes[b]->faces[f].points[p + 1];
						for (int i = 0; i < 6; i++) {
							const FACE& face = boxes[(b + 1) % 2]->faces[i];
							const F3& N = face.normal;
							const F3& C = *face.points[0];
							F3 vAB = B - A;
							float d = -Dot(N, C);
							float D = Dot(N, vAB);
							if (fabsf(D) < EPSILON) continue;
							float t = -(Dot(N, A) + d) / D;
							if (t < 0.0f || t > 1.0f) continue;
							F3 P = A + (vAB * t);
							if (boxes[(b + 1) % 2]->Overlap(P)) return true;
						}
					}
				}
			}
			return false;
		}
		bool Box::Overlap(const Capsule& capsule) const
		{
			return capsule.Overlap(*this);
		}
		F3 Box::GetNormal(const F3& position)
		{

			float maxDot = 0.0f;
			int index = 0;
			for (int i = 0; i < 6; i++) {
				F3 center = ((*faces[i].points[0]) + (*faces[i].points[1]) + (*faces[i].points[2]) + (*faces[i].points[3])) * 0.25f;
				F3 vector = position - center;
				float dot = Dot(faces[i].normal, vector);
				if (dot > maxDot) {
					maxDot = dot;
					index = i;
				}
			}
			return faces[index].normal;
		}
		void Box::operator=(const Box& src)
		{
			name = src.name;
			center = src.center;
			transform = src.transform;

			points[0] = src.points[0];
			points[1] = src.points[1];
			points[2] = src.points[2];
			points[3] = src.points[3];
			points[4] = src.points[4];
			points[5] = src.points[5];
			points[6] = src.points[6];
			points[7] = src.points[7];

			faces[0] = { { points + 5, points + 1, points + 3, points + 7 }, src.faces[0].normal }; // 前 0
			faces[1] = { { points + 1, points + 0, points + 2, points + 3 }, src.faces[1].normal }; // 右 1
			faces[2] = { { points + 0, points + 4, points + 6, points + 2 }, src.faces[2].normal }; // 後 2
			faces[3] = { { points + 4, points + 5, points + 7, points + 6 }, src.faces[3].normal }; // 左 3
			faces[4] = { { points + 4, points + 0, points + 1, points + 5 }, src.faces[4].normal }; // 上 4
			faces[5] = { { points + 3, points + 7, points + 6, points + 2 }, src.faces[5].normal }; // 下 5


			relatedFaces[0][0] = faces + 1;
			relatedFaces[1][0] = faces + 1;
			relatedFaces[2][0] = faces + 1;
			relatedFaces[3][0] = faces + 1;
			relatedFaces[4][0] = faces + 3;
			relatedFaces[5][0] = faces + 3;
			relatedFaces[6][0] = faces + 3;
			relatedFaces[7][0] = faces + 3;

			relatedFaces[0][1] = faces + 4;
			relatedFaces[1][1] = faces + 4;
			relatedFaces[2][1] = faces + 5;
			relatedFaces[3][1] = faces + 5;
			relatedFaces[4][1] = faces + 4;
			relatedFaces[5][1] = faces + 4;
			relatedFaces[6][1] = faces + 5;
			relatedFaces[7][1] = faces + 5;

			relatedFaces[0][2] = faces + 2;
			relatedFaces[1][2] = faces + 0;
			relatedFaces[2][2] = faces + 2;
			relatedFaces[3][2] = faces + 0;
			relatedFaces[4][2] = faces + 2;
			relatedFaces[5][2] = faces + 0;
			relatedFaces[6][2] = faces + 2;
			relatedFaces[7][2] = faces + 0;
		}


		Capsule::Capsule(const F3& position, const F3& scale, const Quaternion& rotate, const std::string& name) : CollisionUnit(name)
		{
			transform = M4x4::TranslatingMatrix(position) * M4x4::RotatingMatrix(rotate) * M4x4::ScalingMatrix(scale);
			invTransform = transform.Inverse();
		}
		Capsule::Capsule(const M4x4& transform, const std::string& name) : invTransform(transform.Inverse()), CollisionUnit(transform, name)
		{

		}
		void Capsule::Translation(const F3& move)
		{
			transform = transform * M4x4::TranslatingMatrix(move);
			invTransform = transform.Inverse();
		}

		void Capsule::Scale(const F3& scale)
		{
			transform = transform * M4x4::ScalingMatrix(scale);
			invTransform = transform.Inverse();
		}

		void Capsule::Rotate(const Quaternion& rotate)
		{
			transform = transform * M4x4::RotatingMatrix(rotate);
			invTransform = transform.Inverse();
		}

		void Capsule::Transform(const M4x4& matrix)
		{
			transform = transform * matrix;
			invTransform = transform.Inverse();
		}

		bool Capsule::Overlap(const F3& p) const {
			F3 A = F3{ 0.0f, 1.0f, 0.0f };
			F3 B = F3{ 0.0f, -1.0f, 0.0f };
			F3 C = invTransform * p;
			F3 AB = B - A;
			F3 AC = C - A;
			float t = (Dot(AC, AB) / Dot(AB, AB));
			t = std::max(0.0f, std::min(1.0f, t));
			Sphere PSphere;
			PSphere.Translation(A + AB * t);
			return PSphere.Overlap(C);
		}

		bool Capsule::Overlap(const Sphere& sphere) const {
			Sphere transSphere(sphere.transform * invTransform);
			F3 A = F3{ 0.0f, 1.0f, 0.0f };
			F3 B = F3{ 0.0f, -1.0f, 0.0f };
			F3 C = transSphere.transform * F3{};
			F3 AB = B - A;
			F3 AC = C - A;
			float t = (Dot(AC, AB) / Dot(AB, AB));
			t = std::max(0.0f, std::min(1.0f, t));
			Sphere PSphere;
			PSphere.Translation(A + AB * t);

			return PSphere.Overlap(transSphere);
		}
		bool Capsule::Overlap(const Point& p) const {
			return Overlap(p.position);
		}
		bool Capsule::Overlap(const AABB& aabb) const {
			return false;
		}
		bool Capsule::Overlap(const Box& box) const {
			Box transBox(box.transform * invTransform);
			F3 A = F3{ 0.0f, 1.0f, 0.0f };
			F3 B = F3{ 0.0f, -1.0f, 0.0f };
			F3 C = transBox.center;
			F3 AB = B - A;
			F3 AC = C - A;
			float t = (Dot(AC, AB) / Dot(AB, AB));
			t = std::max(0.0f, std::min(1.0f, t));
			Sphere PSphere;
			PSphere.Translation(A + AB * t);
			return PSphere.Overlap(transBox);
		}
		bool Capsule::Overlap(const Capsule& capsule) const
		{
			return false;
		}
	}
}




