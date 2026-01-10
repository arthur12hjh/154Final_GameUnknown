#include "pch.h"
#include "UIScore.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "UIHUD.h"

#include "UIInstanceBuffer.h"

#include "BeatSaberCharacter.h"

CUIScore::CUIScore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIScore::CUIScore(const CUIScore& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIScore::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIScore::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIScore::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIScore::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_SPACE))
	//{
	//	m_iScore += (_uint)m_pGameInstance->Random(3.f, 9.f);
	//	m_iHighScore = max(m_iHighScore, m_iScore);
	//}
	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_BACKSPACE))
	//	m_iScore = 0;

	auto pDororong = m_pGameManager->GetBeatSaberCharacter();
	m_iScore = pDororong->GetBeatSaberCharacterDesc().iScore;
	Safe_Release(pDororong);

	if (m_iScore < 100)
		m_iRank = 0;
	else if (m_iScore >= 100 && m_iScore < 199)
		m_iRank = 1;
	else if (m_iScore >= 200 && m_iScore < 299)
		m_iRank = 2;
	else if (m_iScore >= 300 && m_iScore < 399)
		m_iRank = 3;
	else if (m_iScore >= 400)
		m_iRank = 4;

	if(m_iRank != m_iPrevRank)
	{
		m_fScaleRatio = 1.2f;
		m_iPrevRank = m_iRank;
	}

	if (m_fScaleRatio > 1.f)
	{
		m_fScaleRatio -= fTimeDelta * 2.f;
		if (m_fScaleRatio < 1.f)
			m_fScaleRatio = 1.f;
	}

	// 게임 끝나면 한번만 하게 수정해야함
	UI_EVENT_ARG_DESC Arg{};
	Arg.szActionTag = TEXT("Score_Result");
	Arg.Type = UI_EVENT_ARG_DESC::INT;
	Arg.pData = &m_iScore;
	__super::Trigger_Event(TEXT("Score_Result"), &Arg);

	UI_EVENT_ARG_DESC Arg2{};
	Arg2.szActionTag = TEXT("Rank_Result");
	Arg2.Type = UI_EVENT_ARG_DESC::INT;
	Arg2.pData = &m_iRank;

	__super::Trigger_Event(TEXT("Rank_Result"), &Arg2);
}

void CUIScore::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if(FAILED(SetUp_Score()))
		return;
}

HRESULT CUIScore::Render()
{
	__super::Render();

	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;
	//
	//if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::RANK))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pVIBufferCom->Bind_Resources()))
	//	return E_FAIL;
	//
	//if (FAILED(m_pVIBufferCom->Render()))
	//	return E_FAIL;

	if (FAILED(Render_Score()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIScore::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC IconInstanceDesc{};
	IconInstanceDesc.iNumInstance = 5;
	IconInstanceDesc.vAtlasIndex = _float2(1.f, 10.f);
	IconInstanceDesc.vUVAtlasSize = _float4(1.f, 1.f, 1.f, 1.f);
	IconInstanceDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_Score"), reinterpret_cast<CComponent**>(&m_pUIScoreBufferCom), &IconInstanceDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Numbers"),
		TEXT("Com_Texture_UI_Numbers"), reinterpret_cast<CComponent**>(&m_pNumbersTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Rank"),
		TEXT("Com_Texture_UI_Rank"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIScore::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", m_iRank)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_fScaleRatio, sizeof(_float))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUIScore::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIScore::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}

HRESULT CUIScore::Render_Score()
{
	if (FAILED(Bind_ScoreShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SCORE))))
		return E_FAIL;

	if (FAILED(m_pUIScoreBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUIScoreBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIScore::SetUp_Score()
{
	m_ScoreInstances.clear();
	m_ScoreInstances.reserve(5);

	_int ATLAS_COL = 5;
	_int ATLAS_ROW = 1;

	_float fRatio = 54.f / 32.f;

	_float2 vUISize = _float2{ 54.f, 54.f };
	_float fUIWidth = vUISize.x * fRatio * (ATLAS_COL - 1);
	_float fUIHeight = vUISize.y * fRatio * (ATLAS_ROW - 1);

	_float2 vScale = _float2{ (vUISize.x / m_tUIDesc.fSizeX), (vUISize.y / (m_tUIDesc.fSizeY)) };
	_float fWidth = vScale.x * fRatio * (ATLAS_COL - 1);
	_float fHeight = vScale.y * fRatio * (ATLAS_ROW - 1);

	for (size_t i = 0; i < ATLAS_COL; ++i)
	{
		_float2 vPos = _float2{ (vScale.x * fRatio * (i % ATLAS_COL)) - (fWidth * 0.5f) + (100.f / 400.f), -((vScale.y * fRatio * (i / ATLAS_COL)) - (fHeight * 0.5f))};

		_uint iScore = 0;

		if (i == 0)
		{
			iScore = ((m_iScore / 10000) % 10);
			vPos.x += 0.3f;
		}
		else if (i == 1)
		{
			iScore = ((m_iScore / 1000) % 10);
			vPos.x += 0.15f;
		}
		else if (i == 2)
			iScore = ((m_iScore / 100) % 10);
		else if (i == 3)
		{
			iScore = ((m_iScore / 10) % 10);
			vPos.x -= 0.15f;
		}
		else if (i == 4)
		{
			iScore = (m_iScore % 10);
			vPos.x -= 0.3f;
		}

		_uint col = 0;
		_uint row = iScore;

		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4{ 0.f, 0.f, vPos.x, vPos.y };

		inst.vAtlasIndex = _float4{ (_float)col, (_float)row, 0.f, 0.f };

		m_ScoreInstances.push_back(inst);
	}

	m_pUIScoreBufferCom->Update_Instance(m_ScoreInstances);

	return S_OK;
}

HRESULT CUIScore::Bind_ScoreShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pNumbersTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	_float2 atlasCount = { 1.f, 10.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &atlasCount, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CUIScore* CUIScore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIScore* pInstance = new CUIScore(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIScore::Clone(void* pArg)
{
	CUIScore* pInstance = new CUIScore(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIScore");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIScore::Free()
{
	__super::Free();

	Safe_Release(m_pUIScoreBufferCom);

	Safe_Release(m_pNumbersTextureCom);
}
