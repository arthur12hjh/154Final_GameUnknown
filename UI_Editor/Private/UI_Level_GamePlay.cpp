#include "pch.h"
#include "UI_Level_GamePlay.h"

#include "GameInstance.h"
#include "UIHUD.h"

CUI_Level_GamePlay::CUI_Level_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
	
{

}

HRESULT CUI_Level_GamePlay::Initialize()
{
	if (FAILED(Ready_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CUI_Level_GamePlay::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

HRESULT CUI_Level_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨이빈다"));
	return S_OK;
}

HRESULT CUI_Level_GamePlay::Ready_UI(const _wstring& strLayerTag)
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Combat"))))
		return E_FAIL;

	return S_OK;
}


CUI_Level_GamePlay* CUI_Level_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CUI_Level_GamePlay* pInstance = new CUI_Level_GamePlay(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CUI_Level_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Level_GamePlay::Free()
{
	__super::Free();
}
