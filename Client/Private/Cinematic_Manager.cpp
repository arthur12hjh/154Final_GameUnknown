#include "pch.h"
#include "Cinematic_Manager.h"

#include "GameInstance.h"
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
    return S_OK;
}
void CCinematicManager::Start_Cinematic()
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
