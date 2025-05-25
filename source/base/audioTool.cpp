// =======================================================
// audioTool.cpp
// 
// オーディオプレイヤー
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "audioTool.h"

namespace MG {
	std::string AudioPlayer::GetType() { return "AudioPlayer"; }
	AudioPlayer::AudioPlayer(Audio* audio, bool loop) : audio(audio), loop(loop) { }
	AudioPlayer::~AudioPlayer() {}
	void AudioPlayer::SetAudio(Audio* audio) { this->audio = audio; }
	void AudioPlayer::SetLoop(bool loop) { this->loop = loop; }
}
