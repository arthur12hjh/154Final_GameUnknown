#include "pch.h"
#include "CinematicMaker.h"

#include "GameInstance.h"
#include "ClientStruct.h"

#include "ImGuiManager.h"
#include "CinematicMaker.h"

#include "Player.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "Camera_Action.h"

#include "StringHelper.h"
#include "GameManager.h"

CCinematicMaker::CCinematicMaker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

HRESULT CCinematicMaker::Initialize()
{
#ifdef _DEBUG
    m_pGameManager = CGameManager::GetInstance();
    Safe_AddRef(m_pGameManager);

    m_pImGuiManager = CImGuiManager::GetInstance();
#endif

    return S_OK;
}

void CCinematicMaker::Update(_float fTimeDeleta)
{
#ifdef _DEBUG
    if (nullptr == m_pCinematicDatas)
        m_pCinematicDatas = m_pGameManager->Get_CinematicDataMap();

    if (m_bIsCinematicStarted == TRUE)
    {
        m_fTimer += fTimeDeleta * 0.34f;
    }
    _bool bIsOpen = 1 - ENUM_CLASS(m_eVisibility);
    ImGui::Begin("Cinematic Maker", &bIsOpen);

    ImGui::Text("Time : %f", m_fTimer);

    WriteCinematicDesc();

    ImGui::End();

    if (!bIsOpen)
        m_eVisibility = VISIBILITY::HIDDEN;
#endif // _DEBUG
}

HRESULT CCinematicMaker::Render()
{
    return S_OK;
}

void CCinematicMaker::WriteCinematicDesc()
{
#ifdef _DEBUG
    static CINEMATIC_DESC CinematicDesc{};
    static _int iSelectedNodeIndex = -1;

    ImGui::InputInt("CinematicID", (_int*)&CinematicDesc.iCinematicID);
    ImGui::InputText("CinematicName", CinematicDesc.szCinematicName, MAX_PATH);

    ImGui::Separator();

    ImGui::Text("Node Count : %d", (_int)CinematicDesc.CinematicNodeTrackList.size());

    if (ImGui::Button("Add Node"))
    {
        CINEMATIC_NODE_DESC CinematicNode{};
        CinematicNode.eState = CINEMATICNODE_STATE::ACTIVE_CINEOBJ;
        CinematicNode.fTrackPosition = 0.f;
        CinematicNode.iActiveIndex = 0;
        CinematicNode.szObjectTag[0] = '\0';

        CinematicDesc.CinematicNodeTrackList.push_back(CinematicNode);
    }

    ImGui::Separator();

    const char* szStateNames[] = { "ACTIVE_CINEOBJ", "PLAY_CINEOBJ", "ACTIVE_CHARACTER", "ACTIVE_CAMERA", "PLAY_SOUND", "END"};

    for (_uint i = 0; i < CinematicDesc.CinematicNodeTrackList.size(); ++i)
    {
        CINEMATIC_NODE_DESC& Node = CinematicDesc.CinematicNodeTrackList[i];
        string Header = "Node_" + std::to_string(i);

        if (ImGui::CollapsingHeader(Header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushID(i);

            _int iState = (_int)Node.eState;
            if (ImGui::Combo("State", &iState, szStateNames, IM_ARRAYSIZE(szStateNames)))
                Node.eState = (CINEMATICNODE_STATE)iState;

            ImGui::InputFloat("TrackPosition", &Node.fTrackPosition);
            ImGui::InputText("ObjectTag", Node.szObjectTag, MAX_PATH);
            ImGui::InputInt("ActiveIndex", (_int*)&Node.iActiveIndex);

            if (ImGui::Button("Delete Node"))
            {
                CinematicDesc.CinematicNodeTrackList.erase(
                    CinematicDesc.CinematicNodeTrackList.begin() + i);
                ImGui::PopID();
                break;
            }

            ImGui::PopID();
        }
    }

    ImGui::Separator();

    /* Save */
    if (ImGui::Button("Save Cinematic"))
    {
		m_pCinematicDatas->emplace(CinematicDesc.iCinematicID, CinematicDesc);

        m_pGameManager->Save_CinematicData();
    }

    ImGui::Separator();

    static _int iPlayCinematicID = 0;
    ImGui::InputInt("Play Cinematic ID", &iPlayCinematicID);

    if (ImGui::Button("Play Cinematic"))
    {
        m_fTimer = 0.f;
        m_bIsCinematicStarted = TRUE;
        Play_Cinematic(iPlayCinematicID);
    }
#endif // _DEBUG
}

void CCinematicMaker::Play_Cinematic(_uint iSelectedCinematicIndex)
{
#ifdef _DEBUG
	m_pGameManager->Play_Cinematic(iSelectedCinematicIndex);
#endif
}

CCinematicMaker* CCinematicMaker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCinematicMaker* pCinematicMaker = new CCinematicMaker(pDevice, pContext);
    if (FAILED(pCinematicMaker->Initialize()))
    {
        Safe_Release(pCinematicMaker);
        MSG_BOX("Create Fail : Debug Cheat UI");
    }
    return pCinematicMaker;
}

CGameObject* CCinematicMaker::Clone(void* pArg)
{
    return nullptr;
}

void CCinematicMaker::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pGameManager);
#endif // _DEBUG
}
