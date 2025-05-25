// =======================================================
// scene.cpp
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
// =======================================================
#define NOMINMAX
#include "scene.h"
#include "sceneTransitaion.h"
#include "gameObject.h"
#include "renderer.h"
#include "resourceTool.h"
#include <algorithm>

namespace MG {
	void Scene::Init() {}

	LAYER_TYPE Scene::GetLayerType(int layer)
	{
		if (layer < 0 || layer >= LAYER_2D) {
			return LAYER_TYPE_2D;
		}
		return LAYER_TYPE_3D;
	}

	void Scene::SortGameObjects()
	{
		gameObjects.sort([](GameObject* a, GameObject* b) -> bool {
			return a->layer < b->layer;
		});
	}

	std::list<GameObject*>& Scene::GetGameObjects()
	{
		return gameObjects;
	}

	Camera3D* Scene::GetCurrentCamera() const
	{
		return currentCamera;
	}

	Camera2D* Scene::GetCurrentCamera2D() const
	{
		return currentCamera2D;
	}

	void Scene::DoGameObjectCommands()
	{
		while (!gameObjectCommands.empty()) {
			GameObjectCommand command = gameObjectCommands.front();
			gameObjectCommands.pop_front();
			DoGameObjectCommand(command);
		}
	}

	void Scene::DoGameObjectCommand(const GameObjectCommand& command)
	{
		if (command.type == UPDATE) {
			auto itr = std::find(gameObjects.begin(), gameObjects.end(), command.gameObject);
			if (itr != gameObjects.end() && command.gameObject->enable) {
				lockedGameObject = command.gameObject;
				command.gameObject->Update();
				lockedGameObject = nullptr;
			}
		}
		else if (command.type == DRAW) {
			auto itr = std::find(gameObjects.begin(), gameObjects.end(), command.gameObject);
			if (itr != gameObjects.end() && command.gameObject->enable) {
				LAYER_TYPE type = GetLayerType(command.gameObject->layer);
				if (currentLayerType != type) {
					currentLayerType = type;
					if (type == LAYER_TYPE_2D) {
						GetRenderer()->ApplyCamera(currentCamera2D);
					}
					else {
						GetRenderer()->ApplyCamera(currentCamera);
					}
				}
				lockedGameObject = command.gameObject;
				command.gameObject->Draw();
				lockedGameObject = nullptr;
			}
		}
		else if (command.type == ADD) {
			command.gameObject->scene = this;
			command.gameObject->layer = command.layer;
			gameObjects.push_back(command.gameObject);
		}
		else if (command.type == INSERT) {
			int index = command.index;
			index = std::max(index, 0);
			index = std::min(index, (int)gameObjects.size());
			command.gameObject->scene = this;
			command.gameObject->layer = command.layer;
			gameObjects.insert(std::next(gameObjects.begin(), index), command.gameObject);
		}
		else if (command.type == REMOVE) {
			gameObjects.remove(command.gameObject);
			delete command.gameObject;
		}
	}


	// =======================================================
	// ゲームオブジェクトを稼働リストに追加
	// =======================================================
	GameObject* Scene::AddGameObjectPtr(GameObject* gameObject, const int layer)
	{
		DoGameObjectCommand({ gameObject, ADD, layer });
		return gameObject;
	}

	// =======================================================
	// ゲームオブジェクトを稼働リストに挿入
	// =======================================================
	GameObject* Scene::InsertGameObjectPtr(GameObject* gameObject, const unsigned int index, const int layer)
	{
		DoGameObjectCommand({ gameObject, INSERT, layer, index });
		return gameObject;
	}

	// =======================================================
	// ゲームオブジェクトを稼働リストから除外
	// =======================================================
	void Scene::DeleteGameObject(GameObject* gameObject)
	{
		if (lockedGameObject != nullptr && lockedGameObject == gameObject) {
			gameObjectCommands.push_front({ gameObject, REMOVE });
		}
		else {
			DoGameObjectCommand({ gameObject, REMOVE });
		}
	}


