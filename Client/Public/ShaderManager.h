#pragma once

#include "Client_Defines.h"
#include "GameStruct.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CShader;
NS_END

NS_BEGIN(Client)

/* 이름은 락온인데 일단은 플레이어 FSM 3개 바꿔주는 녀석임.. */

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

private:
	CGameInstance* m_pGameInstance = { nullptr };
	map<_wstring, class CShader*>*	m_Shaders;

public:
	static CShaderManager* Create();
	virtual void Free() override;
};

NS_END