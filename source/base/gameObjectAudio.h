// =======================================================
// gameObjectAudio.h
// 
// オーディオゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/18
// =======================================================
#ifndef _GAME_OBJECT_AUDIO_H
#define _GAME_OBJECT_AUDIO_H

#include "gameObject.h"
#include <xaudio2.h>

namespace MG {

	class GameObjectAudio : public GameObject {
	public:
		AudioPlayer* audioPlayer;

		GameObjectAudio(Audio* audio, bool loop = false);
		~GameObjectAudio() override;
		void Draw() override;
		void Update() override;
		void Play();
		void Stop();
		void SetLoop(bool loop);
		void SetVolume(float volume);
		void SetSpeed(float speed);
		bool IsFinished() const;
	};

} // namespace MG

#endif

