#include "pch.h"
#include "MapTool.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Cell.h"
#include "Terrain.h"
#include <fstream>
#include "LightTool.h"

CMapTool::CMapTool()
{

}


HRESULT CMapTool::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();

	list<CGameObject*>* pTerrainList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Terrain"));

	if (pTerrainList != nullptr && !pTerrainList->empty())
	{
		m_pTerrain = dynamic_cast<CTerrain*>(pTerrainList->back());
	}
	else
	{
		m_pTerrain = nullptr; // 안전하게 nullptr로 설정
	}



	return S_OK;
}

void CMapTool::Priority_Update(_float fTimeDelta)
{
}

void CMapTool::Update(_float fTimeDelta)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	if (m_eToolMode == TOOL_MODE::MAP)
	{
		if (m_bIsNaviEditMode && m_eNaviMode == NAVI_MODE::ADD_POINT)
		{

			// 마우스 좌클릭 이벤트 체크
			if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
			{

				_float3 vPickedPoint = {};
				if (true == m_pGameInstance->isPicking(&vPickedPoint))
				{
					_vector vPickPoint = XMVectorSet(vPickedPoint.x, vPickedPoint.y, vPickedPoint.z, 1.f);
					// 1. 네비게이션 포인트 추가
					Add_NaviPoint(vPickPoint);

					// 2. 세 번째 점이 찍혔는지 확인
					if (m_iNaviPointCount == 3)
					{
						Add_NaviCell();
					}
				}

			}
			return; // 네비게이션 모드일 때는 다른 오브젝트 추가 로직은 건너뜁니다.
		}

		if (!m_bIsDeplayMode && m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
		{
			if (m_eCurrentObject == ADD_OBJECT::END)
				return;

			_float3 vPickedPoint = {};
			if (true == m_pGameInstance->isPicking(&vPickedPoint))
			{
				_vector vPickPoint = XMVectorSet(vPickedPoint.x, vPickedPoint.y, vPickedPoint.z, 1.f);
				HRESULT hr = E_FAIL;
				_wstring layerTag = L"";
				_wstring protoTag = L"";

				if (m_eCurrentObject == ADD_OBJECT::PLAYER)
				{
					protoTag = TEXT("Prototype_GameObject_Player"); layerTag = TEXT("Layer_Player");
				}
				else if (m_eCurrentObject == ADD_OBJECT::VIL_BUI03_04)
				{
					protoTag = TEXT("Prototype_GameObject_Vil_Bui03_04"); layerTag = TEXT("Layer_Vil_Bui03_04");
				}
				else if (m_eCurrentObject == ADD_OBJECT::INSCRIPTION_L)
				{
					protoTag = TEXT("Prototype_GameObject_Inscription_L"); layerTag = TEXT("Layer_Inscription_L");
				}
				else if (m_eCurrentObject == ADD_OBJECT::INSCRIPTION_R)
				{
					protoTag = TEXT("Prototype_GameObject_Inscription_R"); layerTag = TEXT("Layer_Inscription_R");
				}
				else if (m_eCurrentObject == ADD_OBJECT::TOMBSTONE)
				{
					protoTag = TEXT("Prototype_GameObject_TombStone"); layerTag = TEXT("Layer_TombStone");
				}
				else if (m_eCurrentObject == ADD_OBJECT::TOMBSTONEBASE1)
				{
					protoTag = TEXT("Prototype_GameObject_TombStoneBase1"); layerTag = TEXT("Layer_TombStoneBase1");
				}
				else if (m_eCurrentObject == ADD_OBJECT::TOMBSTONEBASE2)
				{
					protoTag = TEXT("Prototype_GameObject_TombStoneBase2"); layerTag = TEXT("Layer_TombStoneBase2");
				}
				else if (m_eCurrentObject == ADD_OBJECT::STONEWALL1)
				{
					protoTag = TEXT("Prototype_GameObject_StoneWall1"); layerTag = TEXT("Layer_StoneWall1");
				}
				else if (m_eCurrentObject == ADD_OBJECT::STONEWALL2)
				{
					protoTag = TEXT("Prototype_GameObject_StoneWall2"); layerTag = TEXT("Layer_StoneWall2");
				}
				else if (m_eCurrentObject == ADD_OBJECT::STONE1)
				{
					protoTag = TEXT("Prototype_GameObject_Stone1"); layerTag = TEXT("Layer_Stone1");
				}
				else if (m_eCurrentObject == ADD_OBJECT::STONE2)
				{
					protoTag = TEXT("Prototype_GameObject_Stone2"); layerTag = TEXT("Layer_Stone2");
				}
				else if (m_eCurrentObject == ADD_OBJECT::STONE3)
				{
					protoTag = TEXT("Prototype_GameObject_Stone3"); layerTag = TEXT("Layer_Stone3");
				}
				else if (m_eCurrentObject == ADD_OBJECT::STONE4)
				{
					protoTag = TEXT("Prototype_GameObject_Stone4"); layerTag = TEXT("Layer_Stone4");
				}
				else if (m_eCurrentObject == ADD_OBJECT::STAIR)
				{
					protoTag = TEXT("Prototype_GameObject_Stair"); layerTag = TEXT("Layer_Stair");
				}
				else if (m_eCurrentObject == ADD_OBJECT::BAMBOO)
				{
					protoTag = TEXT("Prototype_GameObject_Bamboo"); layerTag = TEXT("Layer_Bamboo");
				}
				else if (m_eCurrentObject == ADD_OBJECT::REED)
				{
					protoTag = TEXT("Prototype_GameObject_Reed"); layerTag = TEXT("Layer_Reed");
				}
				else if (m_eCurrentObject == ADD_OBJECT::ROCK1)
				{
					protoTag = TEXT("Prototype_GameObject_Rock1"); layerTag = TEXT("Layer_Rock1");
				}
				else if (m_eCurrentObject == ADD_OBJECT::ROCK2)
				{
					protoTag = TEXT("Prototype_GameObject_Rock2"); layerTag = TEXT("Layer_Rock2");
				}
				else if (m_eCurrentObject == ADD_OBJECT::ROCK3)
				{
					protoTag = TEXT("Prototype_GameObject_Rock3"); layerTag = TEXT("Layer_Rock3");
				}
				else if (m_eCurrentObject == ADD_OBJECT::ROCK4)
				{
					protoTag = TEXT("Prototype_GameObject_Rock4"); layerTag = TEXT("Layer_Rock4");
				}
				else if (m_eCurrentObject == ADD_OBJECT::ROCK5)
				{
					protoTag = TEXT("Prototype_GameObject_Rock5"); layerTag = TEXT("Layer_Rock5");
				}
				else if (m_eCurrentObject == ADD_OBJECT::ROCK6)
				{
					protoTag = TEXT("Prototype_GameObject_Rock6"); layerTag = TEXT("Layer_Rock6");
				}
				else if (m_eCurrentObject == ADD_OBJECT::ROCK7)
				{
					protoTag = TEXT("Prototype_GameObject_Rock7"); layerTag = TEXT("Layer_Rock7");
				}
				else if (m_eCurrentObject == ADD_OBJECT::ROCK8)
				{
					protoTag = TEXT("Prototype_GameObject_Rock8"); layerTag = TEXT("Layer_Rock8");
				}

				hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VILLAGE), protoTag, ENUM_CLASS(LEVEL::VILLAGE), layerTag, nullptr);

				if (SUCCEEDED(hr))
				{
					list<CGameObject*>* pObjects = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), layerTag.c_str());
					if (pObjects && !pObjects->empty())
					{
						// 새롭게 추가된 객체를 멤버 변수에 저장합니다.
						m_pLastAddedObject = pObjects->back();

						CTransform* pTransform = dynamic_cast<CTransform*>(m_pLastAddedObject->Find_Component(TEXT("Com_Transform")));
						if (pTransform)
						{
							pTransform->Set_State(STATE::POSITION, vPickPoint);
						}
					}
				}
				else
				{
					OutputDebugStringW(L"Failed to add GameObject to layer!\n");
				}
			}
		}

		if (m_bIsDeplayMode)
		{
			if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::RBUTTON)))
			{
				_float3 vPickedPoint = {};
				m_pPickedObject = Find_Object_To_Pick(ENUM_CLASS(LEVEL::VILLAGE), m_CurrentLayerName, &vPickedPoint);

				if (m_pPickedObject)
				{
					m_bIsDragging = true;
					m_pLastAddedObject = m_pPickedObject;
				}
				else
				{
					m_bIsDragging = false;
				}
			}
			else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
			{
				if (m_bIsDragging && m_pPickedObject)
				{
					_float3 vNewPickedPoint = {};

					// Terrain에 찍은 새로운 월드 좌표를 가져옵니다.
					if (true == m_pGameInstance->isPicking(&vNewPickedPoint))
					{

						_vector vPickedPoint = XMVectorSet(vNewPickedPoint.x, vNewPickedPoint.y, vNewPickedPoint.z, 1.f);

						CTransform* pTransform = dynamic_cast<CTransform*>(m_pPickedObject->Find_Component(TEXT("Com_Transform")));
						if (pTransform)
						{

							pTransform->Set_State(STATE::POSITION, vPickedPoint);

						}
					}
				}
			}
		}
	}

}

