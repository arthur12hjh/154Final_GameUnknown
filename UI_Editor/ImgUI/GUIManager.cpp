#include "pch.h"

#include "GUIManager.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "UIResourceStore.h"
#include "StringHelper.h"

#include "HUDLayer.h"
#include "GameObject.h"

#include "UIHUD.h"
#include "UIPanel.h"
#include "UIWrapper.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIImage.h"

#include "UIAnimManager.h"

// GUI 매니저 싱글톤 구현
IMPLEMENT_SINGLETON(CGUIManager);

// GUI 매니저 초기화: 디바이스, 컨텍스트, ImGui, 에디터 윈도우 등 생성
HRESULT CGUIManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // 게임 인스턴스 참조 획득 및 참조 카운트 증가
    m_pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

    m_pUIResourceStore = CUIResourceStore::GetInstance();

    if (!m_pUIResourceStore)
        return E_FAIL;

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
    m_ViewModes.push_back(TEXT("Editor"));
    m_ViewModes.push_back(TEXT("Debug"));

    m_AnimTrackTags.reserve(3);
    m_AnimTrackTags.push_back(TEXT("Position"));
    m_AnimTrackTags.push_back(TEXT("Size"));
    m_AnimTrackTags.push_back(TEXT("Alpha"));

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
    {
        GUI::Begin("Timer");
        _char szDebug[MAX_PATH]{};
        sprintf_s(szDebug, "deltaTime = %f\n",
            fTimeDelta);
        
        GUI::Text(szDebug);
        GUI::End();

        Editor_Window();
    }

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

        _float space = GUI::GetContentRegionAvail().x - GUI::CalcTextSize("Close").x - GUI::GetStyle().FramePadding.x * 2; // auto left margin
        GUI::Dummy(ImVec2(space, 0));
        GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
        if (GUI::Button("SAVE OBJECTS"))
        {
            if (FAILED(m_pUIHUD->Save_Data(pLayer.first)))
            {
                MSG_BOX("저장 실패");
                return;
            }
        }
        GUI::PopStyleColor();
    }
}

