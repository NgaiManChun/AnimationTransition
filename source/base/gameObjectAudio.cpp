// =======================================================
// gameObjectAudio.cpp
// 
// オーディオゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/18
// =======================================================
#include "gameObjectAudio.h"
#include "audioTool.h"

namespace MG {

	GameObjectAudio::GameObjectAudio(Audio* audio, bool loop) : GameObject()
	{
		audioPlayer = GetAudioTool()->CreateAudioPlayer(audio, loop);
	}

	void GameObjectAudio::Draw() {}
	void GameObjectAudio::Update() {}

	GameObjectAudio::~GameObjectAudio()
	{
		delete audioPlayer;
		audioPlayer = nullptr;
	}

	void GameObjectAudio::Play()
	{
		audioPlayer->Start();
	}

	void GameObjectAudio::Stop()
	{
		audioPlayer->Stop();
	}

	void GameObjectAudio::SetLoop(bool loop)
	{
		audioPlayer->SetLoop(loop);
	}

	void GameObjectAudio::SetVolume(float volume)
	{
		audioPlayer->SetVolume(volume);
	}

	void GameObjectAudio::SetSpeed(float speed)
	{
		audioPlayer->SetSpeed(speed);
	}

	bool GameObjectAudio::IsFinished() const
	{
		return audioPlayer->IsFinished();
	}

} // namespace MG