void CMapTool::Late_Update(_float fTimeDelta)
{
}

HRESULT CMapTool::Render()
{
	ImGui::SetNextWindowSize(ImVec2(300.f, 0.f), ImGuiCond_Once);


	ImGui::Begin("Map Editor");

	ImGui::Text("MapTool");
	if (ImGui::Checkbox("Map Tool Mode", &m_bIsMapMode))
	{
		// 모드가 켜지면 배치 모드(m_eCurrentObject)를 해제하고,
		// 모드가 꺼지면 드래그 상태를 초기화합니다.
		if (m_bIsMapMode)
		{
			m_eToolMode = TOOL_MODE::MAP;
		}
		else
		{
			m_eToolMode = TOOL_MODE::END;
		}
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();


	// 4. 네비게이션 편집 메뉴
	ImGui::Text("Navigation Editor");
	if (ImGui::Checkbox("Navi Edit Mode", &m_bIsNaviEditMode))
	{
		Set_NaviEditMode(m_bIsNaviEditMode);
	}

	if (m_bIsNaviEditMode)
	{
		ImGui::Text("Current Points: %d/3", m_iNaviPointCount);
		ImGui::SameLine();
		if (ImGui::Button("Reset Points"))
		{
			Reset_NaviPoints();
		}

		ImGui::Text("Instruction: Click on Terrain to set 3 vertices (A, B, C).");

		if (m_iNaviPointCount == 3)
		{
			ImGui::Text("Ready to create cell.");
			if (ImGui::Button("CREATE CELL (A, B, C)"))
			{
				Add_NaviCell();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				Reset_NaviPoints();
			}
		}

		ImGui::Spacing();
		ImGui::Separator();

		if (ImGui::Button("Save Navigation Data"))
		{
			if (SUCCEEDED(Save_NavigationData()))
			{
				MessageBoxW(g_hWnd, L"네비게이션 데이터 저장 성공.", L"알림", MB_OK);
			}
			else
			{
				MessageBoxW(g_hWnd, L"네비게이션 데이터 저장 실패.", L"알림", MB_OK | MB_ICONERROR);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Delete Latest Cell"))
		{
			Delete_Latest_NaviCell();
		}

		ImGui::SameLine();
		if (ImGui::Button("Delete All Cells"))
		{
			// CNavigation에 Reset_Line() 함수가 있었으므로 사용
			m_pNavigation->Reset_Line();
			m_pNavigation->SetUp_Neighbors(); // 이웃 재설정
			OutputDebugStringW(L"All Navigation Cells Deleted.\n");
		}
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	ImGui::Text("Registered Models");

	// ----------------------------------------------------
	// 1. 메인 테마/레벨 선택 드롭다운 (콤보 박스 사용 권장)
	// ----------------------------------------------------
	ImGui::Text("Current Theme Selection");
	const _char* themeNames[] = { "Tutorial", "Gorilla", "Scarlet" };
	_int nSelectedTheme = (_int)m_eCurrentMap;

	if (ImGui::Combo("Select Theme", &nSelectedTheme, themeNames, IM_ARRAYSIZE(themeNames)))
	{
		// 테마가 변경되면 현재 오브젝트 선택을 초기화할 수 있습니다.
		m_eCurrentObject = ADD_OBJECT::END;
		m_eCurrentMap = (MAP_THEME)nSelectedTheme;
	}

	if (m_eCurrentMap == MAP_THEME::TUTORIAL)
	{
	}

	else if(m_eCurrentMap == MAP_THEME::SCARLET)
	{

		ImGui::Text("Scarlet Map Objects");
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
		// 2. 모델 배치 / 삭제메뉴
		_int nSelectedModel = -1;
		_int nSelectedBuilding = -1;
		_int nSelectedEnvironment = -1;

		const _char* modelNames[] = { "Player" };
		const _char* buildingNames[] = { "Vil_Bui03_04", "Inscription_L", "Inscription_R", "TombStone", "TombStoneBase1", "TombStoneBase2", "Stair", "StoneWall1", "StoneWall2", 
										 "Stone1", "Stone2", "Stone3", "Stone4" };
		const _char* environmentNames[] = { "Bamboo", "Reed", "Rock1", "Rock2", "Rock3", "Rock4", "Rock5", "Rock6", "Rock7", "Rock8" };

		if (ImGui::CollapsingHeader("Models"))
		{
			if (ImGui::ListBox("##Models", &nSelectedModel, modelNames, IM_ARRAYSIZE(modelNames), 7))
			{
				if (nSelectedModel == 0) // Player
				{
					m_eCurrentObject = ADD_OBJECT::PLAYER;
					m_CurrentLayerName = TEXT("Layer_Player");
				}

			}
		}
		if (ImGui::CollapsingHeader("Buildings"))
		{
			if (ImGui::ListBox("##Buildings", &nSelectedBuilding, buildingNames, IM_ARRAYSIZE(buildingNames), 7))
			{
				if (nSelectedBuilding == 0) // Vil_Bui03_04
				{
					m_eCurrentObject = ADD_OBJECT::VIL_BUI03_04;
					m_CurrentLayerName = TEXT("Layer_Vil_Bui03_04");
				}
				else if (nSelectedBuilding == 1)
				{
					m_eCurrentObject = ADD_OBJECT::INSCRIPTION_L;
					m_CurrentLayerName = TEXT("Layer_Inscription_L");
				}
				else if (nSelectedBuilding == 2)
				{
					m_eCurrentObject = ADD_OBJECT::INSCRIPTION_R;
					m_CurrentLayerName = TEXT("Layer_Inscription_R");
				}
				else if (nSelectedBuilding == 3)
				{
					m_eCurrentObject = ADD_OBJECT::TOMBSTONE;
					m_CurrentLayerName = TEXT("Layer_TombStone");
				}
				else if (nSelectedBuilding == 4)
				{
					m_eCurrentObject = ADD_OBJECT::TOMBSTONEBASE1;
					m_CurrentLayerName = TEXT("Layer_TombStoneBase1");
				}
				else if (nSelectedBuilding == 5)
				{
					m_eCurrentObject = ADD_OBJECT::TOMBSTONEBASE2;
					m_CurrentLayerName = TEXT("Layer_TombStoneBase2");
				}
				else if (nSelectedBuilding == 6)
				{
					m_eCurrentObject = ADD_OBJECT::STAIR;
					m_CurrentLayerName = TEXT("Layer_Stair");
				}
				else if (nSelectedBuilding == 7)
				{
					m_eCurrentObject = ADD_OBJECT::STONEWALL1;
					m_CurrentLayerName = TEXT("Layer_StoneWall1");
				}
				else if (nSelectedBuilding == 8)
				{
					m_eCurrentObject = ADD_OBJECT::STONEWALL2;
					m_CurrentLayerName = TEXT("Layer_StoneWall2");
				}
				else if (nSelectedBuilding == 9)
				{
					m_eCurrentObject = ADD_OBJECT::STONE1;
					m_CurrentLayerName = TEXT("Layer_Stone1");
				}
				else if (nSelectedBuilding == 10)
				{
					m_eCurrentObject = ADD_OBJECT::STONE2;
					m_CurrentLayerName = TEXT("Layer_Stone2");
				}
				else if (nSelectedBuilding == 11)
				{
					m_eCurrentObject = ADD_OBJECT::STONE3;
					m_CurrentLayerName = TEXT("Layer_Stone3");
				}
				else if (nSelectedBuilding == 12)
				{
					m_eCurrentObject = ADD_OBJECT::STONE4;
					m_CurrentLayerName = TEXT("Layer_Stone4");
				}

			}
		}
		if (ImGui::CollapsingHeader("Environments"))
		{
			if (ImGui::ListBox("##Environments", &nSelectedEnvironment, environmentNames, IM_ARRAYSIZE(environmentNames), 7))
			{
				if (nSelectedEnvironment == 0)
				{
					m_eCurrentObject = ADD_OBJECT::BAMBOO;
					m_CurrentLayerName = TEXT("Layer_Bamboo");
				}
				else if (nSelectedEnvironment == 1)
				{
					m_eCurrentObject = ADD_OBJECT::REED;
					m_CurrentLayerName = TEXT("Layer_Reed");
				}
				else if (nSelectedEnvironment == 2)
				{
					m_eCurrentObject = ADD_OBJECT::ROCK1;
					m_CurrentLayerName = TEXT("Layer_Rock1");
				}
				else if (nSelectedEnvironment == 3)
				{
					m_eCurrentObject = ADD_OBJECT::ROCK2;
					m_CurrentLayerName = TEXT("Layer_Rock2");
				}
				else if (nSelectedEnvironment == 4)
				{
					m_eCurrentObject = ADD_OBJECT::ROCK3;
					m_CurrentLayerName = TEXT("Layer_Rock3");
				}
				else if (nSelectedEnvironment == 5)
				{
					m_eCurrentObject = ADD_OBJECT::ROCK4;
					m_CurrentLayerName = TEXT("Layer_Rock4");
				}
				else if (nSelectedEnvironment == 6)
				{
					m_eCurrentObject = ADD_OBJECT::ROCK5;
					m_CurrentLayerName = TEXT("Layer_Rock5");
				}
				else if (nSelectedEnvironment == 7)
				{
					m_eCurrentObject = ADD_OBJECT::ROCK6;
					m_CurrentLayerName = TEXT("Layer_Rock6");
				}
				else if (nSelectedEnvironment == 8)
				{
					m_eCurrentObject = ADD_OBJECT::ROCK7;
					m_CurrentLayerName = TEXT("Layer_Rock7");
				}
				else if (nSelectedEnvironment == 9)
				{
					m_eCurrentObject = ADD_OBJECT::ROCK8;
					m_CurrentLayerName = TEXT("Layer_Rock8");
				}

			}
		}

	}
	
	// 삭제 버튼
	if (ImGui::Button("Delete All"))
	{
		if (m_eCurrentObject == ADD_OBJECT::END) return E_FAIL;

		m_pObjects = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), m_CurrentLayerName);

		if (m_pObjects)
		{
			for (auto pObject : *m_pObjects)
			{
				pObject->Set_Dead(true);
			}
		}

	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Latest"))
	{
		if (m_eCurrentObject == ADD_OBJECT::END) return E_FAIL;

		m_pObject = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), m_CurrentLayerName)->back();

		m_pObject->Set_Dead(true);
	}
	ImGui::SameLine();
	if (ImGui::Button("NONE"))
	{
		m_eCurrentObject = ADD_OBJECT::END;
	}


	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	// 3. 특정좌표로 텔레포트
	ImGui::SetNextItemWidth(50.f);
	ImGui::InputFloat("X", &m_fX);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	ImGui::InputFloat("Y", &m_fY);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	ImGui::InputFloat("Z", &m_fZ);
	ImGui::SameLine();

	if (ImGui::Button("Teleport"))
	{
		m_pCameraTransform->Set_State(STATE::POSITION, XMVectorSet(m_fX, m_fY, m_fZ, 1));
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	ImGui::Text("Object Control");
	if (ImGui::Checkbox("Object Deplay Mode", &m_bIsDeplayMode))
	{
		// 모드가 켜지면 배치 모드(m_eCurrentObject)를 해제하고,
		// 모드가 꺼지면 드래그 상태를 초기화합니다.
		if (m_bIsDeplayMode)
		{
			m_eCurrentObject = ADD_OBJECT::END;
		}
		else
		{
			m_bIsDragging = false;
			m_pPickedObject = nullptr;
		}
	}
	ImGui::Text("Instruction: Select object on screen to drag.");

	// 오브젝트 스케일링, 회전하기
	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	ImGui::Text("Object Rotation");
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f); // 슬라이더 너비 조절
	if (ImGui::SliderFloat("##RotX_Slider", &m_fRotX, -180.f, 180.f))
	{
		Update_Rotation();
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f); // 입력창 너비 조절
	if (ImGui::InputFloat("##RotX_Input", &m_fRotX))
	{
		Update_Rotation();
	}

	// Y축 회전
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	if (ImGui::SliderFloat("##RotY_Slider", &m_fRotY, -180.f, 180.f))
	{
		Update_Rotation();
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	if (ImGui::InputFloat("##RotY_Input", &m_fRotY))
	{
		Update_Rotation();
	}

	// Z축 회전
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	if (ImGui::SliderFloat("##RotZ_Slider", &m_fRotZ, -180.f, 180.f))
	{
		Update_Rotation();
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	if (ImGui::InputFloat("##RotZ_Input", &m_fRotZ))
	{
		Update_Rotation();
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	ImGui::Text("Object Scale");
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	ImGui::InputFloat("##ScaleX", &m_fScaleX);
	ImGui::SameLine();
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	ImGui::InputFloat("##ScaleY", &m_fScaleY);
	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	ImGui::InputFloat("##ScaleZ", &m_fScaleZ);

	if (ImGui::Button("Scale_All_Layer"))
	{
		if (m_eCurrentObject == ADD_OBJECT::END) return E_FAIL;

		m_pObjects = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), m_CurrentLayerName);

		if (m_pObjects)
		{
			for (auto pObject : *m_pObjects)
			{
				CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));
				if (pTransform)
				{
					pTransform->Set_Scale(m_fScaleX, m_fScaleY, m_fScaleZ);
				}
			}
		}
	}

	if (ImGui::Button("Scale_Latest_Layer"))
	{
		if (m_pLastAddedObject)
		{
			// 유효하다면 해당 객체의 Transform 컴포넌트를 찾음
			CTransform* pTransform = dynamic_cast<CTransform*>(m_pLastAddedObject->Find_Component(TEXT("Com_Transform")));
			if (pTransform)
			{
				pTransform->Set_Scale(m_fScaleX, m_fScaleY, m_fScaleZ);
			}
		}

	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	// 에디터 세이브 / 로드
	ImGui::Text("Save  /  Load");
	if (ImGui::Button("Save"))
	{
		// 맵 오브젝트 저장
		if (FAILED(Save_Map_Objects()))
		{
			MessageBoxW(g_hWnd, L"맵 오브젝트 저장 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"맵 오브젝트 저장 성공.", L"알림", MB_OK);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_Map_Objects()))
		{
			MessageBoxW(g_hWnd, L"맵 오브젝트 로드 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"맵 오브젝트 로드 성공.", L"알림", MB_OK);
		}
	}

	ImGui::End();

	return S_OK;
}


void CMapTool::Update_Rotation()
{
	if (m_pLastAddedObject)
	{
		// 유효하다면 해당 객체의 Transform 컴포넌트를 찾음
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLastAddedObject->Find_Component(TEXT("Com_Transform")));
		if (pTransform)
		{
			// Transform 컴포넌트가 있다면 회전 적용
			pTransform->Rotation(XMConvertToRadians(m_fRotX), XMConvertToRadians(m_fRotY), XMConvertToRadians(m_fRotZ));
		}
	}
}

HRESULT CMapTool::Save_Map_Objects()
{
	// 맵 데이터 파일 열기
	std::ofstream ofs("../Bin/DataFiles/MapData.bin", std::ios::binary);
	if (!ofs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// vil_bui03_04 객체 정보 저장
	list<CGameObject*>* pVil_Bui03_04 = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Vil_Bui03_04"));
	_uint iNumMonsters = (pVil_Bui03_04) ? (_uint)pVil_Bui03_04->size() : 0;
	ofs.write(reinterpret_cast<const char*>(&iNumMonsters), sizeof(_uint));

	if (pVil_Bui03_04)
	{
		for (auto pObject : *pVil_Bui03_04)
		{
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));
			if (pTransform)
			{
				SAVEDOBJECTINFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);
				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVEDOBJECTINFO));
			}
		}
	}

	ofs.close();

	return S_OK;
}

