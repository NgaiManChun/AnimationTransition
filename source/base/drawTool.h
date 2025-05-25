// =======================================================
// drawTool.h
// 
// 描画ツール、抽象クラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/06
// =======================================================
#ifndef _DRAW_TOOL_H
#define _DRAW_TOOL_H

#include "MGCommon.h"
#include "collision.h"

namespace MG {

	class DrawTool {
	public:
		virtual void DrawQuad(const Texture* texture, const F3& position, const F3& size,
			const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			const F2& uvOffset = { 0.0f, 0.0f },
			const F2& uvRange = { 1.0f, 1.0f }
		) = 0;
		virtual void DrawCube(const Texture* texture, const F3& position, const F3& size,
			const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }) = 0;

		virtual void DrawModel(const Model* model,
			const F3& position = { 0.0f, 0.0f, 0.0f },
			const F3& size = { 1.0f, 1.0f, 1.0f },
			const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			const F2& uvOffset = { 0.0f, 0.0f },
			const F2& uvRange = { 1.0f, 1.0f }
		) = 0;

		virtual void DrawModel(const Model* model,
			const std::vector<ANIMATION_APPLICANT>& animationApplicants,
			const F3& position = { 0.0f, 0.0f, 0.0f },
			const F3& size = { 1.0f, 1.0f, 1.0f },
			const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
		) = 0;

		virtual void DrawModel(const Model* model,
			const std::vector<ANIMATION_APPLICANT>& animationApplicants0,
			const std::vector<ANIMATION_APPLICANT>& animationApplicants1,
			const float animTransitionT,
			const F3& position = { 0.0f, 0.0f, 0.0f },
			const F3& size = { 1.0f, 1.0f, 1.0f },
			const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
		) = 0;

		virtual void DrawModel(
			const Model* model, 
			const std::map<MODEL_NODE*, M4x4>& transforms, 
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
		) = 0;

		virtual void DrawPolygon(const Texture* texture, const VERTEX* vertices, size_t length,
			TOPOLOGY topology = TOPOLOGY_TRIANGLESTRIP,
			const F3& position = { 0.0f, 0.0f, 0.0f },
			const F3& size = { 1.0f, 1.0f, 1.0f },
			const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
			const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			const F2& uvOffset = { 0.0f, 0.0f },
			const F2& uvRange = { 1.0f, 1.0f }
		) = 0;

		virtual void DrawCube(const M4x4& matrix, const F4& color = { 1.0f, 1.0f, 1.0f, 1.0f }) = 0;
	};

} // namespace MG

#endif