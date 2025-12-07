#include "pch.h"
#include "MapTool_Desert.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Cell.h"
#include "Terrain_Desert.h"
#include <fstream>
#include "Camera_Free.h"
#include "DesertObject.h"
#include "Player_Test.h"
#include "SpawnBox.h"

#include "Instance_Desert.h"

CMapTool_Desert::CMapTool_Desert()
{

}


HRESULT CMapTool_Desert::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::GetInstance();

	list<CGameObject*>* pTerrainList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), TEXT("Layer_Terrain"));
	list<CGameObject*>* pCameraList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), TEXT("Layer_Camera"));
	list<CGameObject*>* pPlayerList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), TEXT("Layer_Player_Test"));

	if (pTerrainList != nullptr && !pTerrainList->empty())
		m_pTerrain = dynamic_cast<CTerrain_Desert*>(pTerrainList->back());
	else
		m_pTerrain = nullptr; 

	if (pCameraList != nullptr && !pCameraList->empty())
		m_pCamera = dynamic_cast<CCamera_Free*>(pCameraList->back());
	else
		m_pCamera = nullptr;

	if (pPlayerList != nullptr && !pPlayerList->empty())
		m_pPlayer = dynamic_cast<CPlayer_Test*>(pPlayerList->back());
	else
		m_pPlayer = nullptr;

	m_pCameraTransform = m_pCamera->GetTransform();
	m_pPlayerTransform = m_pPlayer->GetTransform();

	return S_OK;
}

void CMapTool_Desert::Priority_Update(_float fTimeDelta)
{
}

void CMapTool_Desert::Update(_float fTimeDelta)
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
			if (m_eCurrentObject == DESESRT_RUIN_OBJECT::END)
				return;

			_float3 vPickedPoint = {};
			_float fHeight = m_fHeight;
			if (true == m_pGameInstance->isPicking(&vPickedPoint))
			{
				_vector vPickPoint = XMVectorSet(vPickedPoint.x, vPickedPoint.y, vPickedPoint.z, 1.f);
				HRESULT hr = E_FAIL;
				_wstring layerTag = L"";
				_wstring protoTag = L"";
				CDesertObject::DesertObjectDesc pDesc = {};
				Nayitba_Desc MonsterDesc = {};

				if (m_eCurrentObject == DESESRT_RUIN_OBJECT::DOOR_A)
				{
					protoTag = TEXT("Prototype_GameObject_Building_Ruin"); layerTag = TEXT("Layer_Building_Ruin");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_A");
					
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::DOOR_B)
				{
					protoTag = TEXT("Prototype_GameObject_Building_Ruin"); layerTag = TEXT("Layer_Building_Ruin");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_1A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_1A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_2A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_2A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_3A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_3A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_4A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_4A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_5A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_5A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_6A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_6A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_12A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_12A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_14A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_14A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_14B)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_14B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_15A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_15A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_16A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_16A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_16B)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_16B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_17A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_17A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_17B)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_17B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_18A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_18A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_20A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_20A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_21A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_21A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_22A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_22A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_23A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_23A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_24A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_24A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_35A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_35A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_39A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_39A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_39C)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_39C");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_39D)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_39D");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_43A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_43A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_44A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_44A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_46A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_46A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_50A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_50A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_52A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_52A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_55A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_55A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_58A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_58A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_59A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_59A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_60A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_60A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_61A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_61A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_65A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_65A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_66A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_66A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_67A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_67A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_69A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_69A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_71A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_71A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_80A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_80A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_81A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_81A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_93A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_93A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_95A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_95A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_96A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_96A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_97A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_97A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_98A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_98A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_100A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_100A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_101A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_101A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_103A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_103A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_104A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_104A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_105A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_105A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_106A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_106A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_108A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_108A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_109A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_109A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_110A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_110A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_111A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_111A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_112A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_112A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_113A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_113A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_115A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_115A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_116A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_116A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_117A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_117A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_121A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_121A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_122A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_122A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_123A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_123A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_125A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_125A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_126A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_126A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_127A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_127B)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_127C)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127C");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_127D)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127D");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_127E)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127E");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_128A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_128A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_128B)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_128B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_128C)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_128C");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_131A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_131A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_132A)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_132A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_132B)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_132B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_132C)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_132C");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::CANYON_133B)
				{
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_133B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::LIFT_BODY)
				{
					protoTag = TEXT("Prototype_GameObject_Lift_Body"); layerTag = TEXT("Layer_Lift_Body");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Lift_Body");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::LIFT_CONTROLLER)
				{
					protoTag = TEXT("Prototype_GameObject_Lift_Controller"); layerTag = TEXT("Layer_Lift_Controller");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Lift_Controller");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::LIFT_PLATFORM)
				{
					protoTag = TEXT("Prototype_GameObject_Lift_Platform"); layerTag = TEXT("Layer_Lift_Platform");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Lift_Platform");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::FLOOR7_A)
				{
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::FLOOR7_B)
				{
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_B");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::FLOOR7_C)
				{
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_C");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::FLOOR7_D)
				{
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_D");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::FLOOR7_E)
				{
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_E");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::FLOOR7_F)
				{
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_F");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::LAMP_47A)
				{
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Lamp_47A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::VendingMachine_6A)
				{
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_VendingMachine_6A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::VendingMachine_7A)
				{
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_VendingMachine_7A");
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::GORILLA)
				{
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster");
					pDesc.pComponentTag == nullptr; MonsterDesc.iMonsterID = 1;
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::SPAWN_BOX4)
				{
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster");
					pDesc.pComponentTag == nullptr; MonsterDesc.iMonsterID = 4;
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::SPAWN_BOX5)
				{
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster");
					pDesc.pComponentTag == nullptr; MonsterDesc.iMonsterID = 5;
				}

				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::TERRAIN_DECREASE_RECT)
				{
					if (fHeight > 0)
						fHeight *= -1.f;

					m_pTerrain->Change_Height_Rect(vPickPoint, fHeight, m_fRadius);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::TERRAIN_INCREASE_RECT)
				{
					if (fHeight < 0)
						fHeight;

					m_pTerrain->Change_Height_Rect(vPickPoint, fHeight, m_fRadius);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::TERRAIN_FLAT)
				{
					m_pTerrain->Change_Height_Flat(vPickPoint, fHeight, m_fRadius);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::PLAYER)
				{
					m_pPlayerTransform->Set_State(STATE::POSITION, vPickPoint);
				}

				if (pDesc.pComponentTag == nullptr)
					hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, &MonsterDesc);
				else
					hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, &pDesc);

				if (SUCCEEDED(hr))
				{
					list<CGameObject*>* pObjects = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), layerTag.c_str());
					if (pObjects && !pObjects->empty())
					{
						// 새롭게 추가된 객체를 멤버 변수에 저장합니다.
						m_pLastAddedObject = pObjects->back();

						CTransform* pTransform = dynamic_cast<CTransform*>(m_pLastAddedObject->Find_Component(TEXT("Com_Transform")));

						pTransform->Set_State(STATE::POSITION, vPickPoint);

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
				m_pPickedObject = Find_Object_To_Pick(ENUM_CLASS(LEVEL::DESERT), m_CurrentLayerName, &vPickedPoint);

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

		if (m_pLastAddedObject)
		{
			CTransform* pTransform = dynamic_cast<CTransform*>(m_pLastAddedObject->Find_Component(TEXT("Com_Transform")));
			if (pTransform)
			{
				_vector vPosition = pTransform->Get_State(STATE::POSITION);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_1))
				{
					vPosition += pTransform->Get_State(STATE::LOOK) * m_fMoveSpeed * fTimeDelta;
				}
				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_2))
				{
					vPosition -= pTransform->Get_State(STATE::LOOK) * m_fMoveSpeed * fTimeDelta;
				}
				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_3))
				{
					vPosition -= pTransform->Get_State(STATE::RIGHT) * m_fMoveSpeed * fTimeDelta;
				}
				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_4))
				{
					vPosition += pTransform->Get_State(STATE::RIGHT) * m_fMoveSpeed * fTimeDelta;
				}

				pTransform->Set_State(STATE::POSITION, vPosition);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_5))
					pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fMoveSpeed * fTimeDelta, 0.f, 0.f));

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_6))
					pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) - XMVectorSet(0.f, m_fMoveSpeed * fTimeDelta, 0.f, 0.f));
			}
		}

		if (!m_bIsDeplayMode && m_pGameInstance->KeyPressed(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
		{
			if (m_eCurrentObject == DESESRT_RUIN_OBJECT::END)
				return;

			_float3 vPickedPoint = {};
			_float fHeight = m_fHeight;
			if (true == m_pGameInstance->isPicking(&vPickedPoint))
			{
				_vector vPickPoint = XMVectorSet(vPickedPoint.x, vPickedPoint.y, vPickedPoint.z, 1.f);

				/*if (m_eCurrentObject == DESESRT_RUIN_OBJECT::MASK_WHITE)
				{
					Change_MaskMap_White(vPickedPoint);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::MASK_BLACK)
				{
					Change_MaskMap_Black(vPickedPoint);
				}*/
				if (m_eCurrentObject == DESESRT_RUIN_OBJECT::TERRAIN_UP)
				{
					m_pTerrain->Change_Height_Sculpt(vPickPoint, m_fHeight, m_fRadius, m_fMaxHeight);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::TERRAIN_DOWN)
				{
					m_pTerrain->Change_Height_Sculpt(vPickPoint, -m_fHeight, m_fRadius, m_fMaxHeight);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::TERRAIN_HILL)
				{
					m_pTerrain->Change_Height_Smooth(vPickPoint, m_fSmoothFactor, m_fRadius);
				}

			}
		}


	}

}

