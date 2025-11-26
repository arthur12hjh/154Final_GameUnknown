#include "pch.h"
#include "TestQuest.h"

#include "GameInstance.h"
#include "RewordEvent.h"

CTestQuest::CTestQuest() : CQuest()
{
}

HRESULT CTestQuest::Initialize()
{
    if (FAILED(Ready_Events()))
        return E_FAIL;

    return S_OK;
}

void CTestQuest::Accept_Quest()
{
    m_pGameInstance->Bind_Observer(TEXT("Box_Open"), m_pRewardEvent);
}

void CTestQuest::Clear()
{
    if (m_pRewardEvent)
        m_pRewardEvent->Notify(&m_QuestInfo);
}

HRESULT CTestQuest::Ready_Events()
{
    m_pRewardEvent = CRewordEvent::Create([&](void* pArg) { this->CallBackEvent(pArg); });
    if (nullptr == m_pRewardEvent)
        return E_FAIL;

    return S_OK;
}

void CTestQuest::CallBackEvent(void* pArg)
{
    m_bIsClear = true;

    m_pGameInstance->UnBind_Observer(TEXT("Box_Open"), m_pRewardEvent);
}

CTestQuest* CTestQuest::Create()
{
    CTestQuest* pTestQuest = new CTestQuest();
    if (FAILED(pTestQuest->Initialize()))
    {
        Safe_Release(pTestQuest);
        MSG_BOX("Create Fail : Test Quest");
    }
    return pTestQuest;
}

void CTestQuest::Free()
{
    __super::Free();

    Safe_Release(m_pEvent);
    Safe_Release(m_pRewardEvent);
}