HRESULT CMapTool::Load_Map_Objects()
{
	std::ifstream ifs("../Bin/DataFiles/MapData.bin", std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	_uint iNumVil_Bui03_04s = 0;
	ifs.read(reinterpret_cast<char*>(&iNumVil_Bui03_04s), sizeof(_uint));

	for (_uint i = 0; i < iNumVil_Bui03_04s; ++i)
	{
		SAVEDOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDOBJECTINFO));

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_GameObject_Vil_Bui03_04"),
			ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Vil_Bui03_04"));

		if (SUCCEEDED(hr))
		{
			// Add_GameObject_ToLayer 호출 직후, 전체 리스트에서 마지막에 추가된 객체 가져오기
			list<CGameObject*>* pVil_Bui03_04s = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Vil_Bui03_04"));
			if (pVil_Bui03_04s && !pVil_Bui03_04s->empty())
			{
				CGameObject* pVil_Bui03_04 = pVil_Bui03_04s->back();
				CTransform* pTransform = dynamic_cast<CTransform*>(pVil_Bui03_04->Find_Component(TEXT("Com_Transform")));
				if (pTransform)
				{
					_matrix matWorld = XMLoadFloat4x4(&info.worldMatrix);

					_vector vScale = {};
					_vector vRotation = {};
					_vector vPosition = {};
					XMMatrixDecompose(&vScale, &vRotation, &vPosition, matWorld);

					_float fX, fY, fZ;
					fX = XMVectorGetX(vScale);
					fY = XMVectorGetY(vScale);
					fZ = XMVectorGetZ(vScale);

					pTransform->Set_Scale(fX, fY, fZ);

					pTransform->Set_State(STATE::POSITION, vPosition);

					_matrix matRotation = XMMatrixRotationQuaternion(vRotation);

					pTransform->Set_State(STATE::RIGHT, matRotation.r[0]);
					pTransform->Set_State(STATE::UP, matRotation.r[1]);
					pTransform->Set_State(STATE::LOOK, matRotation.r[2]);

				}
			}
		}
	}

	ifs.close();

	return S_OK;
}

