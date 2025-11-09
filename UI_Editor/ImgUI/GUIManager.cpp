#include "pch.h"

#include "GUIManager.h"
#include "GameInstance.h"
#include "StringHelper.h"

#include "HUDLayer.h"
#include "GameObject.h"

#include "UIHUD.h"
#include "UIBase.h"

// GUI 매니저 싱글톤 구현
IMPLEMENT_SINGLETON(CGUIManager);

// GUI 매니저 초기화: 디바이스, 컨텍스트, ImGui, 에디터 윈도우 등 생성
HRESULT CGUIManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // 게임 인스턴스 참조 획득 및 참조 카운트 증가
    m_pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = pContext;
    Safe_AddRef(m_pContext);

    // ImGui 버전 체크 및 컨텍스트 생성
    IMGUI_CHECKVERSION();
    GUI::CreateContext();

    ImGuiIO& io = GUI::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    GUI::StyleColorsDark(); // 다크 테마 적용

    // ImGui Win32/DX11 초기화
    if (!ImGui_ImplWin32_Init(g_hWnd))
        return E_FAIL;
    if (!ImGui_ImplDX11_Init(m_pDevice, m_pContext))
        return E_FAIL;

    // 뷰 모드
    m_ViewModes.reserve(3);
    m_ViewModes.push_back(TEXT("Default"));
    m_ViewModes.push_back(TEXT("Edit"));
    m_ViewModes.push_back(TEXT("Debug"));

    return S_OK;
}

CGUIManager::CGUIManager()
{
}

void CGUIManager::Update(_float fTimeDelta)
{
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();
    GUI::NewFrame();

    m_iCurrentLevel = m_pGameInstance->GetCurrentLevelID();

    if (m_iPrevLevel != m_iCurrentLevel)
    {
        m_pLayers.clear();

        m_pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

        if (m_pUIHUD == nullptr)
            return;

        m_iPrevLevel = m_iCurrentLevel;
    }

    // 실제 에디터 UI 및 입력 처리
    if (m_szCurViewMode == TEXT("Default"))
        GUI::ShowDemoWindow(); // ImGui 데모 윈도우 표시

    if (m_szCurViewMode == TEXT("Editor"))
        Editor_Window();

    //가장 마지막에 렌더
    ViewMode();
}

// 에디터 윈도우에 ImGui UI를 실제로 그리는 함수
void CGUIManager::Render()
{
    // 4. ImGui UI 렌더링
    GUI::Render(); // ImGui 내부적으로 DrawData 생성
    ImGui_ImplDX11_RenderDrawData(GUI::GetDrawData()); // DX11로 실제 그리기

    // Update and Render additional Platform Windows
    ImGuiIO& io = GUI::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GUI::UpdatePlatformWindows();
        GUI::RenderPlatformWindowsDefault();
    }
}

void CGUIManager::Release_GUI_Manager()
{
    DestroyInstance();
}

void CGUIManager::ViewMode()
{
    GUI::Begin("VIEW MODE");

    GUI::PushID("ViewMode");

    _char sz[MAX_PATH]{};

    CStringHelper::ConvertWideToUTF(m_szCurViewMode.c_str(), sz);

    if (GUI::BeginCombo("", sz))
    {
        for (size_t i = 0; i < m_ViewModes.size(); ++i)
        {
            _char szCur[MAX_PATH]{};

            CStringHelper::ConvertWideToUTF(m_ViewModes[i].c_str(), szCur);

            const bool is_selected = (szCur == sz);
            if (GUI::Selectable(szCur, is_selected))
                m_szCurViewMode = m_ViewModes[i];

            if (is_selected)
                GUI::SetItemDefaultFocus();
        }
        GUI::EndCombo();
    }
    GUI::PopID();

    GUI::End();

}

#pragma region EDITOR
void CGUIManager::Editor_Window()
{
    GUI::Begin("EDITOR WINDOW");
    GUI::SetWindowSize(ImVec2(400, g_iWinSizeY - 200));

    if (GUI::Button("Clone Object"))
    {
        m_ProtoTags.clear();
        m_LayerTags.clear();

        m_ProtoTags.push_back(TEXT("Select Prototype"));
        _wstring prefix = TEXT("Prototype_GameObject_UI_");

        for (auto& pProto : *m_pGameInstance->Get_Prototypes_InLevel(ENUM_CLASS(LEVEL::STATIC)))
        {
            if (pProto.first.find(prefix) == 0)
            {
                size_t pos = pProto.first.find(prefix);
                m_ProtoTags.push_back(pProto.first.substr(pos + prefix.length()));
            }
        }

        for (auto& pProto : *m_pGameInstance->Get_Prototypes_InLevel(m_iCurrentLevel))
        {
            if (pProto.first.find(prefix) == 0)
            {
                size_t pos = pProto.first.find(prefix);
                m_ProtoTags.push_back(pProto.first.substr(pos + prefix.length()));
            }
        }

        m_LayerTags.push_back(TEXT("Add Layer"));

        for (auto& pLayer : m_pLayers)
        {
            m_LayerTags.push_back(pLayer.first);
        }

        m_szCurrentProtoTag = m_ProtoTags[0];
        m_szCurrentLayerTag = m_LayerTags[0];

        strcpy_s(m_szCloneProtoTag, sizeof(m_szCloneProtoTag), "");
        strcpy_s(m_szCloneLayerTag, sizeof(m_szCloneLayerTag), "");

        GUI::OpenPopup("Clone Object");
    }

    if (GUI::BeginPopup("Clone Object"))
    {
        Clone_UI();

        if (GUI::Button("Close")) {
            GUI::CloseCurrentPopup();
        }
        GUI::EndPopup();
    }

    GUI::Separator();
    GUI::Spacing();

    if (GUI::BeginTabBar("EditorTabs"))
    {
        if (GUI::BeginTabItem("Object List"))
        {
            Show_UIObject_List();
            GUI::EndTabItem();
        }
        GUI::EndTabBar();
    }

    GUI::End();
}

