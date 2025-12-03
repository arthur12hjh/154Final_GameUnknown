#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)

/* 플레이어용 파트 오브젝트 */
class CPlayer_Parts abstract : public CPartObject
{
protected:
	CPlayer_Parts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Parts(const CPlayer_Parts& rhs);
	virtual ~CPlayer_Parts() = default;

public:

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void			Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)override {};

protected:
	class CGameManager* m_pGameManager = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

NS_END