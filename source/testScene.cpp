#include "scene.h"
#include "progress.h"
#include "input.h"
#include "config.h"
#include <functional>
using namespace MG;

namespace TestScene {
	static constexpr const char* MODEL = "asset\\model\\kumacchi.mgm";
	static constexpr const char* WALK_ANIMATION = "asset\\model\\kumacchi_walk.mga";
	static constexpr const char* BLINK_ANIMATION = "asset\\model\\kumacchi_blink.mga";
	static constexpr const char* SWING_DOWN_ANIMATION = "asset\\model\\kumacchi_swing_down.mga";
	static constexpr const char* TRAIL_TEXTURE = "asset\\texture\\trail.png";
	static constexpr const char* PAD_MODEL = "asset\\model\\pad.mgm";

	// =======================================================
	// クラス定義
	// =======================================================
	class TestScene : public Scene {
	public:
		struct _CONFIG {
			float ROTATE_SPEED;
			int DUMMY;
		};
	private:
		Model* model;
		Animation* walkAnimation;
		Animation* blinkAnimation;
		Animation* swingDownAnimation;
		Progress walkTime{ 1000.0f, true };
		Progress blinkTime{ 1000.0f, true };
		Progress swingTime{ 700.0f, false };
		Progress animationTransitionTime{ 300.0f, false };
		Progress padProgress{ 4000.0f, true }; // 下敷きの回転係数
		Progress HSVT{ 1400.0f, true }; // 軌道エフェクトの変色係数
		 
		F3 kumaFront{ 0.0f, 0.0f, 1.0f };
		F3 kumaSize{ 0.1f, 0.1f, 0.1f };

		Model* padModel;
		F3 padPosition;
		float padRotate = 0.0f;
		bool swing = false;
		bool walk = false;
		Texture* trailTexture;

		std::vector<VERTEX> verticesPadTrail;
		std::vector<VERTEX> verticesOnHand;

		std::function<void()> updateFunc;
		std::function<void()> animTransFunc;
		std::map<MODEL_NODE*, M4x4> modelTransforms;
		std::map<MODEL_NODE*, M4x4> onHandTransforms;
	public:
		void Init() override;
		//void Uninit() override;
		void Update() override;
		void Draw() override;
		//LAYER_TYPE GetLayerType(int layer) override;

		void UpdateIdle();
		void UpdateWalk();
		void UpdateSwing();

		M4x4 GetWorldMartix();
		
	};


	// =======================================================
	// コンフィグ読み込み
	// =======================================================
	static const TestScene::_CONFIG CONFIG = LoadConfig<TestScene::_CONFIG>("asset\\config.csv", [](const D_KVTABLE& table) -> TestScene::_CONFIG {
		return {
			TABLE_FLOAT_VALUE(table, "ROTATE_SPEED", ((360.0f / 360.0f) * 2.0f * PI))
		};
	});


	// =======================================================
	// シーン登録
	// =======================================================
	static SceneName sceneName = REGISTER_SCENE("test", TestScene);

