// =======================================================
// audioToolDX.h
// 
// DirectX環境のオーディオプレイヤー
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _AUDIO_TOOL_DX_H
#define _AUDIO_TOOL_DX_H

#include "audioTool.h"
#include "resourceTool.h"
#include <xaudio2.h>

namespace MG {
	// =======================================================
	// DirectX環境のオーディオプレイヤー
	// =======================================================
	class AudioPlayerDX : public AudioPlayer {
	protected:
		IXAudio2SourceVoice* sourceVoice;
	public:
		AudioPlayerDX(IXAudio2SourceVoice* sourceVoice, Audio* audio, bool loop = false);
		~AudioPlayerDX();
		void Start();
		void Stop();
		void SetLoop(bool loop);
		void SetVolume(float volume);
		void SetSpeed(float speed);
		bool IsFinished() const;
	};


	// =======================================================
	// DirectX環境のオーディオツール
	// =======================================================
	class AudioToolDX : public AudioTool {
	private:
		IXAudio2* m_Xaudio;
		IXAudio2MasteringVoice* m_MasteringVoice;
	public:
		AudioToolDX();
		~AudioToolDX();
		IXAudio2* GetXAudio();
		AudioPlayer* CreateAudioPlayer(Audio* audio, bool loop = false);
	};

} // namespace MG

#endif 