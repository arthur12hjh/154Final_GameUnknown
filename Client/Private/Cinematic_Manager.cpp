#include "pch.h"
#include "Cinematic_Manager.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "CinematicObject.h"
#include "Player.h"
#include "Nayitba.h"

#include "UIHUD.h"
#include "UIBase.h"

CCinematicManager::CCinematicManager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCinematicManager::Initialize()
{
	m_pCinematicDatas = CGameManager::GetInstance()->Get_CinematicDataMap();

    m_fCinematicTimer = 0.f;
    m_iCurrentCinematicNodeIndex = 0;
    m_iCurrentCinematicID = -1;
    m_bIsCinematicPlaying = false;

    return S_OK;
}
HRESULT CCinematicManager::Update(_float fTimeDelta)
{
    if (FALSE == m_bIsCinematicPlaying)
        return S_OK;

    ++m_fCinematicTimer;
    while (m_iCurrentCinematicNodeIndex < m_pCurrentCinematicDesc->CinematicNodeTrackList.size())
    {
        if (m_fCinematicTimer >= m_pCurrentCinematicDesc->CinematicNodeTrackList[m_iCurrentCinematicID].fTrackPosition)
        {
			Play_Node(m_pCurrentCinematicDesc->CinematicNodeTrackList[m_iCurrentCinematicNodeIndex]);
			++m_iCurrentCinematicNodeIndex;
        }
    }

    return S_OK;
}
HRESULT CCinematicManager::Play_Cinematic(_uint iCinematicID)
{   
	auto iter = m_pCinematicDatas->find(iCinematicID);
	if (iter == m_pCinematicDatas->end())
        return E_FAIL;

    m_fCinematicTimer = 0.f;
    m_iCurrentCinematicID = iCinematicID;
    m_iCurrentCinematicNodeIndex = 0;
    m_pCurrentCinematicDesc = &iter->second;
    m_bIsCinematicPlaying = false;

    return S_OK;
}

HRESULT CCinematicManager::Emplace_CinematicObject(CCinematicObject* pObject)
{
	m_CinematicObjectsMap.emplace(pObject->Get_ObjectTag(), pObject);
    return S_OK;
}

void CCinematicManager::Play_Node(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{

}

CCinematicManager* CCinematicManager::Create()
{
    CCinematicManager* pInstance = new CCinematicManager();

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed : Cinematic Manager");
    }

    return pInstance;
}

void CCinematicManager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
