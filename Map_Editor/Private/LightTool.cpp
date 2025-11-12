#include "pch.h"
#include "LightTool.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Cell.h"
#include "Terrain.h"
#include "Light.h"
#include <fstream>

CLightTool::CLightTool()
{

}


HRESULT CLightTool::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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

void CLightTool::Priority_Update(_float fTimeDelta)
{
}

void CLightTool::Update(_float fTimeDelta)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	if (m_eToolMode == TOOL_MODE::LIGHT)
	{
		if (m_bIsDeplayPointLight)
		{

			// 마우스 좌클릭 이벤트 체크
			if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
			{

				_float3 vPickedPoint = {};
				if (true == m_pGameInstance->isPicking(&vPickedPoint))
				{
					_float4 vPickPoint = XMFLOAT4(vPickedPoint.x, vPickedPoint.y, vPickedPoint.z, 1.f);
					// 1. 네비게이션 포인트 추가

					if (m_bIsDeplayPointLight)
					{
						LIGHT_DESC			LightDesc{};

						LightDesc.eType = LIGHT_TYPE::POINT;
						LightDesc.vDiffuse = _float4(0.f, 1.0f, 0.f, 1.f);
						LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 0.1f);
						LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
						LightDesc.vPosition = vPickPoint;
						LightDesc.fRange = 10.f;

						m_pGameInstance->Add_Light(LightDesc);
					}
					else if (m_bIsDeplayDirLight)
					{
						/*LIGHT_DESC			LightDesc{};

						LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
						LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
						LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
						LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 0.2f);
						LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

						m_pGameInstance->Add_Light(LightDesc);*/
					}
				}

			}
			return; // 네비게이션 모드일 때는 다른 오브젝트 추가 로직은 건너뜁니다.
		}
	}
}

void CLightTool::Late_Update(_float fTimeDelta)
{
}