	// =======================================================
	// 終了処理
	// 
	// 稼働リストの中のゲームオブジェクトはすべて破棄
	// このシーンで取得したリソースもすべて解放
	// （他のスコープで使用中のリソースは除外）
	// =======================================================
	void Scene::Uninit() {
		for (auto itr = gameObjects.begin(); itr != gameObjects.end(); itr++) {
			GameObject* gameObject = *itr;
			delete gameObject;
		}
		gameObjects.clear();
		gameObjectCommands.clear();
		ReleaseResource(sceneName);
	}


	// =======================================================
	// 更新
	// 
	// 稼働リストの中のゲームオブジェクトのUpdate関数を
	// 一つずつ呼び出す
	// =======================================================
	void Scene::Update()
	{
		for (auto itr = gameObjects.begin(); itr != gameObjects.end(); itr++) {
			GameObject* gameObject = *itr;
			gameObjectCommands.push_back({ gameObject, UPDATE });
		}
		DoGameObjectCommands();
	}


	// =======================================================
	// 描画
	// 
	// 稼働リストの中のゲームオブジェクトのDraw関数を
	// 一つずつ呼び出す
	// =======================================================
	void Scene::Draw() {
		for (auto itr = gameObjects.begin(); itr != gameObjects.end(); itr++) {
			gameObjectCommands.push_back({ *itr, DRAW });
		}
		currentLayerType = LAYER_TYPE_NONE;
		Renderer* renderer = GetRenderer();
		renderer->SetAmbient(ambientLight);
		renderer->SetLight(driectLightPosition, driectLight);
		DoGameObjectCommands();
	}

	// =======================================================
	// テクスチャリソースを取得
	// =======================================================
	Texture* Scene::LoadTexture(const std::string& path)
	{
		return GetResourceTool()->LoadTexture(path, sceneName);
	}


	// =======================================================
	// テクスチャリソースを取得、スコープ指定
	// =======================================================
	Texture* Scene::LoadTexture(const std::string& path, const std::string& scope)
	{
		return GetResourceTool()->LoadTexture(path, scope);
	}

	Texture* Scene::RenderText(const std::wstring& text, const FONT& font)
	{
		return GetResourceTool()->RenderText(text, font, sceneName);
	}

	Texture* Scene::RenderText(const std::wstring& text, const FONT& font, const std::string& scope)
	{
		return GetResourceTool()->RenderText(text, font, scope);
	}


	// =======================================================
	// オーディオリソースを取得
	// =======================================================
	Audio* Scene::LoadAudio(const std::string& path)
	{
		return GetResourceTool()->LoadAudio(path, sceneName);
	}


	// =======================================================
	// オーディオリソースを取得、スコープ指定
	// =======================================================
	Audio* Scene::LoadAudio(const std::string& path, const std::string& scope)
	{
		return GetResourceTool()->LoadAudio(path, scope);
	}


	// =======================================================
	// モデルリソースを取得
	// =======================================================
	Model* Scene::LoadModel(const std::string& path)
	{
		return GetResourceTool()->LoadModel(path, sceneName);
	}

	
	// =======================================================
	// モデルリソースを取得、スコープ指定
	// =======================================================
	Model* Scene::LoadModel(const std::string& path, const std::string& scope)
	{
		return GetResourceTool()->LoadModel(path, scope);
	}


	// =======================================================
	// シーン遷移開始
	// 
	// 引数：遷移先
	// =======================================================
	void Scene::SceneTransit(SceneName dest)
	{
		MG::SceneTransit(dest, sceneName);
	}


	// =======================================================
	// シーン遷移開始
	// 
	// 引数：遷移先、遷移効果名
	// =======================================================
	void Scene::SceneTransit(SceneName dest, std::string transitionName)
	{
		MG::SceneTransit(dest, sceneName, transitionName);
	}


	// =======================================================
	// シーン遷移開始
	// 
	// 引数：遷移先、遷移効果名、遷移元
	// 遷移元がこのシーンじゃない場合だけ使用
	// =======================================================
	void Scene::SceneTransit(SceneName dest, std::string transitionName, SceneName src)
	{
		MG::SceneTransit(dest, src, transitionName);
	}