void CGUIManager::Show_UIObject_List()
{
    m_pLayers = m_pUIHUD->Get_Layers();

    for (auto& pLayer : m_pLayers)
    {
        _char szLayerTag[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(pLayer.first.c_str(), szLayerTag);

        if (GUI::TreeNode(szLayerTag))
        {
            const auto* pUIObjects = pLayer.second->Get_UserInterfaces();

            if (pUIObjects)
            {
                for (const auto& pObj : *pUIObjects)
                {
                    _char szUITag[MAX_PATH]{};
                    CStringHelper::ConvertWideToUTF(pObj.first.c_str(), szUITag);

                    if (GUI::TreeNode(szUITag))
                    {
                        if (GUI::IsItemClicked())
                        {
                            int a = 10;
                        }

                        GUI::TreePop();
                    }
                }
            }
            GUI::TreePop();
        }
    }
}

void CGUIManager::Clone_UI()
{
    _char szCurProtoTag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(m_szCurrentProtoTag.c_str(), szCurProtoTag);

    if (GUI::BeginCombo("Proto Tag", szCurProtoTag)) // 드롭다운 시작
    {
        for (int i = 1; i < m_ProtoTags.size(); ++i)
        {
            bool is_selected = (m_szCurrentProtoTag == m_ProtoTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_ProtoTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                m_szCurrentProtoTag = m_ProtoTags[i];
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }

    _char szCurLayerTag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(m_szCurrentLayerTag.c_str(), szCurLayerTag);

    if (GUI::BeginCombo("Layer Tag", szCurLayerTag)) // 드롭다운 시작
    {
        for (int i = 1; i < m_LayerTags.size(); ++i)
        {
            bool is_selected = (m_szCurrentLayerTag == m_LayerTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_LayerTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                m_szCurrentLayerTag = m_LayerTags[i];

                if (i == 0)
                    strcpy_s(m_szCloneLayerTag, sizeof(m_szCloneLayerTag), "");
                else
                    strcpy_s(m_szCloneLayerTag, sizeof(m_szCloneLayerTag), szTag);
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }

    if (m_szCurrentLayerTag == TEXT("Add Layer"))
    {
        GUI::InputText("Input Layer Tag", m_szCloneLayerTag, IM_ARRAYSIZE(m_szCloneLayerTag));
    }

    if (GUI::Button("Clone"))
    {
        if (!(m_iCurrentLevel < 0 || m_iCurrentLevel >= ENUM_CLASS(LEVEL::END))
            && !(m_szCloneLayerTag == ""))
        {
            _wstring szProto{};
            szProto = TEXT("Prototype_GameObject_UI_") + m_szCurrentProtoTag;

           
            _tchar szLayerTag[MAX_PATH];
            CStringHelper::ConvertUTFToWide(m_szCloneLayerTag, szLayerTag);

            _uint iObjectId = 0;

            auto pLayer = m_pUIHUD->Find_Layer(szLayerTag);

            if (pLayer != nullptr)
                iObjectId = pLayer->Get_UserInterfaces()->size();

            _wstring szUITag{};
            szUITag = TEXT("UI_") + m_szCurrentProtoTag + TEXT("_") + to_wstring(iObjectId);

            /*
            CUIBase* pUIObject = nullptr;

            if (pLayer != nullptr)
                pUIObject = dynamic_cast<CUIBase*>(pLayer->Find_GameObject(szUITag.c_str()));

            CUIBase::UIBASE_DESC* pDesc{};
            pDesc->iObjectID = m_iUICnt;
            */

            if (FAILED(m_pUIHUD->Add_UserInterface(m_iCurrentLevel, szProto.c_str(), szLayerTag, szUITag.c_str())))
                return;

            if (szLayerTag != TEXT(""))
                m_szCurrentLayerTag = szLayerTag;

           /* if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCloneProtoLevel, szProto, m_iCloneLayerLevel, m_szCloneLayerTag, &Desc)))
                return;*/
        }
        else
        {
            GUI::OpenPopup("Clone Error");
            if (GUI::BeginPopup("Clone Error"))
            {
                GUI::Text("클론 생성 실패");
                GUI::PushID("Clone Error");
                if (GUI::Button("확인")) {
                    GUI::CloseCurrentPopup();
                }
                GUI::PopID();
                GUI::EndPopup();
            }
        }

        GUI::CloseCurrentPopup();
    }
}
#pragma endregion

void CGUIManager::Free()
{
    __super::Free();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    GUI::DestroyContext();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    //Safe_Release(m_pUIHUD);

    //for (auto& iter : m_pLayers)
    //    Safe_Release(iter);

    Safe_Release(m_pGameInstance);
}
