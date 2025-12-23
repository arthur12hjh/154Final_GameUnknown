#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_Player final : public CPlayer_Parts
{
public:
	typedef struct tagBody_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
	} BODY_PLAYER_DESC;

	enum class BODY_MATERIAL {
		//이 4개는 그냥 둬야함
		DEFAULT, SHADOW, RIMLIGHT, MOTIONBLUR, ORSS, END
	};

private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	_bool isFinish_Att();

	virtual void			Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference) override;
	virtual void			Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef);

	HRESULT Mapping_Shader_Material(_uint iIdx);
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_MotionBlur() override;

private:
	CCollider*			m_pColliderCom = { nullptr };
	_bool				m_isAnimFinish = { false };	
	_bool				m_bIsEnableCollider = { false };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END