// =======================================================
// flipTransitaion.cpp
// 
// フリップシーン遷移
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/29
// =======================================================
#include "sceneTransitaion.h"
#include "progress.h"
using namespace MG;

#define TEXTURE_WHITE "asset\\texture\\white.png"

namespace FlipTransitaion {
	// =======================================================
	// クラス定義
	// 
	// SceneTransitionクラスに継承
	// =======================================================
	class FlipTransitaion : public SceneTransition {
	private:
		Progress progress = Progress(1000.0f, false);
	public:
		void Update() override;
		void Draw() override;
	};

	// =======================================================
	// 遷移効果登録
	// =======================================================
	static TransitionName name = REGISTER_TRANSITION("flip", FlipTransitaion);


	// =======================================================
	// 更新
	// 
	// 各シーンのUpdateが呼び出される前にこっちの処理を行う
	// 遷移処理が終わったら、inTransitionをFALSEにしてください
	// このオブジェクトは自動的に消滅される
	// =======================================================
	void FlipTransitaion::Update()
	{
		inTransition = true;
		if (progress == 0.0f) {
			// 目的シーンを開始する
			StartScene(m_dest);
		}
		else if (progress == 1.0f) {
			// 遷移元のシーンを終了する
			EndScene(m_src);
			inTransition = false;
		}

		// 時間経過処理
		progress.IncreaseValue(GetDeltaTime());
	}


	// =======================================================
	// 描画
	// =======================================================
	void FlipTransitaion::Draw()
	{
		// コピーを取る、リスト変動防止
		std::list<Scene*> runningScenes(*m_runningScenes);

		// スクリーンの中心座標を取得
		F2 screenCenter = GetScreenCenter();

		// スクリーンのサイズを取得
		F2 screenSize = GetScreenSize();

		float position_x = (screenCenter.x - screenSize.x)* (1 - progress) +
			(screenCenter.x + screenSize.x) * progress;

		

		// この時、稼働中のシーンは二つ（遷移先と遷移元）あるのでループ
		for (auto itr = runningScenes.begin(); itr != runningScenes.end(); itr++) {
			Scene* scene = *itr;

			if (scene->sceneName == m_dest) {
				// 遷移先
				if (progress > 0.5f) {
					scene->Draw();
				}
				
			}
			else if (scene->sceneName == m_src) {
				// 遷移元
				if (progress < 0.5f) {
					scene->Draw();
				}
			}
		}

		DrawQuad(
			LoadTexture(TEXTURE_WHITE, name),
			{ position_x, screenCenter.y, 0 },
			{ screenSize.x, screenSize.y, 0 },
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);
	}

} // namespace FlipTransitaion