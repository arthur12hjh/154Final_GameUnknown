#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
class CShader;
NS_END

/*
타겟을 추적하는 빛.
홍련이랑 이브한테 달아놓을 예정입니다.
*/

NS_BEGIN(Client)

class CTargetLight final : public CBase
{
public:
	typedef struct tagTargetLightDesc {
		LIGHT_DESC tLightDesc = {};
		class CGameObject* pTarget = { nullptr };
		_bool      isCinematic = { false };
	} TARGETLIGHT_DESC;
 
private:
	CTargetLight();
	virtual ~CTargetLight() = default;

public:
	void Set_Active(VISIBILITY eVisibility); 
	void Set_Diffuse(_vector vDiffuse);
	void Set_Range(_float fRange);

public:
	HRESULT Initialize(void* pArg);
	void	Chase_Target();

private:
	//외부에서 조절해주려면 들고있긴 해야하니까..
	class CGameInstance* m_pGameInstance = { nullptr };
	CGameObject* m_pTarget = { nullptr };
	CLight*		 m_pLightCom = { nullptr };
	_bool		 m_isActive = { true };
	_bool        m_isCinematic = { false };

private:
	HRESULT Setting_Desc(TARGETLIGHT_DESC* pDesc);

public:
	static CTargetLight* Create(void* pArg);
	virtual void Free() override;
};

NS_END
