#include "pch.h"
#include "UIMonsterStamina.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Nayitba.h"

CUIMonsterStamina::CUIMonsterStamina(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIMonsterStamina::CUIMonsterStamina(const CUIMonsterStamina& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIMonsterStamina::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIMonsterStamina::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIMonsterStamina::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (!m_isRent)
	{
		m_iCurrentStamina = 0;
		m_iMaxStamina = 0;
	}
}

void CUIMonsterStamina::Update(_float fTimeDelta)
{
	auto pMonster = static_cast<CNaytiba*>(m_pParent->GetParent());
	if (pMonster)
	{
		if (pMonster->GetStaticMonsterData()->eNaytiba_Type > NAYTIBA_TYPE::ELITE)
			return;

		auto StaticDesc = pMonster->GetStaticMonsterData();
		auto CurDesc = pMonster->GetMonsterData();

		m_iCurrentStamina = CurDesc.iCurrentStamina;
		m_iMaxStamina = StaticDesc->iMaxStamina;

		// 여기서 사이즈, 위치 잡으면 될듯
		m_tUIDesc.fSizeX = (m_iMaxStamina * 10.f) + ((m_iMaxStamina - 1) * 2.f);

		_float4 vScale{ m_tUIDesc.fSizeX, m_pTransformCom->Get_Scale().y, m_pTransformCom->Get_Scale().z, 1.f };

		m_pTransformCom->Set_Scale(XMLoadFloat4(&vScale));
		m_tUIDesc.fOffsetX = -(static_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fSizeX * 0.5f) + (m_tUIDesc.fSizeX * 0.5f);
	}

	__super::Update(fTimeDelta);

#ifdef _DEBUG
	// 테스트 용
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_B))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && m_fTargetFill > 0.f)
			m_iCurrent -= 1;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && m_fTargetFill < 1.f)
			*m_iCurrentCount += 1;
	}*/
#endif

	/*if (dynamic_cast<CUIBossVitalWrapper*>(m_pParent)->Get_NaytibaDesc() && m_pParent->GetVisibility() == VISIBILITY::VISIBLE)
	{
		CUIBossVitalWrapper* pVitalWrapper = dynamic_cast<CUIBossVitalWrapper*>(m_pParent);

		m_fTargetFill = static_cast<_float>(pVitalWrapper->Get_NaytibaDesc()->iCurrentShield) / static_cast<_float>(pVitalWrapper->Get_NetworkDesc()->iMaxShield);

		m_fCurrentFill = Lerp(m_fCurrentFill, m_fTargetFill, fTimeDelta * m_fSpeed);
	}*/
	
	
}

void CUIMonsterStamina::Late_Update(_float fTimeDelta)
{
	if (m_isRent)
		__super::Late_Update(fTimeDelta);

	m_fCurrentFill = static_cast<_float>(m_iCurrentStamina) / m_iMaxStamina;
}

HRESULT CUIMonsterStamina::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::STAMINA))))
		return E_FAIL;

	if (FAILED(m_pVIBaseBuffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBaseBuffer->Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIMonsterStamina::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBaseBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBuffer))))
		return E_FAIL;

	/* Com_Texture_HP */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Stamina"),
		TEXT("Com_Texture_UI_Boss_Stamina"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMonsterStamina::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 2)))
		return E_FAIL;

	_float2 vTile{};
	vTile.x = 1.f;
	vTile.y = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TileCount", &vTile, sizeof(_float2))))
		return E_FAIL;

	_float fGroupCount{ (_float)m_iMaxStamina };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GroupCount", &fGroupCount, sizeof(_float))))
		return E_FAIL;

	/*_float fScale{ 0.5f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &fScale, sizeof(_float))))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &m_fCurrentFill, sizeof(_float))))
		return E_FAIL;

	_float2 vGap{ };
	vGap.x = 2.0f / m_tUIDesc.fSizeX;
	vGap.y = 0.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVGap", &vGap, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMonsterStamina::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIMonsterStamina::CallbackEvent(void* pArg)
{
}

CUIMonsterStamina* CUIMonsterStamina::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIMonsterStamina* pInstance = new CUIMonsterStamina(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIMonsterStamina::Clone(void* pArg)
{
	CUIMonsterStamina* pInstance = new CUIMonsterStamina(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIMonsterStamina");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIMonsterStamina::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBuffer);
}
