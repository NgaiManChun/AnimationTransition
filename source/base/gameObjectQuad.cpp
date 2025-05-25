// =======================================================
// gameObjectQuad.cpp
// 
// 板ゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/18
// =======================================================
#include "gameObjectQuad.h"

namespace MG {

	GameObjectQuad::GameObjectQuad(Texture* texture, F3 size, F3 position, Quaternion rotate, F4 color, F2 uvOffset, F2 uvRange)
		: GameObject(size, position, rotate, color), texture(texture), uvOffset(uvOffset), uvRange(uvRange)
	{
	}

	void GameObjectQuad::Draw()
	{
		DrawQuad(texture, position, size, rotate, color, uvOffset, uvRange);
	}

} // namespace MG