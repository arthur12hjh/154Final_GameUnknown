#include "pch.h"
#include "LightTool.h"
#include "GameInstance.h"
#include "Light.h"
#include <fstream>

CLightTool::CLightTool()
{

}


HRESULT CLightTool::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();

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
				else if (m_bIsDeplaySpotLight)
				{
					LIGHT_DESC			LightDesc{};

					LightDesc.eType = LIGHT_TYPE::SPOT;
					LightDesc.vDiffuse = _float4(1.f, 1.0f, 1.f, 1.f);
					LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 0.1f);
					LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
					LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
					LightDesc.vPosition = vPickPoint;
					LightDesc.fFalloff = 1.f;
					LightDesc.fTheta = XMConvertToRadians(35.f);
					LightDesc.fPhi = XMConvertToRadians(45.f);
					LightDesc.fRange = 10.f;

					m_pGameInstance->Add_Light(LightDesc);
				}
				else if (m_bIsDeplayDirLight)
				{
						
				}
			}

		}


		if (m_pSelectedLight != nullptr)
		{
			LIGHT_DESC newLightDesc = *m_pSelectedLight->Get_LightDesc();
			_float4& vPos = newLightDesc.vPosition;

			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_1))
			{
				vPos.x += m_fMoveSpeed * fTimeDelta * 60.f;
			}
			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_2))
			{
				vPos.x -= m_fMoveSpeed * fTimeDelta * 60.f;
			}
			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_3))
			{
				vPos.z += m_fMoveSpeed * fTimeDelta * 60.f;
			}
			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_4))
			{
				vPos.z -= m_fMoveSpeed * fTimeDelta * 60.f;
			}
			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_5))
			{
				vPos.y += m_fMoveSpeed * fTimeDelta * 60.f;
			}
			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_6))
			{
				vPos.y -= m_fMoveSpeed * fTimeDelta * 60.f;
			}

			m_pSelectedLight->SetLightInfo(newLightDesc);
		}


		return; // 네비게이션 모드일 때는 다른 오브젝트 추가 로직은 건너뜁니다.
	}
}

void CLightTool::Late_Update(_float fTimeDelta)
{
}

