#pragma once

#include "Base.h"

NS_BEGIN(Engine)

/*
* bin Parser
* - bin 바이너리파일을 읽어와서 Model 구조체 형태로 반환해주는 역할
* - Model 구조체를 bin 바이너리파일로 써주는 역할
*
*/


class CBinParser final : public CBase
{
private:
	CBinParser();
	virtual ~CBinParser() = default;

public:
	HRESULT Initialize();
	HRESULT ReadBin(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);			// 구버전 파서
	HRESULT ReadBinx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);
	HRESULT ReadBinMorph(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);		// 셰이프 키(모프 타겟) 전용 bin파일
	HRESULT WriteBin(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);			// 구버전 파서
	HRESULT WriteBinx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);
	HRESULT WriteBinMorph(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);		// 셰이프 키(모프 타겟) 전용 bin파일

private:
	char* ReadString(ifstream& fileBinaryStream);
	XMFLOAT3 ReadFloat3(ifstream& fileBinaryStream);
	XMFLOAT2 ReadFloat2(ifstream& fileBinaryStream);
	_uint ReadUint(ifstream& fileBinaryStream);
	unsigned int Read_BinNode(ifstream& fileBinaryStream, binModel* pModel);

	void WriteString(ofstream& fileBinaryStream, _char* pStr);
	void WriteFloat3(ofstream& fileBinaryStream, _float3 vTmp);
	void WriteFloat2(ofstream& fileBinaryStream, _float2 vTmp);
	void WriteUint(ofstream& fileBinaryStream, _uint iTmp);
	void Write_BinNode(ofstream& fileBinaryStream, binModel* pModel, binNode* pNode);


public:
	static CBinParser* Create();
	virtual void Free();
};
NS_END