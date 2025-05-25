// =======================================================
// gameObjectQuad.h
// 
// 板ゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/18
// =======================================================
#ifndef _GAME_OBJECT_QUAD_H
#define _GAME_OBJECT_QUAD_H

#include "gameObject.h"
namespace MG {

	class GameObjectQuad : public GameObject {
	public:
		Texture* texture;
		F2 uvOffset;
		F2 uvRange;

		GameObjectQuad(
			Texture* texture,
			F3 size = {}, F3 position = {},
			Quaternion rotate = Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f },
			F4 color = F4{ 1.0f, 1.0f, 1.0f, 1.0f },
			F2 uvOffset = F2{ 0.0f, 0.0f },
			F2 uvRange = F2{ 1.0f, 1.0f }
		);
		void Draw() override;
	};

} // namespace MG

#endif