	// =======================================================
	// 初期化
	// =======================================================
	void TestScene::Init()
	{
		Scene::Init();

		// リソース読み込み
		model = LoadModel(MODEL);
		walkAnimation = LoadAnimation(WALK_ANIMATION);
		blinkAnimation = LoadAnimation(BLINK_ANIMATION);
		swingDownAnimation = LoadAnimation(SWING_DOWN_ANIMATION);
		padModel = LoadModel(PAD_MODEL);

		// カメラを斜めに設置
		currentCamera->SetPosition({ 0.3f, 0.0f, -1.0f });
		currentCamera->SetFront(Normalize(F3{} - currentCamera->GetPosition()));

		trailTexture = LoadTexture(TRAIL_TEXTURE);

		updateFunc = [this]() { UpdateIdle(); };

		animTransFunc = [this]() {
			LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms);
		};
	}


	// =======================================================
	// 終了処理
	// =======================================================
	/*void TestScene::Uninit()
	{
		Scene::Uninit();
	}*/


	// =======================================================
	// 更新
	// =======================================================
	void TestScene::Update()
	{
		if (updateFunc) {
			updateFunc();
		}
		if (animTransFunc) {
			animTransFunc();
		}
		

		// モデルからアイテムを握るポジションを探す
		MODEL_NODE* padNode = FindNodeByName(model->rawModel->rootNode, "padPos");
		if (padNode) {
			M4x4 world = modelTransforms.at(padNode); // 握ってるアイテムのWorld
			LoadNodeWorldTransforms(padModel->rawModel->rootNode, world, onHandTransforms);
		}
		

		// 手持ちアイテムの軌道エフェクト更新
		{
			F4 color = HSV2RGB(HSVT, 1.0f, 1.0f);

			if (verticesOnHand.size() >= 30 * 2) {
				verticesOnHand.erase(verticesOnHand.begin());
				verticesOnHand.erase(verticesOnHand.begin());
			}

			M4x4& transform = onHandTransforms.at(FindNodeByName(padModel->rawModel->rootNode, "pCube1"));
			verticesOnHand.push_back({ transform * F3{  0.0f, 0.0f,   0.5f }, Normalize(transform * F3{ 0.0f , 1.0f, 0.0f }), color, { 0.0f, 0.0f } });
			verticesOnHand.push_back({ transform * F3{  0.0f, 0.0f,  -0.5f }, Normalize(transform * F3{ 0.0f , 1.0f, 0.0f }), color, { 0.0f, 1.0f } });

			int size = verticesOnHand.size();
			for (int i = 0; i < size; i += 2) {
				float u = 1.0f - (float)i / size;
				verticesOnHand[i].texCoord.x = u;
				verticesOnHand[i + 1].texCoord.x = u;
			}

		}

		// 回転している下敷きの更新
		padPosition = F3{ sinf(padProgress * PI * 2), -0.3f, cosf(padProgress * PI * 2) } *0.5f;
		padRotate = padRotate + 720.0f * GetDeltaTime() * 0.001f;
		while (padRotate > 360.0f) {
			padRotate -= 360.0f;
		}

		// 下敷きの軌道エフェクト更新
		{
			F4 color = HSV2RGB(HSVT, 1.0f, 1.0f);

			if (verticesPadTrail.size() >= 30 * 2) {
				verticesPadTrail.erase(verticesPadTrail.begin());
				verticesPadTrail.erase(verticesPadTrail.begin());
			}

			F3 p0 = F3{ 0.0f, 0.0f, 0.5f } *padModel->rawModel->rootNode->children->scale.z * 0.1f;
			F3 p1 = F3{ 0.0f, 0.0f, -0.5f } *padModel->rawModel->rootNode->children->scale.z * 0.1f;

			M4x4 transform = M4x4::ScalingMatrix({ 1.0f, 1.0f, 1.0f }) * M4x4::RotatingMatrix(Quaternion::AxisYDegree(padRotate)) * M4x4::TranslatingMatrix(padPosition);

			verticesPadTrail.push_back({
				transform * p0,
				Normalize(transform * F3{ 0.0f , 1.0f, 0.0f }),
				color, { 0.0f, 0.0f } }
			);
			verticesPadTrail.push_back({
				transform * p1,
				Normalize(transform * F3{ 0.0f , 1.0f, 0.0f }),
				color, { 0.0f, 1.0f } }
			);

			int size = verticesPadTrail.size();
			for (int i = 0; i < size; i += 2) {
				float u = 1.0f - (float)i / size;
				verticesPadTrail[i].texCoord.x = u;
				verticesPadTrail[i + 1].texCoord.x = u;
			}

		}

		padProgress.IncreaseValue(GetDeltaTime());
		HSVT.IncreaseValue(GetDeltaTime());
	}

	void TestScene::UpdateIdle()
	{
		if (
			IsInputDown(INPUT_RIGHT) || 
			IsInputDown(INPUT_LEFT) ||
			IsInputDown(INPUT_UP) || 
			IsInputDown(INPUT_DOWN)
		) {
			// Idle状態からの歩き

			walkTime = 0.0f; // 歩きアニメーションをt0からにする
			updateFunc = [this]() mutable { UpdateWalk(); };

			// アニメーション更新関数を変更
			animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
				LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
					// 遷移元のアニメーション
					{ { blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime } }, 

					// 遷移先のアニメーション
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime }, // まばたきはそのまま
						{ walkAnimation, walkAnimation->rawAnimation->frames * walkTime }
					},

					t // 線形補間係数
				);

				// 補間係数更新
				t.IncreaseValue(GetDeltaTime());
			};
		}
		else if (IsInputTrigger(INPUT_OK)) {
			// Idle状態からの振り下ろし

			swingTime = 0.0f; // 振り下ろしアニメーションをt0からにする
			updateFunc = [this]() mutable { UpdateSwing(); };

			// アニメーション更新関数を変更
			animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
				LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
					// 遷移元のアニメーション
					{ { blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime } },

					// 遷移先のアニメーション
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime }, // まばたきはそのまま
						{ swingDownAnimation, swingDownAnimation->rawAnimation->frames * swingTime }
					},
					
					t // 線形補間係数
				);

				// 補間係数更新
				t.IncreaseValue(GetDeltaTime());
			};
		}

		// まばたきアニメーションの時間更新
		blinkTime.IncreaseValue(GetDeltaTime());
	}

	void TestScene::UpdateWalk()
	{
		bool walk = false;
		F3 direct{ 0.0f, 0.0f, 0.0f };
		if (IsInputDown(INPUT_RIGHT)) {
			direct = { 1.0f, 0.0f, 0.0f };
			walk = true;
		}
		else if (IsInputDown(INPUT_LEFT)) {
			direct = { -1.0f, 0.0f, 0.0f };
			walk = true;
		}
		else if (IsInputDown(INPUT_UP)) {
			direct = { 0.0f, 0.0f, 1.0f };
			walk = true;
		}
		else if (IsInputDown(INPUT_DOWN)) {
			direct = { 0.0f, 0.0f, -1.0f };
			walk = true;
		}

		if (walk) {
			float maxAngle = CONFIG.ROTATE_SPEED * GetDeltaTime() * 0.001f;

			if (acosf(Dot(kumaFront, direct)) > maxAngle) {
				// 角度が最大回転速度より大きいの場合
				if (Dot(kumaFront, Rotate(direct, Quaternion::AxisYDegree(90.0f))) < 0.0f) {
					// 時計回り
					kumaFront = Rotate(kumaFront, Quaternion::AxisYRadian(maxAngle));
				}
				else {
					// 逆時計回り
					kumaFront = Rotate(kumaFront, Quaternion::AxisYRadian(-maxAngle));
				}
			}
			else {
				// 角度が最大回転速度より小さいの場合
				kumaFront = direct;
			}
		}

		if (IsInputTrigger(INPUT_OK)) {
			// 歩き状態から振り下ろし状態へ

			swingTime = 0.0f;
			updateFunc = [this]() mutable { UpdateSwing(); };

			// アニメーション更新関数を変更
			animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
				LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
					// 遷移元のアニメーション
					{ 
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
						{ walkAnimation, walkAnimation->rawAnimation->frames * walkTime }
					},

					// 遷移先のアニメーション
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime }, // まばたきはそのまま
						{ swingDownAnimation, swingDownAnimation->rawAnimation->frames * swingTime }
					}, 

					t // 線形補間係数
				);

				// 補間係数更新
				t.IncreaseValue(GetDeltaTime());
			};
		}
		else if (!walk) {
			// 歩き状態から停止状態へ

			updateFunc = [this]() mutable { UpdateIdle(); };
			
			// アニメーション更新関数を変更
			animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
				LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
					// 遷移元のアニメーション
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
						{ walkAnimation, walkAnimation->rawAnimation->frames * walkTime }
					},

					// 遷移先のアニメーション
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime } // まばたきはそのまま
					}, 

					t // 線形補間係数
				);

				// 補間係数更新
				t.IncreaseValue(GetDeltaTime());
			};
		}

		// 歩きアニメーションの時間更新
		walkTime.IncreaseValue(GetDeltaTime());

		// まばたきアニメーションの時間更新
		blinkTime.IncreaseValue(GetDeltaTime());
	}

	void TestScene::UpdateSwing()
	{
		if (swingTime == 1.0f) {
			// 振り下ろしアニメーション終了したら

			if (
				IsInputDown(INPUT_RIGHT) ||
				IsInputDown(INPUT_LEFT) ||
				IsInputDown(INPUT_UP) ||
				IsInputDown(INPUT_DOWN)
			)
			{
				// 歩き状態へ

				walkTime = 0.0f;
				updateFunc = [this]() mutable { UpdateWalk(); };
				animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
					LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
						// 遷移元のアニメーション
						{
							{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
							{ swingDownAnimation, swingDownAnimation->rawAnimation->frames * swingTime }
						},

						// 遷移先のアニメーション
						{
							{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
							{ walkAnimation, walkAnimation->rawAnimation->frames * walkTime }
						}, t
					);
					t.IncreaseValue(GetDeltaTime());
				};
			}
			else {
				// 停止状態へ

				updateFunc = [this]() mutable { UpdateIdle(); };
				animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
					LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
						// 遷移元のアニメーション
						{
							{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
							{ swingDownAnimation, swingDownAnimation->rawAnimation->frames * swingTime }
						},

						// 遷移先のアニメーション
						{
							{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime }
						}, t
					);
					t.IncreaseValue(GetDeltaTime());
				};
			}
			
		}

		// 振り下ろしアニメーションの時間更新
		swingTime.IncreaseValue(GetDeltaTime());

		// まばたきアニメーションの時間更新
		blinkTime.IncreaseValue(GetDeltaTime());
	}


	// =======================================================
	// 描画
	// =======================================================
	void TestScene::Draw()
	{
		
		GetRenderer()->ApplyCamera(currentCamera);
		
		DrawModel(model, modelTransforms);

		DrawModel(padModel, onHandTransforms);

		DrawModel(padModel, padPosition, { 0.1f, 0.1f, 0.1f }, Quaternion::AxisYDegree(padRotate));

		GetRenderer()->SetBlendState(BLEND_STATE_ADD);
		GetRenderer()->SetDepthState(DEPTH_STATE_NO_WRITE);
		DrawPolygon(trailTexture, verticesOnHand.data(), verticesOnHand.size());
		DrawPolygon(trailTexture, verticesPadTrail.data(), verticesPadTrail.size());
		GetRenderer()->SetDepthState(DEPTH_STATE_ENABLE);
		GetRenderer()->SetBlendState(BLEND_STATE_ALPHA);

		

		Scene::Draw();
	}

	// =======================================================
	// レイヤータイプの定義
	// =======================================================
	/*LAYER_TYPE TestScene::GetLayerType(int layer)
	{
		return Scene::GetLayerType(layer);
	}*/

	M4x4 TestScene::GetWorldMartix() {
		return M4x4::ScalingMatrix(kumaSize) * M4x4::RotatingMatrix(Quaternion::Quaternion(kumaFront));
	}
}
