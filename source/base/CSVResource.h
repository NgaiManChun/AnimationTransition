// =======================================================
// CSVResource.h
// 
// CSVファイルを読み込み関数
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _CSV_RESOURCE_H
#define _CSV_RESOURCE_H
#include <string>
#include <map>
#include <vector>

typedef std::vector<std::string> D_ROW;
typedef std::vector<D_ROW> D_TABLE;

typedef std::map<std::string, std::string> D_KVPAIR;
typedef std::map<std::string, D_KVPAIR> D_KVTABLE;

void ReadCSVFromResource(int resourceId, D_TABLE& result);
void ReadCSVFromPath(std::string path, D_TABLE& result);
void ReadCSVData(std::string& csvData, D_TABLE& result);
void TableToKeyValuePair(const std::string key, D_TABLE& data_table, D_KVTABLE& result);

#endif
