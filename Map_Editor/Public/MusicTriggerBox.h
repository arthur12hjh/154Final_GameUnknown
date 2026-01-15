#pragma once
#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
class CShader;
NS_END

NS_BEGIN(Tool_Map)
class CMusicTriggerBox : public CGameObject
{
public:
	typedef struct TriggerBoxDesc : public GAMEOBJECT_DESC
	{
	}MUSIC_TRIGGER_BOX_DESC;

private:
	CMusicTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMusicTriggerBox(const CMusicTriggerBox& Prototype);
	virtual ~CMusicTriggerBox() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

private:
	CCollider* m_pCollider = { nullptr };
	_bool					m_bIsRender = { false };

private:
	HRESULT					Ready_Components();

public:
	static CMusicTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END