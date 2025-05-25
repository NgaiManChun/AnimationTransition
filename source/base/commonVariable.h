// =======================================================
// commonVariable.h
// 
// プログラム全体に共有する変数
// 
// 作者：魏文俊（ガイ　マンチュン）　2025/02/02
// =======================================================
#ifndef _COMMON_VARIABLE_H
#define _COMMON_VARIABLE_H

#include <map>
#include <unordered_map>
#include <string>

#define IfCommonBool(key, value) ((IsContainBool(key)) ? GetCommonBool(key) : value)
#define IfCommonFloat(key, value) ((IsContainFloat(key)) ? GetCommonFloat(key) : value)
#define IfCommonInt(key, value) ((IsContainInt(key)) ? GetCommonInt(key) : value)
#define IfCommonPointer(key, value) ((IsContainPointer(key)) ? GetCommonPointer(key) : value)
#define IfCommonString(key, value) ((IsContainString(key)) ? GetCommonString(key) : value)

namespace MG {
	void SetCommonBool(const std::string& key, bool value);
	bool GetCommonBool(const std::string& key);
	bool IsContainBool(const std::string& key);

	void SetCommonFloat(const std::string& key, float value);
	float GetCommonFloat(const std::string& key);
	bool IsContainFloat(const std::string& key);

	void SetCommonInt(const std::string& key, int value);
	int GetCommonInt(const std::string& key);
	bool IsContainInt(const std::string& key);

	void SetCommonPointer(const std::string& key, void* value);
	void* GetCommonPointer(const std::string& key);
	bool IsContainPointer(const std::string& key);

	void SetCommonString(const std::string& key, std::string value);
	std::string GetCommonString(const std::string& key);
	bool IsContainString(const std::string& key);
}

#endif