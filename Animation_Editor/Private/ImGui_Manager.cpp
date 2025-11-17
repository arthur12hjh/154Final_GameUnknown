#include "pch.h"

#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "Tool_Manager.h"

#include "GameObject.h"
#include "ContainerObject.h"
#include "Animation.h"

CImGui_Manager::CImGui_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pTool_Manager{ CTool_Manager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pTool_Manager);
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	//LoadFont();

	m_bIsActive = FALSE;
	m_bIsActiveTrigger = FALSE;

	return S_OK;
}

void CImGui_Manager::Priority_Update(_float fTimeDelta)
{
	if (m_bIsActiveTrigger == TRUE)
		m_bIsActive = TRUE;

	if (m_bIsActiveTrigger == FALSE)
		m_bIsActive = FALSE;

	if (m_bIsActive == FALSE)
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}


void CImGui_Manager::Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;

	Update_ToolBar();
	Update_AnimationList();
	Update_KeyFrameTool();
}

void CImGui_Manager::Late_Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;
}

HRESULT CImGui_Manager::Render()
{
	if (m_bIsActive == FALSE)
		return S_OK;

	ShowGUI();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}


void CImGui_Manager::ShowGUI()
{
}

void CImGui_Manager::LoadFont()
{
	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = nullptr;

	string strFontTag = "../Bin/Resources/Fonts/H2GTRM.TTF";

	ifstream ifile;

	ifile.open(strFontTag);
	if (ifile)
	{
		font = io.Fonts->AddFontFromFileTTF(strFontTag.c_str(), 16.f, NULL, io.Fonts->GetGlyphRangesKorean());
	}
	IM_ASSERT(font != NULL);

}

void CImGui_Manager::Create_Character(const _wstring& szCharacterTag)
{
	if (nullptr != m_pSelectedObject)
	{
		m_pSelectedObject->Set_Dead(TRUE);
		m_iSelectedAnimationIndex = 0;
		m_iSelectedEventIndex = 0;
		m_iBeforeEventIndex = 0;
		m_pAnimationList = nullptr;
		m_pSelectedObject = nullptr;
		
	}

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), szCharacterTag, ENUM_CLASS(LEVEL::EDITOR), TEXT("Layer_Player"));

	list<CGameObject*>* pObjLists = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Layer_Player"));

	m_pSelectedObject = pObjLists->back();

	m_pAnimationList = static_cast<CModel*>(static_cast<CContainerObject*>(m_pSelectedObject)->Get_Component(TEXT("Part_Body"), TEXT("Com_Model")))->Get_AnimationList();
}

void CImGui_Manager::Kill_Character()
{
	if (nullptr == m_pSelectedObject)
		return;
	m_pSelectedObject->Set_Dead(TRUE);
	m_iSelectedAnimationIndex = 0;
	m_pAnimationList = nullptr;
	m_pSelectedObject = nullptr;
	m_pAnimationList = nullptr;
}

void CImGui_Manager::Update_ToolBar()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0)); // 화면 상단 좌표
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 30)); // 전체 너비, 높이 30
	ImGui::Begin("Toolbar", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoCollapse);

	if (ImGui::Button("Load"))
	{
		ImGui::OpenPopup("Load");
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		ImGui::OpenPopup("Save");
	}
	ImGui::SameLine();
	if (ImGui::Button("Editor Preferences"))
	{
		ImGui::OpenPopup("EditorPreferences");
	}
	ImGui::SameLine();
	if (ImGui::Button("Animation"))
	{
		ImGui::OpenPopup("Animation");
	}
	ImGui::SameLine();
	if (ImGui::Button("Sound"))
	{
		ImGui::OpenPopup("Sound");
	}
	ImGui::SameLine();
	if (ImGui::Button("Tools"))
	{
		ImGui::OpenPopup("Tools");
	}

	Update_ToolBar_LoadCharacter();
	Update_ToolBar_Save_Animation();


	ImGui::End();
}

