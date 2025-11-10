#include "pch.h"

#include "GUIManager.h"
#include "GameInstance.h"
#include "StringHelper.h"

#include "HUDLayer.h"
#include "GameObject.h"

#include "UIHUD.h"
#include "UIPanel.h"
#include "UIWrapper.h"
#include "UIButton.h"
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

    if (GUI::Button("Create Layer"))
    {
        strcpy_s(m_szCloneTextureComTag, sizeof(m_szCloneTextureComTag), "");

        GUI::OpenPopup("Create Layer");
    }

    if (GUI::BeginPopup("Create Layer"))
    {
        Create_Layer();

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
                for (const auto& pObjPair : *pUIObjects)
                {
                    Client::CUIBase* pObj = dynamic_cast<Client::CUIBase*>(pObjPair.second);
                    if (!pObj->Get_Parent()) // 루트만 그린다 (부모 없는 객체)
                        Draw_Hierarchy(pObj);
                    else
                        continue;
                }
            }
            GUI::TreePop();
        }
    }
}

void CGUIManager::Create_Layer()
{
    GUI::InputText("Input Layer Tag", m_szCloneTextureComTag, IM_ARRAYSIZE(m_szCloneTextureComTag));

    if (GUI::Button("Create"))
    {
        WCHAR szLayerTag[MAX_PATH];
        CStringHelper::ConvertUTFToWide(m_szCloneTextureComTag, szLayerTag);

        WCHAR szUITag[MAX_PATH];
        swprintf_s(szUITag, TEXT("UI_%s_Panel_%d"), szLayerTag, 0);

        WCHAR szTextureComTag[MAX_PATH];
        CStringHelper::ConvertUTFToWide("Prototype_Component_UI_Texture_BackGround", szTextureComTag);

        CUIBase::UIBASE_DESC Desc{};
        Desc.fSizeX = g_iWinSizeX;
        Desc.fSizeY = g_iWinSizeY;
        Desc.fX = g_iHalfWinSizeX;
        Desc.fY = g_iHalfWinSizeY;
        Desc.iDepth = 0;
        Desc.iLevel = m_iCurrentLevel;
        Desc.iObjectID = 0;
        Desc.iPass = 0;
        Desc.iTextureIndex = 0;
        Desc.iRenderGroup = ENUM_CLASS(RENDER::UI);
        Desc.szLayerTag = szLayerTag;
        Desc.szUITag = szUITag;
        Desc.szTextureComTag = szTextureComTag;

        if (FAILED(m_pUIHUD->Add_UserInterface(m_iCurrentLevel, TEXT("Prototype_GameObject_UI_Panel"), szLayerTag, szUITag, nullptr, &Desc)))
            return;
    }
}

