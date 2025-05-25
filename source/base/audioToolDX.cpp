// =======================================================
// audioToolDX.cpp
// 
// DirectX環境のオーディオプレイヤー
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "audioToolDX.h"
#include "resourceToolDX.h"

namespace MG {
	AudioPlayerDX::AudioPlayerDX(IXAudio2SourceVoice* sourceVoice, Audio* audio, bool loop)
		: AudioPlayer(audio, loop), sourceVoice(sourceVoice) {}
	IXAudio2* AudioToolDX::GetXAudio() { return m_Xaudio; }

	AudioPlayerDX::~AudioPlayerDX()
	{
		sourceVoice->Stop();
		sourceVoice->DestroyVoice();
	}

	void AudioPlayerDX::Start()
	{
		Stop();
		// バッファ設定
		XAUDIO2_BUFFER bufinfo;
		AudioDX* audio = (AudioDX*)this->audio;
		memset(&bufinfo, 0x00, sizeof(bufinfo));
		bufinfo.AudioBytes = audio->length;
		bufinfo.pAudioData = audio->soundData;
		bufinfo.PlayBegin = 0;
		bufinfo.PlayLength = audio->playLength;

		// ループ設定
		if (loop)
		{
			bufinfo.LoopBegin = 0;
			bufinfo.LoopLength = audio->playLength;
			bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
		}

		sourceVoice->SubmitSourceBuffer(&bufinfo, NULL);
		sourceVoice->Start(0);
	}

	void AudioPlayerDX::Stop()
	{
		sourceVoice->Stop();
		sourceVoice->FlushSourceBuffers();
	}

	void AudioPlayerDX::SetLoop(bool loop)
	{
		if (this->loop != loop) {
			this->loop = loop;
			if (loop) {
				XAUDIO2_BUFFER bufinfo;
				memset(&bufinfo, 0x00, sizeof(bufinfo));
				AudioDX* audio = (AudioDX*)this->audio;

				bufinfo.AudioBytes = audio->length;
				bufinfo.pAudioData = audio->soundData;
				bufinfo.PlayBegin = 0;
				bufinfo.PlayLength = audio->playLength;

				bufinfo.LoopBegin = 0;
				bufinfo.LoopLength = audio->playLength;
				bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;

				sourceVoice->SubmitSourceBuffer(&bufinfo, NULL);
			}
			else {
				sourceVoice->ExitLoop();
			}
		}
	}

	void AudioPlayerDX::SetVolume(float volume)
	{
		sourceVoice->SetVolume(volume);
	}

	void AudioPlayerDX::SetSpeed(float speed)
	{
		sourceVoice->SetFrequencyRatio(speed);
	}

	bool AudioPlayerDX::IsFinished() const
	{
		XAUDIO2_VOICE_STATE state{ 0 };
		sourceVoice->GetState(&state);
		return state.BuffersQueued == 0;
	}

	AudioToolDX::AudioToolDX()
	{
		// XAudio生成
		XAudio2Create(&m_Xaudio, 0);

		// マスタリングボイス生成
		m_Xaudio->CreateMasteringVoice(&m_MasteringVoice);
	}

	AudioToolDX::~AudioToolDX()
	{
		m_MasteringVoice->DestroyVoice();
		m_Xaudio->Release();
	}

	AudioPlayer* AudioToolDX::CreateAudioPlayer(Audio* audio, bool loop)
	{
		IXAudio2SourceVoice* sourceVoice;
		AudioDX* audioDX = (AudioDX*)audio;
		m_Xaudio->CreateSourceVoice(&sourceVoice, &audioDX->waveFormatEX);

		sourceVoice->Stop();
		sourceVoice->FlushSourceBuffers();

		// バッファ設定
		XAUDIO2_BUFFER bufinfo;

		memset(&bufinfo, 0x00, sizeof(bufinfo));
		bufinfo.AudioBytes = audioDX->length;
		bufinfo.pAudioData = audioDX->soundData;
		bufinfo.PlayBegin = 0;
		bufinfo.PlayLength = audioDX->playLength;

		// ループ設定
		if (loop)
		{
			bufinfo.LoopBegin = 0;
			bufinfo.LoopLength = audioDX->playLength;
			bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
		}

		sourceVoice->SubmitSourceBuffer(&bufinfo, NULL);
		return new AudioPlayerDX(sourceVoice, audio, loop);
	}

} // namespace MG