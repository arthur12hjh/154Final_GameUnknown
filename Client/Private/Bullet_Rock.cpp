#include "pch.h"
#include "Bullet_Rock.h"

#include "GameInstance.h"
#include "Character.h"

CBullet_Rock::CBullet_Rock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CBullet(pDevice, pContext)
{
}

CBullet_Rock::CBullet_Rock(const CBullet_Rock& Prototype) :
	CBullet(Prototype)
{
}

HRESULT CBullet_Rock::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBullet_Rock::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);
    if (FAILED(ADD_Components(*pDesc)))
        return E_FAIL;



	return S_OK;
}

void CBullet_Rock::Priority_Update(_float fTimeDelta)
{
}

void CBullet_Rock::Update(_float fTimeDelta)
{
	if (false == m_bIsAttachment)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta);
		// 여기서 베지어 Lerp
		if (m_bIsBezierLerp)
		{
			m_LerpTime.x += fTimeDelta;
			_float fRatio = m_LerpTime.x / m_LerpTime.y;
			if (0.7f >= fRatio)
			{
				
				m_pTransformCom->Set_State(STATE::POSITION, BezierCurve(5, m_vLerpPoints, fRatio));
			}
			else
				m_pTransformCom->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vProjectileDir), m_fSpeed);
		}
		else
			m_pTransformCom->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vProjectileDir), m_fSpeed);
	}

	Update_BulletCombinedMatrix();
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBullet_Rock::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 8000.f))
	{
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG

		m_pGameInstance->ADD_Collider(m_pColliderCom);
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
	else
		Set_Dead(true);
}

HRESULT CBullet_Rock::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CBullet_Rock::Shoot_Projectile(_vector vTargetPoint, _float fSpeed)
{
	__super::Shoot_Projectile(XMLoadFloat3(&m_vTargetPoint), fSpeed);
	m_LerpTime.x = 0.f;

	_matrix CombinedMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
	XMStoreFloat3(&m_vLerpPoints[0], CombinedMatrix.r[3]);

	_float fLength = XMVectorGetX(XMVector3Length(CombinedMatrix.r[3] - vTargetPoint));
	if (30 > fLength)
	{
		m_bIsBezierLerp = false;
		m_fSpeed = 50.f;
	}
	else
	{
		XMStoreFloat3(&m_vLerpPoints[4], vTargetPoint);
		_vector vBeziorCenter = XMVectorLerp(CombinedMatrix.r[3], vTargetPoint, 0.6f);
		vBeziorCenter.m128_f32[1] = CombinedMatrix.r[3].m128_f32[1];
		XMStoreFloat3(&m_vLerpPoints[1], vBeziorCenter);

		_vector vPoint = (vBeziorCenter + vTargetPoint) / 2.f;
		XMStoreFloat3(&m_vLerpPoints[2], vPoint);

		_vector vPoint2 = (vPoint + vTargetPoint) / 2.f;
		XMStoreFloat3(&m_vLerpPoints[3], vPoint2);
		m_bIsBezierLerp = true;
	}


}

void CBullet_Rock::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	// 여기서 일단 데미지 처리한다
	// 맞으면 맞는 모션 나오게
	DEFAULT_DAMAGE_DESC pDamageDesc = {};
	pDamageDesc.pAttacker = m_pParent;
	pDamageDesc.vHitPoint = vHitPoint;
	pDamageDesc.vHitDir = vHitDir;
	pDamageDesc.pSkillData = m_pSkillData;

	auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
	if(pCharacter)
		pCharacter->Damaged(&pDamageDesc);

	Set_Dead(true);
}

HRESULT CBullet_Rock::ADD_Components(BULLET_DESC& pDesc)
{
	// 모델 생성 하고
	// 충돌체 생성 하자
	/* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bullet_Rock1"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    COBBCollider::OBB_COLLIDER_DESC pOBBDesc = {};
    pOBBDesc.vSize = { 1.f, 1.f, 1.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &pOBBDesc)))
        return E_FAIL;

    m_pColliderCom->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });
	m_pColliderCom->SetColliderHitType(HIT_TYPE(pDesc.iHitType));

	m_pColliderCom->ADD_IgnoreObject(HIT_TYPE(pDesc.iHitType));
	m_pColliderCom->ADD_IgnoreObject(HIT_TYPE::SENCE);
	m_pColliderCom->ADD_IgnoreObject(HIT_TYPE::INTERACTION);

	return S_OK;
}

HRESULT CBullet_Rock::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}



CBullet_Rock* CBullet_Rock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet_Rock* pBullet_Rock = new CBullet_Rock(pDevice, pContext);
	if (FAILED(pBullet_Rock->Initialize_Prototype()))
	{
		Safe_Release(pBullet_Rock);
		MSG_BOX("Create Fail : Bullet Rock");
	}

	return pBullet_Rock;
}

CGameObject* CBullet_Rock::Clone(void* pArg)
{
	CBullet_Rock* pBullet_Rock = new CBullet_Rock(*this);
	if (FAILED(pBullet_Rock->Initialize(pArg)))
	{
		Safe_Release(pBullet_Rock);
		MSG_BOX("Clone Fail : Bullet Rock");
	}

	return pBullet_Rock;
}

void CBullet_Rock::Free()
{
	__super::Free();
}
