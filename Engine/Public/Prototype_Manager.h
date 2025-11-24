#pragma once

#include "Engine_Component.h"

/* 1. 객체들의 원형을 레벨별로 보관한다. */
/* 1. 컴포넌트들의 원형을 레벨별로 보관한다. */
		
NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype, CBase** ppOut = nullptr);
	HRESULT Add_SkeletalPrototype(_uint iLevelIndex, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strPrototypeTag, const _char* pModelFilePath, const string& strSkeletalPath, _fmatrix PreTransformMatrix);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	const map<const _wstring, class CBase*>* Get_Prototypes_InLevel(_uint iLevelIndex);
	void Clear(_uint iLevelIndex);

	class CBase* Get_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

private:
	class CGameInstance*						m_pGameInstance = { nullptr };

	_uint										m_iNumLevels = { };
	map<const _wstring, class CBase*>*			m_pPrototypes = { nullptr };

private:
	class CBase* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
	static CPrototype_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END