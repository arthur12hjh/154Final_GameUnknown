#include "pch.h"
#include "UI_Level_Loading.h"

#include "UI_Loader.h"
#include "GameInstance.h"

#include "UI_Level_Logo.h"
#include "UI_Level_GamePlay.h"

CUI_Level_Loading::CUI_Level_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
{

}

HRESULT CUI_Level_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	/* 다음 레벨에 대한 자원을 로드하여 준비해둔다. */
	m_pLoader = CUI_Loader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	/* 이 레벨을 구성하기위한 객체를 만든다. */
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	return S_OK;
}

void CUI_Level_Loading::Update(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished() &&
		GetKeyState(VK_F1) & 0x8000)
	{
		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
			pNewLevel = CUI_Level_Logo::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CUI_Level_GamePlay::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		}

		if (FAILED(m_pGameInstance->Change_Level(pNewLevel)))
			return;		
	}	
}

HRESULT CUI_Level_Loading::Render()
{
	m_pLoader->Output();

	return S_OK;
}

HRESULT CUI_Level_Loading::Ready_Layer_BackGround()
{
	return S_OK;
}

CUI_Level_Loading* CUI_Level_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID)
{
	CUI_Level_Loading* pInstance = new CUI_Level_Loading(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CUI_Level_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUI_Level_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);


}
