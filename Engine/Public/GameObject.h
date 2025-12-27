#pragma once

#include "Transform.h"

NS_BEGIN(Engine)
class CTransform;
class CCollider;
class CVIBuffer;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		CGameObject*		pParent = nullptr;
		_bool				bIsApplyTransform;
		_bool				bIsQuaternion = false;

		_float3				vScale;
		_float4				vRotation;
		_float3				vPosition;
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);

	virtual void		Priority_Update(_float fTimeDelta);
	virtual void		Update(_float fTimeDelta);
	virtual void		Late_Update(_float fTimeDelta);

	virtual HRESULT		Render();
	virtual HRESULT		Render_Shadow() { return S_OK; }
	virtual HRESULT		Render_MotionBlur() { return S_OK; }
	virtual void		PlayDeadEffect() {}

public:
	void				Set_Dead(_bool isDead) { m_isDead = isDead; }
	_bool isDead() const {
		return m_isDead;
	}

	class CComponent*			Find_Component(const _wstring& strComponentTag);

	void						Set_Team(OBJECT_TEAM eTeam);
	void						SetVisibility(VISIBILITY eVisiblility) {
		m_eVisibility = eVisiblility;
	}

	virtual void SetActive(_bool bIsActive) {
		m_bIsActive = bIsActive;
	}

	virtual void SetActive() {
		m_bIsActive = !m_bIsActive;
	}

	const VISIBILITY&								GetVisibility() { return m_eVisibility;	}
	CTransform*										GetTransform() { return m_pTransformCom; }

	void											SetParent(CGameObject* pParent);
	CGameObject*									GetParent();

	// 오브젝트의 속성을 받아올수 있음
	OBJECT_TEAM										GetTeam() { return m_eTeam; }

	_bool											IsFrustomCulling();
	_float											Get_Depth();
	CCollider*										GetCullingCollider() { return m_pCullingCollider; }
	const map<const _wstring, class CComponent*>*	GetAllComponents() { return &m_Components; }

	void										Set_Occlusion_Culling_Result(_bool isVisible);
	void										Set_Occlusion_CoolDown(_int iCoolDown) { m_iOcclusionCooldown = iCoolDown; }
	_int										Get_Occlusion_CoolDown() const { return m_iOcclusionCooldown; }

	_bool										Get_Occluder_Disabled() { return m_bIsOccluderDisabled; }

protected:
	int											m_iObjectID;
	OBJECT_TEAM									m_eTeam = { OBJECT_TEAM::END };
	_float										m_fDepth = {};

	ID3D11Device*								m_pDevice = { nullptr };
	ID3D11DeviceContext*						m_pContext = { nullptr };

	class CGameInstance*						m_pGameInstance = { nullptr };
	CGameObject*								m_pParent = { nullptr };

	VISIBILITY									m_eVisibility = { VISIBILITY::VISIBLE };

	CTransform*									m_pTransformCom = { nullptr };
	CCollider*									m_pCullingCollider = { nullptr };

	_bool										m_isDead = { false };
	_bool										m_bIsPrevVisibility = { true };
	_bool										m_bIsActive = { true };

	map<const _wstring, class CComponent*>		m_Components;
	_int										m_iOcclusionCooldown = 0;
	_bool										m_bIsOccluderDisabled = { false };	// true	 : 가려지기만 하는애들
																					// false : 가려지고 가리기도 함

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END