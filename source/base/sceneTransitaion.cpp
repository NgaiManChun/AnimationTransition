// =======================================================
// sceneTransition.cpp
// 
// SceneTransitionクラス及びシーン遷移制御
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "sceneTransitaion.h"
#include "scene.h"

namespace MG {
	static map<string, SceneTransition* (*)()>* __transitionInstanceFunctions;

	static TransitionName name = REGISTER_TRANSITION(TRANSITION_DEFAULT, SceneTransition);

	void SceneTransition::Update()
	{
		StartScene(m_dest);
		EndScene(m_src);
		inTransition = false;
	}

	void SceneTransition::Draw()
	{
		// コピーを取る、リスト変動防止
		std::list<Scene*> runningScenes(*m_runningScenes);
		for (auto itr = runningScenes.begin(); itr != runningScenes.end(); itr++) {
			Scene* scene = *itr;
			DoSceneCommand({ scene->sceneName, DRAW });
		}
	}

	void SceneTransition::SetRunningScenes(std::list<Scene*>* runningScenes)
	{
		m_runningScenes = runningScenes;
	}

	void SceneTransition::SetSrc(SceneName src)
	{
		m_src = src.data();
	}

	void SceneTransition::SetDest(SceneName dest)
	{
		m_dest = dest.data();
	}

	bool SceneTransition::InTransition()
	{
		return inTransition;
	}

	TransitionName RegisterTransition(string name, SceneTransition* (*function)())
	{
		if (!__transitionInstanceFunctions) {
			__transitionInstanceFunctions = new map<string, SceneTransition* (*)()>;
		}
		(*__transitionInstanceFunctions)[name] = function;

		return name;
	}

	SceneTransition* CreateTransition(const string& name)
	{
		if ((*__transitionInstanceFunctions)[name]) {
			return (*__transitionInstanceFunctions)[name]();
		}
		return nullptr;
	}

	void UnregisterAllTransition()
	{
		__transitionInstanceFunctions->clear();
		delete __transitionInstanceFunctions;
		__transitionInstanceFunctions = nullptr;
	}
}