HRESULT CLightTool::Render()
{
	ImGui::SetNextWindowSize(ImVec2(300.f, 600.f), ImGuiCond_Once);

	ImGui::Begin("Light Editor");

	ImGui::Text("LightTool");
	if (ImGui::Checkbox("Light Tool Mode", &m_bIsLightMode))
	{
		if (m_bIsLightMode)
		{
			m_eToolMode = TOOL_MODE::LIGHT;
			m_bIsMapMode = false;
		}
		else
		{
			m_eToolMode = TOOL_MODE::END;
		}
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	ImGui::Text("Light_Placement");
	if (ImGui::Button("Point_Light"))
	{
		m_bIsDeplayPointLight = true;
		m_bIsDeplayDirLight = false;
	}

	if (ImGui::Button("Directional_Light"))
	{
		m_bIsDeplayDirLight = true;
		m_bIsDeplayPointLight = false;
		LIGHT_DESC			LightDesc{};

		LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
		LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 0.2f);
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

		m_pGameInstance->Add_Light(LightDesc);
	}

	if (ImGui::Button("Cancel"))
	{
		m_bIsDeplayPointLight = false;
		m_bIsDeplayDirLight = false;
	}

	if (ImGui::Button("Delete"))
	{
		m_pLights = m_pGameInstance->GetAllLight();

		if (m_pLights && !m_pLights->empty())
		{
			for (auto pLight : *m_pLights)
			{
				pLight->SetDead(true);
			}
		}

	}

	ImGui::Spacing();
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	_int nSelectLight = -1;

	m_pLights = (list<class CLight*>*)m_pGameInstance->GetAllLight();

	ImGui::Text("Light Control");
	const char* strLightButtonLabel = "Show Light List";

	// 1. 버튼 생성 및 팝업 열기 요청
	if (ImGui::Button(strLightButtonLabel))
	{
		// 버튼 클릭 시 최신 조명 리스트를 다시 가져옵니다.
		m_pLights = (list<class CLight*>*)m_pGameInstance->GetAllLight();

		m_LightNames.clear();

		if (m_pLights && !m_pLights->empty())
		{
			size_t i = 0;
			for (auto pLight : *m_pLights)
			{
				// 이름 버퍼를 충분히 확보 (ImGui::Selectable은 const char*이 필요)
				static char nameBuffer[128][64] = {};
				if (i < 128)
				{
					sprintf_s(nameBuffer[i], 64, "Light %zd (Type: %s)", i,
						(pLight->Get_LightDesc()->eType == LIGHT_TYPE::DIRECTIONAL ? "Dir" : "Point"));

					m_LightNames.push_back(nameBuffer[i]);
					i++;
				}
			}
		}

		ImGui::OpenPopup(strLightButtonLabel);
	}

	// 2. 팝업 창 시작
	if (ImGui::BeginPopup(strLightButtonLabel))
	{
		ImGui::Text("--- Current Level Light List ---");
		ImGui::Separator();

		if (m_pLights && !m_pLights->empty())
		{
			int idx = 0;
			// m_LightNames와 m_pLights를 동시에 순회하며 인덱스를 비교해야 합니다.
			for (class CLight* pLight : *m_pLights) // CLight* 타입 명시
			{
				const char* lightName = "";
				if (idx < m_LightNames.size()) {
					static char tempName[64];
					sprintf_s(tempName, 64, "Light %d (Type: %s)", idx,
						(pLight->Get_LightDesc()->eType == LIGHT_TYPE::DIRECTIONAL ? "Dir" : "Point"));
					lightName = tempName;
				}


				if (ImGui::Selectable(lightName, m_pSelectedLight == pLight)) // 현재 선택된 조명 표시
				{
					// 조명이 선택되면 포인터와 인덱스를 저장
					m_pSelectedLight = pLight;
					m_iSelectedLightIndex = idx;

					// UI 변수에 조명 속성 로드 (디퓨즈, 스페큘러 등)
					Load_Selected_Light_Desc();

					ImGui::CloseCurrentPopup();
				}
				idx++;
			}
		}
		else
		{
			ImGui::Text("No light list.");
		}

		// 4. 팝업 창 종료
		ImGui::EndPopup();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (m_pSelectedLight)
	{
		ImGui::Text("** Editing Light %d **", m_iSelectedLightIndex);

		// Diffuse (기존 코드를 사용하여 ImGui::InputFloat에 조건부 연결)
		ImGui::Text("Light Diffuse");
		ImGui::PushItemWidth(50.0f);
		if (ImGui::InputFloat("##DiffuseX", &m_fDiffuseX)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##DiffuseY", &m_fDiffuseY)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##DiffuseZ", &m_fDiffuseZ)) Update_Light_Properties();
		ImGui::PopItemWidth();

		// Ambient
		ImGui::Text("Light Ambient");
		ImGui::PushItemWidth(50.0f);
		if (ImGui::InputFloat("##AmbientX", &m_fAmbientX)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##AmbientY", &m_fAmbientY)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##AmbientZ", &m_fAmbientZ)) Update_Light_Properties();
		ImGui::PopItemWidth();

		// Specular
		ImGui::Text("Light Specular");
		ImGui::PushItemWidth(50.0f);
		if (ImGui::InputFloat("##SpecularX", &m_fSpecularX)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##SpecularY", &m_fSpecularY)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##SpecularZ", &m_fSpecularZ)) Update_Light_Properties();
		ImGui::PopItemWidth();

		// Directional Light 전용
		if (m_pSelectedLight->Get_LightDesc()->eType == LIGHT_TYPE::DIRECTIONAL)
		{
			ImGui::Spacing();
			ImGui::Text("Light Direction");
			ImGui::PushItemWidth(50.0f);
			if (ImGui::InputFloat("##DirectionX", &m_fDirectionX)) Update_Light_Properties();
			ImGui::SameLine();
			if (ImGui::InputFloat("##DirectionY", &m_fDirectionY)) Update_Light_Properties();
			ImGui::SameLine();
			if (ImGui::InputFloat("##DirectionZ", &m_fDirectionZ)) Update_Light_Properties();
			ImGui::PopItemWidth();
		}

		// Point Light 전용
		if (m_pSelectedLight->Get_LightDesc()->eType == LIGHT_TYPE::POINT)
		{
			ImGui::Spacing();
			ImGui::Text("Light Range");
			ImGui::PushItemWidth(50.0f);
			if (ImGui::InputFloat("##Range", &m_fRange)) Update_Light_Properties();
			ImGui::PopItemWidth();
		}
	}
	else
	{
		ImGui::Text("Select a light from the list to enable editing.");
	}

	ImGui::Spacing();
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	// 빛 세이브 / 로드
	ImGui::Text("Save  /  Load");
	if (ImGui::Button("Save"))
	{
		// 빛 오브젝트 저장
		if (FAILED(Save_Light_Objects()))
		{
			MessageBoxW(g_hWnd, L"빛 오브젝트 저장 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"빛 오브젝트 저장 성공.", L"알림", MB_OK);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_Light_Objects()))
		{
			MessageBoxW(g_hWnd, L"빛 오브젝트 로드 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"빛 오브젝트 로드 성공.", L"알림", MB_OK);
		}
	}

	ImGui::End();

	return S_OK;
}

HRESULT CLightTool::Save_Light_Objects()
{
	if (m_pLights == nullptr || m_pLights->empty())
	{
		OutputDebugStringW(L"No Lights to save.\n");
		return S_OK;
	}

	// 라이트 데이터
	std::ofstream ofs("C:/coding/jusin/LightData.bin", std::ios::binary);
	if (!ofs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open LightData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	_uint iNumLights = (_uint)m_pLights->size();
	ofs.write(reinterpret_cast<const _char*>(&iNumLights), sizeof(_uint));

	for (auto pLight : *m_pLights)
	{
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc();
		ofs.write(reinterpret_cast<const _char*>(pDesc), sizeof(LIGHT_DESC));
	}

	ofs.close();

	return S_OK;
}

HRESULT CLightTool::Load_Light_Objects()
{
	std::ifstream ifs("../Bin/DataFiles/LightData.bin", std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open LightData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	m_pLights = m_pGameInstance->GetAllLight();

	if (m_pLights && !m_pLights->empty())
	{
		for (auto pLight : *m_pLights)
		{
			pLight->SetDead(true);
		}
	}

	_uint iNumLights = 0;
	ifs.read(reinterpret_cast<_char*>(&iNumLights), sizeof(_uint));
	
	for (_uint i = 0; i < iNumLights; ++i)
	{
		LIGHT_DESC LightDesc;
		ifs.read(reinterpret_cast<_char*>(&LightDesc), sizeof(LIGHT_DESC));

		m_pGameInstance->Add_Light(LightDesc);
	}

	ifs.close();

	return S_OK;
}


void CLightTool::Update_Light_Properties()
{
	if (m_pSelectedLight == nullptr) return;

	LIGHT_DESC newDesc = *m_pSelectedLight->Get_LightDesc();

	newDesc.vDiffuse = _float4(m_fDiffuseX, m_fDiffuseY, m_fDiffuseZ, 1.f);
	newDesc.vAmbient = _float4(m_fAmbientX, m_fAmbientY, m_fAmbientZ, 1.f);
	newDesc.vSpecular = _float4(m_fSpecularX, m_fSpecularY, m_fSpecularZ, 1.f);

	if (newDesc.eType == LIGHT_TYPE::DIRECTIONAL)
	{
		newDesc.vDirection = _float4(m_fDirectionX, m_fDirectionY, m_fDirectionZ, 0.f);
	}

	if (newDesc.eType == LIGHT_TYPE::POINT)
	{
		newDesc.fRange = m_fRange;
	}

	m_pSelectedLight->SetLightInfo(newDesc);
}

void CLightTool::Load_Selected_Light_Desc()
{
	if (m_pSelectedLight == nullptr) return;

	const LIGHT_DESC& desc = *m_pSelectedLight->Get_LightDesc();

	m_fDiffuseX = desc.vDiffuse.x; m_fDiffuseY = desc.vDiffuse.y; m_fDiffuseZ = desc.vDiffuse.z;
	m_fAmbientX = desc.vAmbient.x; m_fAmbientY = desc.vAmbient.y; m_fAmbientZ = desc.vAmbient.z;
	m_fSpecularX = desc.vSpecular.x; m_fSpecularY = desc.vSpecular.y; m_fSpecularZ = desc.vSpecular.z;
	m_fDirectionX = desc.vDirection.x; m_fDirectionY = desc.vDirection.y; m_fDirectionZ = desc.vDirection.z;
	m_fRange = desc.fRange;
}


void CLightTool::Free()
{
	__super::Free();

}