void CImGui_Manager::Update_ToolBar_LoadCharacter()
{
	static int iCurrentIndex = 0;
	static int iBeforeIndex = 0;

	if (ImGui::BeginPopup("Load"))
	{
		ImGui::Text("Load Character");
		if (ImGui::Selectable("Eve")) { iCurrentIndex = 1; }
		if (ImGui::Selectable("Dororong")) { iCurrentIndex = 2; }
		if (ImGui::Selectable("Gigas")) { iCurrentIndex = 3; }

		ImGui::EndPopup();
	}

	if (iCurrentIndex != iBeforeIndex)
	{
		// 구조
		// 캐릭터 객체 코드를 따로 만들 것인가?
		// 캐릭터 객체 코드를 같이 만들면 PartObject 관련 처리는 어떻게 할 것인가?
		switch (iCurrentIndex)
		{
		case 0:
			Kill_Character();
			break;
		case 1:
			Create_Character(TEXT("Prototype_GameObject_Character"));
			break;
		case 2:
			Create_Character(TEXT("Prototype_GameObject_Dororong"));
			break;
		case 3:
			Create_Character(TEXT("Prototype_GameObject_Gigas"));
			break;
		}

		iBeforeIndex = iCurrentIndex;
	}


}

void CImGui_Manager::Update_ToolBar_Save_Animation()
{
	int a = 10;
}

void CImGui_Manager::Update_ToolBar_Editor_Preferences()
{
	static int iCurrentIndex = 0;
	static int iBeforeIndex = 0;

	if (ImGui::BeginPopup("Load"))
	{
		ImGui::Text("Load Character");
		if (ImGui::Selectable("Eve")) { iCurrentIndex = 1; }
		if (ImGui::Selectable("Dororong")) { iCurrentIndex = 2; }
		if (ImGui::Selectable("Gigas")) { iCurrentIndex = 3; }

		ImGui::EndPopup();
	}

	if (iCurrentIndex != iBeforeIndex)
	{
		// 구조
		// 캐릭터 객체 코드를 따로 만들 것인가?
		// 캐릭터 객체 코드를 같이 만들면 PartObject 관련 처리는 어떻게 할 것인가?
		switch (iCurrentIndex)
		{
		case 0:
			Kill_Character();
			break;
		case 1:
			Create_Character(TEXT("Prototype_GameObject_Character"));
			break;
		case 2:
			Create_Character(TEXT("Prototype_GameObject_Dororong"));
			break;
		case 3:
			Create_Character(TEXT("Prototype_GameObject_Gigas"));
			break;
		}

		iBeforeIndex = iCurrentIndex;
	}
}

void CImGui_Manager::Update_AnimationList()
{
	ImGui::SetNextWindowPos(ImVec2(0, 30)); // 화면 상단 좌표
	ImGui::SetNextWindowSize(ImVec2(450, ImGui::GetIO().DisplaySize.y)); // 왼쪽에 갖다붙일거임


	ImGui::Begin(u8"Animation List", NULL);

	if (nullptr == m_pAnimationList)
	{
		ImGui::End();
		m_iSelectedAnimationIndex = 0;
		m_iBeforeAnimationIndex = -1;
		return;
	}

	if (!m_pAnimationList || m_pAnimationList->empty())
	{
		ImGui::Text("No animations loaded.");
		return;
	}

	ImGui::Text("Animation List:");

	if (m_iSelectedAnimationIndex != -1)
	{
		ImVec4 vFontColor = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);

		ImGui::TextColored(vFontColor, "Selected Index:");
		ImGui::SameLine();
		ImGui::Text("%d   ", m_iSelectedAnimationIndex);

		ImGui::TextColored(vFontColor, "Selected Name:");
		ImGui::SameLine();
		ImGui::Text("%s", (*m_pAnimationList)[m_iSelectedAnimationIndex]->Get_Name());

	}

	ImGui::Separator();
	ImGui::Separator();

	for (int i = 0; i < m_pAnimationList->size(); ++i)
	{
		CAnimation* pAnimation = (*m_pAnimationList)[i];
		const char* szAnimationName = pAnimation->Get_Name();


		if (ImGui::Selectable(szAnimationName, m_iSelectedAnimationIndex == i))
		{
			m_iSelectedAnimationIndex = i;
			m_iSelectedEventIndex = 0;
			m_iBeforeEventIndex = 0;
			Update_AnimNotifyList();	// 애니메이션 이름을 기반으로한 map에서 받아오면 될듯. 일단 미루자
		}
	}

	if (m_iSelectedAnimationIndex != m_iBeforeAnimationIndex)
	{
		m_iBeforeAnimationIndex = m_iSelectedAnimationIndex;
		static_cast<CModel*>(static_cast<CContainerObject*>(m_pSelectedObject)->Get_Component(TEXT("Part_Body"), TEXT("Com_Model")))->Set_AnimationIndex(m_iSelectedAnimationIndex);
	}



	ImGui::End();

}

