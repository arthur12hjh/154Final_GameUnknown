#pragma once

#include "MapTool_Defines.h"
#include "Base.h"

/* 1. 다음 레벨에 대한 자원을 로드한다. */

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool_Map)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();
	void Output();

public:
	_bool isFinished() const {
		return m_isFinished;
	}

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL				m_eNextLevelID = { LEVEL::END };

	HANDLE				m_hThread = {};

	_wstring			m_strMessage = {};
	_bool				m_isFinished = { false };

	CRITICAL_SECTION	m_CriticalSection = {};

	CGameInstance* m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_Village();


	HRESULT Loading_For_Desert();
	HRESULT Loading_For_Desert_Building_Ruin(void* pArg);
	HRESULT Loading_For_Desert_Canyon1(void* pArg);
	HRESULT Loading_For_Desert_Canyon1_1(void* pArg);
	HRESULT Loading_For_Desert_Canyon1_2(void* pArg);
	HRESULT Loading_For_Desert_Canyon2(void* pArg);
	HRESULT Loading_For_Desert_Canyon2_1(void* pArg);
	HRESULT Loading_For_Desert_Canyon2_2(void* pArg);
	HRESULT Loading_For_Desert_Deco(void* pArg);
	HRESULT Loading_For_Desert_Archi(void* pArg);
	

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END