#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRigidBody;
class CJointChain;
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
	CCollider*					m_pColliderCom = { nullptr };

	_bool				m_isAnimFinish = { false };	
	_bool				m_bIsEnableCollider = { false };

private:
	HRESULT				Ready_Components();
	HRESULT				Bind_ShaderResources();

	void				Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

private:
	HRESULT				Ready_VerticalJoints();
	HRESULT				Ready_HorizontalJoints();
	HRESULT				Ready_SkirtBoneOrigin();
	HRESULT				Ready_ThighRigidBodies();
	void				Sync_BonesByJoint();

private:
	//수직으로 연결된 조인트 체인들. 
	vector<pair<const _float4x4*, class CRigidBody*>>	m_RootRigidBodies = {};
	vector<class CJointChain*>  m_VerticalJointChains = {};
	//수평으로 연결된 조인트 체인들.
	vector<class CJointChain*>  m_HorizontalJointChains = {};
	// 치마 리지드 바디들. 본 이름과 매핑시켜놨으니..
	// 본 이름으로 매트릭스 찾고, 리지드 바디랑 합성시키는 과정이 똑같이 필요함.
	vector<pair<_wstring, class CRigidBody*>> m_SkirtRigidBodies = {};

	// Actor 로컬에서 Bone 로컬로 가는 오프셋
	vector<pair<_wstring, PxTransform>>  m_SkirtActorToBones = {};
	// 본 스케일 보존
	vector<pair<_wstring, _float3>>      m_SkirtBoneScales = {};
	// 허벅지 콜라이더 2개 (본 이름을 키로 해주자)
	vector<pair<const _float4x4*, class CRigidBody*>> m_ThighRigidBodies = {};

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END