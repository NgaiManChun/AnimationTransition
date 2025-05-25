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
	// �N���X��`
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
		Progress padProgress{ 4000.0f, true }; // ���~���̉�]�W��
		Progress HSVT{ 1400.0f, true }; // �O���G�t�F�N�g�̕ϐF�W��
		 
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
	// �R���t�B�O�ǂݍ���
	// =======================================================
	static const TestScene::_CONFIG CONFIG = LoadConfig<TestScene::_CONFIG>("asset\\config.csv", [](const D_KVTABLE& table) -> TestScene::_CONFIG {
		return {
			TABLE_FLOAT_VALUE(table, "ROTATE_SPEED", ((360.0f / 360.0f) * 2.0f * PI))
		};
	});


	// =======================================================
	// �V�[���o�^
	// =======================================================
	static SceneName sceneName = REGISTER_SCENE("test", TestScene);

	// =======================================================
	// ������
	// =======================================================
	void TestScene::Init()
	{
		Scene::Init();

		// ���\�[�X�ǂݍ���
		model = LoadModel(MODEL);
		walkAnimation = LoadAnimation(WALK_ANIMATION);
		blinkAnimation = LoadAnimation(BLINK_ANIMATION);
		swingDownAnimation = LoadAnimation(SWING_DOWN_ANIMATION);
		padModel = LoadModel(PAD_MODEL);

		// �J�������΂߂ɐݒu
		currentCamera->SetPosition({ 0.3f, 0.0f, -1.0f });
		currentCamera->SetFront(Normalize(F3{} - currentCamera->GetPosition()));

		trailTexture = LoadTexture(TRAIL_TEXTURE);

		updateFunc = [this]() { UpdateIdle(); };

		animTransFunc = [this]() {
			LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms);
		};
	}


	// =======================================================
	// �I������
	// =======================================================
	/*void TestScene::Uninit()
	{
		Scene::Uninit();
	}*/


	// =======================================================
	// �X�V
	// =======================================================
	void TestScene::Update()
	{
		if (updateFunc) {
			updateFunc();
		}
		if (animTransFunc) {
			animTransFunc();
		}
		

		// ���f������A�C�e��������|�W�V������T��
		MODEL_NODE* padNode = FindNodeByName(model->rawModel->rootNode, "padPos");
		if (padNode) {
			M4x4 world = modelTransforms.at(padNode); // �����Ă�A�C�e����World
			LoadNodeWorldTransforms(padModel->rawModel->rootNode, world, onHandTransforms);
		}
		

		// �莝���A�C�e���̋O���G�t�F�N�g�X�V
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

		// ��]���Ă��鉺�~���̍X�V
		padPosition = F3{ sinf(padProgress * PI * 2), -0.3f, cosf(padProgress * PI * 2) } *0.5f;
		padRotate = padRotate + 720.0f * GetDeltaTime() * 0.001f;
		while (padRotate > 360.0f) {
			padRotate -= 360.0f;
		}

		// ���~���̋O���G�t�F�N�g�X�V
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
			// Idle��Ԃ���̕���

			walkTime = 0.0f; // �����A�j���[�V������t0����ɂ���
			updateFunc = [this]() mutable { UpdateWalk(); };

			// �A�j���[�V�����X�V�֐���ύX
			animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
				LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
					// �J�ڌ��̃A�j���[�V����
					{ { blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime } }, 

					// �J�ڐ�̃A�j���[�V����
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime }, // �܂΂����͂��̂܂�
						{ walkAnimation, walkAnimation->rawAnimation->frames * walkTime }
					},

					t // ���`��ԌW��
				);

				// ��ԌW���X�V
				t.IncreaseValue(GetDeltaTime());
			};
		}
		else if (IsInputTrigger(INPUT_OK)) {
			// Idle��Ԃ���̐U�艺�낵

			swingTime = 0.0f; // �U�艺�낵�A�j���[�V������t0����ɂ���
			updateFunc = [this]() mutable { UpdateSwing(); };

			// �A�j���[�V�����X�V�֐���ύX
			animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
				LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
					// �J�ڌ��̃A�j���[�V����
					{ { blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime } },

					// �J�ڐ�̃A�j���[�V����
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime }, // �܂΂����͂��̂܂�
						{ swingDownAnimation, swingDownAnimation->rawAnimation->frames * swingTime }
					},
					
					t // ���`��ԌW��
				);

				// ��ԌW���X�V
				t.IncreaseValue(GetDeltaTime());
			};
		}

		// �܂΂����A�j���[�V�����̎��ԍX�V
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
				// �p�x���ő��]���x���傫���̏ꍇ
				if (Dot(kumaFront, Rotate(direct, Quaternion::AxisYDegree(90.0f))) < 0.0f) {
					// ���v���
					kumaFront = Rotate(kumaFront, Quaternion::AxisYRadian(maxAngle));
				}
				else {
					// �t���v���
					kumaFront = Rotate(kumaFront, Quaternion::AxisYRadian(-maxAngle));
				}
			}
			else {
				// �p�x���ő��]���x��菬�����̏ꍇ
				kumaFront = direct;
			}
		}

		if (IsInputTrigger(INPUT_OK)) {
			// ������Ԃ���U�艺�낵��Ԃ�

			swingTime = 0.0f;
			updateFunc = [this]() mutable { UpdateSwing(); };

			// �A�j���[�V�����X�V�֐���ύX
			animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
				LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
					// �J�ڌ��̃A�j���[�V����
					{ 
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
						{ walkAnimation, walkAnimation->rawAnimation->frames * walkTime }
					},

					// �J�ڐ�̃A�j���[�V����
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime }, // �܂΂����͂��̂܂�
						{ swingDownAnimation, swingDownAnimation->rawAnimation->frames * swingTime }
					}, 

					t // ���`��ԌW��
				);

				// ��ԌW���X�V
				t.IncreaseValue(GetDeltaTime());
			};
		}
		else if (!walk) {
			// ������Ԃ����~��Ԃ�

			updateFunc = [this]() mutable { UpdateIdle(); };
			
			// �A�j���[�V�����X�V�֐���ύX
			animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
				LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
					// �J�ڌ��̃A�j���[�V����
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
						{ walkAnimation, walkAnimation->rawAnimation->frames * walkTime }
					},

					// �J�ڐ�̃A�j���[�V����
					{
						{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime } // �܂΂����͂��̂܂�
					}, 

					t // ���`��ԌW��
				);

				// ��ԌW���X�V
				t.IncreaseValue(GetDeltaTime());
			};
		}

		// �����A�j���[�V�����̎��ԍX�V
		walkTime.IncreaseValue(GetDeltaTime());

		// �܂΂����A�j���[�V�����̎��ԍX�V
		blinkTime.IncreaseValue(GetDeltaTime());
	}

	void TestScene::UpdateSwing()
	{
		if (swingTime == 1.0f) {
			// �U�艺�낵�A�j���[�V�����I��������

			if (
				IsInputDown(INPUT_RIGHT) ||
				IsInputDown(INPUT_LEFT) ||
				IsInputDown(INPUT_UP) ||
				IsInputDown(INPUT_DOWN)
			)
			{
				// ������Ԃ�

				walkTime = 0.0f;
				updateFunc = [this]() mutable { UpdateWalk(); };
				animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
					LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
						// �J�ڌ��̃A�j���[�V����
						{
							{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
							{ swingDownAnimation, swingDownAnimation->rawAnimation->frames * swingTime }
						},

						// �J�ڐ�̃A�j���[�V����
						{
							{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
							{ walkAnimation, walkAnimation->rawAnimation->frames * walkTime }
						}, t
					);
					t.IncreaseValue(GetDeltaTime());
				};
			}
			else {
				// ��~��Ԃ�

				updateFunc = [this]() mutable { UpdateIdle(); };
				animTransFunc = [this, t = Progress{ 300.0f, false }]() mutable {
					LoadNodeWorldTransforms(model->rawModel->rootNode, GetWorldMartix(), modelTransforms,
						// �J�ڌ��̃A�j���[�V����
						{
							{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime },
							{ swingDownAnimation, swingDownAnimation->rawAnimation->frames * swingTime }
						},

						// �J�ڐ�̃A�j���[�V����
						{
							{ blinkAnimation, blinkAnimation->rawAnimation->frames * blinkTime }
						}, t
					);
					t.IncreaseValue(GetDeltaTime());
				};
			}
			
		}

		// �U�艺�낵�A�j���[�V�����̎��ԍX�V
		swingTime.IncreaseValue(GetDeltaTime());

		// �܂΂����A�j���[�V�����̎��ԍX�V
		blinkTime.IncreaseValue(GetDeltaTime());
	}


	// =======================================================
	// �`��
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
	// ���C���[�^�C�v�̒�`
	// =======================================================
	/*LAYER_TYPE TestScene::GetLayerType(int layer)
	{
		return Scene::GetLayerType(layer);
	}*/

	M4x4 TestScene::GetWorldMartix() {
		return M4x4::ScalingMatrix(kumaSize) * M4x4::RotatingMatrix(Quaternion::Quaternion(kumaFront));
	}
}