void CMapTool_Desert::Late_Update(_float fTimeDelta)
{
}

HRESULT CMapTool_Desert::Render()
{
	ImGui::SetNextWindowSize(ImVec2(300.f, 0.f), ImGuiCond_Once);


	ImGui::Begin("Map Editor");

	ImGui::Text("MapTool_Desert");
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

#pragma region HeightMap Menu
	// 1. 하이트맵 높이 조절 메뉴
	/*if (ImGui::Button("INCREASE_CIRCLE"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::TERRAIN_INCREASE_CIRCLE;
	}
	ImGui::SameLine();
	if (ImGui::Button("DECREASE_CIRCLE"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::TERRAIN_DECREASE_CIRCLE;
	}*/
	if (ImGui::Button("INCREASE_RECT"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::TERRAIN_INCREASE_RECT;
	}
	ImGui::SameLine();
	if (ImGui::Button("DECREASE_RECT"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::TERRAIN_DECREASE_RECT;
	}
	ImGui::SameLine();
	if (ImGui::Button("Flatting"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::TERRAIN_FLAT;
	}
	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();
	if (ImGui::Button("TERRAIN_UP"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::TERRAIN_UP;
	}
	ImGui::SameLine();

	if (ImGui::Button("TERRAIN_DOWN"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::TERRAIN_DOWN;
	}
	ImGui::SameLine();
	if (ImGui::Button("TERRAIN_SMOOTH"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::TERRAIN_HILL;
	}

	ImGui::InputFloat("Height", &m_fHeight, 0.1f, 100.f);
	ImGui::InputFloat("Radius", &m_fRadius, 0.01f, 10.f);
	ImGui::InputFloat("Max Height", &m_fMaxHeight, 1.f, 30.f);
	ImGui::InputFloat("Smooth Factor", &m_fSmoothFactor, 0.1f, 1.f);

	ImGui::Text("Change Terrain Height.");

	static _char szHeightMapFilePath[256] = "../Bin/Resources/Maps/Desert/Terrain/Height2.bmp";
	ImGui::InputText("HeightMap File Path", szHeightMapFilePath, sizeof(szHeightMapFilePath));

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	// 에디터 세이브 / 로드
	if (ImGui::Button("Height Map Save"))
	{
		if (FAILED(Save_Terrain_HeightMap(szHeightMapFilePath)))
		{
			MessageBoxW(g_hWnd, L"지형 저장 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"지형 저장 성공.", L"알림", MB_OK);
		}
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();
#pragma endregion

#pragma region MaskMap Menu
	//if (ImGui::Button("MASK_MAP_BALCK"))
	//{
	//	m_eCurrentObject = DESESRT_RUIN_OBJECT::MASK_BLACK;
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("MASK_MAP_WHITE"))
	//{
	//	m_eCurrentObject = DESESRT_RUIN_OBJECT::MASK_WHITE;
	//}
	//if (ImGui::Button("NEW_MASK_MAP"))
	//{
	//	Set_NewMaskMap();
	//	m_pTerrain->Set_MapTool(this);
	//}
	//ImGui::Spacing(); // 메뉴 사이의 간격
	//ImGui::Separator(); // 구분선을 추가
	//ImGui::Spacing();
	//static _char szMaskLoadFilePath[256] = "../Bin/Resources/Maps/Desert/Terrain/ReedMask2.png";
	//ImGui::InputText("MaskMap Load File Path", szMaskLoadFilePath, sizeof(szMaskLoadFilePath));
	//if (ImGui::Button("Load_MASK"))
	//{
	//	Set_LoadMaskMap(szMaskLoadFilePath);
	//	m_pTerrain->Set_MapTool(this);
	//}
	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	/*static _char szMaskMapFilePath[256] = "../Bin/Resources/Maps/Desert/Terrain/ReedMask.png";
	ImGui::InputText("MaskMap Save File Path", szMaskMapFilePath, sizeof(szMaskMapFilePath));

	if (ImGui::Button("Save_MASK"))
	{
		if (FAILED(Save_MaskMap(szMaskMapFilePath)))
		{
			MessageBoxW(g_hWnd, L"마스크 맵 저장 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"마스크 맵 저장 성공.", L"알림", MB_OK);
		}
	}*/

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

#pragma endregion

#pragma region Navigation Menu
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
#pragma endregion

	ImGui::Text("Registered Models");

	// 1. 메인 테마/레벨 선택 드롭다운 
	ImGui::Text("Current Theme Selection");
	const _char* themeNames[] = { "Building_Ruin", "ENVIRONMENT", "XION", "CANYON", "Archi", "Deco"};
	_int nSelectedTheme = (_int)m_eCurrentMap;

	if (ImGui::Combo("Select Theme", &nSelectedTheme, themeNames, IM_ARRAYSIZE(themeNames)))
	{
		// 테마가 변경되면 현재 오브젝트 선택을 초기화할 수 있습니다.
		m_eCurrentObject = DESESRT_RUIN_OBJECT::END;
		m_eCurrentMap = (DESERT_THEME)nSelectedTheme;
	}

	if (m_eCurrentMap == DESERT_THEME::BUILDING_RUIN)
	{
		ImGui::Text("Desert Map Objects");
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
		// 2. 모델 배치 / 삭제메뉴
		_int nSelectedModel = -1;
		_int nSelectedBuilding = -1;
		_int nSelectedEnvironment = -1;

		const _char* modelNames[] = { "Player" };

		const _char* buildingNames[] = { "Door_A", "Door_B", "Xion_Building"};

		const _char* environmentNames[] = { "Bamboo" };

		if (ImGui::CollapsingHeader("Models"))
		{
			if (ImGui::ListBox("##Models", &nSelectedModel, modelNames, IM_ARRAYSIZE(modelNames), 7))
			{
				if (nSelectedModel == 0) // Player
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::PLAYER;
					m_CurrentLayerName = TEXT("Layer_Player_Test");
				}
			}
		}
		if (ImGui::CollapsingHeader("Buildings"))
		{
			if (ImGui::ListBox("##Buildings", &nSelectedBuilding, buildingNames, IM_ARRAYSIZE(buildingNames), 7))
			{
				if (nSelectedBuilding == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_A;
					m_CurrentLayerName = TEXT("Layer_Building_Ruin");
				}
				else if (nSelectedBuilding == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_B;
					m_CurrentLayerName = TEXT("Layer_Building_Ruin");
				}
				else if (nSelectedBuilding == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING;
					m_CurrentLayerName = TEXT("Layer_Building_Ruin");
				}

			}
		}
		if (ImGui::CollapsingHeader("Environments"))
		{
			if (ImGui::ListBox("##Environments", &nSelectedEnvironment, environmentNames, IM_ARRAYSIZE(environmentNames), 7))
			{
				/*if (nSelectedEnvironment == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BAMBOO;
					m_CurrentLayerName = TEXT("Layer_Bamboo");
				}*/
			}
		}
	}
	else if (m_eCurrentMap == DESERT_THEME::ENVIRONMENT)
	{
		ImGui::Text("Desert Common Objects");
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
		// 2. 모델 배치 / 삭제메뉴
		_int nSelectedModel = -1;
		_int nSelectedBuilding = -1;
		_int nSelectedEnvironment = -1;

		const _char* modelNames[] = { "" };

		const _char* buildingNames[] = { "" };

		const _char* environmentNames[] = { "CM_Rock1" };

		if (ImGui::CollapsingHeader("Environments"))
		{
			if (ImGui::ListBox("##Environments", &nSelectedEnvironment, environmentNames, IM_ARRAYSIZE(environmentNames), 7))
			{
				/*if (nSelectedEnvironment == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CM_ROCK1;
					m_CurrentLayerName = TEXT("Layer_CM_Rock1");
				}*/

			}
		}
	}

	else if (m_eCurrentMap == DESERT_THEME::CANYON)
	{
		ImGui::Text("Desert Common Objects");
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
		// 2. 모델 배치 / 삭제메뉴
		_int nSelectedCharacter = -1;
		_int nSelectedVeryBigCanyon = -1;
		_int nSelectedBigCanyon = -1;
		_int nSelectedMiddleCanyon = -1;
		_int nSelectedSmallCanyon = -1;
		_int nSelectedRock = -1;
		_int nSelectedStone = -1;

		const _char* characterNames[] = { "Player", "Gorilla", "Monster4", "Monster5" };
		
		const _char* veryBigCanyonNames[] = { "CANYON_127A","CANYON_127B", "CANYON_127C", "CANYON_127D", "CANYON_127E", "CANYON_128A", "CANYON_128B","CANYON_128C",
												"CANYON_132A", "CANYON_132B", "CANYON_132C", "CANYON_133B" };
		
		const _char* bigCanyonNames[] = {  "CANYON_3A", "CANYON_12A", "CANYON_22A", "CANYON_23A", "CANYON_39A", "CANYON_39C", "CANYON_39D", "CANYON_65A", "CANYON_66A", 
										 "CANYON_67A", "CANYON_69A", "CANYON_71A",   "CANYON_97A", "CANYON_105A", "CANYON_106A", "CANYON_108A", "CANYON_125A", "CANYON_126A" };

		const _char* middleCanyonNames[] = { "CANYON_5A", "CANYON_21A", "CANYON_24A", "CANYON_35A", "CANYON_80A", "CANYON_81A", "CANYON_101A", "CANYON_103A", "CANYON_104A", "CANYON_109A", "CANYON_110A", "CANYON_111A", "CANYON_112A", "CANYON_113A", "CANYON_115A" };
		
		const _char* smallCanyonNames[] = { "CANYON_1A", "CANYON_2A", "CANYON_6A", "CANYON_20A", "CANYON_93A", "CANYON_95A", "CANYON_96A", "CANYON_116A", "CANYON_117A"};
		
		const _char* rockNames[] = { "CANYON_14B", "CANYON_15A", "CANYON_17A", "CANYON_17B", "CANYON_52A", "CANYON_55A", "CANYON_60A", "CANYON_61A", "CANYON_98A", 
									"CANYON_100A", "CANYON_121A", "CANYON_122A", };

		const _char* stoneNames[] = { "CANYON_4A", "CANYON_14A", "CANYON_16A", "CANYON_16B", "CANYON_18A", "CANYON_43A", "CANYON_44A", "CANYON_46A", "CANYON_50A", 
									"CANYON_58A", "CANYON_59A", "CANYON_123A", "CANYON_131A" };

		if (ImGui::CollapsingHeader("Character"))
		{
			if (ImGui::ListBox("##Character", &nSelectedCharacter, characterNames, IM_ARRAYSIZE(characterNames), 7))
			{
				if (nSelectedCharacter == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::PLAYER;
					m_CurrentLayerName = TEXT("Layer_Player_Test");
				}
				else if (nSelectedCharacter == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::GORILLA;
					m_CurrentLayerName = TEXT("Layer_Monster");
				}
				else if (nSelectedCharacter == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SPAWN_BOX4;
					m_CurrentLayerName = TEXT("Layer_Monster");
				}
				else if (nSelectedCharacter == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SPAWN_BOX5;
					m_CurrentLayerName = TEXT("Layer_Monster");
				}

			}
		}

		if (ImGui::CollapsingHeader("Canyon"))
		{
			if (ImGui::CollapsingHeader("Very Big Canyon (12)"))
			{
				if (ImGui::ListBox("Very Big Canyon", &nSelectedVeryBigCanyon, veryBigCanyonNames, IM_ARRAYSIZE(veryBigCanyonNames), 5))
				{
					if (nSelectedVeryBigCanyon == 0) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_127A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 1) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_127B; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 2) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_127C; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 3) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_127D; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 4) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_127E; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 5) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_128A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 6) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_128B; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 7) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_128C; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 8) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_132A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 9) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_132B; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 10) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_132C; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedVeryBigCanyon == 11) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_133B; m_CurrentLayerName = TEXT("Layer_Canyon"); }
				}
			}
			if (ImGui::CollapsingHeader("Big Canyon (18)"))
			{
				if (ImGui::ListBox("##BigCanyonList", &nSelectedBigCanyon, bigCanyonNames, IM_ARRAYSIZE(bigCanyonNames), 5))
				{
					m_CurrentLayerName = TEXT("Layer_Canyon");
					if (nSelectedBigCanyon == 0) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_3A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 1) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_12A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 2) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_22A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 3) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_23A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 4) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_39A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 5) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_39C; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 6) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_39D; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 7) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_65A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 8) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_66A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 9) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_67A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 10) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_69A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 11) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_71A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 12) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_97A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 13) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_105A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 14) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_106A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 15) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_108A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 16) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_125A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedBigCanyon == 17) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_126A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
				}
			}
			if (ImGui::CollapsingHeader("Middle Canyon (15)"))
			{
				if (ImGui::ListBox("##MiddleCanyonList", &nSelectedMiddleCanyon, middleCanyonNames, IM_ARRAYSIZE(middleCanyonNames), 5))
				{
					m_CurrentLayerName = TEXT("Layer_Canyon");
					if (nSelectedMiddleCanyon == 0) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_5A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 1) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_21A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 2) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_24A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 3) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_35A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 4) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_80A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 5) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_81A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 6) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_101A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 7) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_103A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 8) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_104A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 9) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_109A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 10) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_110A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 11) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_111A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 12) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_112A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 13) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_113A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedMiddleCanyon == 14) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_115A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
				}
			}
			if (ImGui::CollapsingHeader("Small Canyon (9)"))
			{
				if (ImGui::ListBox("##SmallCanyonList", &nSelectedSmallCanyon, smallCanyonNames, IM_ARRAYSIZE(smallCanyonNames), 5))
				{
					m_CurrentLayerName = TEXT("Layer_Canyon");
					if (nSelectedSmallCanyon == 0) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_1A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedSmallCanyon == 1) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_2A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedSmallCanyon == 2) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_6A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedSmallCanyon == 3) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_20A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedSmallCanyon == 4) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_93A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedSmallCanyon == 5) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_95A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedSmallCanyon == 6) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_96A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedSmallCanyon == 7) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_116A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedSmallCanyon == 8) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_117A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
				}
			}
			if (ImGui::CollapsingHeader("Rock (12)"))
			{
				if (ImGui::ListBox("##RockList", &nSelectedRock, rockNames, IM_ARRAYSIZE(rockNames), 5))
				{
					m_CurrentLayerName = TEXT("Layer_Canyon");
					if (nSelectedRock == 0) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_14B; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 1) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_15A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 2) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_17A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 3) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_17B; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 4) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_52A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 5) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_55A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 6) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_60A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 7) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_61A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 8) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_98A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 9) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_100A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 10) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_121A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedRock == 11) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_122A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
				}
			}
			if (ImGui::CollapsingHeader("Stone (13)"))
			{
				if (ImGui::ListBox("##StoneList", &nSelectedStone, stoneNames, IM_ARRAYSIZE(stoneNames), 5))
				{
					m_CurrentLayerName = TEXT("Layer_Canyon");
					if (nSelectedStone == 0) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_4A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 1) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_14A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 2) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_16A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 3) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_16B; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 4) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_18A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 5) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_43A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 6) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_44A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 7) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_46A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 8) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_50A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 9) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_58A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 10) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_59A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 11) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_123A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
					else if (nSelectedStone == 12) { m_eCurrentObject = DESESRT_RUIN_OBJECT::CANYON_131A; m_CurrentLayerName = TEXT("Layer_Canyon"); }
				}
			}
		}
	}

	else if (m_eCurrentMap == DESERT_THEME::ARCHI)
	{
		ImGui::Text("Architecture Objects");
		_int nSelectedModel = -1;

		const _char* modelNames[] = { "LIFT_BODY", "LIFT_CONTROLLER", "LIFT_PLATFORM", "FLOOR7_A", "FLOOR7_B", "FLOOR7_C", "FLOOR7_D", "FLOOR7_E", "FLOOR7_F" };

		if (ImGui::CollapsingHeader("Models"))
		{
			if (ImGui::ListBox("##Models", &nSelectedModel, modelNames, IM_ARRAYSIZE(modelNames), 7))
			{
				if (nSelectedModel == 0) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::LIFT_BODY;
					m_CurrentLayerName = TEXT("Layer_Lift_Body");
				}
				else if (nSelectedModel == 1) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::LIFT_CONTROLLER;
					m_CurrentLayerName = TEXT("Layer_Lift_Controller");
				}
				else if (nSelectedModel == 2) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::LIFT_PLATFORM;
					m_CurrentLayerName = TEXT("Layer_Lift_Platform");
				}
				else if (nSelectedModel == 3) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_A;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 4) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_B;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 5) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_C;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 6) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_D;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 7) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_E;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 8) 
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_F;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				
			}
		}
	}
	else if (m_eCurrentMap == DESERT_THEME::DECO)
	{
		ImGui::Text("Deco Objects");
		_int nSelectedModel = -1;

		const _char* modelNames[] = { "Lamp_47A", "VendingMachine_6A", "VendingMachine_7A" };

		if (ImGui::CollapsingHeader("Models"))
		{
			if (ImGui::ListBox("##Models", &nSelectedModel, modelNames, IM_ARRAYSIZE(modelNames), 7))
			{
				if (nSelectedModel == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::LAMP_47A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedModel == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VendingMachine_6A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedModel == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VendingMachine_7A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
	}

	// 삭제 버튼
#pragma region Delete_Object
	if (ImGui::Button("Delete All"))
	{
		m_pObjects = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), m_CurrentLayerName);
		if (m_eCurrentObject == DESESRT_RUIN_OBJECT::END || m_pObjects == nullptr || m_pObjects->empty()) return E_FAIL;


		if (m_pObjects != nullptr)
		{
			for (auto pObject : *m_pObjects)
			{
				pObject->Set_Dead(true);
			}

			m_pObjects->clear();
		}

	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Latest"))
	{
		// m_pObjects는 현재 레이어의 CGameObject* 리스트 포인터입니다.
		m_pObjects = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), m_CurrentLayerName);

		// 2. 유효성 검사
		if (m_eCurrentObject == DESESRT_RUIN_OBJECT::END || m_pObjects == nullptr || m_pObjects->empty())
			return E_FAIL;

		auto iter = m_pObjects->end();
		--iter;

		CGameObject* pObjectToDelete = *iter;

		if (pObjectToDelete != nullptr)
		{
			pObjectToDelete->Set_Dead(true);

			m_pObjects->erase(iter);

			m_pObject = nullptr;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Pick"))
	{
		// m_pObjects는 현재 레이어의 CGameObject* 리스트 포인터입니다.
		if (m_bIsDeplayMode == false)
			return E_FAIL;

		m_pObjects = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), m_CurrentLayerName);

		// 2. 유효성 검사
		if (m_eCurrentObject == DESESRT_RUIN_OBJECT::END || m_pObjects == nullptr)
			return E_FAIL;


		_bool bFound = false;
		auto iter = m_pObjects->begin();
		while (iter != m_pObjects->end())
		{
			if (*iter == m_pPickedObject)
			{
				(*iter)->Set_Dead(true); // 게임 루프에서 해제되도록 설정
				iter = m_pObjects->erase(iter);
				bFound = true;
				break; // 찾았으므로 반복 종료
			}
			else
			{
				++iter;
			}
		}

		if (bFound)
		{
			m_pPickedObject = nullptr;
		}

	}

	ImGui::SameLine();
	if (ImGui::Button("NONE"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::END;
	}


	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();
#pragma endregion

	if (ImGui::CollapsingHeader("Object Placement Control"))
	{
		ImGui::InputFloat("Move Speed", &m_fMoveSpeed, 1.0f, 0.0f, "%.2f");

		if (m_fMoveSpeed < 0.1f)
		{
			m_fMoveSpeed = 0.1f;
		}
	}
	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();
#pragma region Teleport
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
		m_pCameraTransform->Set_State(STATE::POSITION, XMVectorSet(m_fX, m_fY, m_fZ, 1.f));
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();
#pragma endregion

	ImGui::Text("Object Control");
	if (ImGui::Checkbox("Object Deplay Mode", &m_bIsDeplayMode))
	{
		// 모드가 켜지면 배치 모드(m_eCurrentObject)를 해제하고,
		// 모드가 꺼지면 드래그 상태를 초기화합니다.
		if (m_bIsDeplayMode)
		{
			m_eCurrentObject = DESESRT_RUIN_OBJECT::END;
		}
		else
		{
			m_bIsDragging = false;
			m_pPickedObject = nullptr;
		}
	}
	ImGui::Text("Instruction: Select object on screen to drag.");

#pragma region Object_Rotation
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
#pragma endregion

#pragma region Object_Scale
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
#pragma endregion

	if (ImGui::Button("Scale_All_Layer"))
	{
		if (m_eCurrentObject == DESESRT_RUIN_OBJECT::END) return E_FAIL;

		m_pObjects = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), m_CurrentLayerName);

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

	static _char szSaveFilePath[256] = "../Bin/DataFiles/MapData_Desert.bin";
	ImGui::InputText("Map Save File Path", szSaveFilePath, sizeof(szSaveFilePath));

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	// 에디터 세이브 / 로드
	if (ImGui::Button("Save"))
	{
		// 맵 오브젝트 저장
		if (FAILED(Save_Map_Objects(szSaveFilePath)))
		{
			MessageBoxW(g_hWnd, L"맵 오브젝트 저장 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"맵 오브젝트 저장 성공.", L"알림", MB_OK);
		}
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	static _char szLoadFilePath[256] = "../Bin/DataFiles/MapData_Desert.bin";
	ImGui::InputText("Map Load File Path", szLoadFilePath, sizeof(szLoadFilePath));

	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_Map_Objects(szLoadFilePath)))
		{
			MessageBoxW(g_hWnd, L"맵 오브젝트 로드 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"맵 오브젝트 로드 성공.", L"알림", MB_OK);
		}
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	static _char szSaveMonsterFilePath[256] = "../Bin/DataFiles/MonsterData_Desert.bin";
	ImGui::InputText("Monster Save File Path", szSaveMonsterFilePath, sizeof(szSaveMonsterFilePath));

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	// 에디터 세이브 / 로드
	if (ImGui::Button("Save_Monster"))
	{
		// 맵 오브젝트 저장
		if (FAILED(Save_Monster_Objects(szSaveMonsterFilePath)))
		{
			MessageBoxW(g_hWnd, L"몬스터 저장 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"몬스터 저장 성공.", L"알림", MB_OK);
		}
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	static _char szLoadMonsterFilePath[256] = "../Bin/DataFiles/MonsterData_Desert.bin";
	ImGui::InputText("Monster Load File Path", szLoadMonsterFilePath, sizeof(szLoadMonsterFilePath));

	if (ImGui::Button("Load_Monster"))
	{
		if (FAILED(Load_Monster_Objects(szLoadMonsterFilePath)))
		{
			MessageBoxW(g_hWnd, L"몬스터 로드 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"몬스터 로드 성공.", L"알림", MB_OK);
		}
	}

	ImGui::End();

	return S_OK;
}


void CMapTool_Desert::Update_Rotation()
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

HRESULT CMapTool_Desert::Save_Map_Objects(const _char* szFilePath)
{
	// 맵 데이터 파일 열기
	std::ofstream ofs(szFilePath, std::ios::binary);
	if (!ofs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Building_Ruin")))) return S_OK;
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Canyon")))) return S_OK; 
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Lift_Body")))) return S_OK; 
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Lift_Controller")))) return S_OK; 
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Lift_Platform")))) return S_OK; 
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Iron_Floor")))) return S_OK; 
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Deco")))) return S_OK; 

	ofs.close();

	return S_OK;
}

HRESULT CMapTool_Desert::Save_Monster_Objects(const _char* szFilePath)
{
	// 맵 데이터 파일 열기
	std::ofstream ofs(szFilePath, std::ios::binary);
	if (!ofs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Save_Monsters_By_Layer(ofs, TEXT("Layer_Monster")))) return S_OK;

	ofs.close();

	return S_OK;
}


HRESULT CMapTool_Desert::Save_Objects_By_Layer(std::ofstream& ofs, const _tchar* pLayerTag)
{
	list<CGameObject*>* pObj = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
	_uint iNumObjs = (pObj) ? (_uint)pObj->size() : 0;

	ofs.write(reinterpret_cast<const char*>(&iNumObjs), sizeof(_uint));

	if (pObj)
	{
		for (auto pObject : *pObj)
		{
			CDesertObject* pDesertObject = dynamic_cast<CDesertObject*>(pObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));

			if (pTransform && (pDesertObject))
			{
				SAVEDOBJECTINFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);

				const _tchar* pTag = pDesertObject->Get_ComponentTag();
				wcsncpy_s(info.szComponentTag, 256, pTag, _TRUNCATE);

				const _uint pId = pDesertObject->Get_ObjectID();
				info.iObjectID = pId;

				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVEDOBJECTINFO));
			}
		}
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Save_Monsters_By_Layer(ofstream& ofs, const _tchar* pLayerTag)
{
	list<CGameObject*>* pObj = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
	_uint iNumObjs = (pObj) ? (_uint)pObj->size() : 0;

	ofs.write(reinterpret_cast<const char*>(&iNumObjs), sizeof(_uint));

	if (pObj)
	{
		for (auto pObject : *pObj)
		{
			CSpawnBox* pSpawnBox = dynamic_cast<CSpawnBox*>(pObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));

			if (pTransform && (pSpawnBox))
			{
				SAVEDMONSTERINFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);

				const _uint pId = pSpawnBox->Get_MonsterID();
				info.iMonsterId = pId;

				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVEDMONSTERINFO));
			}
		}
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Load_Map_Objects(const _char* szFilePath)
{
	std::ifstream ifs(szFilePath, std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Building_Ruin"), TEXT("Layer_Building_Ruin")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Canyon"), TEXT("Layer_Canyon")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Lift_Body"), TEXT("Layer_Lift_Body")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Lift_Controller"), TEXT("Layer_Lift_Controller")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Lift_Platform"), TEXT("Layer_Lift_Platform")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Iron_Floor"), TEXT("Layer_Iron_Floor")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Deco"), TEXT("Layer_Deco")))) return S_OK;
	ifs.close();

	return S_OK;
}

