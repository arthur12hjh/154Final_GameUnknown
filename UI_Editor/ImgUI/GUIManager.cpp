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
//#include "UIBase.h"
#include "UIText.h"

#include "UIAnimationCom.h";

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
    m_ViewModes.push_back(TEXT("Edit"));
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
        //Desc.fX = 0.f;
        //Desc.fY = 0.f;
        Desc.fX = g_iHalfWinSizeX;
        Desc.fY = g_iHalfWinSizeY;
        Desc.iDepth = 0;
        Desc.iLevel = m_iCurrentLevel;
        Desc.szLayerTag = szLayerTag;
        Desc.szUITag = szUITag;
        Desc.szProtoTag = TEXT("Prototype_GameObject_UI_Panel");

        if (FAILED(m_pUIHUD->Add_UserInterface(m_iCurrentLevel, TEXT("Prototype_GameObject_UI_Panel"), szLayerTag, szUITag, nullptr, &Desc)))
            return;

        GUI::CloseCurrentPopup();
    }
}

void CGUIManager::Add_Child(Client::CUIBase* pParent)
{
    SetUp_UI_Proto_Tags();
    Select_UI_Proto_Tag(m_szCloneProtoTag);

    if (GUI::Button("Create"))
    {
        WCHAR szProto[MAX_PATH]{};
        swprintf_s(szProto, TEXT("Prototype_GameObject_UI_%hs"), m_szCloneProtoTag);

        _uint iObjectIdx = pParent->Get_Children()->size() + 1;

        _char szParentUITag[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(pParent->Get_UIBase_Desc().szUITag.c_str(), szParentUITag);

        WCHAR szUITag[MAX_PATH]{};
        swprintf_s(szUITag, TEXT("%hs_%hs_%d-%d"), szParentUITag, m_szCloneProtoTag, pParent->Get_UIBase_Desc().iDepth + 1, iObjectIdx);

        WCHAR szLayerTag[MAX_PATH]{};
        swprintf_s(szLayerTag, pParent->Get_UIBase_Desc().szLayerTag.c_str());

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

    for (size_t i = 0; i < Desc.iTextIndex; ++i)
    {
        wsprintf(szFullPath, Desc.szFilePath, i);

        // SRV 아직 로드 안됐으면 로드
        if (!loadedSRVs[i])
            loadedSRVs[i] = LoadTextureSRV(szFullPath);

        ID3D11ShaderResourceView* pSRV = loadedSRVs[i];
        if (!pSRV) continue;

        GUI::PushID((int)i);
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

    _char szUITag[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(pObj->Get_UIBase_Desc().szUITag.c_str(), szUITag);

    if (GUI::TreeNode(szUITag))
    {
        if (GUI::Button("View Options"))
        {
            m_bOpenViewOptions = true;
            m_pTargetUI = pObj;

            m_vOldPos.x = m_pTargetUI->Get_UIBase_Desc().fOffsetX;
            m_vOldPos.y = m_pTargetUI->Get_UIBase_Desc().fOffsetY;
            m_vEditedPos = m_vOldPos;

            m_vOldSize.x = m_pTargetUI->Get_UIBase_Desc().fSizeX;
            m_vOldSize.y = m_pTargetUI->Get_UIBase_Desc().fSizeY;
            m_vEditedSize = m_vOldSize;

            if (m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc())
            {
                _char szInputText[MAX_PATH]{};
                CStringHelper::ConvertWideToUTF(m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc()->szText.c_str(), szInputText);
                //m_szInputText = szInputText;
                strcpy_s(m_szInputText, szInputText);
                m_vColor = m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc()->vColor;
            }

            //if (m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc())
            //    m_vColor = m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc()->vColor;
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
    CStringHelper::ConvertWideToUTF(m_pTargetUI->Get_UIBase_Desc().szUITag.c_str(), szUITag);

    GUI::Begin("OPTIONS");
    
    GUI::Title(szUITag);
    
    if (GUI::BeginTabBar("Edit Tab"))
    {
        if (GUI::BeginTabItem("Position"))
        {
            Set_Position(&m_vOldPos, &m_vEditedPos);
            GUI::EndTabItem();
        }

        if (GUI::BeginTabItem("Size"))
        {
            Set_Size(&m_vOldSize, &m_vEditedSize);
            GUI::EndTabItem();
        }

        if (GUI::BeginTabItem("Texture"))
        {
            Set_Texture();
            GUI::EndTabItem();
        }

        if (GUI::BeginTabItem("Animation"))
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

void CGUIManager::Set_Size(_float2* pOldSize, _float2* pEditedSize)
{
    string size = "Origin : " + to_string(pOldSize->x) + ", " + to_string(pOldSize->y);

    GUI::Text(size.c_str());
    GUI::Separator();

    GUI::InputFloat("Size_x", &pEditedSize->x);
    GUI::InputFloat("Size_y", &pEditedSize->y);

    m_pTargetUI->Set_Size(pEditedSize->x, pEditedSize->y);

    if (GUI::Button("Reset"))
    {
        m_pTargetUI->Set_Size(pOldSize->x, pOldSize->y);
        *pEditedSize = *pOldSize;
    }

    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Apply"))
    {
        *pOldSize = *pEditedSize;

        Client::CUIBase::UIBASE_DESC Desc = m_pTargetUI->Get_UIBase_Desc();
        Desc.fSizeX = pOldSize->x;
        Desc.fSizeY = pOldSize->y;

        m_pTargetUI->Set_UIBase_Desc(Desc);
    }
    GUI::PopStyleColor();
}

void CGUIManager::Set_Position(_float2* pOldPos, _float2* pEditedPos)
{
    string CurrentPos = "CurrentPos : " + to_string(m_pTargetUI->Get_UIBase_Desc().fX) + ", " + to_string(m_pTargetUI->Get_UIBase_Desc().fY);

    _vector vPos = m_pTargetUI->GetTransform()->Get_State(STATE::POSITION);
    _float2 vPosXY = { XMVectorGetX(vPos), XMVectorGetY(vPos) };

    string TransformPos = "Transform : " + to_string(vPosXY.x) + ", " + to_string(vPosXY.y);
    string pos = "Origin : " + to_string(pOldPos->x) + ", " + to_string(pOldPos->y);

    GUI::Text(TransformPos.c_str());
    GUI::Separator();
    GUI::Text(CurrentPos.c_str());
    GUI::Separator();
    GUI::Text(pos.c_str());
    GUI::Separator();

    GUI::DragFloat("Position_x", &pEditedPos->x, 1.f, 0.f, 0.f, "%.2f");
    GUI::DragFloat("Position_y", &pEditedPos->y, 1.f, 0.f, 0.f, "%.2f");

    m_pTargetUI->Set_Position(pEditedPos->x, pEditedPos->y);
    //m_pTargetUI->GetTransform()->Set_State(STATE::POSITION, XMVectorSet(pEditedPos->x, pEditedPos->y, 0.f, 1.f));

    if (GUI::Button("Reset"))
    {
        //m_pTargetUI->GetTransform()->Set_State(STATE::POSITION, XMVectorSet(pOldPos->x, pOldPos->y, 0.f, 1.f));
        m_pTargetUI->Set_Position(pOldPos->x, pOldPos->y);
        *pEditedPos = *pOldPos;
    }

    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Apply"))
    {
        *pOldPos = *pEditedPos;

        Client::CUIBase::UIBASE_DESC Desc = m_pTargetUI->Get_UIBase_Desc();
        Desc.fOffsetX = pOldPos->x;
        Desc.fOffsetY = pOldPos->y;

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

    CUIResourceStore::UI_TEXTURE_RESOURCE_DESC ResDesc{};

    ResDesc = m_pUIResourceStore->Get_UI_Texture_Desc(szTextureComTag);

    if (m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc())
    {
        _float fAlpha = m_pTargetUI->Get_UIBase_Desc().fAlpha;

        GUI::DragFloat("Alpha", &fAlpha, 0.01f, 0.f, 1.f);
        m_pTargetUI->Set_Alpha(fAlpha);

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
        m_pTargetUI->Set_TextureCom(m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc()->szTextureComTag,
            m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc()->szProtoTag, 
            m_pTargetUI->Get_UIBase_Desc().Get_UI_Texture_Desc()->iTextureIndex);
    }
    GUI::SameLine();
    GUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.f, 1.0f));
    if (GUI::Button("Apply"))
    {
        Client::CUIBase::UIBASE_DESC UIDesc = m_pTargetUI->Get_UIBase_Desc();

        m_pTargetUI->Set_UIBase_Desc(UIDesc);
    }
    GUI::PopStyleColor();
}

void CGUIManager::Set_Text()
{
    Client::CUIBase::UIBASE_DESC UIDesc = m_pTargetUI->Get_UIBase_Desc();
    Client::CUIBase::UI_TEXT_DESC TextDesc{};

    GUI::InputText("Input Text", m_szInputText, IM_ARRAYSIZE(m_szInputText));

    WCHAR szInputText[MAX_PATH]{};
    CStringHelper::ConvertUTFToWide(m_szInputText, szInputText);

    if (UIDesc.Get_UI_Text_Desc())
    {
        if (GUI::ColorPicker4("MyColorPicker", (float*)&m_vColor)) {
            // 색상이 변경될 때 처리
            TextDesc.vColor = m_vColor;
            TextDesc.szText = szInputText;
            UIDesc.Set_UI_Text_Desc(TextDesc);
            m_pTargetUI->Set_UIBase_Desc(UIDesc);
        }
    }

    if (GUI::Button("Apply"))
    {
        TextDesc.szText = szInputText;
        TextDesc.vColor = m_vColor;

        UIDesc.Set_UI_Text_Desc(TextDesc);

        m_pTargetUI->Set_UIBase_Desc(UIDesc);
    }
}

void CGUIManager::Set_Animation()
{
    // 위치, 크기, alpha, loop 조절해서 ui한테 보내기

    if (GUI::Button("Add Anim"))
    {
        GUI::OpenPopup("Add Anim");
    }

    if (GUI::BeginPopup("Add Anim"))
    {
        Add_Animation();

        if (GUI::Button("Close")) {
            GUI::CloseCurrentPopup();
        }
        GUI::EndPopup();
    }

    for (auto& AnimDesc : m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Descs())
    {
        _char szAnimTag[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF((TEXT("AnimTag : ") + AnimDesc.first).c_str(), szAnimTag);

        if (GUI::TreeNode(szAnimTag))
        {
            GUI::DragFloat("Duration", &AnimDesc.second->fDuration, 0.01f, 0.f, 0.f, "%.2f");
            GUI::Checkbox("Loop", &AnimDesc.second->isLoop);

            if (GUI::Button("Add Track"))
            {
                GUI::OpenPopup("Add Track");
            }

            GUI::Separator();

            _char szDebug[MAX_PATH]{};
            sprintf_s(szDebug, "deltaTime = %f\nanimDeltaTime = %f\nplayTime = %.3f\nalpha = %.3f, pos.x = %.3f, pos.y = %.3f",
                m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime")),
                m_pTargetUI->Get_AnimationCom()->Get_DeltaTime(),
                m_pTargetUI->Get_AnimationCom()->Get_PlayTime(),
                m_pTargetUI->Get_UIBase_Desc().fAlpha,
                m_pTargetUI->Get_UIBase_Desc().fOffsetX,
                m_pTargetUI->Get_UIBase_Desc().fOffsetY);

            GUI::Text(szDebug);
            GUI::Separator();
            if (GUI::Button("Play"))
            {
                m_pTargetUI->Play_Anim(AnimDesc.first);
            }
            GUI::SameLine();
            if (GUI::Button("Pause"))
            {
                m_pTargetUI->Pause_Anim();
            }
            GUI::SameLine();
            if (GUI::Button("Stop"))
            {
                m_pTargetUI->Stop_Anim();
            }

            if (GUI::BeginPopup("Add Track"))
            {
                Add_AnimTrack(AnimDesc.first);

                if (GUI::Button("Close")) {
                    GUI::CloseCurrentPopup();
                }
                GUI::EndPopup();
            }

            for (auto& TrackDesc : m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(AnimDesc.first)->Get_UI_Track_Descs())
            {
                _char szTrackTag[MAX_PATH]{};
                CStringHelper::ConvertWideToUTF(TrackDesc.first.c_str(), szTrackTag);                    

                if (GUI::TreeNode(szTrackTag))
                {
                    Set_AnimTrack(AnimDesc.first, TrackDesc.first);
                    GUI::TreePop();
                }
            }

            GUI::TreePop();
        }
    }
}

void CGUIManager::Add_Animation()
{
    GUI::InputText("Anim Tag", m_szInputAnimTag, IM_ARRAYSIZE(m_szInputText));

    if (GUI::Button("Add"))
    {
        WCHAR szAnimTag[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide(m_szInputAnimTag, szAnimTag);

        Client::CUIBase::UI_ANIM_DESC AnimDesc{};
        
        Client::CUIBase::UIBASE_DESC Desc = m_pTargetUI->Get_UIBase_Desc();
        Desc.Add_UI_Anim_Desc(szAnimTag, AnimDesc);

        m_pTargetUI->Set_UIBase_Desc(Desc);
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
    //GUI::InputText("Track Tag", m_szInputTrackTag, IM_ARRAYSIZE(m_szInputTrackTag));

    //GUI::DragFloat(m_szInputTrackTag, m_vPram.x, );

    _char szText[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF((TEXT("TrackTag : ") + szTrackTag).c_str(), szText);

    GUI::Text(szText);

    if (szTrackTag == TEXT("Position"))
    {
        if (GUI::TreeNode("StartPram"))
        {
            GUI::DragFloat("x", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.x, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("y", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.y, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("z", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.z, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("w", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.w, 1.f, 0.f, 0.f, "%.2f");
            GUI::TreePop();
        }
        if (GUI::TreeNode("EndPram"))
        {
            GUI::DragFloat("x", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.x, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("y", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.y, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("z", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.z, 1.f, 0.f, 0.f, "%.2f");
            GUI::DragFloat("w", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.w, 1.f, 0.f, 0.f, "%.2f");
            GUI::TreePop();
        }
    }

    if (szTrackTag == TEXT("Alpha"))
    {
        GUI::DragFloat("Start Alpha", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vStartParam.x, 0.01f, 0.f, 1.f, "%.2f");
        GUI::DragFloat("End Alpha", &m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Get_UI_Track_Desc(szTrackTag)->vEndParam.x, 0.01f, 0.f, 1.f, "%.2f");
    }
}

void CGUIManager::Add_AnimTrack(_wstring szAnimTag)
{
    //GUI::InputText("Track Tag", m_szCurrentTrackTag, IM_ARRAYSIZE(m_szCurrentTrackTag));

    Select_AnimTrack_Tags(m_szCurrentTrackTag);


    if (GUI::Button("Add"))
    {
        WCHAR szTrackTag[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide(m_szCurrentTrackTag, szTrackTag);

       /* Client::CUIBase::UI_ANIM_DESC AnimDesc{};

        Client::CUIBase::UIBASE_DESC Desc{};
        Desc = m_pTargetUI->Get_UIBase_Desc();
        Desc.Add_UI_Anim_Desc(szAnimTag, AnimDesc);

        m_pTargetUI->Set_UIBase_Desc(Desc);*/

       /* Client::CUIBase::UI_ANIM_TRACK_DESC TrackDesc{};

        Desc.Add_UI_Track_Desc(szTrackTag, TrackDesc);*/

        Client::CUIBase::UI_ANIM_TRACK_DESC TrackDesc{};
        TrackDesc.szTrackTag = szTrackTag;

        if (TrackDesc.szTrackTag == TEXT("Position"))
        {
            TrackDesc.vStartParam.x = m_pTargetUI->Get_UIBase_Desc().fOffsetX;
            TrackDesc.vStartParam.y = m_pTargetUI->Get_UIBase_Desc().fOffsetY;
        }
        if (TrackDesc.szTrackTag == TEXT("Alpha"))
        {
            TrackDesc.vStartParam.x = m_pTargetUI->Get_UIBase_Desc().fAlpha;
        }

        m_pTargetUI->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag)->Add_UI_Track_Desc(szTrackTag, TrackDesc);

        //strcpy_s(m_szCurrentTrackTag, sizeof(m_szCurrentTrackTag), "");

       /* Client::CUIBase::UI_ANIM_DESC AnimDesc{};
        AnimDesc = *;

        AnimDesc.Add_UI_Track_Desc(szTrackTag, TrackDesc);*/

        //Client::CUIBase::UIBASE_DESC Desc{};
        //Desc = m_pTargetUI->Get_UIBase_Desc();
        ////Desc.Add_UI_Anim_Desc(szAnimTag, AnimDesc);

        //m_pTargetUI->Set_UIBase_Desc(Desc);
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

    //Safe_Release(m_pUIHUD);
    //
    //for (auto& iter : m_pLayers)
    //    Safe_Release(iter);

    Safe_Release(m_pGameInstance);
}
