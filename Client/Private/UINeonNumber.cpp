#include "pch.h"
#include "UINeonNumber.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "UIHUD.h"

#include "UIInstanceBuffer.h"

CUINeonNumber::CUINeonNumber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUINeonNumber::CUINeonNumber(const CUINeonNumber& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUINeonNumber::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUINeonNumber::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUINeonNumber::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUINeonNumber::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
		m_iCombo += (_uint)m_pGameInstance->Random(1.f, 4.f);
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_BACKSPACE))
		m_iCombo = 0;
	
	if(m_iCombo != m_iPrevCombo)
	{
		m_fScaleRatio = 1.2f;
		m_iPrevCombo = m_iCombo;
	}

	if (m_fScaleRatio > 1.f)
	{
		m_fScaleRatio -= fTimeDelta * 2.f;
		if (m_fScaleRatio < 1.f)
			m_fScaleRatio = 1.f;
	}
}

void CUINeonNumber::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (FAILED(SetUp_NeonNumber()))
		return;
}

HRESULT CUINeonNumber::Render()
{
	if (m_iCombo < 5)
		return S_OK;

	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::COMBO))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if(FAILED(Render_NeonNumber()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUINeonNumber::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC IconInstanceDesc{};
	IconInstanceDesc.iNumInstance = 3;
	IconInstanceDesc.vAtlasIndex = _float2(5.f, 2.f);
	IconInstanceDesc.vUVAtlasSize = _float4(1.f, 1.f, 1.f, 1.f);
	IconInstanceDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_NeonNumber"), reinterpret_cast<CComponent**>(&m_pUINeonNumberBufferCom), &IconInstanceDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_NeonNumber"),
		TEXT("Com_Texture_UI_NeonNumber"), reinterpret_cast<CComponent**>(&m_pNeonNumberTextureCom))))
		return E_FAIL;
	
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Combo"),
		TEXT("Com_Texture_UI_Combo"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUINeonNumber::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_fScaleRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUINeonNumber::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUINeonNumber::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}

HRESULT CUINeonNumber::Render_NeonNumber()
{
	if (FAILED(Bind_NeonNumberShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::NEON_NUMBER))))
		return E_FAIL;

	if (FAILED(m_pUINeonNumberBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUINeonNumberBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUINeonNumber::SetUp_NeonNumber()
{
	m_NeonNumberInstances.clear();
	m_NeonNumberInstances.reserve(3);

	_int ATLAS_COL = 3;
	_int ATLAS_ROW = 1;

	_float fRatio = 128.f / 256.f;

	_float2 vUISize = _float2{ 128.f, 128.f };
	_float fUIWidth = vUISize.x * fRatio * (ATLAS_COL - 1);
	_float fUIHeight = vUISize.y * fRatio * (ATLAS_ROW - 1);

	_float2 vScale = _float2{ (vUISize.x / m_tUIDesc.fSizeX) * m_fScaleRatio, (vUISize.y / (m_tUIDesc.fSizeY)) * m_fScaleRatio };
	_float fWidth = vScale.x * fRatio * (ATLAS_COL - 1);
	_float fHeight = vScale.y * fRatio * (ATLAS_ROW - 1);

	for (size_t i = 0; i < ATLAS_COL; ++i)
	{
		_float2 vPos = _float2{ (vScale.x * fRatio * (i % ATLAS_COL)) - (fWidth * 0.5f), -((vScale.y * fRatio * (i / ATLAS_COL)) - (fHeight * 0.5f) + (20.f / 188.f)) };

		_uint iCombo = 0;

		if (i == 0)
		{
			iCombo = ((m_iCombo / 100) % 10);
			vPos.x -= 0.1f;
		}
		else if (i == 1)
			iCombo = ((m_iCombo / 10) % 10);
		else if (i == 2)
		{
			iCombo = (m_iCombo % 10);
			vPos.x += 0.1f;
		}

		_uint col = iCombo % 5;
		_uint row = iCombo / 5;

		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4{ 0.f, 0.f, vPos.x, vPos.y };

		inst.vAtlasIndex = _float4{ (_float)col, (_float)row, 0.f, 0.f };

		m_NeonNumberInstances.push_back(inst);
	}

	m_pUINeonNumberBufferCom->Update_Instance(m_NeonNumberInstances);

	return S_OK;
}

HRESULT CUINeonNumber::Bind_NeonNumberShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pNeonNumberTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	_float2 atlasCount = { 5.f, 2.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &atlasCount, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CUINeonNumber* CUINeonNumber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUINeonNumber* pInstance = new CUINeonNumber(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUINeonNumber::Clone(void* pArg)
{
	CUINeonNumber* pInstance = new CUINeonNumber(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUINeonNumber");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUINeonNumber::Free()
{
	__super::Free();

	Safe_Release(m_pUINeonNumberBufferCom);

	Safe_Release(m_pNeonNumberTextureCom);
}