HRESULT CMapTool_Desert::Load_Monster_Objects(const _char* szFilePath)
{
	std::ifstream ifs(szFilePath, std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Load_Monsters_By_Layer(ifs, TEXT("Prototype_GameObject_SpawnBox"), TEXT("Layer_Monster")))) return S_OK;
	ifs.close();

	return S_OK;
}

HRESULT CMapTool_Desert::Load_Objects_By_Layer(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDOBJECTINFO));

		CDesertObject::DESERT_OBJECT_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iObjectId = info.iObjectID;
		Desc.pComponentTag = info.szComponentTag;

		_vector vScale = {};
		_vector vRotation = {};
		_vector vPosition = {};
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

		XMStoreFloat3(&Desc.vScale, vScale);
		XMStoreFloat4(&Desc.vRotation, vRotation);
		XMStoreFloat3(&Desc.vPosition, vPosition);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag,
			ENUM_CLASS(LEVEL::DESERT), pLayerTag, &Desc);
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Load_Monsters_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDMONSTERINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDMONSTERINFO));

		NAYITBA_DESC Desc = {};
		Desc.iMonsterID = info.iMonsterId;

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag,
			ENUM_CLASS(LEVEL::DESERT), pLayerTag, &Desc);

		if (SUCCEEDED(hr))
		{
			list<CGameObject*>* pObjs = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
			if (pObjs && !pObjs->empty())
			{
				CGameObject* pObj = pObjs->back();
				CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Find_Component(TEXT("Com_Transform")));
				if (pTransform)
				{
					_matrix matWorld = XMLoadFloat4x4(&info.worldMatrix);

					_vector vScale = {};
					_vector vRotation = {};
					_vector vPosition = {};
					XMMatrixDecompose(&vScale, &vRotation, &vPosition, matWorld);

					_matrix matScale = XMMatrixScaling(XMVectorGetX(vScale), XMVectorGetY(vScale), XMVectorGetZ(vScale));
					_matrix matRotation = XMMatrixRotationQuaternion(vRotation);
					_matrix matTranslation = XMMatrixTranslationFromVector(vPosition);

					// 순서: Scale * Rotation * Translation (SRT 순서)
					_matrix matFinalWorld = matScale * matRotation * matTranslation;

					_float4x4* pWorldMatrixDest = const_cast<_float4x4*>(pTransform->Get_WorldMatrixPtr());
					XMStoreFloat4x4(pWorldMatrixDest, matFinalWorld);

				}
			}
		}
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Load_Instancing_By_Layer(ifstream& ifs, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	if (iNumObjs == 0) return S_OK;

	map<wstring, pair<vector<VTX_INSTANCE_MODEL>, _uint>> groupedInstanceData;

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDOBJECTINFO));

		_matrix matWorld = XMLoadFloat4x4(&info.worldMatrix);
		_vector vScale, vRotation, vPosition;
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, matWorld);

		_matrix matFinalWorld = XMMatrixScaling(XMVectorGetX(vScale), XMVectorGetY(vScale), XMVectorGetZ(vScale))
			* XMMatrixRotationQuaternion(vRotation)
			* XMMatrixTranslationFromVector(vPosition);

		VTX_INSTANCE_MODEL InstanceData{};
		XMStoreFloat4(&InstanceData.vRight, matFinalWorld.r[0]);
		XMStoreFloat4(&InstanceData.vUp, matFinalWorld.r[1]);
		XMStoreFloat4(&InstanceData.vLook, matFinalWorld.r[2]);
		XMStoreFloat4(&InstanceData.vTranslation, matFinalWorld.r[3]);

		wstring strTag = info.szComponentTag;
		_uint iObjectID = info.iObjectID;

		groupedInstanceData[strTag].first.push_back(InstanceData);
		groupedInstanceData[strTag].second = iObjectID;
	}

	for (auto& pair : groupedInstanceData)
	{
		Engine::MODEL_INSTANCE_LOAD_DESC FinalLoadDesc = {};
		FinalLoadDesc.iNumInstance = (_uint)pair.second.first.size();

		FinalLoadDesc.pPrototypeTag = pair.first.c_str();
		FinalLoadDesc.iObjectID = pair.second.second;
		FinalLoadDesc.pInstancingData = const_cast<vector<VTX_INSTANCE_MODEL>*>(&pair.second.first);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT),
			TEXT("Prototype_GameObject_Instance_Desert"),
			ENUM_CLASS(LEVEL::DESERT), pLayerTag,
			&FinalLoadDesc);

		if (FAILED(hr))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}


