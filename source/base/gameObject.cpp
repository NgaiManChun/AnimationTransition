// =======================================================
// gameObject.cpp
// 
// ゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "gameObject.h"

namespace MG {
	
	// =======================================================
	// 基底ゲームオブジェクト
	// =======================================================
	GameObject::GameObject(const F3& size, const F3& position, const Quaternion& rotate, const F4& color)
		: size(size), position(position), rotate(rotate), color(color), scene(nullptr) {}
	GameObject::~GameObject() {
		for (const COLLISION_UNIT& collisionUnit : collisionUnits) {
			delete collisionUnit.local;
			delete collisionUnit.world;
		}
		collisionUnits.clear();
	}
	void GameObject::Update() {
		UpdateWorldCollisionUnits();
	}
	void GameObject::Draw(){}

	bool GameObject::Collision(CollisionUnit* other, bool trigger)
	{
		if (trigger) {
			std::list<COLLISION_PAIR> pairs;
			for (const COLLISION_UNIT& collisionUnit : collisionUnits) {
				if (collisionUnit.world->Overlap(other)) {
					pairs.push_back({ collisionUnit.world, other });
				}
			}
			if (!pairs.empty()) {
				OnCollision(nullptr, pairs);
				return true;
			}
		}
		else {
			for (const COLLISION_UNIT& collisionUnit : collisionUnits) {
				if (collisionUnit.world->Overlap(other)) return true;
			}
		}
		return false;
	}

	bool GameObject::Collision(GameObject* gameObject, bool trigger)
	{
		if (trigger) {
			std::list<COLLISION_PAIR> pairs0;
			std::list<COLLISION_PAIR> pairs1;
			for (const COLLISION_UNIT& a : collisionUnits) {
				for (const COLLISION_UNIT& b : gameObject->GetCollisionUnits()) {
					if (a.world->Overlap(b.world)) {
						pairs0.push_back({ a.world, b.world });
						pairs1.push_back({ b.world, a.world });
					}
				}
			}
			if (!pairs0.empty()) {
				OnCollision(gameObject, pairs0);
				gameObject->OnCollision(this, pairs1);
				return true;
			}
		}
		else {
			for (const COLLISION_UNIT& a : collisionUnits) {
				for (const COLLISION_UNIT& b : gameObject->GetCollisionUnits()) {
					if (a.world->Overlap(b.world)) {
						return true;
					}
				}
			}
		}
		
		return false;
	}

	void GameObject::OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs)
	{
	}

	void GameObject::AddCollisionUnits(const ARRANGEMENT* arrangement)
	{
		if (arrangement) {
			collisionUnits.reserve(arrangement->instanceNum);
			for (int i = 0; i < arrangement->instanceNum; i++) {
				MODEL_INSTANCE& instance = arrangement->instances[i];
				COLLISION_TYPE type;
				if (!strcmp(instance.instance, "collisionSphere")) {
					type = COLLISION_TYPE_SPHERE;
				}
				else if (!strcmp(instance.instance, "collisionBox")) {
					type = COLLISION_TYPE_BOX;
				}
				else if (!strcmp(instance.instance, "collisionAABB")) {
					type = COLLISION_TYPE_AABB;
				}
				else if (!strcmp(instance.instance, "collisionPoint")) {
					type = COLLISION_TYPE_POINT;
				}
				else {
					continue;
				}
				CollisionUnit* collisionUnit = CollisionUnit::Create(
					type,
					M4x4::ScalingMatrix(instance.scale) * M4x4::RotatingMatrix(instance.rotate) * M4x4::TranslatingMatrix(instance.position),
					instance.name
				);
				collisionUnits.push_back({
					collisionUnit,
					CollisionUnit::Create(collisionUnit),
					type
				});
			}
			collisionUnits.shrink_to_fit();
		}
	}
	void GameObject::AddCollisionUnit(COLLISION_TYPE type, const M4x4& transform, const std::string name)
	{
		CollisionUnit* collisionUnit = CollisionUnit::Create(type, transform, name);
		collisionUnits.push_back({
			collisionUnit,
			CollisionUnit::Create(collisionUnit),
			type
		});
	}

	void GameObject::ClearCollisionUnits()
	{
		for (COLLISION_UNIT& collisionUnit : collisionUnits) {
			delete collisionUnit.local;
			delete collisionUnit.world;
		}
		collisionUnits.clear();
		collisionUnits.shrink_to_fit();
	}

	void GameObject::UpdateWorldCollisionUnits()
	{

		M4x4 world = M4x4::ScalingMatrix(size) * M4x4::RotatingMatrix(rotate)* M4x4::TranslatingMatrix(position);
		for (COLLISION_UNIT& collisionUnit : collisionUnits) {
			delete collisionUnit.world;
			collisionUnit.world = CollisionUnit::Create(collisionUnit.local);
			collisionUnit.world->Transform(world);
		}

		//for (int i = 0; i < collisionUnits.size(); i++) {
		//	CollisionUnit* worldCollisionUnit = worldCollisionUnits[i];
		//	if (typeid(*worldCollisionUnit) == typeid(Box)) {
		//		*((Box*)worldCollisionUnit) = *((Box*)collisionUnits[i]);
		//	}
		//	else if (typeid(*worldCollisionUnit) == typeid(Sphere)) {
		//		*((Box*)worldCollisionUnit) = *((Box*)collisionUnits[i]);
		//	}
		//	else if (typeid(*worldCollisionUnit) == typeid(AABB)) {
		//		*((Box*)worldCollisionUnit) = *((Box*)collisionUnits[i]);
		//	}
		//	else if (typeid(*worldCollisionUnit) == typeid(Point)) {
		//		*((Box*)worldCollisionUnit) = *((Box*)collisionUnits[i]);
		//	}
		//	
		//	//*worldCollisionUnits[i] = *collisionUnits[i];
		//	worldCollisionUnits[i]->Transform(world);
		//}

		/*for (CollisionUnit* collisionUnit : collisionUnits) {
			*collisionUnit 
			worldCollisionUnits.push_back({ 
				collisionUnit.type, 
				collisionUnit.transform * world,
				collisionUnit.name
			});
		}*/
	}

	const std::vector<GameObject::COLLISION_UNIT>& GameObject::GetCollisionUnits()
	{
		return collisionUnits;
	}

} // namespace MG