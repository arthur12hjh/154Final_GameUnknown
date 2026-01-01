#pragma once

#include "Character.h"
#include "ClientStruct.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)
class CCinematicObject : public CCharacter
{
public:
	typedef struct CinematicObjectDesc : CGameObject::GAMEOBJECT_DESC
	{
		_wstring			szObjectTag;
	} CINEMATICOBJECT_DESC;

protected:
	CCinematicObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicObject(const CCinematicObject& Prototype);
	virtual ~CCinematicObject() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;

	virtual HRESULT ActiveCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc) { m_bIsActive = TRUE; return S_OK; }
	virtual HRESULT PlayCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc) { m_bIsActive = TRUE; m_iCinematicCode = CinematicNodeDesc.iActiveIndex; return S_OK; }

	_wstring Get_ObjectTag() const { return m_szObjectTag; }

	virtual HRESULT Set_Cinematic_Object(const ANIM_NOTIFY& NotifyReference);

protected:
	_wstring m_szObjectTag;

	_int m_iCinematicCode = -1;

public:
	static CCinematicObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END