void CMapTool_Desert::Delete_All_Before_Load(const _tchar* pLayerTag)
{
	m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
	if (m_pObjects)
	{
		for (auto pObject : *m_pObjects)
		{
			pObject->Set_Dead(true);
		}
	}
}


HRESULT CMapTool_Desert::Save_Terrain_HeightMap(const _char* szHeightMapFilePath)
{
	wchar_t wszFilePath[256] = L"";
	size_t convertedChars = 0;

	if (mbstowcs_s(&convertedChars, wszFilePath, 256, szHeightMapFilePath, _TRUNCATE) != 0)
	{
		return E_FAIL;
	}

	const _tchar* pTerrainHeightMapPath = wszFilePath;
	if (!m_pTerrain)
		return E_FAIL;

	CVIBuffer_Terrain* pVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pTerrain->Find_Component(TEXT("Com_VIBuffer")));
	if (!pVIBuffer)
		return E_FAIL;

	_float* pHeightData = pVIBuffer->Get_HeightData();
	_uint iNumVerticesX = pVIBuffer->Get_NumVerticesX();
	_uint iNumVerticesZ = pVIBuffer->Get_NumVerticesZ();

	if (!pHeightData || iNumVerticesX == 0 || iNumVerticesZ == 0)
	{
		Safe_Delete_Array(pHeightData);
		return E_FAIL;
	}

	_ulong iNumVertices = iNumVerticesX * iNumVerticesZ;

	_uint* pPixels = new _uint[iNumVertices];
	ZeroMemory(pPixels, sizeof(_uint) * iNumVertices);

	for (_uint i = 0; i < iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < iNumVerticesX; ++j)
		{
			_uint iIndex = i * iNumVerticesX + j;

			_float fHeightValue = pHeightData[iIndex] * 10.f;

			_uint ucHeight = static_cast<_uint>(fHeightValue);

			// R, G, B 채널에 동일한 높이 값을 설정
			pPixels[iIndex] = fHeightValue;
		}
	}

	// 3. 비트맵 파일 헤더를 설정합니다.
	BITMAPFILEHEADER fh{};
	fh.bfType = 0x4D42;
	fh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (iNumVerticesX * iNumVerticesZ * 4);
	fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER ih{};
	ih.biSize = sizeof(BITMAPINFOHEADER);
	ih.biWidth = iNumVerticesX;
	ih.biHeight = iNumVerticesZ;
	ih.biPlanes = 1;
	ih.biBitCount = 32; // 32비트
	ih.biCompression = BI_RGB; // 압축 없음
	ih.biSizeImage = iNumVerticesX * iNumVerticesZ * 4;

	HANDLE hFile = CreateFile(pTerrainHeightMapPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		Safe_Delete_Array(pPixels);
		Safe_Delete_Array(pHeightData);
		return E_FAIL;
	}

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwBytesWritten, nullptr);
	WriteFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwBytesWritten, nullptr);
	WriteFile(hFile, pPixels, ih.biSizeImage, &dwBytesWritten, nullptr);


	CloseHandle(hFile);
	Safe_Delete_Array(pPixels);
	Safe_Delete_Array(pHeightData);

	return S_OK;

}

