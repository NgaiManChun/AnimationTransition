// =======================================================
// gameObject.h
// 
// ゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "MGCommon.h"
#include "collision.h"
#include "resourceTool.h"
#include <list>
#include <typeinfo>
using namespace MG::Collision;

namespace MG {

	

	// =======================================================
	// 基底ゲームオブジェクト
	// =======================================================
	class GameObject {
	public:
		struct COLLISION_UNIT {
			CollisionUnit* local;
			CollisionUnit* world;
			COLLISION_TYPE type;
		};
	private:
		std::vector<COLLISION_UNIT> collisionUnits;
	protected:
		void AddCollisionUnits(const ARRANGEMENT* arrangement);
		void AddCollisionUnit(COLLISION_TYPE type, const M4x4& transform = M4x4::TranslatingMatrix({}), const std::string name = "");
		void ClearCollisionUnits();
		
	public:
		bool enable = true;
		F3 size;
		F3 position;
		F3 velocity = {};
		Quaternion rotate;
		F4 color;
		Scene* scene;
		int layer = 0;
		GameObject(
			const F3& size = { 1.0f, 1.0f, 1.0f }, 
			const F3& position = {}, 
			const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f }, 
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
		);
		virtual ~GameObject();
		virtual void Update();
		virtual void Draw();
		virtual bool Collision(CollisionUnit* other, bool trigger = true);
		virtual bool Collision(GameObject* gameObject, bool trigger = true);
		virtual void OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs);
		virtual void UpdateWorldCollisionUnits();
		const std::vector<GameObject::COLLISION_UNIT>& GetCollisionUnits();
	};

	

} // namespace MG

#endif

