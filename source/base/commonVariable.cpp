#include "commonVariable.h"

namespace MG {
	static std::unordered_map<std::string, bool> _bool_vars;
	static std::unordered_map<std::string, float> _float_vars;
	static std::unordered_map<std::string, int> _int_vars;
	static std::unordered_map<std::string, void*> _pointer_vars;
	static std::unordered_map<std::string, std::string> _string_vars;

	void SetCommonBool(const std::string& key, bool value)
	{
		_bool_vars[key] = value;
	}

	bool GetCommonBool(const std::string& key)
	{
		return _bool_vars[key];
	}

	bool IsContainBool(const std::string& key)
	{
		return _bool_vars.count(key);
	}

	void SetCommonFloat(const std::string& key, float value)
	{
		_float_vars[key] = value;
	}

	float GetCommonFloat(const std::string& key)
	{
		return _float_vars[key];
	}

	bool IsContainFloat(const std::string& key)
	{
		return _float_vars.count(key);
	}

	void SetCommonInt(const std::string& key, int value)
	{
		_int_vars[key] = value;
	}

	int GetCommonInt(const std::string& key)
	{
		return _int_vars[key];
	}

	bool IsContainInt(const std::string& key)
	{
		return _int_vars.count(key);
	}

	void SetCommonPointer(const std::string& key, void* value)
	{
		_pointer_vars[key] = value;
	}

	void* GetCommonPointer(const std::string& key)
	{
		return _pointer_vars[key];
	}

	bool IsContainPointer(const std::string& key)
	{
		return _pointer_vars.count(key);
	}

	bool IsContainString(const std::string& key)
	{
		return false;
	}

	void SetCommonString(const std::string& key, std::string value)
	{
		_string_vars[key] = value.data();
	}

	std::string GetCommonString(const std::string& key)
	{
		return _string_vars[key].data();
	}

}