//HRESULT CMapTool_Desert::Save_MaskMap(const _char* szFilePath)
//{
//	if (m_pMaskTexture2D == nullptr)
//	{
//		return E_FAIL;
//	}
//
//	// 1. 와이드 문자열 버퍼 선언
//	wchar_t wszFilePath[256] = L"";
//	size_t convertedChars = 0;
//
//	// 2. 멀티바이트 문자열 (char*)을 와이드 문자열 (wchar_t*)로 변환
//	// szFilePath의 내용을 wszFilePath 버퍼로 안전하게 변환합니다.
//	if (mbstowcs_s(&convertedChars, wszFilePath, 256, szFilePath, _TRUNCATE) != 0)
//	{
//		return E_FAIL;
//	}
//
//	if (m_pMaskTexture2D != nullptr)
//	{
//		if (FAILED(DirectX::SaveWICTextureToFile(m_pContext, m_pMaskTexture2D, GUID_ContainerFormatPng, wszFilePath)))
//			return E_FAIL;
//	}
//
//	return S_OK;
//}

//HRESULT CMapTool_Desert::Set_LoadMaskMap(const _char* szFilePath)
//{
//	// 기존 리소스 해제
//	Safe_Release(m_pMaskTexture2D);
//	Safe_Release(m_pMaskSRV);
//
//	// 1. 와이드 문자열 버퍼 선언 및 변환 (DirectX::Load 함수는 wchar_t* 경로를 받음)
//	wchar_t wszFilePath[256] = L"";
//	size_t convertedChars = 0;
//	if (mbstowcs_s(&convertedChars, wszFilePath, 256, szFilePath, _TRUNCATE) != 0)
//	{
//		return E_FAIL;
//	}
//
//	// 2. 파일에서 텍스처를 로드하여 GPU 렌더링용 ID3D11Texture2D 생성 (기본 Usage: D3D11_USAGE_DEFAULT)
//	ID3D11Resource* pLoadResource = nullptr;
//	ID3D11Texture2D* pRenderTexture = nullptr;
//
//	// DirectX Tool Kit의 LoadWICTextureFromFileName 함수를 사용하여 텍스처 로드 및 SRV 생성
//	// 여기서 pLoadResource는 내부적으로 생성된 ID3D11Texture2D 포인터입니다.
//	if (FAILED(DirectX::CreateWICTextureFromFile(m_pDevice, m_pContext, wszFilePath, &pLoadResource, &m_pMaskSRV)))
//	{
//		return E_FAIL;
//	}
//
//	// 3. 로드된 리소스에서 ID3D11Texture2D 포인터 가져오기
//	// 로드된 텍스처의 속성을 파악하고, GPU 리소스를 준비합니다.
//	if (pLoadResource)
//	{
//		// ID3D11Resource를 ID3D11Texture2D로 캐스팅
//		pRenderTexture = reinterpret_cast<ID3D11Texture2D*>(pLoadResource);
//	}
//	else
//	{
//		// 로드 실패 또는 캐스팅 실패 시 SRV와 pLoadResource를 정리하고 반환
//		Safe_Release(m_pMaskSRV);
//		return E_FAIL;
//	}
//
//	// 4. CPU 접근(편집)용 스테이징 Texture2D 생성 (m_pMaskTexture2D)
//	D3D11_TEXTURE2D_DESC RenderDesc{};
//	pRenderTexture->GetDesc(&RenderDesc); // 로드된 텍스처의 속성을 가져옴
//
//	D3D11_TEXTURE2D_DESC StageDesc = RenderDesc;
//	StageDesc.Usage = D3D11_USAGE_STAGING;
//	StageDesc.BindFlags = 0;
//	StageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
//	StageDesc.MiscFlags = 0;
//
//	if (FAILED(m_pDevice->CreateTexture2D(&StageDesc, nullptr, &m_pMaskTexture2D)))
//	{
//		Safe_Release(pLoadResource);
//		return E_FAIL;
//	}
//
//	// 5. GPU 텍스처의 내용을 CPU 스테이징 텍스처로 복사
//	// 이제 m_pMaskTexture2D를 Map/Unmap으로 편집할 수 있습니다.
//	m_pContext->CopyResource(m_pMaskTexture2D, pRenderTexture);
//
//	// pLoadResource는 로드된 GPU 텍스처이며, SRV 생성에 사용되었으므로 Safe_Release 처리
//	Safe_Release(pLoadResource);
//
//	// 로드 성공 알림
//	MessageBoxW(g_hWnd, L"마스크 맵 로드 성공.", L"알림", MB_OK);
//
//	return S_OK;
//}