void CGUIManager::Create_Layer()
{
    GUI::InputText("Input Layer Tag", m_szCloneLayerTag, IM_ARRAYSIZE(m_szCloneLayerTag));

    GUI::InputText("UI Tag", m_szCloneUITag, IM_ARRAYSIZE(m_szCloneUITag));

    if (GUI::Button("Create"))
    {
        WCHAR szLayerTag[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide(m_szCloneLayerTag, szLayerTag);

        WCHAR szUIID[MAX_PATH]{};
        swprintf_s(szUIID, TEXT("UI_%s_Panel_%d"), szLayerTag, 0);

        WCHAR szUITag[MAX_PATH]{};
        swprintf_s(szUITag, TEXT("%hs"), m_szCloneUITag);

        WCHAR szTextureComTag[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide("Prototype_Component_UI_Texture_BackGround", szTextureComTag);

        CUIBase::UIBASE_DESC Desc{};
        Desc.fSizeX = g_iWinSizeX;
        Desc.fSizeY = g_iWinSizeY;
        Desc.fX = g_iHalfWinSizeX;
        Desc.fY = g_iHalfWinSizeY;
        Desc.iDepth = 0;
        Desc.iLevel = m_iCurrentLevel;
        Desc.szLayerTag = szLayerTag;
        Desc.szUIID = szUIID;
        Desc.szUITag = szUITag;
        Desc.szProtoTag = TEXT("Prototype_GameObject_UI_Panel");

        if (FAILED(m_pUIHUD->Add_UserInterface(m_iCurrentLevel, TEXT("Prototype_GameObject_UI_Panel"), szLayerTag, szUITag, nullptr, &Desc)))
            return;

        strcpy_s(m_szCloneUITag, sizeof(m_szCloneUITag), "");

        GUI::CloseCurrentPopup();
    }
}

void CGUIManager::Add_Child(Client::CUIBase* pParent)
{
    SetUp_UI_Proto_Tags();
    Select_UI_Proto_Tag(m_szCloneProtoTag);

    GUI::InputText("UI Tag", m_szCloneUITag, IM_ARRAYSIZE(m_szCloneUITag));

    if (GUI::Button("Create"))
    {
        WCHAR szProto[MAX_PATH]{};
        swprintf_s(szProto, TEXT("Prototype_GameObject_UI_%hs"), m_szCloneProtoTag);

        _uint iObjectIdx = pParent->Get_Children()->size() + 1;

        _char szParentUITag[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(pParent->Get_UIBase_Desc().szUITag.c_str(), szParentUITag);

        WCHAR szUIID[MAX_PATH]{};
        swprintf_s(szUIID, TEXT("%hs_%hs_%d-%d"), szParentUITag, m_szCloneProtoTag, pParent->Get_UIBase_Desc().iDepth + 1, iObjectIdx);

        WCHAR szLayerTag[MAX_PATH]{};
        swprintf_s(szLayerTag, pParent->Get_UIBase_Desc().szLayerTag.c_str());

        WCHAR szUITag[MAX_PATH]{};
        //swprintf_s(szUITag, TEXT("%hs_%hs"), szParentUITag, m_szCloneUITag);
        CStringHelper::ConvertUTFToWide(m_szCloneUITag, szUITag);

        CGameObject* pCreatedObj = nullptr;

        CUIBase::UIBASE_DESC Desc{};
        Desc.fSizeX = 100.f;
        Desc.fSizeY = 100.f;
        //Desc.fX = 0.f;
        //Desc.fY = 0.f;
        Desc.fX = pParent->Get_UIBase_Desc().fX;
        Desc.fY = pParent->Get_UIBase_Desc().fY;
        Desc.iDepth = pParent->Get_UIBase_Desc().iDepth + 1;
        Desc.iLevel = m_iCurrentLevel;
        Desc.szUIID = szUIID;
        Desc.szUITag = szUITag;
        Desc.szLayerTag = pParent->Get_UIBase_Desc().szLayerTag.c_str();
        Desc.szProtoTag = szProto;

        if (FAILED(m_pUIHUD->Add_UserInterface(m_iCurrentLevel, szProto, pParent->Get_UIBase_Desc().szLayerTag.c_str(), szUITag, &pCreatedObj, &Desc)))
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
        strcpy_s(m_szCloneUITag, sizeof(m_szCloneUITag), "");
        strcpy_s(m_szCloneTextureComTag, sizeof(m_szCloneTextureComTag), "");
        GUI::CloseCurrentPopup();
    }
}

void CGUIManager::Select_UI_Proto_Tag(_char* Outstr)
{
    //_char szCurProtoTag[MAX_PATH]{};
    //CStringHelper::ConvertWideToUTF(m_szCurrentProtoTag.c_str(), szCurProtoTag);

    WCHAR szCurProtoTag[MAX_PATH]{};
    CStringHelper::ConvertUTFToWide(Outstr, szCurProtoTag);

    if (GUI::BeginCombo("UI Proto Tag", Outstr)) // 드롭다운 시작
    {
        for (int i = 1; i < m_ProtoTags.size(); ++i)
        {
            bool is_selected = (szCurProtoTag == m_ProtoTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_ProtoTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                //m_szCurrentProtoTag = m_ProtoTags[i];
                CStringHelper::ConvertWideToUTF(m_ProtoTags[i].c_str(), Outstr);
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }
}

void CGUIManager::Select_Texture_Tag(_char* Outstr)
{
    WCHAR szCurTextureTag[MAX_PATH]{};
    CStringHelper::ConvertUTFToWide(Outstr, szCurTextureTag);

    if (GUI::BeginCombo("TextureCom Tag", Outstr)) // 드롭다운 시작
    {
        for (int i = 1; i < m_TextureComTags.size(); ++i)
        {
            bool is_selected = (szCurTextureTag == m_TextureComTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_TextureComTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                CStringHelper::ConvertWideToUTF(m_TextureComTags[i].c_str(), Outstr);
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }
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
}

void CGUIManager::SetUp_Texture_Tags()
{
    m_TextureComTags.clear();

    m_TextureComTags.push_back(TEXT("Select Texture"));
    _wstring prefix = TEXT("Com_Texture_");

    for (auto& pTexture : *m_pUIResourceStore->Get_UI_Texture_Descs())
    {
        if (pTexture.first.find(prefix) == 0)
        {
            size_t pos = pTexture.first.find(prefix);
            m_TextureComTags.push_back(pTexture.first.substr(pos + prefix.length()));
        }
    }
}

void CGUIManager::View_Textures(_wstring szTag, void* pDesc)
{
    CUIResourceStore::UI_TEXTURE_RESOURCE_DESC Desc = *static_cast<CUIResourceStore::UI_TEXTURE_RESOURCE_DESC*>(pDesc);

    GUI::Begin("Texture Browser");

    const float thumbSize = 96.0f;
    const float padding = 8.0f;
    const int itemsPerRow = 4;

    vector<ID3D11ShaderResourceView*> loadedSRVs(Desc.iTextIndex, nullptr);

    _tchar szFullPath[MAX_PATH]{};

    _int iSelectedIndex = m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc()->iTextureIndex;

    for (size_t i = 0; i < Desc.iTextIndex; ++i)
    {
        _bool isSelected = (iSelectedIndex == i);

        wsprintf(szFullPath, Desc.szFilePath, i);

        // SRV 아직 로드 안됐으면 로드
        if (!loadedSRVs[i])
            loadedSRVs[i] = LoadTextureSRV(szFullPath);

        ID3D11ShaderResourceView* pSRV = loadedSRVs[i];
        if (!pSRV) continue;

        GUI::PushID((int)i);
        if (isSelected)
            GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
        if (GUI::ImageButton("", (ImTextureID)pSRV, ImVec2(thumbSize, thumbSize)))
        {
            m_iCurrentTextureIndex = (int)i; // 선택된 텍스처 저장
            m_pTargetUI->Set_TextureCom(szTag, Desc.szProtoTag, m_iCurrentTextureIndex);
        }

        if (GUI::IsItemHovered())
        {
            _char szTextureComTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(szTag.c_str(), szTextureComTag);

            GUI::SetTooltip("%hs-%d", szTextureComTag, i);
        }

        GUI::PopID();
        if (isSelected)
            GUI::PopStyleColor();

        if ((i + 1) % itemsPerRow != 0)
            GUI::SameLine();
        else
            GUI::Dummy(ImVec2(0, padding));
    }


    GUI::End();
}

void CGUIManager::Draw_Hierarchy(Client::CUIBase* pObj)
{
    if (!pObj)
        return;

    _char szUIID[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(pObj->Get_UIBase_Desc().szUIID.c_str(), szUIID);

    _char szUITag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(pObj->Get_UIBase_Desc().szUITag.c_str(), szUITag);

    GUI::PushID(szUIID);
    if (GUI::TreeNode(szUITag))
    {
        if (GUI::Button("View Options"))
        {
            m_bOpenViewOptions = true;
            m_pTargetUI = pObj;

            m_bVisible = m_pTargetUI->Get_UIBase_Desc().iVisiblity == ENUM_CLASS(VISIBILITY::VISIBLE) ? true : false;
            m_pTargetUI->Set_Follow_Parent(false);
           /* m_vOldPos.x = m_pTargetUI->Get_UIBase_Desc().fOffsetX;
            m_vOldPos.y = m_pTargetUI->Get_UIBase_Desc().fOffsetY;
            m_vEditedPos = m_vOldPos;

            m_vOldSize.x = m_pTargetUI->Get_UIBase_Desc().fSizeX;
            m_vOldSize.y = m_pTargetUI->Get_UIBase_Desc().fSizeY;
            m_vEditedSize = m_vOldSize;
            
            */

            if (m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc())
            {
                _char szInputText[MAX_PATH]{};
                CStringHelper::ConvertWideToUTF(m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc()->szText.c_str(), szInputText);
                //m_szInputText = szInputText;
                strcpy_s(m_szInputText, szInputText);
                m_vColor = m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc()->vColor;
            }
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
    GUI::PopID();
}

void CGUIManager::View_Options()
{
    _char szUITag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(m_pTargetUI->Get_UIBase_Desc().szUITag.c_str(), szUITag);

    GUI::Begin("OPTIONS");
    
    GUI::Title(szUITag);
    
    GUI::Checkbox("Visible", &m_bVisible);

    m_pTargetUI->SetVisibility(m_bVisible ? VISIBILITY::VISIBLE : VISIBILITY::HIDDEN);
    GUI::DragFloat("Alpha", &m_pTargetUI->Get_UIBase_Desc().fAlpha, 0.01f, 0.f, 1.f);

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

        if (GUI::BeginTabItem("Animation List"))
        {
            Set_Animation();
            GUI::EndTabItem();
        }

        if (dynamic_cast<Client::CUIText*>(m_pTargetUI))
        {
            if (GUI::BeginTabItem("Text"))
            {
                Set_Text();
                GUI::EndTabItem();
            }
        }

        GUI::EndTabBar();
    }
    
    _float space = GUI::GetContentRegionAvail().y - GUI::CalcTextSize("Delete").x - GUI::GetStyle().FramePadding.x * 2; // auto top margin
    GUI::Dummy(ImVec2(0, space));
    /*GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Save"))
    {
    }
    GUI::PopStyleColor();
    GUI::SameLine();
    if (GUI::Button("Reset"))
    {
    }
    GUI::SameLine();*/
    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.f, 0.f, 1.0f));
    if (GUI::Button("Delete"))
    {
        m_pUIHUD->Remove_UserInterface(m_pTargetUI->Get_UIBase_Desc().szLayerTag.c_str(), m_pTargetUI->Get_UIBase_Desc().szUITag.c_str());
        m_pLayers = m_pUIHUD->Get_Layers();
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
        m_pTargetUI->Set_Follow_Parent(true);
        m_pTargetUI = nullptr;
    }
    GUI::PopStyleColor();
    GUI::End();
}

void CGUIManager::Set_Size()
{
    string size = "Origin : " + to_string(m_pTargetUI->Get_UIBase_OriginDesc().fSizeX) + ", " + to_string(m_pTargetUI->Get_UIBase_OriginDesc().fSizeY);

    GUI::Text(size.c_str());
    GUI::Separator();

    GUI::Checkbox("Ratio Lock", &m_bRatioLock);

    if (m_bRatioLock)
    {
        GUI::InputFloat("Ratio_x", &m_vRatio.x);
        GUI::InputFloat("Ratio_y", &m_vRatio.y);

        _float aspect = m_vRatio.y / m_vRatio.x;

        ////GUI::DragFloat("Size_x", &pEditedSize->x);

        _float fNewY = m_pTargetUI->Get_UIBase_Desc().fSizeX * aspect;
        m_pTargetUI->Get_UIBase_Desc().fSizeY = fNewY;
        //GUI::DragFloat("Size_y", &pEditedSize->y);
    }

    GUI::DragFloat("Size_x", &m_pTargetUI->Get_UIBase_Desc().fSizeX, 1.f, 0.f, 1600.f );
    GUI::DragFloat("Size_y", &m_pTargetUI->Get_UIBase_Desc().fSizeY, 1.f, 0.f, 900.f);

    m_pTargetUI->Set_Size(m_pTargetUI->Get_UIBase_Desc().fSizeX, m_pTargetUI->Get_UIBase_Desc().fSizeY);

    if (GUI::Button("Reset"))
    {
        m_pTargetUI->Set_Size(m_pTargetUI->Get_UIBase_OriginDesc().fSizeX, m_pTargetUI->Get_UIBase_OriginDesc().fSizeY);
        //*pEditedSize = *pOldSize;
        m_pTargetUI->Get_UIBase_Desc().fSizeX = m_pTargetUI->Get_UIBase_OriginDesc().fSizeX;
        m_pTargetUI->Get_UIBase_Desc().fSizeY = m_pTargetUI->Get_UIBase_OriginDesc().fSizeY;
    }

    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Apply"))
    {
        //*pOldSize = *pEditedSize;

        Client::CUIBase::UIBASE_DESC Desc = m_pTargetUI->Get_UIBase_Desc();
        Desc.fSizeX = m_pTargetUI->Get_UIBase_Desc().fSizeX;
        Desc.fSizeY = m_pTargetUI->Get_UIBase_Desc().fSizeY;

        m_pTargetUI->Set_UIBase_OriginDesc(Desc);
    }
    GUI::PopStyleColor();
}

void CGUIManager::Set_Position()
{
    string CurrentPos = "CurrentPos : " + to_string(m_pTargetUI->Get_UIBase_Desc().fX) + ", " + to_string(m_pTargetUI->Get_UIBase_Desc().fY);

    _vector vPos = m_pTargetUI->GetTransform()->Get_State(STATE::POSITION);
    _float2 vPosXY = { XMVectorGetX(vPos), XMVectorGetY(vPos) };

    string TransformPos = "Transform : " + to_string(vPosXY.x) + ", " + to_string(vPosXY.y);
    string pos = "Origin : " + to_string(m_pTargetUI->Get_UIBase_OriginDesc().fOffsetX) + ", " + to_string(m_pTargetUI->Get_UIBase_OriginDesc().fOffsetY);

    GUI::Text(TransformPos.c_str());
    GUI::Separator();
    GUI::Text(CurrentPos.c_str());
    GUI::Separator();
    GUI::Text(pos.c_str());
    GUI::Separator();

    GUI::DragFloat("Position_x", &m_pTargetUI->Get_UIBase_Desc().fOffsetX, 1.f, 0.f, 0.f, "%.2f");
    GUI::DragFloat("Position_y", &m_pTargetUI->Get_UIBase_Desc().fOffsetY, 1.f, 0.f, 0.f, "%.2f");

    m_pTargetUI->Set_Position(m_pTargetUI->Get_UIBase_Desc().fOffsetX, m_pTargetUI->Get_UIBase_Desc().fOffsetY);

    if (GUI::Button("Reset"))
    {
        m_pTargetUI->Set_Position(m_pTargetUI->Get_UIBase_OriginDesc().fOffsetX, m_pTargetUI->Get_UIBase_OriginDesc().fOffsetY);
        //*pEditedPos = *pOldPos;
        m_pTargetUI->Get_UIBase_Desc().fOffsetX = m_pTargetUI->Get_UIBase_OriginDesc().fOffsetX;
        m_pTargetUI->Get_UIBase_Desc().fOffsetY = m_pTargetUI->Get_UIBase_OriginDesc().fOffsetY;
    }

    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Apply"))
    {
        //*pOldPos = *pEditedPos;

        Client::CUIBase::UIBASE_DESC Desc = m_pTargetUI->Get_UIBase_Desc();
        Desc.fOffsetX = m_pTargetUI->Get_UIBase_Desc().fOffsetX;
        Desc.fOffsetY = m_pTargetUI->Get_UIBase_Desc().fOffsetY;

        m_pTargetUI->Set_UIBase_OriginDesc(Desc);
    }
    GUI::PopStyleColor();
}

void CGUIManager::Set_Texture()
{
    SetUp_Texture_Tags();

    /*if(m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc()->szTextureComTag != TEXT(""))
    {
        _wstring prefix = TEXT("Com_Texture_");
        size_t pos = m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc()->szTextureComTag.find(prefix);
        CStringHelper::ConvertWideToUTF(
            m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc()->szTextureComTag.substr(pos + prefix.length()).c_str(),
            m_szCloneTextureComTag);
    }*/

    Select_Texture_Tag(m_szCloneTextureComTag);

    WCHAR szTextureComTag[MAX_PATH]{};
    swprintf_s(szTextureComTag, TEXT("Com_Texture_%hs"), m_szCloneTextureComTag);

    CUIResourceStore::UI_TEXTURE_RESOURCE_DESC ResDesc{};

    ResDesc = m_pUIResourceStore->Get_UI_Texture_Desc(szTextureComTag);

    if (m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc())
    {
        _int iPass = static_cast<_int>(m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc()->iPass);

        GUI::InputInt("Pass", &iPass);

        m_pTargetUI->Set_Pass(static_cast<_uint>(iPass));
    }

    if(ResDesc.pTexture)
    {
        View_Textures(szTextureComTag, &ResDesc);
    }

    if (GUI::Button("Reset"))
    {
        m_pTargetUI->Get_UIBase_Desc().fAlpha = m_pTargetUI->Get_UIBase_OriginDesc().fAlpha;
        m_pTargetUI->Set_TextureCom(m_pTargetUI->Get_UIBase_OriginDesc().Get_UI_Texture_Desc()->szTextureComTag,
            m_pTargetUI->Get_UIBase_OriginDesc().Get_UI_Texture_Desc()->szProtoTag,
            m_pTargetUI->Get_UIBase_OriginDesc().Get_UI_Texture_Desc()->iTextureIndex);
    }
    GUI::SameLine();
    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Apply"))
    {
        Client::CUIBase::UIBASE_DESC UIDesc = m_pTargetUI->Get_UIBase_Desc();

        m_pTargetUI->Set_UIBase_OriginDesc(UIDesc);
    }
    GUI::PopStyleColor();
}

void CGUIManager::Set_Text()
{
    Client::CUIBase::UIBASE_DESC UIDesc = m_pTargetUI->Get_UIBase_Desc();
    Client::CUIBase::UI_TEXT_DESC TextDesc{};

    GUI::InputText("Input Text", m_szInputText, IM_ARRAYSIZE(m_szInputText));

    if (GUI::Button("Apply Text"))
    {
        WCHAR szInputText[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide(m_szInputText, szInputText);
        
        m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc()->szText = szInputText;
        m_pTargetUI->Get_UIBase_OriginDesc().Get_UI_Text_Desc()->szText = szInputText;
        //UIDesc.Set_UI_Text_Desc(TextDesc);

        //m_pTargetUI->Set_UIBase_OriginDesc(m_pTargetUI->Get_UIBase_Desc());
    }

    if (UIDesc.Get_UI_Text_Desc())
    {
        if (GUI::ColorPicker4("MyColorPicker", (float*)&m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc()->vColor)) {
            // 색상이 변경될 때 처리
           /* TextDesc.vColor = m_vColor;
            TextDesc.szText = szInputText;
            UIDesc.Set_UI_Text_Desc(TextDesc);
            m_pTargetUI->Set_UIBase_Desc(UIDesc);*/
        }
    }

   
    if (GUI::Button("Apply Color"))
    {
        m_pTargetUI->Get_UIBase_OriginDesc().Get_UI_Text_Desc()->vColor = m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc()->vColor;
    }
}

void CGUIManager::Set_Animation()
{
    // 위치, 크기, alpha, loop 조절해서 ui한테 보내기

    if (GUI::Button("Add Anim"))
    {
        strcpy_s(m_szInputAnimName, sizeof(m_szInputAnimName), "");
        GUI::OpenPopup("Add Anim");
    }
    // GUI::SameLine();

    if (GUI::BeginPopup("Add Anim"))
    {
        Add_Animation();

        if (GUI::Button("Close")) {
            GUI::CloseCurrentPopup();
        }
        GUI::EndPopup();
    }
    
    for (auto& AnimTag : m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Tags())
    {
        _char szAnimTag[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF((TEXT("AnimTag : ") + AnimTag.first).c_str(), szAnimTag);

        //CUIAnimInstance::UI_ANIM_DESC AnimDesc{};
        //
        //auto Anim = m_pUIHUD->Get_AnimMgr()->Get_AnimData(AnimTag.second);

        if (GUI::TreeNode(szAnimTag))
        {
            GUI::Separator();

            //if (Anim != nullptr)
            //{
            //    Edit_Animation(AnimTag.first, m_pUIHUD->Get_AnimMgr()->Get_AnimData(AnimTag.second));
            //    //AnimDesc = *m_pUIHUD->Get_AnimMgr()->Get_AnimData(AnimTag.second);
            //}
            //else
            //{
            //    AnimDesc.szAnimTag = AnimTag.second;
            //    Edit_Animation(AnimTag.first, &AnimDesc);
            //}

            Edit_Animation(AnimTag.first, AnimTag.second);
            GUI::TreePop();
        }
    }

    /*if (m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Tags().size() > 0)
    {
        for (auto& Anim : m_pUIHUD->Get_AnimMgr()->Get_AnimDatas())
        {
            CUIAnimInstance::UI_ANIM_DESC AnimDesc{ Anim.second };

            _char szAnimTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF((TEXT("AnimTag : ") + Anim.first).c_str(), szAnimTag);

            if (GUI::TreeNode(szAnimTag))
            {
                Edit_Animation(&AnimDesc);
                GUI::TreePop();
            }
        }
    }*/
}

void CGUIManager::Add_Animation()
{
    GUI::InputText("Anim Tag", m_szInputAnimTag, IM_ARRAYSIZE(m_szInputText));

    WCHAR szAnimTag[MAX_PATH]{};
    CStringHelper::ConvertUTFToWide(m_szInputAnimTag, szAnimTag);
    
    if (GUI::Button("Add"))
    {
        GUI::OpenPopup("Select Anim");
    }

    if (GUI::BeginPopup("Select Anim"))
    {
        if (GUI::Button("Import Prefab"))
        {
            m_pUIHUD->Get_AnimMgr()->Load_Anim_Files();

            m_AnimPrefabTags.clear();

            for (auto& AnimTag : m_pUIHUD->Get_AnimMgr()->Get_AnimDatas())
                m_AnimPrefabTags.push_back(AnimTag.first);

            GUI::OpenPopup("Import Prefab");
        }

        if (GUI::BeginPopup("Import Prefab"))
        {
            Select_Anim_Prefabs(m_szCurrentAnimPrefab);

            if (GUI::Button("Import"))
            {
                WCHAR szAnimNameTag[MAX_PATH]{};
                CStringHelper::ConvertUTFToWide(m_szCurrentAnimPrefab, szAnimNameTag);

                Client::CUIAnimInstance::UI_ANIM_DESC AnimDesc{};
                m_pUIHUD->Get_AnimMgr()->Import_Anim_Prefab(szAnimNameTag, &AnimDesc);

                m_pTargetUI->Get_UIBase_Desc().Add_UI_Anim(szAnimTag, AnimDesc.szAnimTag);
            }

            if (GUI::Button("Close")) {
                GUI::CloseCurrentPopup();
            }
            GUI::EndPopup();
        }

        GUI::SameLine();

        if (GUI::Button("Create Anim"))
        {
            GUI::OpenPopup("Create Anim");
        }

        if (GUI::BeginPopup("Create Anim"))
        {
            GUI::InputText("Anim Name", m_szInputAnimName, IM_ARRAYSIZE(m_szInputAnimName));

            if (GUI::Button("Create"))
            {
                WCHAR szAnimName[MAX_PATH]{};
                CStringHelper::ConvertUTFToWide(m_szInputAnimName, szAnimName);

                if (FAILED(m_pUIHUD->Get_AnimMgr()->Create_Prefab(szAnimName)))
                {
                    MSG_BOX("애니메이션 생성 실패");
                }
                else
                {
                    m_pTargetUI->Get_UIBase_Desc().Add_UI_Anim(szAnimTag, szAnimName);
                    strcpy_s(m_szInputAnimName, sizeof(m_szInputAnimName), "");
                    GUI::CloseCurrentPopup();
                }

                if (GUI::Button("Close"))
                {
                    strcpy_s(m_szInputAnimName, sizeof(m_szInputAnimName), "");
                    GUI::CloseCurrentPopup();
                }
            }
            GUI::EndPopup();
        }

        if (GUI::Button("Close")) {
            GUI::CloseCurrentPopup();
        }

        GUI::EndPopup();
    }
}

void CGUIManager::Edit_Animation(_wstring szAnimTag, _wstring szPrefabTag)
{
    //CUIAnimInstance::UI_ANIM_DESC AnimDesc{ *static_cast<CUIAnimInstance::UI_ANIM_DESC*>(pDesc) };

    _char szAnimName[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF((TEXT("Anim Name : ") + szPrefabTag).c_str(), szAnimName);

    GUI::Text(szAnimName);

    /*if (GUI::Button("Change Name"))
    {
        strcpy_s(m_szInputAnimName, sizeof(m_szInputAnimName), "");
        GUI::OpenPopup("Change Name");            
    }

    if (GUI::BeginPopup("Change Name"))
    {
        GUI::InputText("Anim Name", m_szInputAnimName, IM_ARRAYSIZE(m_szInputAnimName));

        if (GUI::Button("Apply"))
        {
            WCHAR szAnimName[MAX_PATH]{};
            CStringHelper::ConvertUTFToWide(m_szInputAnimName, szAnimName);

            m_pUIHUD->Get_AnimMgr()->Delete_Prefab(AnimDesc.szAnimTag);
            m_pUIHUD->Get_AnimMgr()->Create_Prefab(szAnimName);
            m_pTargetUI->Get_UIBase_Desc().Add_UI_Anim(szAnimTag, szAnimName);

            strcpy_s(m_szInputAnimName, sizeof(m_szInputAnimName), "");

            GUI::CloseCurrentPopup();
        }

        if (GUI::Button("Close"))                
            GUI::CloseCurrentPopup();
        GUI::EndPopup();
    }*/

    GUI::DragFloat("Duration", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szPrefabTag)->fDuration, 0.01f, 0.f, 0.f, "%.2f");
    GUI::Checkbox("Loop", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szPrefabTag)->isLoop);
    GUI::Checkbox("Influence", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szPrefabTag)->isInfluenceChildren);

    if (GUI::Button("Add Track"))
    {
        GUI::OpenPopup("Add Track");
    }

    if (GUI::BeginPopup("Add Track"))
    {
        Add_AnimTrack(m_pUIHUD->Get_AnimMgr()->Get_AnimData(szPrefabTag)->szAnimTag);

        if (GUI::Button("Close")) {
            GUI::CloseCurrentPopup();
        }
        GUI::EndPopup();
    }

    /* if (GUI::TreeNode("Debug"))
        {
            _char szDebug[MAX_PATH]{};
            sprintf_s(szDebug, "deltaTime = %f\nanimDeltaTime = %f\nplayTime = %.3f\nalpha = %.3f\npos.x = %.3f, pos.y = %.3f\nsize.x = %.3f, size.y = %.3f",
                m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime")),
                m_pTargetUI->Get_AnimationCom()->Get_PlayTime(),
                m_pTargetUI->Get_UIBase_Desc().fAlpha,
                m_pTargetUI->Get_UIBase_Desc().fOffsetX,
                m_pTargetUI->Get_UIBase_Desc().fOffsetY,
                m_pTargetUI->Get_UIBase_Desc().fSizeX,
                m_pTargetUI->Get_UIBase_Desc().fSizeY);

            GUI::Text(szDebug);

            GUI::TreePop();
        }*/

    GUI::Separator();
    if (GUI::Button("Play"))
    {
        m_pUIHUD->Anim_Play(m_pTargetUI->Get_UIBase_Desc().szLayerTag, m_pTargetUI->Get_UIBase_Desc().szUITag, m_pUIHUD->Get_AnimMgr()->Get_AnimData(szPrefabTag)->szAnimTag);
    }
    /* GUI::SameLine();
        if (GUI::Button("Pause"))
        {
            m_pTargetUI->Pause_Anim();
        }
        GUI::SameLine();
        if (GUI::Button("Stop"))
        {
            m_pTargetUI->Stop_Anim();
        }*/

    GUI::Separator();

    if (m_pUIHUD->Get_AnimMgr()->Get_AnimData(m_pUIHUD->Get_AnimMgr()->Get_AnimData(szPrefabTag)->szAnimTag))
    {
        for (auto& TrackDesc : m_pUIHUD->Get_AnimMgr()->Get_AnimData(szPrefabTag)->Get_UI_Track_Descs())
        {
            _char szTrackTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(TrackDesc.first.c_str(), szTrackTag);

            if (GUI::TreeNode(szTrackTag))
            {
                Set_AnimTrack(szPrefabTag, TrackDesc.first);
                GUI::Separator();

                GUI::TreePop();
            }
        }
    }

    if (GUI::Button("Apply"))
    {

    }
    GUI::SameLine();
    if (GUI::Button("Delete"))
    {
        m_pTargetUI->Get_UIBase_Desc().Delete_UI_Anim(szAnimTag);
        m_pUIHUD->Get_AnimMgr()->Delete_Prefab(szPrefabTag);
    }
    GUI::SameLine();
    if (GUI::Button("Export Prefab"))
    {
        GUI::OpenPopup("Export Prefab");
    }

    if (GUI::BeginPopup("Export Prefab"))
    {
        GUI::InputText("Prefab Tag", m_szInputAnimTag, IM_ARRAYSIZE(m_szInputText));

        if (GUI::Button("Export")) {
            WCHAR szAnimTag[MAX_PATH]{};
            CStringHelper::ConvertUTFToWide(m_szInputAnimTag, szAnimTag);

            m_pUIHUD->Get_AnimMgr()->Export_Anim_Prefab(szAnimTag, m_pUIHUD->Get_AnimMgr()->Get_AnimData(szPrefabTag));
            strcpy_s(m_szInputAnimName, sizeof(m_szInputAnimName), "");
            GUI::CloseCurrentPopup();
        }
        GUI::SameLine();
        if (GUI::Button("Close")) {
            strcpy_s(m_szInputAnimName, sizeof(m_szInputAnimName), "");
            GUI::CloseCurrentPopup();
        }
        GUI::EndPopup();
    }
}

void CGUIManager::Select_AnimTrack_Tags(_char* Outstr)
{
    WCHAR szCurAnimTrackTag[MAX_PATH]{};
    CStringHelper::ConvertUTFToWide(Outstr, szCurAnimTrackTag);

    if (GUI::BeginCombo("AnimTrack Tag", Outstr)) // 드롭다운 시작
    {
        for (int i = 0; i < m_AnimTrackTags.size(); ++i)
        {
            bool is_selected = (szCurAnimTrackTag == m_AnimTrackTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_AnimTrackTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                CStringHelper::ConvertWideToUTF(m_AnimTrackTags[i].c_str(), Outstr);
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }
}

void CGUIManager::Set_AnimTrack(_wstring szAnimTag, _wstring szTrackTag)
{
    _char szText[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF((TEXT("TrackTag : ") + szTrackTag).c_str(), szText);

    GUI::Text(szText);

    if (szTrackTag == TEXT("Position"))
    {
        if (GUI::TreeNode("StartPram"))
        {
            GUI::DragFloat("x", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.x, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("y", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.y, 1.f, 0.f, 0.f, "%.2f");
            m_pTargetUI->Set_Position(m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.x,
                m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.y);

            if (GUI::Button("Reset"))
                m_pTargetUI->Set_Position(m_pTargetUI->Get_UIBase_OriginDesc().fOffsetX, m_pTargetUI->Get_UIBase_OriginDesc().fOffsetY);
            
            GUI::TreePop();
        }
        if (GUI::TreeNode("EndPram"))
        {
            GUI::DragFloat("x", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.x, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("y", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.y, 1.f, 0.f, 0.f, "%.2f");
            m_pTargetUI->Set_Position(m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.x,
                m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.y);

            if (GUI::Button("Reset"))
                m_pTargetUI->Set_Position(m_pTargetUI->Get_UIBase_OriginDesc().fOffsetX, m_pTargetUI->Get_UIBase_OriginDesc().fOffsetY);

            GUI::TreePop();
        }
    }

    if (szTrackTag == TEXT("Size"))
    {
        if (GUI::TreeNode("StartPram"))
        {
            GUI::DragFloat("x", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.x, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("y", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.y, 1.f, 0.f, 0.f, "%.2f");
            m_pTargetUI->Set_Size(m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.x,
                m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.y);

            if (GUI::Button("Reset"))
                m_pTargetUI->Set_Size(m_pTargetUI->Get_UIBase_OriginDesc().fSizeX, m_pTargetUI->Get_UIBase_OriginDesc().fSizeY);

            GUI::TreePop();
        }
        if (GUI::TreeNode("EndPram"))
        {
            GUI::DragFloat("x", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.x, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("y", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.y, 1.f, 0.f, 0.f, "%.2f");
            m_pTargetUI->Set_Size(m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.x,
                m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.y);
            
            if (GUI::Button("Reset"))
                m_pTargetUI->Set_Size(m_pTargetUI->Get_UIBase_OriginDesc().fSizeX, m_pTargetUI->Get_UIBase_OriginDesc().fSizeY);

            GUI::TreePop();
        }
    }

    if (szTrackTag == TEXT("Alpha"))
    {
        if (GUI::TreeNode("StartPram"))
        {
            GUI::DragFloat("Start Alpha", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.x, 0.01f, 0.f, 1.f, "%.2f");
            m_pTargetUI->Set_Alpha(m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.x);
            
            if (GUI::Button("Reset"))
                m_pTargetUI->Set_Alpha(m_pTargetUI->Get_UIBase_OriginDesc().fAlpha);

            GUI::TreePop();
        }
        if (GUI::TreeNode("EndPram"))
        {
            GUI::DragFloat("End Alpha", &m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.x, 0.01f, 0.f, 1.f, "%.2f");
            m_pTargetUI->Set_Alpha(m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.x);

            if (GUI::Button("Reset"))
                m_pTargetUI->Set_Alpha(m_pTargetUI->Get_UIBase_OriginDesc().fAlpha);

            GUI::TreePop();
        }
    }
}

void CGUIManager::Add_AnimTrack(_wstring szAnimTag)
{
    Select_AnimTrack_Tags(m_szCurrentTrackTag);

    if (GUI::Button("Add"))
    {
        WCHAR szTrackTag[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide(m_szCurrentTrackTag, szTrackTag);

        Client::CUIAnimInstance::UI_ANIM_TRACK_DESC TrackDesc{};
        TrackDesc.szTrackTag = szTrackTag;

        if (TrackDesc.szTrackTag == TEXT("Position"))
        {
            TrackDesc.vStartParam.x = m_pTargetUI->Get_UIBase_Desc().fOffsetX;
            TrackDesc.vStartParam.y = m_pTargetUI->Get_UIBase_Desc().fOffsetY;
            TrackDesc.vEndParam.x = m_pTargetUI->Get_UIBase_Desc().fOffsetX;
            TrackDesc.vEndParam.y = m_pTargetUI->Get_UIBase_Desc().fOffsetY;
        }
        if (TrackDesc.szTrackTag == TEXT("Size"))
        {
            TrackDesc.vStartParam.x = m_pTargetUI->Get_UIBase_Desc().fSizeX;
            TrackDesc.vStartParam.y = m_pTargetUI->Get_UIBase_Desc().fSizeY;
            TrackDesc.vEndParam.x = m_pTargetUI->Get_UIBase_Desc().fSizeX;
            TrackDesc.vEndParam.y = m_pTargetUI->Get_UIBase_Desc().fSizeY;
        }
        if (TrackDesc.szTrackTag == TEXT("Alpha"))
        {
            TrackDesc.vStartParam.x = m_pTargetUI->Get_UIBase_Desc().fAlpha;
            TrackDesc.vEndParam.x = m_pTargetUI->Get_UIBase_Desc().fAlpha;
        }

        m_pUIHUD->Get_AnimMgr()->Get_AnimData(szAnimTag)->Add_UI_Track_Desc(szTrackTag, TrackDesc);
    }
}

void CGUIManager::Select_Anim_Prefabs(_char* Outstr)
{
    WCHAR szCurAnimPrefabTag[MAX_PATH]{};
    CStringHelper::ConvertUTFToWide(Outstr, szCurAnimPrefabTag);

    if (GUI::BeginCombo("AnimTrack Tag", Outstr)) // 드롭다운 시작
    {
        for (int i = 0; i < m_AnimPrefabTags.size(); ++i)
        {
            bool is_selected = (szCurAnimPrefabTag == m_AnimPrefabTags[i]);

            _char szTag[MAX_PATH]{};
            CStringHelper::ConvertWideToUTF(m_AnimPrefabTags[i].c_str(), szTag);

            if (GUI::Selectable(szTag, is_selected))
            {
                CStringHelper::ConvertWideToUTF(m_AnimPrefabTags[i].c_str(), Outstr);
            }

            if (is_selected)
                GUI::SetItemDefaultFocus(); // 기본 포커스 설정
        }
        GUI::EndCombo();
    }
}

ID3D11ShaderResourceView* CGUIManager::LoadTextureSRV(const _wstring& path)
{
    ID3D11ShaderResourceView* pSRV = nullptr;
    HRESULT hr = CreateWICTextureFromFile(m_pDevice, path.c_str(), nullptr, &pSRV);
    if (FAILED(hr))
        return nullptr;
    return pSRV;
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

    Safe_Release(m_pUIHUD);
    Safe_Release(m_pTargetUI);
    Safe_Release(m_pUIResourceStore);
    
    for (auto& iter : m_pLayers)
        Safe_Release(iter.second);

    Safe_Release(m_pGameInstance);
}
