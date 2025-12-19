#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBone;
class CRigidBody;
class CJointChain;
NS_END

NS_BEGIN(Client)

class CPonyTail_Player final : public CPlayer_Parts
{
public:
	typedef struct tagPonyTail_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
		void* pBodyPtr = { nullptr };
	} PONYTAIL_PLAYER_DESC;

private:
	CPonyTail_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPonyTail_Player(const CPonyTail_Player& Prototype);
	virtual ~CPonyTail_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	CModel* m_pBodyModelCom = { nullptr };

	class CJointChain* m_pJointChain = { nullptr };
	//피직스로 흔들리는 연산을 해주기 위함.
	class CRigidBody* m_pHairRoot = { nullptr };
	vector<pair<_wstring ,class CRigidBody*>> m_HairLinks = {};

	const _float4x4* m_pHairRootBone = { nullptr };
	JOINT_CHAIN_DESC* m_tRootDesc = { nullptr };
	JOINT_CHAIN_DESC* m_tLinkDesc = { nullptr };
	_int			 m_iBoneJointCount = { 1 };

	vector<PxTransform> m_vecHairActorToBone;  // Actor 로컬에서 Bone 로컬로 가는 오프셋(월드 기준으로 계산해도 됨)
	vector<_float3>     m_vecHairBoneScale;    // 본 스케일 보존(볼륨 죽는 문제도 같이 잡음)
	_bool               m_isHairBindInit = false;
private:
	HRESULT Ready_Components();
	HRESULT Ready_HairJoints();
	HRESULT Ready_RootHair();
	HRESULT Ready_HairBoneMapping();
	HRESULT Ready_ChildHair();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BoneToPartBody(void* pArg);

	void Sync_BonesByJoint();

public:
	static CPonyTail_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END