//HRESULT CMapTool_Desert::Set_NewMaskMap()
//{
//	Safe_Release(m_pMaskTexture2D);
//	Safe_Release(m_pMaskSRV);
//
//	D3D11_TEXTURE2D_DESC		TextureDesc{};
//	TextureDesc.Width = 512;
//	TextureDesc.Height = 512;
//	TextureDesc.MipLevels = 1;
//	TextureDesc.ArraySize = 1;
//	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//
//	TextureDesc.SampleDesc.Quality = 0;
//	TextureDesc.SampleDesc.Count = 1;
//	TextureDesc.Usage = D3D11_USAGE_STAGING;
//	TextureDesc.BindFlags = 0;
//	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
//	TextureDesc.MiscFlags = 0;
//
//	_uint* pPixel = new _uint[512 * 512];
//
//	for (size_t i = 0; i < 512; i++)
//	{
//		for (size_t j = 0; j < 512; j++)
//		{
//			_uint iIndex = i * 512 + j;
//			pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
//		}
//	}
//
//	D3D11_SUBRESOURCE_DATA		InitialDesc{};
//	InitialDesc.pSysMem = pPixel;
//	InitialDesc.SysMemPitch = 512 * 4;
//
//	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialDesc, &m_pMaskTexture2D)))
//		return E_FAIL;
//
//	// 2. GPU 렌더링용 Texture2D 생성
//	D3D11_TEXTURE2D_DESC RenderTextureDesc = TextureDesc;
//	RenderTextureDesc.Usage = D3D11_USAGE_DEFAULT;				// GPU 기본 사용
//	RenderTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// 쉐이더 리소스로 바인드
//	RenderTextureDesc.CPUAccessFlags = 0;						// CPU 접근 불가
//
//	ID3D11Texture2D* pRenderTexture = { nullptr };
//	if (FAILED(m_pDevice->CreateTexture2D(&RenderTextureDesc, nullptr, &pRenderTexture)))
//		return E_FAIL;
//
//	// 3. Shader Resource View 생성
//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
//	SRVDesc.Format = RenderTextureDesc.Format;
//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	SRVDesc.Texture2D.MipLevels = 1;
//
//	if (FAILED(m_pDevice->CreateShaderResourceView(pRenderTexture, &SRVDesc, &m_pMaskSRV)))
//	{
//		Safe_Release(pRenderTexture);
//		return E_FAIL;
//	}
//
//	m_pContext->CopyResource(pRenderTexture, m_pMaskTexture2D);
//
//	Safe_Release(pRenderTexture);
//	Safe_Delete_Array(pPixel);
//
//	return S_OK;
//}