void CGUIManager::Add_Child(Client::CUIBase* pParent)
{
    _char szCurProtoTag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(m_szCurrentProtoTag.c_str(), szCurProtoTag);

    if (GUI::BeginCombo("UI Proto Tag", szCurProtoTag)) // 드롭다운 시작
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

    _char szCurTextureComTag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(m_szCurrentTextureComTag.c_str(), szCurTextureComTag);

    if (GUI::BeginCombo("TextureCom Tag", szCurTextureComTag)) // 드롭다운 시작
    {
        for (int i = 1; i < m_TextureComTags.size(); ++i)
        {
            bool is_selected = (m_szCurrentTextureComTag == m_TextureComTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_TextureComTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                m_szCurrentTextureComTag = m_TextureComTags[i];
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }

    if (GUI::Button("Create"))
    {
        wstring szProto{};
        szProto = TEXT("Prototype_GameObject_UI_") + m_szCurrentProtoTag;

        wstring szTextureComTag{};
        szTextureComTag = TEXT("Prototype_Component_UI_Texture_") + m_szCurrentTextureComTag;

        //_uint iObjectId = pParent->Get_UIBase_Desc().iObjectID + 1;
        _uint iObjectId = pParent->Get_Children()->size() + 1;

        WCHAR szUITag[MAX_PATH];
        swprintf_s(szUITag, TEXT("UI_%s_%d-%d"), m_szCurrentProtoTag.c_str(), pParent->Get_UIBase_Desc().iDepth + 1, iObjectId);

        WCHAR szLayerTag[MAX_PATH];
        swprintf_s(szLayerTag, pParent->Get_UIBase_Desc().szLayerTag.c_str());

        CGameObject* pCreatedObj = nullptr;

        CUIBase::UIBASE_DESC Desc{};
        Desc.fSizeX = 100.f;
        Desc.fSizeY = 20.f;
        Desc.fX = pParent->Get_UIBase_Desc().fX;
        Desc.fY = pParent->Get_UIBase_Desc().fY;
        Desc.iDepth = pParent->Get_UIBase_Desc().iDepth + 1;
        Desc.iLevel = m_iCurrentLevel;
        Desc.iObjectID = iObjectId;
        Desc.iPass = 0;
        Desc.iTextureIndex = 1;
        Desc.iRenderGroup = ENUM_CLASS(RENDER::UI);
        Desc.szUITag = szUITag;
        Desc.szLayerTag = pParent->Get_UIBase_Desc().szLayerTag.c_str();
        Desc.szTextureComTag = szTextureComTag;

        if (FAILED(m_pUIHUD->Add_UserInterface(m_iCurrentLevel, szProto.c_str(), pParent->Get_UIBase_Desc().szLayerTag.c_str(), szUITag, &pCreatedObj, &Desc)))
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

            return;
        }

        pParent->Add_Child(pCreatedObj);

        if (dynamic_cast<Client::CUIBase*>(pCreatedObj))
        {
            dynamic_cast<Client::CUIBase*>(pCreatedObj)->Set_Parent(pParent);
        }

        GUI::CloseCurrentPopup();
    }
}

void CGUIManager::SetUp_Tags_For_Add_Child()
{
    m_ProtoTags.clear();
    m_TextureComTags.clear();

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

    m_TextureComTags.push_back(TEXT("Select Texture"));
    _wstring prefix2 = TEXT("Prototype_Component_UI_Texture_");
    for (auto& pProto : *m_pGameInstance->Get_Prototypes_InLevel(ENUM_CLASS(LEVEL::STATIC)))
    {
        if (pProto.first.find(prefix2) == 0)
        {
            size_t pos = pProto.first.find(prefix2);
            m_TextureComTags.push_back(pProto.first.substr(pos + prefix2.length()));
        }
    }

    for (auto& pProto : *m_pGameInstance->Get_Prototypes_InLevel(m_iCurrentLevel))
    {
        if (pProto.first.find(prefix2) == 0)
        {
            size_t pos = pProto.first.find(prefix2);
            m_TextureComTags.push_back(pProto.first.substr(pos + prefix2.length()));
        }
    }

    m_szCurrentProtoTag = m_ProtoTags[0];
    m_szCurrentTextureComTag = m_TextureComTags[0];

    strcpy_s(m_szCloneTextureComTag, sizeof(m_szCloneTextureComTag), "");
}

void CGUIManager::Draw_Hierarchy(Client::CUIBase* pObj)
{
    if (!pObj)
        return;

    _char szUITag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(pObj->Get_UIBase_Desc().szUITag.c_str(), szUITag);

    if (GUI::TreeNode(szUITag))
    {
        if (GUI::IsItemClicked())
        {
            int a = 10;
        }

        if (GUI::Button("Add Child"))
        {
            SetUp_Tags_For_Add_Child();

            GUI::OpenPopup("Add Child");
        }

        if (GUI::BeginPopup("Add Child"))
        {
            Add_Child(pObj);

            if (GUI::Button("Close")) {
                GUI::CloseCurrentPopup();
            }
            GUI::EndPopup();
        }

        const auto* children = pObj->Get_Children();
        if (children)
        {
            for (auto* pChild : *children)
            {
                Draw_Hierarchy(pChild);
            }
        }

        GUI::TreePop();
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
    //
    //for (auto& iter : m_pLayers)
    //    Safe_Release(iter);

    Safe_Release(m_pGameInstance);
}
