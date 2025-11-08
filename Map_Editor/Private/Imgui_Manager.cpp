#include "pch.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Cell.h"
#include "Village.h"
#include <fstream>

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager() 
{

}


HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	m_pVillage = dynamic_cast<CVillage*>(m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Village_Mou"))->front());

	return S_OK;
}

void CImgui_Manager::Priority_Update(_float fTimeDelta)
{
}

void CImgui_Manager::Update(_float fTimeDelta)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

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

	if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
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

}

void CImgui_Manager::Late_Update(_float fTimeDelta)
{
}

HRESULT CImgui_Manager::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(300.f, 0.f), ImGuiCond_Once);


    ImGui::Begin("Map Editor");

    ImGui::Spacing(); // 메뉴 사이의 간격
    ImGui::Separator(); // 구분선을 추가
    ImGui::Spacing();


    // =========================================================================
    // 4. 네비게이션 편집 메뉴
    // =========================================================================
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

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
    // 2. 모델 배치 / 삭제메뉴
    _int nSelectedModel = -1;
    _int nSelectedBuilding = -1;
    _int nSelectedEnvironment = -1;

    const _char* modelNames[] = { "Player" };
    const _char* buildingNames[] = { "BlackSmithShop", "Forge", "Gate", "TowerGate", "LUMBERYARD_WHEEL", "QUARRY1", "RAMP", "FENCE1", "WALL1", "WALLSTRAIGHT1" };
    const _char* environmentNames[] = { "BirchTree1", "RowanTree3", "YewTree1", "Stick", "Iron", "Rock", "Flint", "WoormWood", "MergedTree" };

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

    // 삭제 버튼
   


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

	// 에디터 세이브 / 로드
	ImGui::Text("Save  /  Load");
	if (ImGui::Button("Save"))
	{
		// 지형 높이맵 저장
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
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	

	return S_OK;
}

void CImgui_Manager::Update_Rotation()
{
}

HRESULT CImgui_Manager::Save_Map_Objects()
{
	// 맵 데이터 파일 열기
	std::ofstream ofs("C:/coding/jusin/MapData.bin", std::ios::binary);
	if (!ofs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// village_mou 객체 정보 저장
	list<CGameObject*>* pPlayer = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Player"));
	_uint iNumMonsters = (pPlayer) ? (_uint)pPlayer->size() : 0;
	ofs.write(reinterpret_cast<const char*>(&iNumMonsters), sizeof(_uint));

	if (pPlayer)
	{
		for (auto pObject : *pPlayer)
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

HRESULT CImgui_Manager::Load_Map_Objects()
{
	std::ifstream ifs("../Bin/DataFiles/MapData.bin", std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	_uint iNumPlayers = 0;
	ifs.read(reinterpret_cast<char*>(&iNumPlayers), sizeof(_uint));

	for (_uint i = 0; i < iNumPlayers; ++i)
	{
		SAVEDOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDOBJECTINFO));

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VILLAGE),TEXT("Prototype_GameObject_Player"),
			ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Player"));

		if (SUCCEEDED(hr))
		{
			// Add_GameObject_ToLayer 호출 직후, 전체 리스트에서 마지막에 추가된 객체 가져오기
			list<CGameObject*>* pPlayers = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Layer_Player"));
			if (pPlayers && !pPlayers->empty())
			{
				CGameObject* pPlayer = pPlayers->back();
				CTransform* pTransform = dynamic_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
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

					XMMATRIX matRotation = XMMatrixRotationQuaternion(vRotation);

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

void CImgui_Manager::Set_NaviEditMode(_bool bMode)
{
	m_bIsNaviEditMode = bMode;

	if (m_bIsNaviEditMode)
	{
		if (m_pVillage)
		{
			m_pNavigation = m_pVillage->Get_Navigation();
			m_eNaviMode = NAVI_MODE::ADD_POINT;
		}
		else
		{
			m_bIsNaviEditMode = false; // 실패 시 다시 끔
		}
	}
	else
	{
		// 모드가 꺼질 때 리셋 및 해제
		Reset_NaviPoints();
		m_eNaviMode = NAVI_MODE::NONE;
	}
}

void CImgui_Manager::Add_NaviPoint(_fvector vPickedPoint)
{
	if (m_iNaviPointCount < 3)
	{
		_vector vVillageWorldPos = XMVectorZero();

		CTransform* pVillageTransform = nullptr;

		if (m_pVillage)
		{
			// m_pVillage에서 Transform 컴포넌트를 찾습니다.
			pVillageTransform = dynamic_cast<CTransform*>(m_pVillage->Find_Component(TEXT("Com_Transform")));
			if (pVillageTransform)
			{
				vVillageWorldPos = pVillageTransform->Get_State(STATE::POSITION);
			}
		}

		// 픽킹된 world 좌표를 village의 local좌표로 변환
		_vector vLocalPickedPoint = XMVectorSubtract(vPickedPoint, vVillageWorldPos);
		_vector vFinalPoint = vLocalPickedPoint;

		// 1. 반경 m_fNaviSnapRadius 내에 기존 정점이 있는지 검색
		_vector vClosestPoint = XMVectorZero();
		if (m_pNavigation->Find_Closest_Point(vLocalPickedPoint, m_fNaviSnapRadius, &vClosestPoint))
		{
			// 2. 기존 정점이 있다면, 그 정점의 좌표를 사용합니다. (정점 스냅핑)
			vFinalPoint = vClosestPoint;

		}
		else
		{
			vFinalPoint = vLocalPickedPoint;
		}

		m_vNaviPoints[m_iNaviPointCount] = vFinalPoint;
		m_iNaviPointCount++;

		_float3 vP;
		XMStoreFloat3(&vP, vFinalPoint); // vFinalPoint를 사용하도록 수정
		wchar_t szBuffer[256];
		swprintf_s(szBuffer, 256, L"Navi Point %d added: (%.2f, %.2f, %.2f)\n", m_iNaviPointCount, vP.x, vP.y, vP.z);

		OutputDebugStringW(szBuffer);
	}
}

void CImgui_Manager::Reset_NaviPoints()
{
	m_iNaviPointCount = 0;
	m_eNaviMode = NAVI_MODE::ADD_POINT;
}

HRESULT CImgui_Manager::Add_NaviCell()
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

HRESULT CImgui_Manager::Save_NavigationData()
{
	if (!m_pNavigation)
		return E_FAIL;

	const vector<class Engine::CCell*>* pCells = m_pNavigation->Get_Cells();
	if (pCells->empty())
	{
		OutputDebugStringW(L"No Navigation Cells to save.\n");
		return S_OK;
	}

	std::ofstream ofs("C:/coding/jusin/Navigation.bin", std::ios::binary);
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

void CImgui_Manager::Delete_Latest_NaviCell()
{
	if (m_pNavigation)
	{
		m_pNavigation->Delete_Line(); // CNavigation에 구현된 Delete_Line() 사용
		m_pNavigation->SetUp_Neighbors(); // 이웃 재설정
		OutputDebugStringW(L"Latest Navigation Cell Deleted.\n");
	}
}

_bool CImgui_Manager::Picking(_float3 vPickPos)
{
	m_pGameInstance->isPicking(&vPickPos);

	return false;
}

void CImgui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

}