//void CMapTool_Desert::Change_MaskMap_Black(_float3 vPickedPoint)
//{
//	D3D11_MAPPED_SUBRESOURCE		SubResource{};
//
//	m_pContext->Map(m_pMaskTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);
//
//	//const _float fTerrainHalfSize = 256.f;
//	const _int iTexelSize = 512;
//	const _float fTexelSize = 512;
//
//	_int iCenterTexelX = static_cast<_int>(vPickedPoint.x);
//
//	_float fFlippedZ = fTexelSize - (vPickedPoint.z);
//	_int iCenterTexelY = static_cast<_int>(fFlippedZ);
//
//
//	_int iBrushRadius = static_cast<_int>(m_fRadius);
//
//	_uint iRowSize = SubResource.RowPitch / sizeof(_uint);
//
//	_int iStartX = max(0, iCenterTexelX - iBrushRadius);
//	_int iEndX = min(iTexelSize, iCenterTexelX + iBrushRadius);
//	_int iStartZ = max(0, iCenterTexelY - iBrushRadius);
//	_int iEndZ = min(iTexelSize, iCenterTexelY + iBrushRadius);
//
//	for (_int i = iStartZ; i < iEndZ; i++)
//	{
//		char* pRowStart = static_cast<char*>(SubResource.pData) + (i * SubResource.RowPitch);
//		_uint* pRow = reinterpret_cast<_uint*>(pRowStart);
//
//		for (_int j = iStartX; j < iEndX; j++)
//		{
//			// 픽셀 데이터 쓰기
//			pRow[j] = D3DCOLOR_ARGB(255, 0, 0, 0); // (MASK_BLACK일 경우)
//		}
//	}
//	m_pContext->Unmap(m_pMaskTexture2D, 0);
//
//	ID3D11Resource* pGpuResource = { nullptr };
//	m_pMaskSRV->GetResource(&pGpuResource);
//
//	if (pGpuResource)
//	{
//		m_pContext->CopyResource(pGpuResource, m_pMaskTexture2D);
//		Safe_Release(pGpuResource);
//	}
//}
//
//void CMapTool_Desert::Change_MaskMap_White(_float3 vPickedPoint)
//{
//	D3D11_MAPPED_SUBRESOURCE		SubResource{};
//
//	m_pContext->Map(m_pMaskTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);
//
//	//const _float fTerrainHalfSize = 256.f;
//	const _int iTexelSize = 512;
//	const _float fTexelSize = 512;
//
//	_int iCenterTexelX = static_cast<_int>(vPickedPoint.x);
//
//	_float fFlippedZ = fTexelSize - (vPickedPoint.z);
//	_int iCenterTexelY = static_cast<_int>(fFlippedZ);
//
//
//	_int iBrushRadius = static_cast<_int>(m_fRadius);
//
//	_uint iRowSize = SubResource.RowPitch / sizeof(_uint);
//
//	_int iStartX = max(0, iCenterTexelX - iBrushRadius);
//	_int iEndX = min(iTexelSize, iCenterTexelX + iBrushRadius);
//	_int iStartZ = max(0, iCenterTexelY - iBrushRadius);
//	_int iEndZ = min(iTexelSize, iCenterTexelY + iBrushRadius);
//
//	for (_int i = iStartZ; i < iEndZ; i++)
//	{
//		for (_int j = iStartX; j < iEndX; j++)
//		{
//			//_float fDistSq = (i - iCenterTexelY) * (i - iCenterTexelY) + (j - iCenterTexelX) * (j - iCenterTexelX);
//
//			/*if (fDistSq <= m_fRadius * m_fRadius)
//			{
//				_uint* pRow = static_cast<_uint*>(SubResource.pData) + (i * iRowSize);
//				pRow[j] = D3DCOLOR_ARGB(255, 255, 255, 255);
//			}*/
//			_uint* pRow = static_cast<_uint*>(SubResource.pData) + (i * iRowSize);
//			pRow[j] = D3DCOLOR_ARGB(255, 255, 255, 255);
//		}
//	}
//	m_pContext->Unmap(m_pMaskTexture2D, 0);
//
//	// m_pMaskSRV 생성할떄 사용했던 원본ID3D11Texture2D를 다시 가져오거나
//	// MapTool_Desert에서 m_pMaskSRV의 텍스처 포인터를 저장했다면 그것을 사용
//	ID3D11Resource* pGpuResource = { nullptr };
//	m_pMaskSRV->GetResource(&pGpuResource);
//
//	if (pGpuResource)
//	{
//		m_pContext->CopyResource(pGpuResource, m_pMaskTexture2D);
//		Safe_Release(pGpuResource);
//	}
//}

