#include "BlendObject.h"
#include "GameInstance.h"

CBlendObject::CBlendObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CBlendObject::CBlendObject(const CBlendObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBlendObject::Priority_Update(_float fTimeDelta)
{
}

void CBlendObject::Update(_float fTimeDelta)
{
}

void CBlendObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}

void CBlendObject::Compute_Depth()
{
	_vector		vCamPosition = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

	m_fDepth = XMVectorGetX(XMVector3Length(vCamPosition - m_pTransformCom->Get_State(STATE::POSITION)));
}

void CBlendObject::Free()
{
	__super::Free();


}