void CImGui_Manager::Update_AnimNotifyList()
{
	m_pCurrentAnimationEventList = nullptr;
	/***********************************************************************
		██╗    ██╗ █████╗ ██████╗ ███╗   ██╗██╗███╗   ██╗ ██████╗ 
		██║    ██║██╔══██╗██╔══██╗████╗  ██║██║████╗  ██║██╔════╝ 
		██║ █╗ ██║███████║██████╔╝██╔██╗ ██║██║██╔██╗ ██║██║  ███╗
		██║███╗██║██╔══██║██╔══██╗██║╚██╗██║██║██║╚██╗██║██║   ██║
		╚███╔███╔╝██║  ██║██║  ██║██║ ╚████║██║██║ ╚████║╚██████╔╝
		 ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═══╝ ╚═════╝ 
                                                          
		████████████████████████████████████████████████████████████████████████████
	⚠ 이후 DataManager 제작 시 map에서 vector<ANIM_NOTIFY>* 받아오게끔 만들어야함 ⚠
		████████████████████████████████████████████████████████████████████████████
************************************************************************/

	m_pCurrentAnimationEventList = new vector<ANIM_NOTIFY>;
}

void CImGui_Manager::Update_KeyFrameTool()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 500, 30)); // 화면 상단 좌표
	ImGui::SetNextWindowSize(ImVec2(500, ImGui::GetIO().DisplaySize.y)); // 오른쪽에 갖다붙일거임

	ImGui::Begin(u8"KeyFrame", NULL, ImGuiWindowFlags_MenuBar);

	Update_TimeLine();
	Update_EventMaker();

	ImGui::End();
}