void CMapTool_Desert::Set_NaviEditMode(_bool bMode)
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

void CMapTool_Desert::Add_NaviPoint(_fvector vPickedPoint)
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

void CMapTool_Desert::Reset_NaviPoints()
{
	m_iNaviPointCount = 0;
	m_eNaviMode = NAVI_MODE::ADD_POINT;
}

HRESULT CMapTool_Desert::Add_NaviCell()
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

HRESULT CMapTool_Desert::Save_NavigationData()
{
	if (!m_pNavigation)
		return E_FAIL;

	const vector<class Engine::CCell*>* pCells = m_pNavigation->Get_Cells();
	if (pCells->empty())
	{
		OutputDebugStringW(L"No Navigation Cells to save.\n");
		return S_OK;
	}

	std::ofstream ofs("../Bin/DataFiles/Navigation_Desert.bin", std::ios::binary);
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

void CMapTool_Desert::Delete_Latest_NaviCell()
{
	if (m_pNavigation)
	{
		m_pNavigation->Delete_Line();
		m_pNavigation->SetUp_Neighbors();
		OutputDebugStringW(L"Latest Navigation Cell Deleted.\n");
	}
}

CGameObject* CMapTool_Desert::Find_Object_To_Pick(_uint iLevelIndex, const _wstring& strLayerTag, _float3* pPickedPoint)
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

void CMapTool_Desert::Compute_Picking_Ray(_float3* pRayOrigin, _float3* pRayDir)
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

_bool CMapTool_Desert::Intersect_Ray_Sphere(_fvector vRayOrigin, _fvector vRayDir, _fvector vSphereCenter, _float fRadius, _float* pDistance)
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



void CMapTool_Desert::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTexture2D);
}
