#include "pch.h"

#include "GUIManager.h"
#include "GameInstance.h"
#include "StringHelper.h"

#include "HUDLayer.h"
#include "GameObject.h"

#include "UIHUD.h"

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
        //m_pLayers.clear();

        m_pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

        if (m_pUIHUD == nullptr)
            return;

        m_pLayers = m_pUIHUD->Get_Layers();

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

    _char str[MAX_PATH]{};

    CStringHelper::ConvertWideToUTF(m_szCurViewMode.c_str(), str);

    if (GUI::BeginCombo("", str))
    {
        for (size_t i = 0; i < m_ViewModes.size(); ++i)
        {
            _char strCur[MAX_PATH]{};

            CStringHelper::ConvertWideToUTF(m_ViewModes[i].c_str(), strCur);

            const bool is_selected = (strCur == str);
            if (GUI::Selectable(strCur, is_selected))
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

        m_LayerTags.push_back(TEXT("Select Layer"));

        for (auto& pLayer : m_pLayers)
        {
            m_LayerTags.push_back(pLayer.first);
        }

        m_strCurrentProtoTag = m_ProtoTags[0];
        m_strCurrentLayerTag = m_LayerTags[0];

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
    for (auto& pLayer : m_pLayers)
    {
        _char strLayerTag[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(pLayer.first.c_str(), strLayerTag);

        if (GUI::TreeNode(strLayerTag))
        {
            const auto* pUIObjects = pLayer.second->Get_UserInterfaces();

            if (pUIObjects)
            {
                _int i = 0;
                for (const auto& pObj : *pUIObjects)
                {
                    _char strUITag[MAX_PATH]{};
                    CStringHelper::ConvertWideToUTF((pObj.first + TEXT(" - ") + to_wstring(i)).c_str(), strUITag);

                    if (GUI::TreeNode(strUITag))
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
    _char strCurProtoTag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(m_strCurrentProtoTag.c_str(), strCurProtoTag);

    if (GUI::BeginCombo("Proto Tag", strCurProtoTag)) // 드롭다운 시작
    {
        for (int i = 1; i < m_ProtoTags.size(); ++i)
        {
            bool is_selected = (m_strCurrentProtoTag == m_ProtoTags[i]);

            _char str[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_ProtoTags[i].c_str(), str);

            if (GUI::Selectable(str, is_selected))
            {
                m_strCurrentProtoTag = m_ProtoTags[i];
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }

    _char strCurLayerTag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(m_strCurrentLayerTag.c_str(), strCurLayerTag);

    if (GUI::BeginCombo("Layer Tag", strCurLayerTag)) // 드롭다운 시작
    {
        for (int i = 1; i < m_LayerTags.size(); ++i)
        {
            bool is_selected = (m_strCurrentLayerTag == m_LayerTags[i]);

            _char str[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_LayerTags[i].c_str(), str);

            if (GUI::Selectable(str, is_selected))
            {
                m_strCurrentLayerTag = m_LayerTags[i];
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
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
