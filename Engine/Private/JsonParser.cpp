#include "JsonParser.h"

void CJsonParser::ReadJsonData(const char* pFilePath, Json& OutData)
{
	ios_base::openmode flag;
	flag = ios::in;

	ifstream ifs(pFilePath);
	if (ifs.is_open())
	{
		ifs >> OutData;
	}
	ifs.close();
}

void CJsonParser::SaveJsonData(const char* pFilePath, Json& OutData)
{
	ios_base::openmode flag;
	flag = ios::out | ios::trunc;

	ofstream ofs(pFilePath, flag);
	if (ofs.is_open())
	{
		ofs << OutData.dump(4);
	}
	ofs.close();
}