HRESULT CLightTool::Render()
{
	ImGui::SetNextWindowSize(ImVec2(300.f, 600.f), ImGuiCond_Once);

	ImGui::Begin("Light Editor");
	if(m_pSelectedLight)
		m_pGameInstance->Select_LightRender(m_pSelectedLight);

	ImGui::Text("LightTool");
	if (ImGui::Checkbox("Light Tool Mode", &m_bIsLightMode))
	{
		if (m_bIsLightMode)
		{`
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
		m_bIsDeplaySpotLight = false;
	}

	if (ImGui::Button("Directional_Light"))
	{
		m_bIsDeplayDirLight = true;
		m_bIsDeplayPointLight = false;
		m_bIsDeplaySpotLight = false;
		LIGHT_DESC			LightDesc{};

		LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
		LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 0.2f);
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

		m_pGameInstance->Add_Light(LightDesc);
	}

	if (ImGui::Button("Spot_Light"))
	{
		m_bIsDeplaySpotLight = true;
		m_bIsDeplayPointLight = false;
		m_bIsDeplayDirLight = false;
	}

	if (ImGui::Button("Cancel"))
	{
		m_bIsDeplayPointLight = false;
		m_bIsDeplayDirLight = false;
		m_bIsDeplaySpotLight = false;
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

	if (ImGui::Button("Delete_Latest"))
	{
		m_pLights = m_pGameInstance->GetAllLight();

		if (m_pLights && !m_pLights->empty())
		{
			CLight* pLight = m_pLights->back();
			pLight->SetDead(true);
		}

	}

	if (ImGui::Button("Delete_Select"))
	{
		if (m_pSelectedLight != nullptr)
		{
			m_pSelectedLight->SetDead(true);
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
					/*sprintf_s(nameBuffer[i], 64, "Light %zd (Type: %s)", i,
						(pLight->Get_LightDesc()->eType == LIGHT_TYPE::DIRECTIONAL ? "Dir" : "Point"));*/

					if (pLight->Get_LightDesc()->eType == LIGHT_TYPE::DIRECTIONAL)
						sprintf_s(nameBuffer[i], 64, "Light %zd (Type: %s)", i, "Dir");
					else if (pLight->Get_LightDesc()->eType == LIGHT_TYPE::POINT)
						sprintf_s(nameBuffer[i], 64, "Light %zd (Type: %s)", i, "Point");
					else if (pLight->Get_LightDesc()->eType == LIGHT_TYPE::SPOT)
						sprintf_s(nameBuffer[i], 64, "Light %zd (Type: %s)", i, "Spot");

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
					/*sprintf_s(tempName, 64, "Light %d (Type: %s)", idx,
						(pLight->Get_LightDesc()->eType == LIGHT_TYPE::DIRECTIONAL ? "Dir" : "Point"));*/

					if (pLight->Get_LightDesc()->eType == LIGHT_TYPE::DIRECTIONAL)
					{
						sprintf_s(tempName, 64, "Light %d (Type: %s)", idx, "Dir");
					}
					else if (pLight->Get_LightDesc()->eType == LIGHT_TYPE::POINT)
					{
						sprintf_s(tempName, 64, "Light %d (Type: %s)", idx, "Point");
					}
					else if (pLight->Get_LightDesc()->eType == LIGHT_TYPE::SPOT)
					{
						sprintf_s(tempName, 64, "Light %d (Type: %s)", idx, "Spot");
					}

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
		ImGui::PushItemWidth(100.0f);
		if (ImGui::InputFloat("##DiffuseX", &m_fDiffuseX, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##DiffuseY", &m_fDiffuseY, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##DiffuseZ", &m_fDiffuseZ, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::PopItemWidth();
		// Ambient
		ImGui::Text("Light Ambient");
		ImGui::PushItemWidth(100.0f);
		if (ImGui::InputFloat("##AmbientX", &m_fAmbientX, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##AmbientY", &m_fAmbientY, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##AmbientZ", &m_fAmbientZ, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::PopItemWidth();
		// Specular
		ImGui::Text("Light Specular");
		ImGui::PushItemWidth(100.0f);
		if (ImGui::InputFloat("##SpecularX", &m_fSpecularX, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##SpecularY", &m_fSpecularY, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::SameLine();
		if (ImGui::InputFloat("##SpecularZ", &m_fSpecularZ, 0.01f, 10.f)) Update_Light_Properties();
		ImGui::PopItemWidth();
		// Directional Light 전용
		if (m_pSelectedLight->Get_LightDesc()->eType == LIGHT_TYPE::DIRECTIONAL)
		{
			ImGui::Spacing();
			ImGui::Text("Light Direction");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##DirectionX", &m_fDirectionX, -1000.f, 1000.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##DirectionX_Input", &m_fDirectionX)) Update_Light_Properties();

			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##DirectionY", &m_fDirectionY, -1000.f, 1000.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##DirectionY_Input", &m_fDirectionY)) Update_Light_Properties();

			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##DirectionZ", &m_fDirectionZ, -1000.f, 1000.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##DirectionZ_Input", &m_fDirectionZ)) Update_Light_Properties();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

		}
		// Point Light 전용
		else if (m_pSelectedLight->Get_LightDesc()->eType == LIGHT_TYPE::POINT)
		{
			ImGui::Spacing();
			ImGui::Text("Light Range");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##Range", &m_fRange, 0.f, 500.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##Range_Input", &m_fRange)) Update_Light_Properties();
		}
		else if (m_pSelectedLight->Get_LightDesc()->eType == LIGHT_TYPE::SPOT)
		{
			ImGui::Spacing();
			ImGui::Text("Light Direction");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##DirectionX", &m_fDirectionX, -5000.f, 5000.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##DirectionX_Input", &m_fDirectionX)) Update_Light_Properties();

			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##DirectionY", &m_fDirectionY, -5000.f, 5000.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##DirectionY_Input", &m_fDirectionY)) Update_Light_Properties();

			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##DirectionZ", &m_fDirectionZ, -5000.f, 5000.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##DirectionZ_Input", &m_fDirectionZ)) Update_Light_Properties();

			ImGui::Spacing();
			ImGui::Text("Light Range");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##Range", &m_fRange, 0.f, 1000.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##Range_Input", &m_fRange)) Update_Light_Properties();

			ImGui::Spacing();
			ImGui::Text("Light Falloff");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##Falloff", &m_fFalloff, 0.f, 50.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##Falloff_Input", &m_fFalloff)) Update_Light_Properties();

			ImGui::Spacing();
			ImGui::Text("Light Theta");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##Theta", &m_fTheta, 0.f, 360.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##Theta_Input", &m_fTheta)) Update_Light_Properties();

			ImGui::Spacing();
			ImGui::Text("Light Phi");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::SliderFloat("##Phi", &m_fPhi, 0.f, 360.f)) Update_Light_Properties();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat("##Phi_Input", &m_fPhi)) Update_Light_Properties();
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

	//if (m_pLights && !m_pLights->empty())
	//{
	//	for (class CLight* pLight : *m_pLights) // CLight* 타입 명시
	//	{
	//		if (pLight->Get_LightDesc()->eType == LIGHT_TYPE::POINT)
	//		{
	//		}
	//	}
	//}

	//// 라이트 데이터
	//std::ofstream ofs("../Bin/DataFiles/LightData.bin", std::ios::binary);
	//if (!ofs.is_open())
	//{
	//	MessageBoxW(g_hWnd, L"Failed to open LightData", L"Error", MB_OK | MB_ICONERROR);
	//	return E_FAIL;
	//}

	//_uint iNumLights = (_uint)m_pLights->size();
	//ofs.write(reinterpret_cast<const _char*>(&iNumLights), sizeof(_uint));

	//for (auto pLight : *m_pLights)
	//{
	//	const LIGHT_DESC* pDesc = pLight->Get_LightDesc();
	//	ofs.write(reinterpret_cast<const _char*>(pDesc), sizeof(LIGHT_DESC));
	//}

	//ofs.close();

	//return S_OK;
	// 
	// 
	// 1. POINT 라이트만 따로 수집
	vector<CLight*> PointLights;
	for (auto& pLight : *m_pLights)
	{
		if (pLight->Get_LightDesc()->eType == LIGHT_TYPE::POINT)
		{
			PointLights.push_back(pLight);
		}
	}

	if (PointLights.empty())
		return S_OK;

	std::ofstream ofs("../Bin/DataFiles/LightData.bin", std::ios::binary);
	if (!ofs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open LightData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	_uint iNumLights = (_uint)PointLights.size();
	ofs.write(reinterpret_cast<const _char*>(&iNumLights), sizeof(_uint));

	for (auto& pPointLight : PointLights)
	{
		const LIGHT_DESC* pDesc = pPointLight->Get_LightDesc();
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
	else if (newDesc.eType == LIGHT_TYPE::POINT)
	{
		newDesc.fRange = m_fRange;
	}
	else if (newDesc.eType == LIGHT_TYPE::SPOT)
	{
		newDesc.vDirection = _float4(m_fDirectionX, m_fDirectionY, m_fDirectionZ, 0.f);
		newDesc.fRange = m_fRange;
		newDesc.fFalloff = m_fFalloff;
		newDesc.fTheta = XMConvertToRadians(m_fTheta);
		newDesc.fPhi = XMConvertToRadians(m_fPhi);
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
	m_fFalloff = desc.fFalloff;
	m_fTheta = XMConvertToDegrees(desc.fTheta);
	m_fPhi = XMConvertToDegrees(desc.fPhi);
}


void CLightTool::Free()
{
	__super::Free();

}
