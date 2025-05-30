// =======================================================
// sceneTransition.h
// 
// SceneTransitionクラス及びシーン遷移制御
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _SCENE_TRANSITAION_H
#define _SCENE_TRANSITAION_H

#include "MGCommon.h"
#include "scene.h"


// =======================================================
// シーン遷移登録用の定型文
// REGISTER_TRANSITION(シーン遷移名, クラス名)
// =======================================================
#define REGISTER_TRANSITION(name, className) \
		MG::RegisterTransition(name, []() -> MG::SceneTransition* { return (MG::SceneTransition*)new className(); })


namespace MG {
	typedef std::string TransitionName;
	constexpr const char* TRANSITION_DEFAULT = "default";

	class SceneTransition {
	protected:
		std::list<Scene*>* m_runningScenes;
		SceneName m_src;
		SceneName m_dest;
		bool inTransition = true;
	public:

		virtual void Update();

		virtual void Draw();

		void SetRunningScenes(std::list<Scene*>* runningScenes);

		void SetSrc(SceneName src);

		void SetDest(SceneName dest);

		bool InTransition();
	};

	TransitionName RegisterTransition(string name, SceneTransition* (*function)());

	SceneTransition* CreateTransition(const string& name);

	void UnregisterAllTransition();

} // namespace MG

#endif