#pragma once

#include "Base.h"

NS_BEGIN(Engine)

/*
* - 단순하게, FBX를 Assimp로 읽어서 binModel로 전달해주는 역할
*/

class CFbxParser final : public CBase
{
private:
	CFbxParser();
	virtual ~CFbxParser() = default;

public:
	HRESULT Initialize();
	HRESULT ReadFbx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);
	unsigned int Get_BinNodeIndex(const aiNode* pAINode, binModel* pModel);

private:
	//aiScene은 모델 정보를 갖는 파일, Assimp::Importer는 모델을 불러오는 객체이다.
	//aiScene은 모든 모델 정보를 갖고 있기에 엄청나게 무겁다.
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;



public:
	static CFbxParser* Create();
	virtual void Free();
};

NS_END