	void Scene::SetRenderTarget(RenderTarget* renderTarget)
	{
		this->renderTarget = renderTarget;
	}

	RenderTarget* Scene::GetRenderTarget() const
	{
		return renderTarget;
	}


	// =======================================================
	// 以下シーン制御関連
	// =======================================================

	static map<string, Scene* (*)()>* __sceneInstanceFunctions;
	static map<string, Scene*>* __instances;
	static list<Scene*>* __runningScenes;
	static Scene* lockedScene = nullptr;
	static list<SceneCommand> __sceneCommands;
	static SceneTransition* sceneTransition;


	// =======================================================
	// シーン登録関数
	// 
	// 引数１はシーン名
	// 引数２はインスタンス作成のCALLBACK関数
	// 
	// 定型文　REGISTER_SCENE(シーン名, クラス名)
	// で利用するのがおススメ
	// =======================================================
	SceneName RegisterScene(string name, Scene* (*function)())
	{
		if (!__sceneInstanceFunctions) {
			__sceneInstanceFunctions = new map<string, Scene* (*)()>;
		}
		if (!__instances) {
			__instances = new map<string, Scene*>;
		}
		if (!__runningScenes) {
			__runningScenes = new list<Scene*>;
		}
		(*__sceneInstanceFunctions)[name] = function;
		(*__instances)[name] = nullptr;

		return name;
	}

	void DoSceneCommands()
	{
		while (!__sceneCommands.empty()) {
			SceneCommand command = __sceneCommands.front();
			__sceneCommands.pop_front();
			DoSceneCommand(command);
		}
	}

	void DoSceneCommand(SceneCommand command)
	{
		if (command.type == UPDATE) {
			if ((*__instances)[command.sceneName]) {
				Scene* scene = (*__instances)[command.sceneName];
				auto itr = std::find(__runningScenes->begin(), __runningScenes->end(), scene);
				if (itr != __runningScenes->end()) {
					lockedScene = scene;
					scene->Update();
					scene->SortGameObjects();
					lockedScene = nullptr;
				}
			}
		}
		else if (command.type == DRAW) {
			if ((*__instances)[command.sceneName]) {
				Scene* scene = (*__instances)[command.sceneName];
				auto itr = std::find(__runningScenes->begin(), __runningScenes->end(), scene);
				if (itr != __runningScenes->end()) {
					lockedScene = scene;
					scene->Draw();
					lockedScene = nullptr;
				}
			}
		}
		else if (command.type == START) {
			Scene* scene = LoadScene(command.sceneName);
			if (scene) {
				auto itr = std::find(__runningScenes->begin(), __runningScenes->end(), scene);
				if (itr != __runningScenes->end()) {
					__runningScenes->erase(itr);
				}
				lockedScene = scene;
				scene->Init();
				lockedScene = nullptr;
				__runningScenes->push_front(scene);
			}
		}
		else if (command.type == END) {
			if ((*__instances)[command.sceneName]) {
				Scene* scene = (*__instances)[command.sceneName];
				auto itr = std::find(__runningScenes->begin(), __runningScenes->end(), scene);
				if (itr != __runningScenes->end()) {
					lockedScene = scene;
					scene->Uninit();
					lockedScene = nullptr;
					__runningScenes->erase(itr);
				}
			}
		}
		else if (command.type == LOAD) {
			if (!(*__instances)[command.sceneName]) {
				if ((*__sceneInstanceFunctions)[command.sceneName]) {
					(*__instances)[command.sceneName] = (*__sceneInstanceFunctions)[command.sceneName]();
				}
			}
		}
		else if (command.type == RELEASE) {
			Scene* scene = (*__instances)[command.sceneName];
			auto itr = std::find(__runningScenes->begin(), __runningScenes->end(), scene);
			if (scene && itr != __runningScenes->end()) {
				__runningScenes->erase(itr);
				delete scene;
				__instances->erase(command.sceneName);
			}
		}
	}

