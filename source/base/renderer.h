// =======================================================
// renderer.h
// 
// レンダラー、抽象クラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _RENDERER_H
#define _RENDERER_H

#include "MGCommon.h"
#include "camera.h"

namespace MG {

	enum BLEND_STATE
	{
		BLEND_STATE_NONE = 0,	// ブレンドしない
		BLEND_STATE_ALPHA,		// 普通のαブレンド
		BLEND_STATE_ADD,			// 加算合成
		BLEND_STATE_SUB,			// 減算合成
		BLEND_STATE_DEST_OUT,	// くりぬき
		BLEND_STATE_MAX
	};

	enum DEPTH_STATE
	{
		DEPTH_STATE_ENABLE = 0,			//  WRITE, DRAW LESS
		DEPTH_STATE_DRAW_ALL,			//  WRITE, DRAW ALL
		DEPTH_STATE_NO_WRITE,			//  NO WRITE, DRAW LESS
		DEPTH_STATE_NO_WRITE_DRAW_ALL,	//  NO WRITE, DRAW ALL
		DEPTH_STATE_MAX
	};

	enum RASTERIZER_STATE
	{
		RASTERIZER_STATE_CULL_NONE = 0,
		RASTERIZER_STATE_CULL_BACK,
		RASTERIZER_STATE_WIREFRAME,
		RASTERIZER_STATE_MAX
	};

	class RenderTarget {
	public:
		RenderTarget(Texture* texture);
		Texture* const texture;
	};

	class Renderer {
	public:
		virtual void SetViewport(float x, float y, float width, float height) = 0;
		virtual void SetBlendState(BLEND_STATE state) = 0;
		virtual void SetDepthState(DEPTH_STATE state) = 0;
		virtual void SetRasterizerState(RASTERIZER_STATE state) = 0;
		virtual void SetWorld(const F3& size, const Quaternion& rotate, const F3& position) = 0;
		virtual void ApplyCamera(Camera* camera) = 0;
		virtual void UpdateWorldViewProjection() = 0;
		virtual void UpdateConstant() = 0;
		virtual void UpdateLight() = 0;
		virtual void SetAmbient(const F4& color) = 0;
		virtual void SetLightPosition(const F3& position) = 0;
		virtual void SetLightColor(const F4& color) = 0;
		virtual void SetLight(const F3& position, const F4& color) = 0;
		virtual void SetColor(const F4& color) = 0;
		virtual void SetUVOffset(const F2& uvOffset) = 0;
		virtual void SetUVRange(const F2& uvRange) = 0;
		virtual void SetBones(MESH_BONE* bones, size_t size) = 0;
		virtual void SetUseBone(bool enable) = 0;
		virtual RenderTarget* CreateRenderTarget(unsigned int width = GetScreenWidth(), unsigned int height = GetScreenHeight()) = 0;
		virtual void ReleaseRenderTarget(RenderTarget* renderTarget) = 0;
		virtual void SetRenderTarget(RenderTarget* renderTarget) = 0;
		virtual RenderTarget* GetSubRenderTarget() = 0;
		virtual void ClearRenderTargetView(RenderTarget* renderTarget = nullptr, bool clearDepth = true) = 0;
		
	};

} // namespace MG

#endif 