void CMapTool::Set_NaviEditMode(_bool bMode)
{
	m_bIsNaviEditMode = bMode;

	if (m_bIsNaviEditMode)
	{
		// 터레인에서 CNavigation 컴포넌트를 가져옵니다.
		if (m_pTerrain)
		{
			// CNavigation* m_pNavigationCom를 가져오는 함수가 CTerrain에 없으므로, 
			// 임시로 Get_Component를 사용한다고 가정합니다. 실제 엔진 환경에 맞게 수정해야 합니다.
			m_pNavigation = m_pTerrain->Get_NavigationComponent();
		}
		else
		{
			MSG_BOX("Failed to Get CNavigation from Terrain");
			m_bIsNaviEditMode = false;
		}
	}
	else
	{
		// 모드가 꺼질 때 리셋 및 해제
		Reset_NaviPoints();
		m_eNaviMode = NAVI_MODE::NONE;
	}
}

void CMapTool::Add_NaviPoint(_fvector vPickedPoint)
{
	if (m_iNaviPointCount < 3)
	{
		_vector vFinalPoint = vPickedPoint;

		// 1. 반경 m_fNaviSnapRadius 내에 기존 정점이 있는지 검색
		_vector vClosestPoint = XMVectorZero();
		if (m_pNavigation->Find_Closest_Point(vPickedPoint, m_fNaviSnapRadius, &vClosestPoint))
		{
			vFinalPoint = vClosestPoint;

		}

		m_vNaviPoints[m_iNaviPointCount] = vFinalPoint;
		m_iNaviPointCount++;

		_float3 vP;
		XMStoreFloat3(&vP, vFinalPoint); // vFinalPoint를 사용하도록 수정
		wchar_t szBuffer[256];
		wsprintfW(szBuffer, L"Navi Point %d added: (%.2f, %.2f, %.2f)\n", m_iNaviPointCount, vP.x, vP.y, vP.z);
		OutputDebugStringW(szBuffer);
	}
}

