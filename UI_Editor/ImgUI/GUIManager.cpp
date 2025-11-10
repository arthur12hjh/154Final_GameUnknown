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

    if (m_bOpenViewOptions && m_pTargetUI)
        View_Options();

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
        strcpy_s(m_szCloneLayerTag, sizeof(m_szCloneLayerTag), "");

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
    GUI::InputText("Input Layer Tag", m_szCloneLayerTag, IM_ARRAYSIZE(m_szCloneLayerTag));

    if (GUI::Button("Create"))
    {
        WCHAR szLayerTag[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide(m_szCloneLayerTag, szLayerTag);

        WCHAR szUITag[MAX_PATH]{};
        swprintf_s(szUITag, TEXT("UI_%s_Panel_%d"), szLayerTag, 0);

        WCHAR szTextureComTag[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide("Prototype_Component_UI_Texture_BackGround", szTextureComTag);

        CUIBase::UIBASE_DESC Desc{};
        Desc.fSizeX = g_iWinSizeX;
        Desc.fSizeY = g_iWinSizeY;
        Desc.fX = g_iHalfWinSizeX;
        Desc.fY = g_iHalfWinSizeY;
        Desc.iDepth = 0;
        Desc.iLevel = m_iCurrentLevel;
        Desc.iPass = 0;
        Desc.iTextureIndex = 0;
        Desc.iRenderGroup = ENUM_CLASS(RENDER::UI);
        Desc.szLayerTag = szLayerTag;
        Desc.szUITag = szUITag;
        //Desc.szTextureComTag = szTextureComTag;

        if (FAILED(m_pUIHUD->Add_UserInterface(m_iCurrentLevel, TEXT("Prototype_GameObject_UI_Panel"), szLayerTag, szUITag, nullptr, &Desc)))
            return;

        GUI::CloseCurrentPopup();
    }
}

void CGUIManager::Add_Child(Client::CUIBase* pParent)
{
    SetUp_UI_Proto_Tags();
    Select_UI_Proto_Tag(m_szCloneProtoTag);

   /* SetUp_Texture_Tags();
    Select_Texture_Tag(m_szCloneTextureComTag);*/

    if (GUI::Button("Create"))
    {
        WCHAR szProto[MAX_PATH]{};
        swprintf_s(szProto, TEXT("Prototype_GameObject_UI_%hs"), m_szCloneProtoTag);

        //WCHAR szTextureComTag[MAX_PATH]{};
        //swprintf_s(szTextureComTag, TEXT("Prototype_Component_UI_Texture_%hs"), m_szCloneTextureComTag);

        _uint iObjectIdx = pParent->Get_Children()->size() + 1;

        WCHAR szUITag[MAX_PATH];
        swprintf_s(szUITag, TEXT("UI_%hs_%d-%d"), m_szCloneProtoTag, pParent->Get_UIBase_OriginDesc().iDepth + 1, iObjectIdx);

        WCHAR szLayerTag[MAX_PATH];
        swprintf_s(szLayerTag, pParent->Get_UIBase_OriginDesc().szLayerTag.c_str());

        CGameObject* pCreatedObj = nullptr;

        CUIBase::UIBASE_DESC Desc{};
        Desc.fSizeX = 100.f;
        Desc.fSizeY = 20.f;
        Desc.fX = pParent->Get_UIBase_OriginDesc().fX;
        Desc.fY = pParent->Get_UIBase_OriginDesc().fY;
        Desc.iDepth = pParent->Get_UIBase_OriginDesc().iDepth + 1;
        Desc.iLevel = m_iCurrentLevel;
        Desc.iPass = 0;
        Desc.iTextureIndex = 1;
        Desc.iRenderGroup = ENUM_CLASS(RENDER::UI);
        Desc.szUITag = szUITag;
        Desc.szLayerTag = pParent->Get_UIBase_OriginDesc().szLayerTag.c_str();
        //Desc.szTextureComTag = szTextureComTag;

        if (FAILED(m_pUIHUD->Add_UserInterface(m_iCurrentLevel, szProto, pParent->Get_UIBase_OriginDesc().szLayerTag.c_str(), szUITag, &pCreatedObj, &Desc)))
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
            dynamic_cast<Client::CUIBase*>(pCreatedObj)->Set_Parent(pParent);

        strcpy_s(m_szCloneProtoTag, sizeof(m_szCloneProtoTag), "");
        strcpy_s(m_szCloneTextureComTag, sizeof(m_szCloneTextureComTag), "");
        GUI::CloseCurrentPopup();
    }
}

void CGUIManager::Select_UI_Proto_Tag(char* _Outstr)
{
    //_char szCurProtoTag[MAX_PATH]{};
    //CStringHelper::ConvertWideToUTF(m_szCurrentProtoTag.c_str(), szCurProtoTag);

    WCHAR szCurProtoTag[MAX_PATH]{};
    CStringHelper::ConvertUTFToWide(_Outstr, szCurProtoTag);

    if (GUI::BeginCombo("UI Proto Tag", _Outstr)) // 드롭다운 시작
    {
        for (int i = 1; i < m_ProtoTags.size(); ++i)
        {
            bool is_selected = (szCurProtoTag == m_ProtoTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_ProtoTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                //m_szCurrentProtoTag = m_ProtoTags[i];
                CStringHelper::ConvertWideToUTF(m_ProtoTags[i].c_str(), _Outstr);
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }
}

void CGUIManager::Select_Texture_Tag(char* _Outstr)
{
    WCHAR szCurTextureTag[MAX_PATH]{};
    CStringHelper::ConvertUTFToWide(_Outstr, szCurTextureTag);

    if (GUI::BeginCombo("TextureCom Tag", _Outstr)) // 드롭다운 시작
    {
        for (int i = 1; i < m_TextureComTags.size(); ++i)
        {
            bool is_selected = (szCurTextureTag == m_TextureComTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_TextureComTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                CStringHelper::ConvertWideToUTF(m_TextureComTags[i].c_str(), _Outstr);
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }
}

void CGUIManager::Select_Texture_Index(_uint* _Out)
{
}

void CGUIManager::SetUp_UI_Proto_Tags()
{
    m_ProtoTags.clear();

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

    //m_szCurrentProtoTag = m_ProtoTags[0];
}

void CGUIManager::SetUp_Texture_Tags()
{
    m_TextureComTags.clear();

    m_TextureComTags.push_back(TEXT("Select Texture"));
    _wstring prefix = TEXT("Com_Texture_");
    for (auto& pLayer : m_pUIHUD->Get_Textures())
    {
        if (pLayer.first.find(prefix) == 0)
        {
            size_t pos = pLayer.first.find(prefix);
            m_TextureComTags.push_back(pLayer.first.substr(pos + prefix.length()));
        }
    }

    int a = 0;

    /*for (auto& pLayer : *m_pGameInstance->Get_Prototypes_InLevel(m_iCurrentLevel))
    {
        if (pLayer.first.find(prefix) == 0)
        {
            size_t pos = pLayer.first.find(prefix);
            m_TextureComTags.push_back(pLayer.first.substr(pos + prefix.length()));
        }
    }*/
    //m_szCurrentTextureComTag = m_TextureComTags[0];
    //strcpy_s(m_szCloneTextureComTag, sizeof(m_szCloneTextureComTag), "");
}

void CGUIManager::SetUp_Texture_Index()
{

}

void CGUIManager::Draw_Hierarchy(Client::CUIBase* pObj)
{
    if (!pObj)
        return;

    _char szUITag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(pObj->Get_UIBase_OriginDesc().szUITag.c_str(), szUITag);

    if (GUI::TreeNode(szUITag))
    {
        if (GUI::Button("View Options"))
        {
            m_bOpenViewOptions = true;
            m_pTargetUI = pObj;

            m_vOldPos.x = m_pTargetUI->Get_UIBase_OriginDesc().fX;
            m_vOldPos.y = m_pTargetUI->Get_UIBase_OriginDesc().fY;
            m_vEditedPos = m_vOldPos;

            m_vOldSize.x = m_pTargetUI->Get_UIBase_OriginDesc().fSizeX;
            m_vOldSize.y = m_pTargetUI->Get_UIBase_OriginDesc().fSizeY;
            m_vEditedSize = m_vOldSize;
        }
        GUI::SameLine();
        if (GUI::Button("Add Child"))
        {
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

void CGUIManager::View_Options()
{
    _char szUITag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(m_pTargetUI->Get_UIBase_OriginDesc().szUITag.c_str(), szUITag);

    GUI::Begin("OPTIONS");
    
    GUI::Title(szUITag);
    
    if (GUI::BeginTabBar("Edit Tab"))
    {
        if (GUI::BeginTabItem("Position"))
        {
            Set_Position();
            GUI::EndTabItem();
        }

        if (GUI::BeginTabItem("Size"))
        {
            Set_Size();
            GUI::EndTabItem();
        }

        if (GUI::BeginTabItem("Texture"))
        {
            Set_Texture();
            GUI::EndTabItem();
        }

        GUI::EndTabBar();
    }
    

    _float space = GUI::GetContentRegionAvail().y - GUI::CalcTextSize("Save").x - GUI::GetStyle().FramePadding.x * 2; // auto top margin
    GUI::Dummy(ImVec2(0, space));
    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Save"))
    {
    }
    GUI::PopStyleColor();
    GUI::SameLine();
    if (GUI::Button("Reset"))
    {
    }
    GUI::SameLine();
    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.f, 0.f, 1.0f));
    if (GUI::Button("Delete"))
    {
    }
    GUI::PopStyleColor();
    GUI::SameLine();
    space = GUI::GetContentRegionAvail().x - GUI::CalcTextSize("Close").x - GUI::GetStyle().FramePadding.x * 2; // auto left margin
    GUI::Dummy(ImVec2(space, 0));
    GUI::SameLine();

    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    if (GUI::Button("Close"))
    {
        m_bOpenViewOptions = false;
        m_pTargetUI = nullptr;
    }
    GUI::PopStyleColor();
    GUI::End();
}

void CGUIManager::Set_Size()
{
    string scale = "Origin : " + to_string(m_vOldSize.x) + ", " + to_string(m_vOldSize.y);

    GUI::InputFloat("Size_x", &m_vEditedSize.x);
    GUI::InputFloat("Size_y", &m_vEditedSize.y);

    m_pTargetUI->Set_Size(m_vEditedSize.x, m_vEditedSize.y);

    if (GUI::Button("Reset"))
    {
        m_pTargetUI->Set_Size(m_vOldSize.x, m_vOldSize.y);
        m_vEditedSize = m_vOldSize;
    }

    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Save"))
    {
        m_vOldSize = m_vEditedSize;

        Client::CUIBase::UIBASE_DESC Desc = m_pTargetUI->Get_UIBase_OriginDesc();
        Desc.fSizeX = m_vOldSize.x;
        Desc.fSizeY = m_vOldSize.y;

        m_pTargetUI->Set_UIBase_OriginDesc(Desc);
    }
    GUI::PopStyleColor();
}

void CGUIManager::Set_Position()
{
    string pos = "Origin : " + to_string(m_vOldPos.x) + ", " + to_string(m_vOldPos.y);

    GUI::DragFloat("Position_x", &m_vEditedPos.x, 1.f, 0.f, 0.f, "%.2f");
    GUI::DragFloat("Position_y", &m_vEditedPos.y, 1.f, 0.f, 0.f, "%.2f");

    m_pTargetUI->Set_Position(m_vEditedPos.x, m_vEditedPos.y);

    if (GUI::Button("Reset"))
    {
        m_pTargetUI->Set_Position(m_vOldPos.x, m_vOldPos.y);
        m_vEditedPos = m_vOldPos;
    }

    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Save"))
    {
        m_vOldPos = m_vEditedPos;

        Client::CUIBase::UIBASE_DESC Desc = m_pTargetUI->Get_UIBase_OriginDesc();
        Desc.fX = m_vOldPos.x;
        Desc.fY = m_vOldPos.y;

        m_pTargetUI->Set_UIBase_OriginDesc(Desc);
    }
    GUI::PopStyleColor();
}

void CGUIManager::Set_Texture()
{
    SetUp_Texture_Tags();
    Select_Texture_Tag(m_szCloneTextureComTag);

    WCHAR szTextureComTag[MAX_PATH]{};
    swprintf_s(szTextureComTag, TEXT("Com_Texture_%hs"), m_szCloneTextureComTag);

    _uint iTextureIndex = 0;

    //if (m_szCloneTextureComTag != "")
    //{
    //    SetUp_Texture_Index();
    //    //Select_Texture_Index(m_szCloneTextureComTag);
    //}

    if (GUI::Button("Apply"))
    {
        m_pTargetUI->Set_TextureCom(szTextureComTag, m_iCurrentTextureIndex);
    }

    if (GUI::Button("Reset"))
    {
        m_pTargetUI->Set_TextureCom(m_pTargetUI->Get_UIBase_OriginDesc().szTextureComTag, m_pTargetUI->Get_UIBase_OriginDesc().iTextureIndex);
    }
    GUI::SameLine();
    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Save"))
    {
        Client::CUIBase::UIBASE_DESC Desc = m_pTargetUI->Get_UIBase_OriginDesc();
        Desc.szTextureComTag = szTextureComTag;
        Desc.iTextureIndex = m_iCurrentTextureIndex;

        m_pTargetUI->Set_UIBase_OriginDesc(Desc);
    }
    GUI::PopStyleColor();
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
