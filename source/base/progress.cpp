// =======================================================
// progress.cpp
// 
// 0.0fから目的値まで累計して、
// 現在値と目的値の比率0.0f～1.0fとして使えるクラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/07/06
// =======================================================
#include "progress.h"

void Progress::SetValue(float value) 
{
	_value = value;
	
	// 0.0f～目的値まで、はみ出させない
	if (loop) {
		while (_value > _duration) {
			_value -= _duration;
		}
		while (_value < 0) {
			_value += _duration;
		}
	}
	else if (_value > _duration) {
		_value = _duration;
	}
	else if (_value < 0.0f) {
		_value = 0.0f;
	}
}

float Progress::GetValue() 
{
	return _value;
}

void Progress::SetDuration(float duration) 
{
	if (_duration > 0.0f) {
		_duration = duration;
		SetValue(_value);	// はみ出し処理
	}
}

float Progress::GetDuration() {
	return _duration;
}

// =======================================================
// 現在値に加算する関数
// =======================================================
float Progress::IncreaseValue(float value) 
{
	SetValue(GetValue() + value);
	return _value / _duration;
}

// =======================================================
// floatとして使う時の挙動
// =======================================================
Progress::operator float() const
{
	return _value / _duration;
}

void Progress::operator=(const float t)
{
	 SetValue(_duration * t);
}
