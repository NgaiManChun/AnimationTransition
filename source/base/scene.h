// =======================================================
// scene.h
// 
// Sceneクラス及びScene制御
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/07/06
// 
// 2024/09/05:
// UninitScene() 全インスタンスのUninitを呼び出し追加
// DoGameObjectCommands() Insertできないバグ修正
// 
// 2024/11/18:
// AddGameObjectとInsertGameObjectをマクロ化
// 
// 2024/01/09:
// SortGameObjects()を追加、
// シーンごとにGameObjectのソート方法定義できる
// =======================================================
#ifndef _SCENE_H
#define _SCENE_H

#include "MGCommon.h"
#include "gameObject.h"
#include "gameObjectQuad.h"
#include "gameObjectAudio.h"
#include "camera.h"
#include "resourceTool.h"
#include "collision.h"
#include "renderer.h"


// =======================================================
// シーン登録用の定型文
// REGISTER_SCENE(シーン名, クラス名)
// =======================================================
#define REGISTER_SCENE(name, className) \
MG::RegisterScene(name, []() -> MG::Scene* { MG::Scene* scene = (MG::Scene*)new className(); scene->sceneName = name; return scene; }); \
																       

#define AddGameObject(constructor, ...)					\
([this, ptr = new constructor]() {						\
	AddGameObjectPtr(ptr, ##__VA_ARGS__);				\
	return ptr;											\
})()													\

#define InsertGameObject(constructor, index, ...)		\
([this, ptr = new constructor]() {						\
	InsertGameObjectPtr(ptr, index, ##__VA_ARGS__);		\
	return ptr;											\
})()													\

namespace MG {
	typedef std::string SceneName;
	typedef int LAYER_TYPE;
	constexpr const int LAYER_2D = 10;
	constexpr const LAYER_TYPE LAYER_TYPE_NONE = 0;
	constexpr const LAYER_TYPE LAYER_TYPE_3D = 1;
	constexpr const LAYER_TYPE LAYER_TYPE_2D = 2;

	enum SCENE_COMMAND_TYPE {
		START,
		END,
		LOAD,
		RELEASE,
		UPDATE,
		DRAW
	};

	/*enum LAYER_TYPE {
		LAYER_TYPE_NONE,
		LAYER_TYPE_3D,
		LAYER_TYPE_2D
	};*/

	struct SceneCommand {
		SceneName sceneName;
		SCENE_COMMAND_TYPE type;
	};

	class Scene {
		enum GAME_OBJECT_LIST_COMMAND_TYPE {
			ADD,
			INSERT,
			REMOVE,
			UPDATE,
			DRAW
		};
		struct GameObjectCommand {
			GameObject* gameObject;
			GAME_OBJECT_LIST_COMMAND_TYPE type;
			int layer;
			unsigned int index;
		};
		
	private:
		Camera2D defaultCamera2D;
		Camera3D defaultCamera;
		GameObject* lockedGameObject = nullptr;
		LAYER_TYPE currentLayerType = LAYER_TYPE_NONE;
		std::list<GameObjectCommand> gameObjectCommands;
		void DoGameObjectCommands();
		void DoGameObjectCommand(const GameObjectCommand& command);
	protected:
		Camera2D* currentCamera2D = &defaultCamera2D;
		Camera3D* currentCamera = &defaultCamera;
		F4 ambientLight = { 1.0f, 1.0f, 1.0f, 0.7f };
		F4 driectLight = { 1.0f, 1.0f, 1.0f, 0.3f };
		F3 driectLightPosition = { 5.0f, 5.0f, -1.0f };
		std::list<GameObject*> gameObjects;
		RenderTarget* renderTarget = nullptr;
	public:
		SceneName sceneName;

		virtual ~Scene() = default;

		virtual void Init();

		// =======================================================
		// 終了処理
		// 
		// 稼働リストの中のゲームオブジェクトはすべて破棄
		// このシーンで取得したリソースもすべて解放
		// （他のスコープで使用中のリソースは除外）
		// =======================================================
		virtual void Uninit();

		// =======================================================
		// 更新
		// 
		// 稼働リストの中のゲームオブジェクトのUpdate関数を
		// 一つずつ呼び出す
		// =======================================================
		virtual void Update();


		// =======================================================
		// 描画
		// 
		// 稼働リストの中のゲームオブジェクトのDraw関数を
		// 一つずつ呼び出す
		// =======================================================
		virtual void Draw();

		virtual LAYER_TYPE GetLayerType(int layer);

		virtual void SortGameObjects();

		virtual std::list<GameObject*>& GetGameObjects();

		virtual Camera3D* GetCurrentCamera() const;

		virtual Camera2D* GetCurrentCamera2D() const;

		// ゲームオブジェクトを稼働リストに追加
		GameObject* AddGameObjectPtr(GameObject* gameObject, const int layer = 0);

		// ゲームオブジェクトを稼働リストに挿入
		GameObject* InsertGameObjectPtr(GameObject* gameObject, const unsigned int index, const int layer = 0);

		// ゲームオブジェクトを稼働リストから除外
		void DeleteGameObject(GameObject* gameObject);

		// テクスチャリソースを取得
		Texture* LoadTexture(const std::string& path);

		// テクスチャリソースを取得、スコープ指定
		Texture* LoadTexture(const std::string& path, const std::string& scope);

		Texture* RenderText(const std::wstring& text, const FONT& font = {});

		Texture* RenderText(const std::wstring& text, const FONT& font, const std::string& scope);

		// オーディオリソースを取得
		Audio* LoadAudio(const std::string& path);

		// オーディオリソースを取得、スコープ指定
		Audio* LoadAudio(const std::string& path, const std::string& scope);

		// モデルリソースを取得
		Model* LoadModel(const std::string& path);

		// モデルリソースを取得、スコープ指定
		Model* LoadModel(const std::string& path, const std::string& scope);

		// シーン遷移開始
		void SceneTransit(SceneName dest);

		// シーン遷移開始
		void SceneTransit(SceneName dest, std::string transitionName);

		// シーン遷移開始
		void SceneTransit(SceneName dest, std::string transitionName, SceneName src);

		void SetRenderTarget(RenderTarget* renderTarget);

		RenderTarget* GetRenderTarget() const;
	};


	// =======================================================
	// シーン制御関数
	// =======================================================
	
	void UninitScene();
	SceneName RegisterScene(std::string name, Scene* (*function)());
	void DoSceneCommands();
	void DoSceneCommand(SceneCommand command);
	Scene* LoadScene(const std::string& name);
	void ReleaseScene(const std::string& name);
	void StartScene(const std::string& name);
	void EndScene(const std::string& name);
	void UpdateScene();
	void DrawScene();
	void SceneTransit(SceneName dest, SceneName src);
	void SceneTransit(SceneName dest, SceneName src, std::string transitionName);
	bool InTransition();

} // namespace MG

#endif