#include "pch.h"
#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Scarlet.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID, _bool bIsResetProtoTypes)
{
	m_eNextLevelID = eNextLevelID;
	m_bIsProtoTypes = bIsResetProtoTypes;

	/* 다음 레벨에 대한 자원을 로드하여 준비해둔다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	/* 이 레벨을 구성하기위한 객체를 만든다. */
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	
	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished() &&
		m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F1))
	{
		m_pGameInstance->Clear_LevelResource(m_bIsProtoTypes);

		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::SCARLET:
			pNewLevel = CLevel_Scarlet::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		}

		if (FAILED(m_pGameInstance->Change_Level(pNewLevel)))
			return;		
	}	
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Output();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID, _bool bIsResetProtoTypes)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(eNextLevelID, bIsResetProtoTypes)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);


}
