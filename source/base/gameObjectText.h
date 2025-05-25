// =======================================================
// gameObjectText.h
// 
// 文字ゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/12/17
// =======================================================
#ifndef _GAME_OBJECT_TEXT_H
#define _GAME_OBJECT_TEXT_H

#include "gameObject.h"
namespace MG {

	enum TEXT_ORIGIN_HORIZONTAL {
		TEXT_ORIGIN_HORIZONTAL_LEFT,
		TEXT_ORIGIN_HORIZONTAL_CENTER,
		TEXT_ORIGIN_HORIZONTAL_RIGHT
	};

	enum TEXT_ORIGIN_VERTICAL {
		TEXT_ORIGIN_VERTICAL_TOP,
		TEXT_ORIGIN_VERTICAL_CENTER,
		TEXT_ORIGIN_VERTICAL_BOTTOM
	};

	enum TEXT_RESIZE {
		TEXT_RESIZE_KEEP_RATIO_BY_HEIGHT,
		TEXT_RESIZE_KEEP_RATIO_BY_WIDTH,
		TEXT_RESIZE_USE_RAW,
		TEXT_RESIZE_USE_SCALE
	};

	class GameObjectText : public GameObject {
	private:
		std::wstring value;
		FONT font = {};
		bool rendered = false;
		Texture* texture = nullptr;
	public:
		TEXT_RESIZE resize = TEXT_RESIZE_KEEP_RATIO_BY_HEIGHT;
		TEXT_ORIGIN_HORIZONTAL originHorizontal = TEXT_ORIGIN_HORIZONTAL_LEFT;
		TEXT_ORIGIN_VERTICAL originVertical = TEXT_ORIGIN_VERTICAL_TOP;

		GameObjectText() = default;
		GameObjectText(
			const std::wstring value,
			const FONT& font = {},
			const TEXT_RESIZE resize = TEXT_RESIZE_KEEP_RATIO_BY_HEIGHT,
			const TEXT_ORIGIN_HORIZONTAL originHorizontal = TEXT_ORIGIN_HORIZONTAL_LEFT,
			const TEXT_ORIGIN_VERTICAL originVertical = TEXT_ORIGIN_VERTICAL_TOP,
			const F3& size = { 1.0f, 1.0f, 1.0f },
			const F3& position = {},
			const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
		);

		std::wstring GetValue() const;
		void SetValue(const std::wstring& value);
		FONT GetFont() const;
		void SetFont(const FONT& font);

		void Draw() override;
		void Update() override;
	};

} // namespace MG

#endif