void CMapTool::Reset_NaviPoints()
{
	m_iNaviPointCount = 0;
	m_eNaviMode = NAVI_MODE::ADD_POINT;
}

HRESULT CMapTool::Add_NaviCell()
{
	if (m_iNaviPointCount != 3 || !m_pNavigation)
		return E_FAIL;

	_float3 vPoints[3];
	XMStoreFloat3(&vPoints[0], m_vNaviPoints[0]);
	XMStoreFloat3(&vPoints[1], m_vNaviPoints[1]);
	XMStoreFloat3(&vPoints[2], m_vNaviPoints[2]);

	HRESULT hr = m_pNavigation->Add_Cell_From_Editor(vPoints);

	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 셀 추가가 성공했으면 이웃 정보를 새로고침합니다.
	m_pNavigation->SetUp_Neighbors();


	Reset_NaviPoints(); // 셀 생성 후 포인트 리셋
	m_eNaviMode = NAVI_MODE::ADD_POINT;

	return S_OK;
}

HRESULT CMapTool::Save_NavigationData()
{
	if (!m_pNavigation)
		return E_FAIL;

	const vector<class Engine::CCell*>* pCells = m_pNavigation->Get_Cells();
	if (pCells->empty())
	{
		OutputDebugStringW(L"No Navigation Cells to save.\n");
		return S_OK;
	}

	std::ofstream ofs("../Bin/DataFiles/Navigation.bin", std::ios::binary);
	if (!ofs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open Navigation", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	_ulong dwByte = {};
	_float3 vPoints[3] = {};

	for (auto& pCell : *pCells)
	{
		XMStoreFloat3(&vPoints[0], pCell->Get_Point(NAVI_POINT::A));
		XMStoreFloat3(&vPoints[1], pCell->Get_Point(NAVI_POINT::B));
		XMStoreFloat3(&vPoints[2], pCell->Get_Point(NAVI_POINT::C));
		ofs.write(reinterpret_cast<const char*>(vPoints), sizeof(_float3) * 3);
	}

	ofs.close();

	OutputDebugStringW(L"Navigation Data Saved to Navigation.bin\n");

	return S_OK;
}

void CMapTool::Delete_Latest_NaviCell()
{
	if (m_pNavigation)
	{
		m_pNavigation->Delete_Line();
		m_pNavigation->SetUp_Neighbors(); 
		OutputDebugStringW(L"Latest Navigation Cell Deleted.\n");
	}
}

CGameObject* CMapTool::Find_Object_To_Pick(_uint iLevelIndex, const _wstring& strLayerTag, _float3* pPickedPoint)
{
	list<CGameObject*>* pObjects = m_pGameInstance->GetAllObejctToLayer(iLevelIndex, strLayerTag.c_str());
	if (pObjects == nullptr || pObjects->empty())
		return nullptr;

	CGameObject* pClosestObject = nullptr;
	_float fMinDistance = FLT_MAX;

	_float3 vRayOrigin_f, vRayDir_f;
	Compute_Picking_Ray(&vRayOrigin_f, &vRayDir_f);

	XMVECTOR vRayOrigin = XMLoadFloat3(&vRayOrigin_f);
	XMVECTOR vRayDir = XMLoadFloat3(&vRayDir_f);


	for (auto pObject : *pObjects)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));
		_float fRadius = 5.f;

		if (pTransform == nullptr) continue; 

		XMVECTOR vSphereCenter = pTransform->Get_State(STATE::POSITION);

		_float fDistance = 0.f;

		if (Intersect_Ray_Sphere(vRayOrigin, vRayDir, vSphereCenter, fRadius, &fDistance))
		{
			if (fDistance < fMinDistance)
			{
				fMinDistance = fDistance;
				pClosestObject = pObject;
			}
		}
	}

	if (pClosestObject != nullptr && pPickedPoint != nullptr)
	{
		_float3 vTempPoint = {};
		m_pGameInstance->isPicking(&vTempPoint);
		*pPickedPoint = vTempPoint;

		return pClosestObject;
	}

	return pClosestObject;
}

