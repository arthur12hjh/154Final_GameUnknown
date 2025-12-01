#include "pch.h"
#include "UIBeta.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIBeta::CUIBeta(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIBeta::CUIBeta(const CUIBeta& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIBeta::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBeta::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter() };

	if (pCharactor)
	{
		m_iMaxCount = const_cast<LONGLONG*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iMaxBetaEnergy);
		m_iCurrentCount = const_cast<LONGLONG*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iCurrentBetaEnergy);
	}

#ifdef _DEBUG
	else
	{
		m_iMaxCount = const_cast<LONGLONG*>(&dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera())->Get_Desc()->iMaxBetaEnergy);
		m_iCurrentCount = const_cast<LONGLONG*>(&dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera())->Get_Desc()->iCurrentBetaEnergy);
	}
#endif // DEBUG

	Safe_Release(pCharactor);

	return S_OK;
}

void CUIBeta::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIBeta::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

#ifdef _DEBUG
	// 테스트 용
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_O))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && m_fTargetFill > 0.f)
			*m_iCurrentCount -= 1;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && m_fTargetFill < 1.f)
			*m_iCurrentCount += 1;
	}*/
#endif

	m_fTargetFill = static_cast<_float>(*m_iCurrentCount) / static_cast<_float>(*m_iMaxCount);

	if (m_fCurrentFill < m_fTargetFill)
		m_fCurrentFill = min(m_fCurrentFill + fTimeDelta * m_fSpeed, m_fTargetFill);
	else
		m_fCurrentFill = max(m_fCurrentFill - fTimeDelta * m_fSpeed, m_fTargetFill);

	// (2) 정확한 정수 타일 계산 (float 오차 제거)
	_float exactFill = m_fCurrentFill * *m_iMaxCount;

	// 타일 경계에서 튐 방지용
	_int filledTiles = (_int)floor(exactFill + 0.00001f);

	// 셰이더로 갈 때는 float(0~1)로 환산
	m_fCurrentFill = (_float)filledTiles / *m_iMaxCount;
}

void CUIBeta::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	_int filledTiles = static_cast<_int>(m_fCurrentFill * *m_iMaxCount);

	// 현재 묶음 인덱스
	_int currentGroup = filledTiles / m_iPerCount;  // 0~5 범위

	// 묶음이 새로 완성된 순간 감지
	if (currentGroup != m_iPrevGroupFilled)
	{
		// ★ 묶음이 새로 채워짐
		// currentGroup가 증가 방향일 때만 체크하는게 더 깔끔함
		if (currentGroup > m_iPrevGroupFilled)
		{
			// ---- 여기에서 이펙트 출력 ----
			// 예: TriggerEffect(currentGroup);

			int a = 0;
		}

		m_iPrevGroupFilled = currentGroup;
	}
}

HRESULT CUIBeta::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(7)))
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

HRESULT CUIBeta::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBaseBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBuffer))))
		return E_FAIL;

	/* Com_Texture_HP */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp"),
		TEXT("Com_Texture_Beta"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBeta::Bind_ShaderResources()
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

	_float2 vTile{};
	vTile.x = 2.f;
	vTile.y = 2.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TileCount", &vTile, sizeof(_float2))))
		return E_FAIL;

	_float fGroupCount{ 5.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GroupCount", &fGroupCount, sizeof(_float))))
		return E_FAIL;
	
	_float fFillAmount = m_fCurrentFill;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &fFillAmount, sizeof(_float))))
		return E_FAIL;

	_float2 vGap{ };
	vGap.x = 6.0f / m_tUIDesc.fSizeX;
	vGap.y = 0.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVGap", &vGap, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBeta::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

//HRESULT CUIBeta::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
//{
//	return S_OK;
//}

void CUIBeta::CallbackEvent(void* pArg)
{
}

HRESULT CUIBeta::Bind_GlowShaderResources()
{
	return S_OK;
}

CUIBeta* CUIBeta::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBeta* pInstance = new CUIBeta(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBeta::Clone(void* pArg)
{
	CUIBeta* pInstance = new CUIBeta(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBeta");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBeta::Free()
{
	__super::Free();

	Safe_Release(m_pFXTexture);
	Safe_Release(m_pVIBaseBuffer);
}
