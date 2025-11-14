#include "pch.h"
#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Camera_Free.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
	
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain_Sand(TEXT("Layer_Terrain_Sand"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	//m_pGameInstance->ADD_DelayFunction(TEXT("Effect_Create"), 10.f, [&]()
	//	{
	//		Ready_Layer_Effect(TEXT("Layer_Effect"));
	//	});

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	Load_Map_Data();

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	

}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨이빈다"));
	return S_OK;
}

void CLevel_GamePlay::FontRender()
{
	SetWindowText(g_hWnd, TEXT("쓰레드 풀 동작"));
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	Load_Light_Data();

	/*LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.4f, 0.2f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(30.f, 5.f, 20.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/


	SHADOW_LIGHT_DESC		ShadowDesc{};
	ShadowDesc.vEye = _float4(0.f, 15.f, 0.f, 1.f);
	ShadowDesc.vAt = _float4(10.f, 0.f, 10.f, 1.f);
	ShadowDesc.fFovy = XMConvertToRadians(120.0f);
	ShadowDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	ShadowDesc.fNear = 0.1f;
	ShadowDesc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Ready_Shadow_Light(ShadowDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ForkLift"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;

	//}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain_Sand(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain_Sand"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			CameraDesc{};
	CameraDesc.fFov = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.1f;

	auto pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc);
	m_pGameInstance->Add_Camera(TEXT("FreeCamera"), static_cast<CCamera*>(pCamera));
	m_pGameInstance->SetMainCamera(TEXT("FreeCamera"));

	pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc);
	m_pGameInstance->Add_Camera(TEXT("PlayerCamera"), static_cast<CCamera*>(pCamera));

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;	

	for (size_t i = 0; i < 30; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PxTestProp"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

#ifdef _DEBUG
 	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ShaderTestModel"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
	//	return E_FAIL;

#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	for (size_t i = 0; i < 5; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ForkLift"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;

	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Test_InstanceModel"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Explosion"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	for (size_t i = 0; i < 50; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sprite_Explosion"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Map_Data()
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

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vil_Bui03_04"),
			ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Vil_Bui03_04"));

		if (SUCCEEDED(hr))
		{
			// Add_GameObject_ToLayer 호출 직후, 전체 리스트에서 마지막에 추가된 객체 가져오기
			list<CGameObject*>* pVil_Bui03_04s = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Vil_Bui03_04"));
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

HRESULT CLevel_GamePlay::Load_Light_Data()
{
	std::ifstream ifs("../Bin/DataFiles/LightData2.bin", std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open LightData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	const list<CLight*>* pLights = m_pGameInstance->GetAllLight();

	if (pLights && !pLights->empty())
	{
		for (auto pLight : *pLights)
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


CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}





void CLevel_GamePlay::Free()
{
	__super::Free();


}