	void UninitScene()
	{
		for (const auto& pair : (*__instances)) {
			if ((*__instances)[pair.first]) {
				(*__instances)[pair.first]->Uninit();
			}
			delete (*__instances)[pair.first];
			(*__instances)[pair.first] = nullptr;
		}
		for (auto& pair : *__sceneInstanceFunctions) {
			(*__sceneInstanceFunctions)[pair.first] = nullptr;
		}
		__sceneInstanceFunctions->clear();
		__instances->clear();
		__runningScenes->clear();
		delete __sceneInstanceFunctions;
		delete __instances;
		delete __runningScenes;
	}


	// =======================================================
	// シーンのインスタンス生成
	// 
	// 指定したシーンのインスタンスを生成する
	// 同じシーンのインスタンスは同時に複数作らない
	// Init関数は呼び出されない
	// =======================================================
	Scene* LoadScene(const string& name)
	{
		DoSceneCommand({ name, LOAD });
		return (*__instances)[name];
	}


	// =======================================================
	// シーンのインスタンス破棄
	// 
	// 指定したシーンのインスタンスを破棄する
	// Uninit関数は呼び出されない
	// =======================================================
	void ReleaseScene(const string& name)
	{
		if (lockedScene != nullptr && lockedScene == (*__instances)[name]) {
			__sceneCommands.push_front({ name, RELEASE });
		}
		else {
			DoSceneCommand({ name, RELEASE });
		}
	}


	// =======================================================
	// シーン開始
	// 
	// 指定したシーンを稼働シーン行列の最前に追加する
	// シーンのInit関数は呼び出される
	// インスタンスがまだ生成されてないシーンは生成される
	// =======================================================
	void StartScene(const string& name)
	{
		DoSceneCommand({ name, START });
	}


	// =======================================================
	// シーン終了
	// 
	// 指定したシーンを稼働シーン行列から除外
	// シーンのUninit関数は呼び出される
	// インスタンスは破棄されない
	// =======================================================
	void EndScene(const string& name)
	{
		DoSceneCommand({ name, END });
	}


	// =======================================================
	// シーン更新
	// 
	// 稼働シーン行列の中のシーンのUpdate関数を一つずつ呼び出す
	// シーン遷移中は先にシーン遷移更新する
	// =======================================================
	void UpdateScene()
	{
		if (sceneTransition) {
			if (!sceneTransition->InTransition()) {
				delete sceneTransition;
				sceneTransition = nullptr;
			}
		}
		if (sceneTransition) {
			sceneTransition->Update();
		}
		for (auto itr = __runningScenes->begin(); itr != __runningScenes->end(); itr++) {
			Scene* scene = *itr;
			__sceneCommands.push_back({ scene->sceneName, UPDATE });
		}
		DoSceneCommands();
	}


	// =======================================================
	// シーン描画
	// 
	// 稼働シーン行列の中のシーンのDraw関数を一つずつ呼び出す
	// シーン遷移中はシーン遷移のDraw関数だけを実行する
	// =======================================================
	void DrawScene()
	{
		if (sceneTransition) {
			sceneTransition->Draw();
		}
		else {
			for (auto itr = __runningScenes->begin(); itr != __runningScenes->end(); itr++) {
				Scene* scene = *itr;
				__sceneCommands.push_back({ scene->sceneName, DRAW });
			}
		}
		DoSceneCommands();
	}

	void SceneTransit(SceneName dest, SceneName src)
	{
		SceneTransit(dest, src, TRANSITION_DEFAULT);
	}


	// =======================================================
	// シーン遷移開始
	// =======================================================
	void SceneTransit(SceneName dest, SceneName src, std::string transitionName)
	{
		if (!sceneTransition) {
			sceneTransition = CreateTransition(transitionName);
			sceneTransition->SetRunningScenes(__runningScenes);
			sceneTransition->SetDest(dest);
			sceneTransition->SetSrc(src);
		}
	}


	// =======================================================
	// シーン遷移中かどうかをチェック
	// =======================================================
	bool InTransition()
	{
		if (sceneTransition) {
			return sceneTransition->InTransition();
		}
		return false;
	}
}