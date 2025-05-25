#include "gameObjectText.h"
#include "resourceTool.h"

namespace MG {

	static std::hash<void*> ptrToHash;

	GameObjectText::GameObjectText(
		const std::wstring value,
		const FONT& font,
		const TEXT_RESIZE resize,
		const TEXT_ORIGIN_HORIZONTAL originHorizontal,
		const TEXT_ORIGIN_VERTICAL originVertical,
		const F3& size,
		const F3& position,
		const Quaternion& rotate,
		const F4& color
	) : GameObject(size, position, rotate, color), value(value.c_str()), font(font), resize(resize), originHorizontal(originHorizontal), originVertical(originVertical)
	{

	}

	std::wstring GameObjectText::GetValue() const {
		return value.c_str();
	}
	void GameObjectText::SetValue(const std::wstring& value) {
		if (this->value != value) {
			this->value = value.c_str();
			rendered = false;
		}
	}
	FONT GameObjectText::GetFont() const {
		return font;
	}
	void GameObjectText::SetFont(const FONT& font) {
		this->font = font;
		rendered = false;
	}

	void GameObjectText::Draw() {
		
		if (texture) {
			F3 offset = {};
			if (originHorizontal == TEXT_ORIGIN_HORIZONTAL_LEFT) {
				offset.x = size.x * 0.5f;
			}
			else if (originHorizontal == TEXT_ORIGIN_HORIZONTAL_RIGHT) {
				offset.x = size.x * -0.5f;
			}
			if (originVertical == TEXT_ORIGIN_VERTICAL_TOP) {
				offset.y = size.y * -0.5f;
			}
			else if (originVertical == TEXT_ORIGIN_VERTICAL_BOTTOM) {
				offset.y = size.y * 0.5f;
			}
			offset = Rotate(offset, rotate);
			DrawQuad(texture, position + offset, size, rotate, color);
		}
		
	}

	void GameObjectText::Update()
	{
		if (!rendered) {
			std::string scope = "gameobjecttext:" + std::to_string(ptrToHash(this));
			if (texture) {
				ReleaseResource((Resource*)texture, scope);
				texture = nullptr;
			}
			texture = RenderText(value, font, scope);
			rendered = true;
		}
		if (texture) {
			if (resize == TEXT_RESIZE_KEEP_RATIO_BY_HEIGHT) {
				float ratio = (float)texture->GetWidth() / texture->GetHeight();
				size.x = size.y * ratio;
			}
			else if (resize == TEXT_RESIZE_KEEP_RATIO_BY_WIDTH) {
				float ratio = (float)texture->GetHeight() / texture->GetWidth();
				size.y = size.x * ratio;
			}
			else if (resize == TEXT_RESIZE_USE_RAW) {
				size.x = texture->GetWidth();
				size.y = texture->GetHeight();
			}
		}
	}

} // namespace MG