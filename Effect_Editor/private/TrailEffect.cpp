#include "pch.h"
#include "TrailEffect.h"
#include "Trail.h"
#include "TrailData.h"

#include "GameInstance.h"

CTrailEffect::CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTrailEffect::CTrailEffect(const CTrailEffect& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTrailEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrailEffect::Initialize(void* pArg)
{
	GAMEOBJECT_DESC desc = {};
	desc.fRotationPerSec = 1.f;
	desc.fSpeedPerSec = 15.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	CTrail::TRAILHIGHLOW TrailDesc{};
	TrailDesc.vHigh = { 0.f, 1.f, 0.f,0.f };
	TrailDesc.vLow = { 0.f, -1.f, 0.f,0.f };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TOOL), TEXT("Prototype_Component_Trail"),
		TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pTrail), &TrailDesc)))
		return E_FAIL;
	Refresh();
	return S_OK;
}

void CTrailEffect::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
	m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * m_fSpeed);

	m_pTrail->Update_Trail(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), fTimeDelta, true);
}

void CTrailEffect::Late_Update(_float fTimeDelta)
{
}

HRESULT CTrailEffect::Render(CTrailData* pTrailData)
{
	if (FAILED(Bind_ShaderResources(pTrailData->Get_Data())))
		return E_FAIL;
	if (FAILED(pTrailData->Bind_Texture(m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pTrail->Render()))
		return E_FAIL;
	return S_OK;
}

void CTrailEffect::Refresh()
{
	Safe_Release(m_pShaderCom);
	m_fTime = 0;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0, 0, 0, 1));
	m_pTransformCom->Rotation(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(45.f));
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrailEffect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
}

HRESULT CTrailEffect::Bind_ShaderResources(CTrailData::TRAIL_DATA tData)
{
	//카메라의 여러 정보들을 받아올 수 있어 여기서 fFar 받아올 수 있음.
	//카메라 Far 값을 받아오는 변수는 "g_fFar" 로 세팅해줘. 
	//클라에선 g_fFar 알아서 세팅해주니까 걱정안해도 돼.
	CAMERA_INFO CamInfo = m_pGameInstance->Get_CurrentCamInfo();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &CamInfo.fFar, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUV", &tData.fMaskUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUVSpeed", &tData.fMaskUVSpeed, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUVSize", &tData.fMaskUVSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUV", &tData.fDiffuseUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUVSpeed", &tData.fDiffuseUVSpeed, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUVSize", &tData.fDiffuseUVSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUV", &tData.fDissolveUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUVSpeed", &tData.fDissolveUVSpeed, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUVSize", &tData.fDissolveUVSize, sizeof(_float2))))
		return E_FAIL;
	return S_OK;
}

CTrailEffect* CTrailEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrailEffect* pInstance = new CTrailEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrailEffect::Clone(void* pArg)
{
	CTrailEffect* pInstance = new CTrailEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrailEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrailEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTrail);
}