void CImGui_Manager::Update_TimeLine()
{
	if (nullptr == m_pAnimationList)
	{
		ImGui::Text("Select Animation");
		return;
	}
	
	CAnimation* pAnimation = (*m_pAnimationList)[m_iSelectedAnimationIndex];
	if (nullptr == pAnimation)
	{
		ImGui::Text("Invalid Animation");
		return;
	}
	
	ImVec2 vCanvasSize = ImVec2(ImGui::GetContentRegionAvail().x, 360.f);

	ImGui::InvisibleButton("Canvas_Timeline", vCanvasSize, ImGuiButtonFlags_MouseButtonLeft);	// 이걸로 타임라인 캔버스 전체를 클릭 가능한 영역으로 만듬
	
	ImVec2 vCanvasLeftTop = ImGui::GetItemRectMin();
	ImVec2 vCanvasRightBottom = ImGui::GetItemRectMax();
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	
	pDrawList->AddRectFilled(vCanvasLeftTop, vCanvasRightBottom, IM_COL32(40, 40, 45, 255)); // 이걸로 타임라인바 배경 색 채우는거임
	
	// 타임라인 세로선 그릴 때 쓸 좌표
	_float fVerticalLineTop = vCanvasLeftTop.y;
	_float fVerticalLineBottom = vCanvasRightBottom.y;

	_float fCenterX = (vCanvasLeftTop.x + vCanvasRightBottom.x) * 0.5f;
	_float fCenterY = (vCanvasLeftTop.y + vCanvasRightBottom.y) * 0.5f;
	
	// 현재 키프레임이 타임라인의 x좌표 어디에 있는지 표시할 float값
	//_float fSaturatedCurrentKeyFrameRatio = max(pAnimation->Get_AnimationKeyFrameIndex() / pAnimation->Get_Duration(), 0.f);
	_float fSaturatedCurrentKeyFrameRatio = pAnimation->Get_SaturatedTrackPosition();

	fSaturatedCurrentKeyFrameRatio = min(fSaturatedCurrentKeyFrameRatio, 1.f);
	_float fCurrentFramePosition = vCanvasLeftTop.x + fSaturatedCurrentKeyFrameRatio * vCanvasSize.x;
	
	// 눈금 사이 키프레임
	_int iFrameTick = 0;
	
	if (pAnimation->Get_Duration() <= 10)
		iFrameTick = 1;
	else if(pAnimation->Get_Duration() <= 50)
		iFrameTick = 5;
	else if (pAnimation->Get_Duration() <= 100)
		iFrameTick = 10;
	else
		iFrameTick = 20;
	
	// 138프레임일 경우, 20 키프레임 단위로 0-20-40-60-80-100-120-140 총 8개 그려야함.
	_int iNumVerticalLines = (pAnimation->Get_Duration() + iFrameTick - 1) / iFrameTick;

	ImU32 vVerticalLineColor = IM_COL32(120, 120, 130, 255);
	ImU32 vTextColor = IM_COL32(220, 220, 220, 255);

	for (_int iCurrentLine = 0; iCurrentLine <= iNumVerticalLines; ++iCurrentLine)
	{
		_uint iCurrentFrame = iCurrentLine * iFrameTick;

		_float fCurrentKeyPositionX = max(iCurrentFrame / pAnimation->Get_Duration(), 0.f);
		fCurrentKeyPositionX = min(iCurrentFrame / pAnimation->Get_Duration(), 1.f);
		fCurrentKeyPositionX = vCanvasLeftTop.x + fCurrentKeyPositionX * vCanvasSize.x;

		pDrawList->AddLine(ImVec2(fCurrentKeyPositionX, fVerticalLineTop), ImVec2(fCurrentKeyPositionX, fVerticalLineBottom), vVerticalLineColor, 1.0f); // ImVec으로는 색 설정이 안됨


		char szCurrentKeyFrame[8];
		sprintf_s(szCurrentKeyFrame, "%d", iCurrentFrame);
		pDrawList->AddText(ImVec2(fCurrentKeyPositionX , vCanvasLeftTop.y ), vTextColor, szCurrentKeyFrame);
	}

	ImU32 vCursorFrameColor = IM_COL32(180, 220, 255, 255);

	// 여기까지가 기본 키프레임 툴바고, 이제 현재 키 프레임을 표시하자
	pDrawList->AddLine(ImVec2(fCurrentFramePosition, vCanvasLeftTop.y), ImVec2(fCurrentFramePosition, vCanvasRightBottom.y), vCursorFrameColor);
	pDrawList->AddCircle(ImVec2(fCurrentFramePosition, vCanvasLeftTop.y), 5.f, vCursorFrameColor);

	///
	/// 클릭 관련 처리 넣기 V
	/// 클릭하면 해당 키프레임으로 이동 V
	/// 우클릭하면 이벤트 추가 V
	/// 
	/// [ 이벤트 ]
	///  -> 동그라미 이벤트 아이콘 표시 V
	///  -> 동그라미 이벤트 아이콘 클릭시 이벤트 편집 창 오픈
	///  -> 방향키로 키프레임 이동 가능
	/// 
	///  -> AnimationDataManager에 있는 map인 	map<char*, vector<ANIM_NOTIFY>>* m_pAnimationNotifyMap에서 m_pCurrentAnimationEventList 하나를 빼옴 
	///  -> 이건 그 애니메이션의 모든 노티파이를 담고 있음
	///  -> 없을 경우 vector 새로 만들어서 넣어줌
	/// 
	///  ->	unsigned int					iNotifyKeyFrame;
	///  ->	std::string						szNotifyTag;
	///  ->	std::vector<std::string>		szNotifyArguments; 
	///
	///
	///  이 이벤트들을 통해 게임에서 나타낼 것들
	///		-> PlaySound
	///			-> 함수태그, 소리 코드
	///		-> PlayEffect
	///			-> 함수태그, 이펙트 코드
	///		-> SpawnObject
	///			-> 함수 태그, 오브젝트 코드
	///		-> PlayCamera
	///			-> 함수 태그, 카메라 코드
	///		-> PlayCameraShake
	///			-> 함수 태그, 지속 시간 등

	// 타임라인에 이벤트 아이콘 그리기

	vector<pair<_float, _float>> vIconPositions;

	if (m_pCurrentAnimationEventList != nullptr)
	{

		ImU32 vIconColor = IM_COL32(255, 70, 30, 255);
		ImU32 vTextColor = IM_COL32(220, 220, 220, 255);

		_uint iIconIndex = 0;

		for (auto& iCurrentIcon : *m_pCurrentAnimationEventList)
		{
			_float fIconPositionX = max(iCurrentIcon.iNotifyKeyFrame / pAnimation->Get_Duration(), 0.f);
			fIconPositionX = min(iCurrentIcon.iNotifyKeyFrame / pAnimation->Get_Duration(), 1.f);
			fIconPositionX = vCanvasLeftTop.x + fIconPositionX * vCanvasSize.x;

			pDrawList->AddCircleFilled(ImVec2(fIconPositionX, vCanvasLeftTop.y + 20.f +  iIconIndex * 25.f), 5.f, vIconColor);

			pDrawList->AddText(ImVec2(fIconPositionX + 3.f, vCanvasLeftTop.y + 20.f + iIconIndex * 25.f - 8.f), vTextColor, iCurrentIcon.szNotifyTag.c_str());
			pDrawList->AddText(ImVec2(fIconPositionX + 3.f, vCanvasLeftTop.y + 20.f + iIconIndex * 25.f + 2.f), vTextColor, iCurrentIcon.szNotifyArg01.c_str());

			vIconPositions.push_back(make_pair(fIconPositionX, vCanvasLeftTop.y + 20.f + iIconIndex * 25.f));

			++iIconIndex;
		}
	}


	_bool bIsCanvasHovered = ImGui::IsItemHovered(); // 마우스가 타임라인 바 위에 있는지
	ImVec2 vMousePos = ImGui::GetMousePos();

	if (bIsCanvasHovered)
	{
		_float fRatioX = (vMousePos.x - vCanvasLeftTop.x) / vCanvasSize.x;
		_float fPosY = (vMousePos.y - vCanvasLeftTop.y);

		fRatioX = Clamp(fRatioX, 0.f, 1.f);
		_bool bIsCursorOnTrackPosition = TRUE;
		fPosY > 5.f ? bIsCursorOnTrackPosition = FALSE : bIsCursorOnTrackPosition = TRUE;

		// 마우스 오버시 이벤트 처리는 따로..

		// 해당 키프레임으로 이동...
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (bIsCursorOnTrackPosition)
			{
				pAnimation->Set_CurrentTrackPosition(fRatioX * pAnimation->Get_Duration());
			}
			else
			{
				// 이벤트 원이랑 위치 비교..
				_int iIndex = 0;
				for (auto& vIconPos : vIconPositions)
				{
					if (XMVectorGetX(XMVector3Length(XMVectorSet(vIconPos.first - vMousePos.x, vIconPos.second - vMousePos.y, 0.f, 0.f))) < 6.f)
					{
						m_iClickedKeyFrame = static_cast<_uint>(fRatioX * pAnimation->Get_Duration());

						if (nullptr == m_pCurrentAnimationEventList)
						{
							Update_AnimNotifyList();
						}

						m_iSelectedEventIndex = iIndex;
						ImGui::OpenPopup("EventMaker");
					}
					else
						++iIndex;
				}
			}
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			m_iClickedKeyFrame = static_cast<_uint>(fRatioX * pAnimation->Get_Duration());

			if (nullptr == m_pCurrentAnimationEventList)
			{
				Update_AnimNotifyList();
			}

			ANIM_NOTIFY AnimNotify;
			AnimNotify.iNotifyKeyFrame = m_iClickedKeyFrame;
			AnimNotify.szNotifyTag.clear();
			AnimNotify.szNotifyArg01.clear();
			AnimNotify.szNotifyArg02.clear();

			m_pCurrentAnimationEventList->push_back(AnimNotify);
			m_iSelectedEventIndex = m_pCurrentAnimationEventList->size() - 1;
			ImGui::OpenPopup("EventMaker");
		}

	}



}

