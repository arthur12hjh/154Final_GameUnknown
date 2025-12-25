#pragma once

#include "Client_Defines.h"
#include "GameStruct.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CShader;
class CReserveDeferred;
NS_END

NS_BEGIN(Client)

class CShaderManager : public CBase
{
private:
	CShaderManager();
	virtual ~CShaderManager() = default;


public:
	HRESULT		Initialize();
	HRESULT		Add_Shader(LEVEL eLevelID, const _wstring& strShaderTag, class CShader* pShader);

	class CShader* Get_Shader(LEVEL eLevelID, const _wstring& strShaderTag);

	void Update(_float fTimeDelta);
	void Clear(LEVEL eLevelID);
	HRESULT Bind_CamInfo(LEVEL eLevelID);

	HRESULT Add_ReserveDeferred(const _wstring& strReserveDeferredTag, class CReserveDeferred* pReserveDeferred = nullptr);
	void Set_Active_ReserveDeferred(const _wstring& strReserveDeferredTag, _bool bFlag);
	void Set_Desc_ReserveDeferred(const _wstring& strReserveDeferredTag, void* pArg);

private:
	CGameInstance* m_pGameInstance = { nullptr };
	map<_wstring, class CShader*>* m_Shaders = {};
	map<_wstring, class CReserveDeferred*> m_ReserveDeferredShaders = {};

public:
	static CShaderManager* Create();
	virtual void Free() override;
};

NS_END