#include "pch.h"
#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"

#include "Level_AnimationEditor.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

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
	if (true == m_pLoader->isFinished())
	{
		CLevel* pNewLevel = { nullptr };

		m_pGameInstance->Clear_LevelResource(true);
		switch (m_eNextLevelID)
		{
		case LEVEL::EDITOR:
			pNewLevel = CLevel_AnimationEditor::Create(m_pDevice, m_pContext, m_eNextLevelID);
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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BackGround"),
		ENUM_CLASS(LEVEL::LOADING), TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
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