void CImGui_Manager::Update_EventMaker()
{
	if (nullptr == m_pCurrentAnimationEventList)
		return;

	if (m_iSelectedEventIndex < 0)
		return;

	static _char szNotifyEditTag[MAX_PATH];
	static _char szNotifyArg01[MAX_PATH];
	static _char szNotifyArg02[MAX_PATH];

	// 먼저 기존 이벤트랑 다르면 초기화시켜줘야한다.
	if (m_iBeforeEventIndex != m_iSelectedEventIndex)
	{
		if (m_iSelectedEventIndex >= 0 && m_iSelectedEventIndex < m_pCurrentAnimationEventList->size())
		{
			ANIM_NOTIFY SelectedAnimNotify = (*m_pCurrentAnimationEventList)[m_iSelectedEventIndex];

			strcpy_s(szNotifyEditTag, SelectedAnimNotify.szNotifyTag.c_str());
			strcpy_s(szNotifyArg01, SelectedAnimNotify.szNotifyArg01.c_str());
			strcpy_s(szNotifyArg02, SelectedAnimNotify.szNotifyArg02.c_str());

			m_iClickedKeyFrame = SelectedAnimNotify.iNotifyKeyFrame;

		}
		else
		{
			szNotifyEditTag[0] = '\0';
			szNotifyArg01[0] = '\0';
			szNotifyArg02[0] = '\0';
		}

		m_iBeforeEventIndex = m_iSelectedEventIndex;
	}

	if (ImGui::BeginPopupModal("EventMaker", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Animation Notify Maker");
		ImGui::Separator();

		ImGui::InputText("Event Tag", szNotifyEditTag, sizeof(szNotifyEditTag));
		ImGui::InputText("Event Arg01", szNotifyArg01, sizeof(szNotifyArg01));
		ImGui::InputText("Event Arg02", szNotifyArg02, sizeof(szNotifyArg02));
		ImGui::InputInt("KeyFrame", &m_iClickedKeyFrame);
		if(m_iClickedKeyFrame < 0)
			m_iClickedKeyFrame = 0;

		if(m_iClickedKeyFrame > (*m_pAnimationList)[m_iSelectedAnimationIndex]->Get_Duration())
			m_iClickedKeyFrame = (*m_pAnimationList)[m_iSelectedAnimationIndex]->Get_Duration();

		if (ImGui::Button("Save"))
		{
			ANIM_NOTIFY& SelectedAnimNotify = (*m_pCurrentAnimationEventList)[m_iSelectedEventIndex];
			SelectedAnimNotify.szNotifyTag = szNotifyEditTag;
			SelectedAnimNotify.szNotifyArg01 = szNotifyArg01;
			SelectedAnimNotify.szNotifyArg02 = szNotifyArg02;
			SelectedAnimNotify.iNotifyKeyFrame = m_iClickedKeyFrame;

			m_iBeforeEventIndex = -1;
			ImGui::CloseCurrentPopup();

			m_iClickedKeyFrame = -1;
		}

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(20.f, 0.f));
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			m_iSelectedEventIndex = -1;
			m_iSelectedEventIndex = -1;
			ImGui::CloseCurrentPopup();

			m_iClickedKeyFrame = -1;
		}

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(20.f, 0.f));
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			m_pCurrentAnimationEventList->erase(m_pCurrentAnimationEventList->begin() + m_iSelectedEventIndex);
			m_iSelectedEventIndex = -1;
			m_iSelectedEventIndex = -1;
			ImGui::CloseCurrentPopup();

			m_iClickedKeyFrame = -1;
		}


		ImGui::EndPopup();
	}

}

CImGui_Manager* CImGui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImGui_Manager* pInstance = new CImGui_Manager();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed to Create : CImGui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTool_Manager);
}
