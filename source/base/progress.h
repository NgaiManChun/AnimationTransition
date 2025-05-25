// =======================================================
// progress.h
// 
// 0.0fから目的値まで累計して、
// 現在値と目的値の比率0.0f～1.0fとして使えるクラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/07/06
// 
// 2024/12/27 「operator =」追加
// =======================================================
#ifndef _PROGRESS_H
#define _PROGRESS_H

class Progress {
private:
	float _value;		// 現在値
	float _duration;	// 目的値、マイナス不可

public:
	// パブリック変数
	bool loop;			// 現在値が目的値超えた時ループさせるか

	// パブリック関数
	Progress() :
		Progress(0.0f, false)
	{}
	Progress(float duration, bool loop) :
		_value(0.0f),
		_duration(duration),
		loop(loop)
	{}
	void SetValue(float value);
	float GetValue();
	void SetDuration(float duration);
	float GetDuration();
	operator float() const;
	void operator =(const float t);

	// 現在値に加算する関数
	float IncreaseValue(float value);
	
};

#endif