void CMapTool::Compute_Picking_Ray(_float3* pRayOrigin, _float3* pRayDir)
{
	_float fWinWidth = (_float)g_iWinSizeX;
	_float fWinHeight = (_float)g_iWinSizeY;

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_float fX = (2.0f * ptMouse.x / fWinWidth) - 1.0f;
	_float fY = 1.0f - (2.0f * ptMouse.y / fWinHeight);
	_matrix ViewMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
	_matrix ProjMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));

	_matrix matInv = XMMatrixInverse(nullptr, ViewMatrix * ProjMatrix);

	XMVECTOR vNear = XMVectorSet(fX, fY, 0.0f, 1.0f);
	XMVECTOR vFar = XMVectorSet(fX, fY, 1.0f, 1.0f);

	vNear = XMVector3TransformCoord(vNear, matInv);
	vFar = XMVector3TransformCoord(vFar, matInv);

	XMStoreFloat3(pRayOrigin, vNear);

	XMVECTOR vRayDir = XMVector3Normalize(vFar - vNear);
	XMStoreFloat3(pRayDir, vRayDir);
}

_bool CMapTool::Intersect_Ray_Sphere(_fvector vRayOrigin, _fvector vRayDir, _fvector vSphereCenter, _float fRadius, _float* pDistance)
{
	// C = Sphere Center, R = Radius
	// L = C - O (Sphere Center에서 Ray Origin으로 향하는 벡터)
	XMVECTOR vL = vSphereCenter - vRayOrigin;

	// t_ca = Ray 방향 D와 L의 내적 (Ray 방향으로의 L의 투영 길이)
	// t_ca가 음수이면 Ray가 Sphere의 반대 방향을 향하고 있음 (이미 지나침)
	_float t_ca = XMVectorGetX(XMVector3Dot(vL, vRayDir));
	if (t_ca < 0.0f)
		return false;

	// d^2 = L의 길이 제곱 - t_ca의 제곱 (Ray와 Sphere 중심 C 사이의 최단 거리 제곱)
	XMVECTOR vL_Sq = XMVector3LengthSq(vL);
	_float d_sq = XMVectorGetX(vL_Sq) - (t_ca * t_ca);

	_float fRadiusSq = fRadius * fRadius;

	// d^2 > R^2 이면 충돌하지 않음
	if (d_sq > fRadiusSq)
		return false;

	// t_hc = Ray가 Sphere 내부를 지나가는 반 현(Half-Chord)의 길이
	// t_hc = sqrt(R^2 - d^2)
	_float t_hc = sqrtf(fRadiusSq - d_sq);

	// t는 Ray Origin에서 충돌 지점까지의 거리
	// t = t_ca - t_hc (Ray가 Sphere에 처음 진입하는 지점)
	_float t = t_ca - t_hc;


	*pDistance = t;


	return true;
}



void CMapTool::Free()
{
	__super::Free();
}
