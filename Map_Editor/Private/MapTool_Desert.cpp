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
#include "Interaction.h"
#include "Instance_Desert.h"
#include "Lift_Controller.h"
#include "Lift_Platform.h"
#include "Npc.h"
#include "Dororong_Saber.h"
#include "SpawnBox.h"

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

				CInteraction::Prob_Interaction_Desc pInteractionDesc = {};
				pInteractionDesc.iInteractionID = 0;

				CLift_Controller::LIFT_CONTROLLER_DESC pLiftControllerDesc = {};
				CLift_Platform::LIFT_PLATFORM_DESC pLiftPlatformDesc = {};

				CSpawnBox::MONSTER_DESC MonsterDesc = {};
				CNpc::NPC_DESC NpcDesc = {};

				switch (m_eCurrentObject)
				{
#pragma region RuinBuilding
				case DESESRT_RUIN_OBJECT::DOOR_A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_A");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_B");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_C");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_D");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_E:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_E");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_F:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_F");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_G:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_G");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_H:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_H");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_I:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_I");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_J:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_J");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_N:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_N");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_P:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_P");
					break;
				case DESESRT_RUIN_OBJECT::DOOR_R:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_R");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR_A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor_A");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR_B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor_B");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR_C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor_C");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR_D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor_D");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR_E:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor_E");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR_F:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor_F");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR_G:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor_G");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR_H:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor_H");
					break;
				case DESESRT_RUIN_OBJECT::FRAME_A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Frame_A");
					break;
				case DESESRT_RUIN_OBJECT::FRAME_B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Frame_B");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_A");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_B");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_C");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_D");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_BUILDING_A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_Building_A");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_BUILDING_B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_Building_B");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_BUILDING_C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_Building_C");
					break;
				case DESESRT_RUIN_OBJECT::STONE009_A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Stone009_A");
					break;
				case DESESRT_RUIN_OBJECT::STONE009_B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Stone009_B");
					break;
				case DESESRT_RUIN_OBJECT::WALL_A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_A");
					break;
				case DESESRT_RUIN_OBJECT::WALL_B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_B");
					break;
				case DESESRT_RUIN_OBJECT::WALL_C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_C");
					break;
				case DESESRT_RUIN_OBJECT::WALL_D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_D");
					break;
				case DESESRT_RUIN_OBJECT::WALL_E:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_E");
					break;
				case DESESRT_RUIN_OBJECT::WALL_F:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_F");
					break;
				case DESESRT_RUIN_OBJECT::WALL_G:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_G");
					break;
				case DESESRT_RUIN_OBJECT::WALL_H:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_H");
					break;
				case DESESRT_RUIN_OBJECT::WALL_I:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall_I");
					break;
				case DESESRT_RUIN_OBJECT::WALL007_A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall007_A");
					break;
				case DESESRT_RUIN_OBJECT::WALL007_B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall007_B");
					break;
				case DESESRT_RUIN_OBJECT::WALL007_C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall007_C");
					break;
				case DESESRT_RUIN_OBJECT::WALL007_D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall007_D");
					break;
				case DESESRT_RUIN_OBJECT::WALL007_E:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall007_E");
					break;
				case DESESRT_RUIN_OBJECT::WALL007_F:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall007_F");
					break;
				case DESESRT_RUIN_OBJECT::WALL007_G:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wall007_G");
					break;
#pragma endregion

#pragma region CanyonObjects
				case DESESRT_RUIN_OBJECT::CANYON_1A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_1A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_2A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_2A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_3A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_3A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_4A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_4A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_5A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_5A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_6A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_6A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_12A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_12A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_14A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_14A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_14B:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_14B");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_15A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_15A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_16A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_16A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_16B:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_16B");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_17A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_17A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_17B:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_17B");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_18A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_18A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_20A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_20A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_21A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_21A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_22A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_22A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_23A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_23A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_24A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_24A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_35A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_35A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_39A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_39A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_39C:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_39C");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_39D:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_39D");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_43A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_43A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_44A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_44A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_46A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_46A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_50A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_50A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_52A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_52A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_55A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_55A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_58A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_58A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_59A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_59A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_60A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_60A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_61A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_61A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_65A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_65A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_66A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_66A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_67A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_67A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_69A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_69A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_71A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_71A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_80A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_80A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_81A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_81A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_93A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_93A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_95A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_95A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_96A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_96A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_97A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_97A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_98A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_98A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_100A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_100A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_101A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_101A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_103A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_103A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_104A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_104A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_105A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_105A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_106A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_106A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_108A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_108A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_109A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_109A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_110A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_110A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_111A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_111A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_112A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_112A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_113A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_113A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_115A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_115A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_116A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_116A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_117A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_117A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_121A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_121A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_122A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_122A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_123A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_123A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_125A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_125A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_126A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_126A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_127A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_127B:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127B");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_127C:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127C");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_127D:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127D");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_127E:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_127E");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_128A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_128A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_128B:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_128B");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_128C:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_128C");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_131A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_131A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_132A:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_132A");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_132B:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_132B");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_132C:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_132C");
					break;
				case DESESRT_RUIN_OBJECT::CANYON_133B:
					protoTag = TEXT("Prototype_GameObject_Canyon"); layerTag = TEXT("Layer_Canyon");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Canyon_133B");
					break;
#pragma endregion
#pragma region LiftObjects
				case DESESRT_RUIN_OBJECT::LIFT_BODY:
					protoTag = TEXT("Prototype_GameObject_Lift_Body"); layerTag = TEXT("Layer_Lift_Body");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Lift_Body");

					break;
				case DESESRT_RUIN_OBJECT::LIFT_CONTROLLER:
					protoTag = TEXT("Prototype_GameObject_Lift_Controller"); layerTag = TEXT("Layer_Lift_Controller");
					pLiftControllerDesc.pComponentTag = TEXT("Prototype_Component_Model_Lift_Controller");
					pLiftControllerDesc.iInteractionID = 1;
					pLiftControllerDesc.bIsControllerType = m_bIsLiftControllerType;
					pLiftControllerDesc.iPlatformID = m_iPlatformID;
					pLiftControllerDesc.iPosition = m_iPosition;
					break;
				case DESESRT_RUIN_OBJECT::LIFT_PLATFORM:
					protoTag = TEXT("Prototype_GameObject_Lift_Platform"); layerTag = TEXT("Layer_Lift_Platform");
					pLiftPlatformDesc.pComponentTag = TEXT("Prototype_Component_Model_Lift_Platform");
					pLiftPlatformDesc.iPlatFormID = m_iPlatformID;	pLiftPlatformDesc.fMoveDistance = m_fPlatformMoveDistance;
					break;
#pragma endregion
#pragma region Floor Objects
				case DESESRT_RUIN_OBJECT::FLOOR7_A:
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_A");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR7_B:
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_B");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR7_C:
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_C");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR7_D:
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_D");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR7_E:
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_E");
					break;
				case DESESRT_RUIN_OBJECT::FLOOR7_F:
					protoTag = TEXT("Prototype_GameObject_Iron_Floor"); layerTag = TEXT("Layer_Iron_Floor");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Floor7_F");
					break;
#pragma endregion
#pragma region Camp/Misc Objects
				case DESESRT_RUIN_OBJECT::LAMP_47A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Lamp_47A");
					break;
				case DESESRT_RUIN_OBJECT::VendingMachine_6A:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_VendingMachine_6A");
					pInteractionDesc.iInteractionID = 3;
					break;
				case DESESRT_RUIN_OBJECT::VendingMachine_7A:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_VendingMachine_7A");
					pInteractionDesc.iInteractionID = 3;
					break;
				case DESESRT_RUIN_OBJECT::POSTER_4D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_4D");
					break;
				case DESESRT_RUIN_OBJECT::BASE_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Base_1A");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1B");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1D");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1E:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1E");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1F:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1F");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1G:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1G");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1H:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1H");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1I:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1I");
					pInteractionDesc.iInteractionID = 2;
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1J:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1J");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1K:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1K");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1L:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1L");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1M:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1M");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1N:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1N");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1R:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1R");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1S:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1S");
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1T:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1T");
					pInteractionDesc.iInteractionID = 3;
					break;
				case DESESRT_RUIN_OBJECT::CAMP_1W:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Camp_1W");
					break;
				case DESESRT_RUIN_OBJECT::REPAIRCONSOLE:
					protoTag = TEXT("Prototype_GameObject_RepairConsole"); layerTag = TEXT("Layer_RepairConsole");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_RepairConsole");
					pInteractionDesc.iInteractionID = 999;
					break;
				case DESESRT_RUIN_OBJECT::TOP_ROOF:
					protoTag = TEXT("Prototype_GameObject_Top_Roof"); layerTag = TEXT("Layer_Top_Roof");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Top_Roof");
					pInteractionDesc.iInteractionID = 999;
					break;
#pragma endregion
#pragma region Corpse/Container Objects
				case DESESRT_RUIN_OBJECT::CORPSE_1A:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Corpse_1A");
					pInteractionDesc.iInteractionID = 6;
					break;
				case DESESRT_RUIN_OBJECT::CORPSE_1B:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Corpse_1B");
					pInteractionDesc.iInteractionID = 6;
					break;
				case DESESRT_RUIN_OBJECT::CORPSE_2A:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Corpse_2A");
					pInteractionDesc.iInteractionID = 6;
					break;
				case DESESRT_RUIN_OBJECT::CORPSE_2B:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Corpse_2B");
					pInteractionDesc.iInteractionID = 6;
					break;
				case DESESRT_RUIN_OBJECT::CORPSE_2C:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Corpse_2C");
					pInteractionDesc.iInteractionID = 6;
					break;
				case DESESRT_RUIN_OBJECT::CORPSE_3A:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Corpse_3A");
					pInteractionDesc.iInteractionID = 6;
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_2A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_2A");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_2B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_2B");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_2C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_2C");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_2D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_2D");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_3A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_3A");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_4B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_4B");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_5A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_5A");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_5B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_5B");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_5C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_5C");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_5D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_5D");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_5E:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_5E");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_7B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_7B");
					break;
				case DESESRT_RUIN_OBJECT::CONTAINER_7F:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Container_7F");
					break;
				case DESESRT_RUIN_OBJECT::SCIFI_DOOR:
					protoTag = TEXT("Prototype_GameObject_SciFi_Door"); layerTag = TEXT("Layer_SciFi_Door");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_SciFi_Door");
					pInteractionDesc.iInteractionID = 4;
					break;
#pragma endregion
#pragma region Desert Ruin, Canbox, fences, buildings, wheels, bridges, vehicle
				case DESESRT_RUIN_OBJECT::CANBOX:
					protoTag = TEXT("Prototype_GameObject_CanBox"); layerTag = TEXT("Layer_CanBox");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_CanBox");
					pInteractionDesc.iInteractionID = 5;
					break;
				case DESESRT_RUIN_OBJECT::FENCE_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Fence_1A");
					break;
				case DESESRT_RUIN_OBJECT::FENCE_1B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Fence_1B");
					break;
				case DESESRT_RUIN_OBJECT::FENCE_1F:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Fence_1F");
					break;
				case DESESRT_RUIN_OBJECT::FENCE_1H:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Fence_1H");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_7A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_7A");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_7B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_7B");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_7C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_7C");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_7D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_7D");
					break;
				case DESESRT_RUIN_OBJECT::RUIN_7E:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Ruin_7E");
					break;
				case DESESRT_RUIN_OBJECT::BUILDING_4B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Building_4B");
					break;
				case DESESRT_RUIN_OBJECT::BUILDING_4C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Building_4C");
					break;
				case DESESRT_RUIN_OBJECT::BUILDING_4D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Building_4D");
					break;
				case DESESRT_RUIN_OBJECT::WHEEL_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wheel_1A");
					break;
				case DESESRT_RUIN_OBJECT::WHEEL_1B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wheel_1B");
					break;
				case DESESRT_RUIN_OBJECT::WHEEL_1C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Wheel_1C");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_4A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_4A");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_4B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_4B");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_4D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_4D");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_4H:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_4H");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_4L:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_4L");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_4M:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_4M");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_4N:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_4N");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_5:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_5");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_6:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_6");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_8:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_8");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_14A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_14A");
					break;
				case DESESRT_RUIN_OBJECT::BRIDGE_14B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Bridge_14B");
					break;
				case DESESRT_RUIN_OBJECT::VEHICLE_2A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Vehicle_2A");
					break;
				case DESESRT_RUIN_OBJECT::VEHICLE_2B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Vehicle_2B");
					break;
				case DESESRT_RUIN_OBJECT::VEHICLE_3B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Vehicle_3B");
					break;
				case DESESRT_RUIN_OBJECT::VEHICLE_4C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Vehicle_4C");
					break;
				case DESESRT_RUIN_OBJECT::VEHICLE_6A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Vehicle_6A");
					break;
				case DESESRT_RUIN_OBJECT::VEHICLE_8B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Vehicle_8B");
					break;
				case DESESRT_RUIN_OBJECT::VEHICLE_14A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Vehicle_14A");
					break;
				case DESESRT_RUIN_OBJECT::VEHICLE_14B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Vehicle_14B");
					break;
#pragma endregion

#pragma region Crane, Trash, Sign
				case DESESRT_RUIN_OBJECT::CRANE_1C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Crane_1C");
					break;
				case DESESRT_RUIN_OBJECT::CRANE_11:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Crane_11");
					break;
				case DESESRT_RUIN_OBJECT::CRANE_13:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Crane_13");
					break;
				case DESESRT_RUIN_OBJECT::TRASH_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Trash_1A");
					break;
				case DESESRT_RUIN_OBJECT::TRASH_2A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Trash_2A");
					break;
				case DESESRT_RUIN_OBJECT::TRASH_2B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Trash_2B");
					break;
				case DESESRT_RUIN_OBJECT::TRASH_4A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Trash_4A");
					break;
				case DESESRT_RUIN_OBJECT::TRASH_9A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Trash_9A");
					break;
				case DESESRT_RUIN_OBJECT::TRASH_17A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Trash_17A");
					break;
				case DESESRT_RUIN_OBJECT::SIGN_11A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Sign_11A");
					break;
				case DESESRT_RUIN_OBJECT::SIGN_11B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Sign_11B");
					break;
				case DESESRT_RUIN_OBJECT::SIGN_11F:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Sign_11F");
					break;
				case DESESRT_RUIN_OBJECT::SIGN_12B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Sign_12B");
					break;
				case DESESRT_RUIN_OBJECT::SIGN_12C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Sign_12C");
					break;
				case DESESRT_RUIN_OBJECT::SIGN_36B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Sign_36B");
					break;
#pragma endregion

#pragma region Tree, Grass
				case DESESRT_RUIN_OBJECT::TREE_1A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_1A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_2A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_2A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_3A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_3A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_4A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_4A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_5B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_5B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_5C:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_5C");
					break;
				case DESESRT_RUIN_OBJECT::TREE_6A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_6A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_6B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_6B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_7A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_7A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_7B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_7B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_8A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_8A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_8Aa:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_8Aa");
					break;
				case DESESRT_RUIN_OBJECT::TREE_8B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_8B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_8Ba:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_8Ba");
					break;

				case DESESRT_RUIN_OBJECT::TREE_10A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_10A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_11B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_11B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_15A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_15A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_15B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_15B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_16A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_16A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_17A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_17A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_18A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_18A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_19A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_19A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_20A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_20A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_21A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_21A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_23A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_23A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_25A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_25A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_26A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_26A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_27A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_27A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_29A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_29A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_30A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_30A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_31A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_31A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_32A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_32A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_33A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_33A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_34A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_34A");
					break;

				case DESESRT_RUIN_OBJECT::TREE_40B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_40C:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40C");
					break;
				case DESESRT_RUIN_OBJECT::TREE_40D:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40D");
					break;
				case DESESRT_RUIN_OBJECT::TREE_40E:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40E");
					break;
				case DESESRT_RUIN_OBJECT::TREE_40F:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40F");
					break;
				case DESESRT_RUIN_OBJECT::TREE_40G:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40G");
					break;
				case DESESRT_RUIN_OBJECT::TREE_40H:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40H");
					break;
				case DESESRT_RUIN_OBJECT::TREE_40I:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40I");
					break;
				case DESESRT_RUIN_OBJECT::TREE_40J:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_40J");
					break;

				case DESESRT_RUIN_OBJECT::TREE_42A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_42A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_43A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_43A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_43B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_43B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_44A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_44A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_44B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_44B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_44D:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_44D");
					break;
				case DESESRT_RUIN_OBJECT::TREE_44F:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_44F");
					break;
				case DESESRT_RUIN_OBJECT::TREE_45A:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_45A");
					break;
				case DESESRT_RUIN_OBJECT::TREE_45B:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_45B");
					break;
				case DESESRT_RUIN_OBJECT::TREE_45C:
					protoTag = TEXT("Prototype_GameObject_Desert_Tree"); layerTag = TEXT("Layer_Desert_Tree");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Tree_45C");
					break;
				case DESESRT_RUIN_OBJECT::DEADSHRUBS_A:
					protoTag = TEXT("Prototype_GameObject_Desert_Grass"); layerTag = TEXT("Layer_Desert_Grass");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_DeadShrubs_A");
					break;
				case DESESRT_RUIN_OBJECT::DEADSHRUBS_B:
					protoTag = TEXT("Prototype_GameObject_Desert_Grass"); layerTag = TEXT("Layer_Desert_Grass");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_DeadShrubs_B");
					break;
#pragma endregion

#pragma region Box_Objects
				case DESESRT_RUIN_OBJECT::BOX_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_1A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_1B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_1B");
					break;
				case DESESRT_RUIN_OBJECT::BOX_2C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_2C");
					break;
				case DESESRT_RUIN_OBJECT::BOX_4A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_4A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_5A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_5A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_6A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_6A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_11A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_11A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_13A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_13A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_14A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_14A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_16A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_16A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_16B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_16B");
					break;
				case DESESRT_RUIN_OBJECT::BOX_19A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_19A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_20A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_20A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_20B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_20B");
					break;
				case DESESRT_RUIN_OBJECT::BOX_20C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_20C");
					break;
				case DESESRT_RUIN_OBJECT::BOX_20D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_20D");
					break;
				case DESESRT_RUIN_OBJECT::BOX_20E:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_20E");
					break;
				case DESESRT_RUIN_OBJECT::BOX_20F:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_20F");
					break;
				case DESESRT_RUIN_OBJECT::BOX_21A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_21A");
					break;
				case DESESRT_RUIN_OBJECT::BOX_21B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_21B");
					break;
				case DESESRT_RUIN_OBJECT::BOX_26A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Box_26A");
					break;
				case DESESRT_RUIN_OBJECT::ITEM_BOX:
					protoTag = TEXT("Prototype_GameObject_DisplayBox"); layerTag = TEXT("Layer_DisplayBox");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_DisplayBox");
					pInteractionDesc.iInteractionID = 9;
					break;

#pragma endregion
#pragma region Poster, Garden, RestRoom, Statue
				case DESESRT_RUIN_OBJECT::GARDEN_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Garden_1A");
					break;
				case DESESRT_RUIN_OBJECT::GARDEN_1B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Garden_1B");
					break;
				case DESESRT_RUIN_OBJECT::GARDEN_1C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Garden_1C");
					break;
				case DESESRT_RUIN_OBJECT::RESTROOM_4A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Restroom_4A");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_1A");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_2A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_2A");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_2B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_2B");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_3A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_3A");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_3B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_3B");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_4A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_4A");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_4B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_4B");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_4C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_4C");
					break;
				case DESESRT_RUIN_OBJECT::POSTER_4E:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Poster_4E");
					break;
				case DESESRT_RUIN_OBJECT::STATUE_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Statue_1A");
					break;
				case DESESRT_RUIN_OBJECT::STATUE_19B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Statue_19B");
					break;
				case DESESRT_RUIN_OBJECT::STATUE_24B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Statue_24B");
					break;
				case DESESRT_RUIN_OBJECT::STATUE_31B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Statue_31B");
					break;
				case DESESRT_RUIN_OBJECT::STATUE_40B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Statue_40B");
					break;
#pragma endregion
#pragma region Duct
				case DESESRT_RUIN_OBJECT::DUCT_1A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_1A");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_1B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_1B");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_1C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_1C");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_1D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_1D");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_1E:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_1E");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_1G:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_1G");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_1H:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_1H");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_3A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_3A");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_3B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_3B");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_3C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_3C");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_4A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_4A");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_4B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_4B");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_4C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_4C");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_6A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_6A");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_8A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_8A");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_9A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_9A");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_9B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_9B");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_9C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_9C");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_9D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_9D");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_10A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_10A");
					break;

				case DESESRT_RUIN_OBJECT::DUCT_13A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Duct_13A");
					break;
#pragma endregion
#pragma region Furniture
				case DESESRT_RUIN_OBJECT::FURNITURE_7A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_7A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_9A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_9A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_47A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_47A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_50A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_50A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_50B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_50B");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_50C:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_50C");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_50D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_50D");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_51A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_51A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_57A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_57A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_59A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_59A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_77A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_77A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_79A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_79A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_83A:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_83A");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_87B:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_87B");
					break;

				case DESESRT_RUIN_OBJECT::FURNITURE_87D:
					protoTag = TEXT("Prototype_GameObject_Deco"); layerTag = TEXT("Layer_Deco");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Furniture_87D");
					break;
#pragma endregion

#pragma region Xion_Building
				case DESESRT_RUIN_OBJECT::XION_BUILDING_3A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building_3A");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING_4A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building_4A");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING_5A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building_5A");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING_6A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building_6A");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING_8:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building_8");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING_9A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building_9A");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING_17:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building_17");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING4_1A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building4_1A");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING4_1B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building4_1B");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING4_1C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building4_1C");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING4_1D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building4_1D");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING4_1E:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building4_1E");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING4_1F:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building4_1F");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING5_4:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building5_4");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING5_callD4:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building5_callD4");
					break;

				case DESESRT_RUIN_OBJECT::XION_BUILDING5_callD5:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Building5_callD5");
					break;

				case DESESRT_RUIN_OBJECT::STORE_3:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Store_3");
					break;

				case DESESRT_RUIN_OBJECT::STORE_4:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Store_4");
					break;
#pragma endregion
#pragma region Stair
				case DESESRT_RUIN_OBJECT::STAIR_1A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Stair_1A");
					break;

				case DESESRT_RUIN_OBJECT::STAIR_1B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Stair_1B");
					break;

				case DESESRT_RUIN_OBJECT::STAIR_1C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Stair_1C");
					break;

				case DESESRT_RUIN_OBJECT::STAIR_2A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Stair_2A");
					break;

				case DESESRT_RUIN_OBJECT::STAIR_3A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Stair_3A");
					break;

				case DESESRT_RUIN_OBJECT::STAIR_3B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Stair_3B");
					break;
#pragma endregion
#pragma region Xion_Wall
				case DESESRT_RUIN_OBJECT::XION_WALL_1C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1C");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1D");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1Db:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1Db");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1G:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1G");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1I:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1I");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1K:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1K");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1Q:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1Q");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1R:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1R");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1S:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1S");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_1T:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_1T");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_2A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_2A");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_2B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_2B");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_4H:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_4H");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_5A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_5A");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_8A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_8A");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_9A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_9A");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_9B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_9B");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_10A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_10A");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_10B:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_10B");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_10C:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_10C");
					break;

				case DESESRT_RUIN_OBJECT::XION_WALL_10D:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Xion_Wall_10D");
					break;
#pragma endregion
#pragma region Door
				case DESESRT_RUIN_OBJECT::DOOR_CONTROLLER:
					protoTag = TEXT("Prototype_GameObject_Interaction_NonAnim"); layerTag = TEXT("Layer_Interaction");
					pInteractionDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_Controller");
					pInteractionDesc.iInteractionID = 10;
					break;

				case DESESRT_RUIN_OBJECT::DOOR_SHUTTER:
					protoTag = TEXT("Prototype_GameObject_Shutter"); layerTag = TEXT("Layer_Shutter");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Shutter");
					break;

				case DESESRT_RUIN_OBJECT::DOOR_3A:
					protoTag = TEXT("Prototype_GameObject_Architecture"); layerTag = TEXT("Layer_Architecture");
					pDesc.pComponentTag = TEXT("Prototype_Component_Model_Door_3A");
					break;
#pragma endregion

#pragma region Monster
				case DESESRT_RUIN_OBJECT::GORILLA:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_Gorilla"); MonsterDesc.iMonsterID = 1;
					break;
				case DESESRT_RUIN_OBJECT::BEHOLDER:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_Beholder"); MonsterDesc.iMonsterID = 2;
					break;
				case DESESRT_RUIN_OBJECT::BARNACLE_A:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_BanacleA"); MonsterDesc.iMonsterID = 3;
					break;
				case DESESRT_RUIN_OBJECT::STATUE_A:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_StatueA"); MonsterDesc.iMonsterID = 4;
					break;
				case DESESRT_RUIN_OBJECT::STATUE_B:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_StatueB"); MonsterDesc.iMonsterID = 5;
					break;
				case DESESRT_RUIN_OBJECT::SUNFLOWER:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_SunFlower"); MonsterDesc.iMonsterID = 6;
					break;
				case DESESRT_RUIN_OBJECT::ANTLION:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_Minion11"); MonsterDesc.iMonsterID = 7;
					break;
				case DESESRT_RUIN_OBJECT::TENTACLE:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_Tentacle"); MonsterDesc.iMonsterID = 9;
					break;
				case DESESRT_RUIN_OBJECT::DROIDTURRET:
					protoTag = TEXT("Prototype_GameObject_SpawnBox"); layerTag = TEXT("Layer_Monster"); pDesc.pComponentTag == nullptr;
					MonsterDesc.pComponentTag = TEXT("Prototype_Component_Model_DroidTurret"); MonsterDesc.iMonsterID = 10;
					break;
#pragma endregion
#pragma region NPC
				case DESESRT_RUIN_OBJECT::NPC_SCARLET:
					protoTag = TEXT("Prototype_GameObject_Npc"); layerTag = TEXT("Layer_Npc");
					NpcDesc.pComponentTag = TEXT("Prototype_Component_Model_Scarlet_Body"); NpcDesc.iNpcID = 1;
					break;
				case DESESRT_RUIN_OBJECT::NPC_SHOP:
					protoTag = TEXT("Prototype_GameObject_Npc"); layerTag = TEXT("Layer_Npc");
					NpcDesc.pComponentTag = TEXT("Prototype_Component_Model_D1G-g2r_Body"); NpcDesc.iNpcID = 4;
					break;

				case DESESRT_RUIN_OBJECT::NPC_DORORONG:
					protoTag = TEXT("Prototype_GameObject_Npc"); layerTag = TEXT("Layer_Npc");
					NpcDesc.pComponentTag = TEXT("Prototype_Component_Model_Cinematic_Dororong"); NpcDesc.iNpcID = 5;
					break;
#pragma endregion 

#pragma region Dororong SABER
				case DESESRT_RUIN_OBJECT::PAD:
					protoTag = TEXT("Prototype_GameObject_Pad"); layerTag = TEXT("Layer_Pad");
					pDesc.pComponentTag == nullptr;
					break;
				case DESESRT_RUIN_OBJECT::RAIL:
					protoTag = TEXT("Prototype_GameObject_Rail"); layerTag = TEXT("Layer_Rail");
					pDesc.pComponentTag == nullptr;
					break;
				case DESESRT_RUIN_OBJECT::INDICATOR:
					protoTag = TEXT("Prototype_GameObject_Beat_Indicator"); layerTag = TEXT("Layer_Beat_Indicator");
					pDesc.pComponentTag == nullptr;
					break;

				case DESESRT_RUIN_OBJECT::DORORONG_BOX:
					protoTag = TEXT("Prototype_GameObject_DororongBox"); layerTag = TEXT("Layer_DororongBox");
					pDesc.pComponentTag == nullptr;
					break;
#pragma endregion

#pragma region Terrain
				case DESESRT_RUIN_OBJECT::TERRAIN_DECREASE_RECT:
					if (fHeight > 0)
						fHeight *= -1.f;
					m_pTerrain->Change_Height_Rect(vPickPoint, fHeight, m_fRadius);
					break;
				case DESESRT_RUIN_OBJECT::TERRAIN_INCREASE_RECT:
					if (fHeight < 0)
						fHeight;
					m_pTerrain->Change_Height_Rect(vPickPoint, fHeight, m_fRadius);
					break;
				case DESESRT_RUIN_OBJECT::TERRAIN_FLAT:
					m_pTerrain->Change_Height_Flat(vPickPoint, fHeight, m_fRadius);
					break;
				case DESESRT_RUIN_OBJECT::PLAYER:
					m_pPlayerTransform->Set_State(STATE::POSITION, vPickPoint);
					break;
				}
#pragma endregion
				if (m_eCurrentObject == DESESRT_RUIN_OBJECT::LIFT_CONTROLLER)
				{
					hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, &pLiftControllerDesc);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::LIFT_PLATFORM)
				{
					hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, &pLiftPlatformDesc);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::NPC_SCARLET || m_eCurrentObject == DESESRT_RUIN_OBJECT::NPC_SHOP || m_eCurrentObject == DESESRT_RUIN_OBJECT::NPC_DORORONG)
				{
					hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, &NpcDesc);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::PAD || m_eCurrentObject == DESESRT_RUIN_OBJECT::RAIL || m_eCurrentObject == DESESRT_RUIN_OBJECT::INDICATOR || m_eCurrentObject == DESESRT_RUIN_OBJECT::DORORONG_BOX)
				{
					hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, nullptr);
				}
				else if (pInteractionDesc.iInteractionID == 0)
				{
					if(pDesc.pComponentTag == nullptr)
						hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, &MonsterDesc);
					else	
						hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, &pDesc);

				}
				else
				{
					hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), protoTag, ENUM_CLASS(LEVEL::DESERT), layerTag, &pInteractionDesc);
				}

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

#pragma region Another_Function
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

				if (m_eCurrentObject == DESESRT_RUIN_OBJECT::MASK_RED)
				{
					Change_MaskMap_Red(vPickedPoint);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::MASK_GREEN)
				{
					Change_MaskMap_Green(vPickedPoint);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::MASK_BLUE)
				{
					Change_MaskMap_Blue(vPickedPoint);
				}
				else if (m_eCurrentObject == DESESRT_RUIN_OBJECT::MASK_BLACK)
				{
					Change_MaskMap_Black(vPickedPoint);
				}
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
#pragma endregion

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
	if (ImGui::Button("MASK_MAP_BALCK"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::MASK_BLACK;
	}
	ImGui::SameLine();
	if (ImGui::Button("MASK_MAP_RED"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::MASK_RED;
	}
	if (ImGui::Button("MASK_MAP_GREEN"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::MASK_GREEN;
	}
	ImGui::SameLine();
	if (ImGui::Button("MASK_MAP_BLUE"))
	{
		m_eCurrentObject = DESESRT_RUIN_OBJECT::MASK_BLUE;
	}
	if (ImGui::Button("NEW_MASK_MAP"))
	{
		Set_NewMaskMap();
		m_pTerrain->Set_MapTool(this);
	}
	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();
	static _char szMaskLoadFilePath[256] = "../Bin/Resources/Maps/Desert/Terrain/TerrainMask.png";
	ImGui::InputText("MaskMap Load File Path", szMaskLoadFilePath, sizeof(szMaskLoadFilePath));
	if (ImGui::Button("Load_MASK"))
	{
		Set_LoadMaskMap(szMaskLoadFilePath);
		m_pTerrain->Set_MapTool(this);
	}
	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	static _char szMaskMapFilePath[256] = "../Bin/Resources/Maps/Desert/Terrain/TerrainMask.png";
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
	}

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
	const _char* themeNames[] = { "Building_Ruin", "ENVIRONMENT", "XION", "CANYON", "Archi", "Deco", "DORORONG_SABER"};
	_int nSelectedTheme = (_int)m_eCurrentMap;

	if (ImGui::Combo("Select Theme", &nSelectedTheme, themeNames, IM_ARRAYSIZE(themeNames)))
	{
		// 테마가 변경되면 현재 오브젝트 선택을 초기화할 수 있습니다.
		m_eCurrentObject = DESESRT_RUIN_OBJECT::END;
		m_eCurrentMap = (DESERT_THEME)nSelectedTheme;
	}

	_int nSelectedCharacter = -1;
	_int nSelectedNpc = -1;
	const _char* characterNames[] = { "Player", "Gorilla", "BEHOLDER", "BARNACLE_A", "STATUE_A", "STATUE_B", 
									  "SUNFLOWER", "ANTLION", "TENTACLE", "DROIDTURRET" };
	const _char* npcNames[] = { "NPC_SCARLET", "NPC_SHOP", "NPC_DORORONG"};

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
				m_eCurrentObject = DESESRT_RUIN_OBJECT::BEHOLDER;
				m_CurrentLayerName = TEXT("Layer_Monster");
			}
			else if (nSelectedCharacter == 3)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::BARNACLE_A;
				m_CurrentLayerName = TEXT("Layer_Monster");
			}
			else if (nSelectedCharacter == 4)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::STATUE_A;
				m_CurrentLayerName = TEXT("Layer_Monster");
			}
			else if (nSelectedCharacter == 5)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::STATUE_B;
				m_CurrentLayerName = TEXT("Layer_Monster");
			}
			else if (nSelectedCharacter == 6)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::SUNFLOWER;
				m_CurrentLayerName = TEXT("Layer_Monster");
			}
			else if (nSelectedCharacter == 7)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::ANTLION;
				m_CurrentLayerName = TEXT("Layer_Monster");
			}
			else if (nSelectedCharacter == 8)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::TENTACLE;
				m_CurrentLayerName = TEXT("Layer_Monster");
			}
			else if (nSelectedCharacter == 9)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::DROIDTURRET;
				m_CurrentLayerName = TEXT("Layer_Monster");
			}
		}
		if (ImGui::ListBox("##NPC", &nSelectedNpc, npcNames, IM_ARRAYSIZE(npcNames), 7))
		{
			if (nSelectedNpc == 0)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::NPC_SCARLET;
				m_CurrentLayerName = TEXT("Layer_Npc");
			}
			else if (nSelectedNpc == 1)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::NPC_SHOP;
				m_CurrentLayerName = TEXT("Layer_Npc");
			}
			else if (nSelectedNpc == 2)
			{
				m_eCurrentObject = DESESRT_RUIN_OBJECT::NPC_DORORONG;
				m_CurrentLayerName = TEXT("Layer_Npc");
			}
		}
	}

	if (m_eCurrentMap == DESERT_THEME::BUILDING_RUIN)
	{
		ImGui::Text("Desert Map Objects");
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
		// 2. 모델 배치 / 삭제메뉴
		_int nSelectedDoor = -1;
		_int nSelectedRuin = -1;
		_int nSelectedFrame = -1;
		_int nSelectedWall = -1;

		const _char* DoorNames[] = { "DOOR_A", "DOOR_B", "DOOR_C", "DOOR_D", "DOOR_E", "DOOR_F", "DOOR_G", "DOOR_H", "DOOR_I", "DOOR_J", "DOOR_N", "DOOR_P", "DOOR_R" };
		const _char* RuinNames[] = { "RUIN_A", "RUIN_B", "RUIN_C", "RUIN_D", "RUIN_BUILDING_A", "RUIN_BUILDING_B", "RUIN_BUILDING_C" };
		const _char* FrameNames[] = { "FLOOR_A", "FLOOR_B", "FLOOR_C", "FLOOR_D", "FLOOR_E", "FLOOR_F", "FLOOR_G", "FLOOR_H", "FRAME_A", "FRAME_B",	"STONE009_A", "STONE009_B" };
		const _char* WallNames[] = { "WALL_A", "WALL_B", "WALL_C", "WALL_D", "WALL_E", "WALL_F", "WALL_G", "WALL_H", "WALL_I",
									"WALL007_A", "WALL007_B", "WALL007_C", "WALL007_D", "WALL007_E", "WALL007_F", "WALL007_G" };

		if (ImGui::CollapsingHeader("Door"))
		{
			if (ImGui::ListBox("##Doors", &nSelectedDoor, DoorNames, IM_ARRAYSIZE(DoorNames), 7))
			{
				// 0 ~ 12 인덱스
				if (nSelectedDoor == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_E;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_F;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_G;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_H;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_I;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_J;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_N;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_P;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDoor == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_R;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}

		if (ImGui::CollapsingHeader("Ruin"))
		{
			if (ImGui::ListBox("##Ruins", &nSelectedRuin, RuinNames, IM_ARRAYSIZE(RuinNames), 7))
			{
				// 0 ~ 6 인덱스
				if (nSelectedRuin == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_BUILDING_A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_BUILDING_B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_BUILDING_C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}

		if (ImGui::CollapsingHeader("Frame & Floor"))
		{
			if (ImGui::ListBox("##Frames", &nSelectedFrame, FrameNames, IM_ARRAYSIZE(FrameNames), 7))
			{
				// 0 ~ 11 인덱스 (FLOOR_A ~ H, FRAME_A ~ B, STONE009_A ~ B)
				if (nSelectedFrame == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR_A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR_B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR_C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR_D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR_E;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR_F;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR_G;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR_H;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FRAME_A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FRAME_B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STONE009_A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedFrame == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STONE009_B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}

		if (ImGui::CollapsingHeader("Wall"))
		{
			if (ImGui::ListBox("##Walls", &nSelectedWall, WallNames, IM_ARRAYSIZE(WallNames), 7))
			{
				// 0 ~ 8 인덱스 (WALL_A ~ I)
				if (nSelectedWall == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_E;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_F;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_G;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_H;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL_I;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}

				// 9 ~ 15 인덱스 (WALL007_A ~ G)
				else if (nSelectedWall == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL007_A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL007_B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL007_C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL007_D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 13)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL007_E;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 14)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL007_F;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedWall == 15)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WALL007_G;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}

	}
	else if (m_eCurrentMap == DESERT_THEME::ENVIRONMENT)
	{
		ImGui::Text("Desert Common Objects");
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
		// 2. 모델 배치 / 삭제메뉴
		_int nSelectedDesertTree = -1;
		_int nSelectedShrub = -1;

		const _char* desertTreeNames[] = { "TREE_1A", "TREE_2A", "TREE_3A", "TREE_4A", "TREE_5B", "TREE_5C", "TREE_6A", "TREE_6B", "TREE_7A", "TREE_7B",
			"TREE_8A", "TREE_8Aa", "TREE_8B", "TREE_8Ba", "TREE_10A", "TREE_11B", "TREE_15A", "TREE_15B", "TREE_16A", "TREE_17A", "TREE_18A", "TREE_19A",
			"TREE_20A", "TREE_21A", "TREE_23A", "TREE_25A", "TREE_26A", "TREE_27A", "TREE_29A", "TREE_30A", "TREE_31A", "TREE_32A", "TREE_33A", "TREE_34A",
			"TREE_40B", "TREE_40C", "TREE_40D", "TREE_40E", "TREE_40F", "TREE_40G", "TREE_40H", "TREE_40I", "TREE_40J",
			"TREE_42A", "TREE_43A", "TREE_43B", "TREE_44A", "TREE_44B", "TREE_44D", "TREE_44F", "TREE_45A", "TREE_45B", "TREE_45C" };

		const _char* desertShrubNames[] = { "DEADSHRUBS_A", "DEAD_SHRUBS_B" };

		if (ImGui::CollapsingHeader("Desert_Trees"))
		{
			if (ImGui::ListBox("##Desert_Trees", &nSelectedDesertTree, desertTreeNames, IM_ARRAYSIZE(desertTreeNames), 7))
			{
				if (nSelectedDesertTree == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_1A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_2A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_3A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_4A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_5B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_5C;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_6A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_6B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_7A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_7B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_8A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_8Aa;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_8B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 13)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_8Ba;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 14)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_10A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 15)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_11B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 16)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_15A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 17)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_15B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 18)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_16A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 19)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_17A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 20)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_18A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 21)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_19A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 22)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_20A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 23)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_21A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 24)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_23A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 25)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_25A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 26)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_26A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 27)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_27A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 28)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_29A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 29)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_30A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 30)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_31A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 31)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_32A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 32)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_33A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 33)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_34A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 34)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 35)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40C;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 36)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40D;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 37)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40E;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 38)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40F;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 39)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40G;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 40)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40H;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 41)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40I;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 42)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_40J;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 43)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_42A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 44)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_43A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 45)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_43B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 46)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_44A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 47)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_44B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 48)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_44D;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 49)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_44F;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 50)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_45A;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 51)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_45B;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}
				else if (nSelectedDesertTree == 52)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TREE_45C;
					m_CurrentLayerName = TEXT("Layer_Desert_Tree");
				}

			}
		}
		if (ImGui::CollapsingHeader("Desert_Grass"))
		{
			if (ImGui::ListBox("##Desert_Shrubs", &nSelectedShrub, desertShrubNames, IM_ARRAYSIZE(desertShrubNames), 7))
			{
				if (nSelectedShrub == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DEADSHRUBS_A;
					m_CurrentLayerName = TEXT("Layer_Desert_Grass");
				}
				else if (nSelectedShrub == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DEADSHRUBS_B;
					m_CurrentLayerName = TEXT("Layer_Desert_Grass");
				}
			}
		}
	}
	else if (m_eCurrentMap == DESERT_THEME::CANYON)
	{
		ImGui::Text("Desert Common Objects");
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
		// 2. 모델 배치 / 삭제메뉴

		_int nSelectedVeryBigCanyon = -1;
		_int nSelectedBigCanyon = -1;
		_int nSelectedMiddleCanyon = -1;
		_int nSelectedSmallCanyon = -1;
		_int nSelectedRock = -1;
		_int nSelectedStone = -1;

		const _char* veryBigCanyonNames[] = { "CANYON_127A","CANYON_127B", "CANYON_127C", "CANYON_127D", "CANYON_127E", "CANYON_128A", "CANYON_128B","CANYON_128C",
												"CANYON_132A", "CANYON_132B", "CANYON_132C", "CANYON_133B" };

		const _char* bigCanyonNames[] = { "CANYON_3A", "CANYON_12A", "CANYON_22A", "CANYON_23A", "CANYON_39A", "CANYON_39C", "CANYON_39D", "CANYON_65A", "CANYON_66A",
										 "CANYON_67A", "CANYON_69A", "CANYON_71A",   "CANYON_97A", "CANYON_105A", "CANYON_106A", "CANYON_108A", "CANYON_125A", "CANYON_126A" };

		const _char* middleCanyonNames[] = { "CANYON_5A", "CANYON_21A", "CANYON_24A", "CANYON_35A", "CANYON_80A", "CANYON_81A", "CANYON_101A", "CANYON_103A", "CANYON_104A", "CANYON_109A", "CANYON_110A", "CANYON_111A", "CANYON_112A", "CANYON_113A", "CANYON_115A" };

		const _char* smallCanyonNames[] = { "CANYON_1A", "CANYON_2A", "CANYON_6A", "CANYON_20A", "CANYON_93A", "CANYON_95A", "CANYON_96A", "CANYON_116A", "CANYON_117A" };

		const _char* rockNames[] = { "CANYON_14B", "CANYON_15A", "CANYON_17A", "CANYON_17B", "CANYON_52A", "CANYON_55A", "CANYON_60A", "CANYON_61A", "CANYON_98A",
									"CANYON_100A", "CANYON_121A", "CANYON_122A", };

		const _char* stoneNames[] = { "CANYON_4A", "CANYON_14A", "CANYON_16A", "CANYON_16B", "CANYON_18A", "CANYON_43A", "CANYON_44A", "CANYON_46A", "CANYON_50A",
									"CANYON_58A", "CANYON_59A", "CANYON_123A", "CANYON_131A" };



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
		_int nSelectedLift = -1;
		_int nSelectedRuin = -1;
		_int nSelectedBridge = -1;

		const _char* modelNames[] = { "FLOOR7_A", "FLOOR7_B", "FLOOR7_C", "FLOOR7_D", "FLOOR7_E", "FLOOR7_F" };
		const _char* liftNames[] = { "LIFT_BODY", "LIFT_CONTROLLER", "LIFT_PLATFORM" };
		const _char* ruinNames[] = { "RUIN_7A", "RUIN_7B", "RUIN_7C", "RUIN_7D", "RUIN_7E" };
		const _char* bridgeNames[] = { "BRIDGE_4A","BRIDGE_4B","BRIDGE_4D","BRIDGE_4H","BRIDGE_4L","BRIDGE_4M","BRIDGE_4N","BRIDGE_5","BRIDGE_6","BRIDGE_8","BRIDGE_14A","BRIDGE_14B" };

		if (ImGui::CollapsingHeader("Models"))
		{
			if (ImGui::ListBox("##Models", &nSelectedModel, modelNames, IM_ARRAYSIZE(modelNames), 7))
			{
				if (nSelectedModel == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_A;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_B;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_C;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_D;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_E;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}
				else if (nSelectedModel == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FLOOR7_F;
					m_CurrentLayerName = TEXT("Layer_Iron_Floor");
				}

			}
		}

		if (ImGui::CollapsingHeader("Lifts"))
		{
			if (ImGui::ListBox("##Lifts", &nSelectedLift, liftNames, IM_ARRAYSIZE(liftNames), 7))
			{
				if (nSelectedLift == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::LIFT_BODY;
					m_CurrentLayerName = TEXT("Layer_Lift_Body");
				}
				else if (nSelectedLift == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::LIFT_CONTROLLER;
					m_CurrentLayerName = TEXT("Layer_Lift_Controller");

					m_bShowLiftControllerWindow = true;
				}
				else if (nSelectedLift == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::LIFT_PLATFORM;
					m_CurrentLayerName = TEXT("Layer_Lift_Platform");

					m_bShowLiftPlatformWindow = true;
				}

			}
		}
		if (true == m_bShowLiftControllerWindow)
		{
			if (ImGui::Begin("Lift Controller Type Setting", &m_bShowLiftControllerWindow))
			{
				ImGui::Text("Controller Type(false : OUT, true : IN)");
				if (ImGui::RadioButton("OUT (false)", m_bIsLiftControllerType == false))
				{
					m_bIsLiftControllerType = false;
				}
				if (ImGui::RadioButton("IN (true)", m_bIsLiftControllerType == true))
				{
					m_bIsLiftControllerType = true;
				}

				ImGui::Text("LiftPlatform_ID");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(100.f);
				ImGui::InputInt("##Platform ID", &m_iPlatformID);

				ImGui::Text("LiftPosition : (0)Top, (1)Bottom");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(100.f);
				ImGui::InputInt("##Position", &m_iPosition);

			}

			ImGui::End();
		}
		if (true == m_bShowLiftPlatformWindow)
		{
			if (ImGui::Begin("Lift Platform Type Setting", &m_bShowLiftPlatformWindow))
			{
				ImGui::Text("LiftPlatform_Move_Distance");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(50.f);
				ImGui::InputFloat("##Distance", &m_fPlatformMoveDistance);

				ImGui::Text("LiftPlatform_ID");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(100.f);
				ImGui::InputInt("##Platform ID", &m_iPlatformID);
			}

			ImGui::End();
		}
		if (ImGui::CollapsingHeader("Ruin"))
		{
			if (ImGui::ListBox("##Ruin", &nSelectedRuin, ruinNames, IM_ARRAYSIZE(ruinNames), 7))
			{
				if (nSelectedRuin == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_7A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_7B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_7C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_7D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedRuin == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RUIN_7E;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}
		if (ImGui::CollapsingHeader("Bridge"))
		{
			if (ImGui::ListBox("##Bridge", &nSelectedBridge, bridgeNames, IM_ARRAYSIZE(bridgeNames), 7))
			{
				if (nSelectedBridge == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_4A; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_4B; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_4D; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_4H; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_4L; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_4M; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_4N; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_5; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_6; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_8; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_14A; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedBridge == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BRIDGE_14B; m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}
	}
	else if (m_eCurrentMap == DESERT_THEME::DECO)
	{
		ImGui::Text("Deco Objects");
		_int nSelectedModel = -1;
		_int nSelectedCamp = -1;
		_int nSelectedCorpse = -1;
		_int nSelectedContainer = -1;
		_int nSelectedFence = -1;
		_int nSelectedVehicle = -1;
		_int nSelectedSign = -1;
		_int nSelectedCrane = -1;
		_int nSelectedTrash = -1;

		const _char* modelNames[] = { "Lamp_47A", "VendingMachine_6A", "VendingMachine_7A", "Poster_4D", "SciFi_Door", "CanBox",
									  "Building_4B", "Building_4C", "Building_4D", "Wheel_1A", "Wheel_1B", "Wheel_1C" };
		const _char* campNames[] = { "BASE_1A", "CAMP_1B", "CAMP_1D", "CAMP_1E", "CAMP_1F", "CAMP_1G", "CAMP_1H", "CAMP_1J", "CAMP_1K", "CAMP_1L",
									"CAMP_1M", "CAMP_1N", "CAMP_1R", "CAMP_1S", "CAMP_1T", "CAMP_1W", "REPAIRCONSOLE", "TOP_ROOF", "CAMP_1I" };
		const _char* corpseNamees[] = { "CORPSE_1A", "CORPSE_1B", "CORPSE_2A", "CORPSE_2B", "CORPSE_2C", "CORPSE_3A" };
		const _char* containerNamees[] = { "CONTAINER_2A", "CONTAINER_2B",	"CONTAINER_2C",	"CONTAINER_2D",	"CONTAINER_3A",	"CONTAINER_4B",	"CONTAINER_5A",	"CONTAINER_5B",	"CONTAINER_5C",	"CONTAINER_5D",	"CONTAINER_5E",	"CONTAINER_7B",	"CONTAINER_7F" };
		const _char* fenceNames[] = { "FENCE_1A", "FENCE_1B", "FENCE_1F", "FENCE_1H" };
		const _char* vehicleNames[] = { "VEHICLE_2A", "VEHICLE_2B", "VEHICLE_3B", "VEHICLE_4C", "VEHICLE_6A", "VEHICLE_8B", "VEHICLE_14A", "VEHICLE_14B" };
		const _char* signNames[] = { "SIGN_11A", "SIGN_11B", "SIGN_11F", "SIGN_12B", "SIGN_12C", "SIGN_36B" };
		const _char* craneNames[] = { "CRANE_1C", "CRANE_11", "CRANE_13" };
		const _char* trashNames[] = { "TRASH_1A", "TRASH_2A", "TRASH_2B", "TRASH_4A", "TRASH_9A", "TRASH_17A" };

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
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
				else if (nSelectedModel == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VendingMachine_7A;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
				else if (nSelectedModel == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_4D;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedModel == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SCIFI_DOOR;
					m_CurrentLayerName = TEXT("Layer_SciFi_Door");
				}
				else if (nSelectedModel == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CANBOX;
					m_CurrentLayerName = TEXT("Layer_CanBox");
				}
				else if (nSelectedModel == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BUILDING_4B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedModel == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BUILDING_4C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedModel == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BUILDING_4D;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedModel == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WHEEL_1A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedModel == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WHEEL_1B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedModel == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::WHEEL_1C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
		if (ImGui::CollapsingHeader("Camps"))
		{
			if (ImGui::ListBox("##Camps", &nSelectedCamp, campNames, IM_ARRAYSIZE(campNames), 7))
			{
				if (nSelectedCamp == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BASE_1A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1D;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1E;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1F;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1G;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1H;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1J;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1K;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1L;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1M;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1N;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1R;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 13)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1S;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 14)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1T;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
				else if (nSelectedCamp == 15)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1W;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCamp == 16)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::REPAIRCONSOLE;
					m_CurrentLayerName = TEXT("Layer_RepairConsole");
				}
				else if (nSelectedCamp == 17)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TOP_ROOF;
					m_CurrentLayerName = TEXT("Layer_Top_Roof");
				}
				else if (nSelectedCamp == 18)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CAMP_1I;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
			}
		}
		if (ImGui::CollapsingHeader("Corpse"))
		{
			if (ImGui::ListBox("##Corpse", &nSelectedCorpse, corpseNamees, IM_ARRAYSIZE(corpseNamees), 7))
			{
				if (nSelectedCorpse == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CORPSE_1A;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
				else if (nSelectedCorpse == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CORPSE_1B;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
				else if (nSelectedCorpse == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CORPSE_2A;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
				else if (nSelectedCorpse == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CORPSE_2B;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
				else if (nSelectedCorpse == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CORPSE_2C;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
				else if (nSelectedCorpse == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CORPSE_3A;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}
			}
		}
		if (ImGui::CollapsingHeader("Container"))
		{
			if (ImGui::ListBox("##Container", &nSelectedContainer, containerNamees, IM_ARRAYSIZE(containerNamees), 7))
			{
				if (nSelectedContainer == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_2A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_2B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_2C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_2D;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_3A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_4B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_5A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_5B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_5C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_5D;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_5E;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_7B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedContainer == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CONTAINER_7F;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
		if (ImGui::CollapsingHeader("Fence"))
		{
			if (ImGui::ListBox("##Fence", &nSelectedFence, fenceNames, IM_ARRAYSIZE(fenceNames), 7))
			{
				if (nSelectedFence == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FENCE_1A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedFence == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FENCE_1B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedFence == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FENCE_1F;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedFence == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FENCE_1H;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
		if (ImGui::CollapsingHeader("Vehicle"))
		{
			if (ImGui::ListBox("##Vehicle", &nSelectedVehicle, vehicleNames, IM_ARRAYSIZE(vehicleNames), 7))
			{
				if (nSelectedVehicle == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VEHICLE_2A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedVehicle == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VEHICLE_2B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedVehicle == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VEHICLE_3B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedVehicle == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VEHICLE_4C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedVehicle == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VEHICLE_6A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedVehicle == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VEHICLE_8B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedVehicle == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VEHICLE_14A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedVehicle == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::VEHICLE_14B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
		if (ImGui::CollapsingHeader("Sign"))
		{
			if (ImGui::ListBox("##Signs", &nSelectedSign, signNames, IM_ARRAYSIZE(signNames), 7))
			{
				if (nSelectedSign == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SIGN_11A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedSign == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SIGN_11B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedSign == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SIGN_11F;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedSign == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SIGN_12B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedSign == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SIGN_12C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedSign == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::SIGN_36B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
		if (ImGui::CollapsingHeader("Crane"))
		{
			if (ImGui::ListBox("##Cranes", &nSelectedCrane, craneNames, IM_ARRAYSIZE(craneNames), 3))
			{
				if (nSelectedCrane == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CRANE_1C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCrane == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CRANE_11;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedCrane == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::CRANE_13;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
		if (ImGui::CollapsingHeader("Trash"))
		{
			if (ImGui::ListBox("##Trashs", &nSelectedTrash, trashNames, IM_ARRAYSIZE(trashNames), 6))
			{
				if (nSelectedTrash == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TRASH_1A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedTrash == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TRASH_2A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedTrash == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TRASH_2B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedTrash == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TRASH_4A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedTrash == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TRASH_9A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedTrash == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::TRASH_17A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
	}
	else if (m_eCurrentMap == DESERT_THEME::XION)
	{
		ImGui::Text("Desert Common Objects");
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 모델 배치 메뉴
		// 2. 모델 배치 / 삭제메뉴
		_int nSelectedBox = -1;
		_int nSelectedDecos = -1;
		_int nSelectedDuct = -1;
		_int nSelectedXionBuilding = -1;
		_int nSelectedStair = -1;
		_int nSelectedXionWall = -1;
		_int nSelectedFurniture = -1;
		
		const _char* xionBoxNames[] = { "BOX_1A","BOX_1B", "BOX_2C", "BOX_4A", "BOX_5A", "BOX_6A", "BOX_11A", "BOX_13A", "BOX_14A", "BOX_16A", "BOX_16B", "BOX_19A", "BOX_20A",
										"BOX_20B", "BOX_20C", "BOX_20D", "BOX_20E", "BOX_20F", "BOX_21A", "BOX_21B", "BOX_26A", "ITEM_BOX" };

		const _char* xionDecoNames[] = { "GARDEN_1A", "GARDEN_1B", "GARDEN_1C", "RESTROOM_4A", "POSTER_1A", "POSTER_2A", "POSTER_2B", "POSTER_3A", "POSTER_3B", "POSTER_4A", "POSTER_4B", "POSTER_4C", "POSTER_4E",
										 "STATUE_1A", "STATUE_19B", "STATUE_24B", "STATUE_31B", "STATUE_40B", "DOOR_3A", "DOOR_SHUTTER", "DOOR_CONTROOLER"};
		const _char* xionDuctNames[] = { "DUCT_1A", "DUCT_1B", "DUCT_1C", "DUCT_1D", "DUCT_1E", "DUCT_1G", "DUCT_1H","DUCT_3A", "DUCT_3B", "DUCT_3C", "DUCT_4A", "DUCT_4B", "DUCT_4C", "DUCT_6A",
										 "DUCT_8A", "DUCT_9A", "DUCT_9B", "DUCT_9C", "DUCT_9D", "DUCT_10A", "DUCT_13A" };
		const _char* xionBuildingNames[] = { "XION_BUILDING_3A", "XION_BUILDING_4A", "XION_BUILDING_5A", "XION_BUILDING_6A", "XION_BUILDING_8","XION_BUILDING_9A",
											 "XION_BUILDING_17", "XION_BUILDING4_1A", "XION_BUILDING4_1B", "XION_BUILDING4_1C", "XION_BUILDING4_1D", "XION_BUILDING4_1E",
											 "XION_BUILDING4_1F", "XION_BUILDING5_4", "XION_BUILDING5_callD4", "XION_BUILDING5_callD5", "STORE_3", "STORE_4" };
		const _char* stairNames[] = { "STAIR_1A", "STAIR_1B", "STAIR_1C", "STAIR_2A", "STAIR_3A", "STAIR_3B" };
		const _char* xionWallNames[] = { "XION_WALL_1C", "XION_WALL_1D", "XION_WALL_1Db", "XION_WALL_1G", "XION_WALL_1I", "XION_WALL_1K", "XION_WALL_1Q", "XION_WALL_1R", "XION_WALL_1S", 
										 "XION_WALL_1T", "XION_WALL_2A", "XION_WALL_2B", "XION_WALL_4H", "XION_WALL_5A", "XION_WALL_8A", "XION_WALL_9A", 
										 "XION_WALL_9B", "XION_WALL_10A","XION_WALL_10B", "XION_WALL_10C", "XION_WALL_10D" };
		const _char* furnitureNames[] = { "FURNITURE_7A", "FURNITURE_9A", "FURNITURE_47A", "FURNITURE_50A", "FURNITURE_50B", "FURNITURE_50C", "FURNITURE_50D", "FURNITURE_51A", 
										  "FURNITURE_57A", "FURNITURE_59A","FURNITURE_77A", "FURNITURE_79A", "FURNITURE_83A", "FURNITURE_87B", "FURNITURE_87D" };

		if (ImGui::CollapsingHeader("Xion_Boxes"))
		{
			if (ImGui::ListBox("##Xion_Boxes", &nSelectedBox, xionBoxNames, IM_ARRAYSIZE(xionBoxNames), 7))
			{
				if (nSelectedBox == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_1A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_1B; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_2C; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_4A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_5A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_6A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_11A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_13A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_14A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_16A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_16B; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_19A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_20A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 13)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_20B; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 14)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_20C; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 15)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_20D; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 16)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_20E; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 17)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_20F; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 18)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_21A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 19)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_21B; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 20)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::BOX_26A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedBox == 21)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::ITEM_BOX; m_CurrentLayerName = TEXT("Layer_DisplayBox");
				}
			}
		}
		if (ImGui::CollapsingHeader("Xion_Deco"))
		{
			if (ImGui::ListBox("##Xion_Deco", &nSelectedDecos, xionDecoNames, IM_ARRAYSIZE(xionDecoNames), 7))
			{
				if (nSelectedDecos == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::GARDEN_1A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::GARDEN_1B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::GARDEN_1C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RESTROOM_4A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_1A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_2A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_2B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_3A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_3B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_4A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_4B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_4C;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::POSTER_4E;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 13)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STATUE_1A;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 14)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STATUE_19B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 15)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STATUE_24B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 16)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STATUE_31B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 17)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STATUE_40B;
					m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDecos == 18)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_3A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedDecos == 19)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_SHUTTER;
					m_CurrentLayerName = TEXT("Layer_Shutter");
				}
				else if (nSelectedDecos == 20)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DOOR_CONTROLLER;
					m_CurrentLayerName = TEXT("Layer_Interaction");
				}

			}
		}
		if (ImGui::CollapsingHeader("Xion_Duct"))
		{
			if (ImGui::ListBox("##Xion_Duct", &nSelectedDuct, xionDuctNames, IM_ARRAYSIZE(xionDuctNames), 7))
			{
				if (nSelectedDuct == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_1A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_1B; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_1C; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_1D; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_1E; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_1G; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_1H; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_3A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_3B; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_3C; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_4A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_4B; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_4C; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 13)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_6A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 14)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_8A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 15)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_9A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 16)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_9B; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 17)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_9C; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 18)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_9D; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 19)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_10A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
				else if (nSelectedDuct == 20)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DUCT_13A; m_CurrentLayerName = TEXT("Layer_Deco");
				}
			}
		}
		if (ImGui::CollapsingHeader("Xion_Building"))
		{
			if (ImGui::ListBox("##Xion_Building", &nSelectedXionBuilding, xionBuildingNames, IM_ARRAYSIZE(xionBuildingNames), 7))
			{
				if (nSelectedXionBuilding == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING_3A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING_4A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING_5A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING_6A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING_8;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING_9A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING_17;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING4_1A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING4_1B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING4_1C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING4_1D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING4_1E;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING4_1F;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 13)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING5_4;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 14)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING5_callD4;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 15)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_BUILDING5_callD5;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 16)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STORE_3;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionBuilding == 17)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STORE_4;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}
		if (ImGui::CollapsingHeader("Stair"))
		{
			if (ImGui::ListBox("##Stair", &nSelectedStair, stairNames, IM_ARRAYSIZE(stairNames), 7))
			{
				if (nSelectedStair == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STAIR_1A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedStair == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STAIR_1B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedStair == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STAIR_1C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedStair == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STAIR_2A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedStair == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STAIR_3A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedStair == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::STAIR_3B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}
		if (ImGui::CollapsingHeader("Xion_Wall"))
		{
			if (ImGui::ListBox("##Xion_Wall", &nSelectedXionWall, xionWallNames, IM_ARRAYSIZE(xionWallNames), 7))
			{
				if (nSelectedXionWall == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1Db;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1G;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 4)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1I;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 5)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1K;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 6)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1Q;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 7)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1R;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 8)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1S;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 9)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_1T;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 10)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_2A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 11)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_2B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 12)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_4H;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 13)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_5A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 14)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_8A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 15)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_9A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 16)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_9B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 17)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_10A;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 18)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_10B;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 19)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_10C;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
				else if (nSelectedXionWall == 20)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::XION_WALL_10D;
					m_CurrentLayerName = TEXT("Layer_Architecture");
				}
			}
		}
		if (ImGui::CollapsingHeader("Furniture"))
		{
			if (ImGui::ListBox("##Furniture", &nSelectedFurniture, furnitureNames, IM_ARRAYSIZE(furnitureNames), 7))
			{
				if (nSelectedFurniture == 0) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_7A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 1) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_9A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 2) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_47A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 3) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_50A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 4) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_50B; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 5) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_50C; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 6) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_50D; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 7) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_51A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 8) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_57A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 9) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_59A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 10) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_77A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 11) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_79A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 12) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_83A; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 13) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_87B; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
				else if (nSelectedFurniture == 14) 
				{ 
					m_eCurrentObject = DESESRT_RUIN_OBJECT::FURNITURE_87D; 
					m_CurrentLayerName = TEXT("Layer_Deco"); 
				}
			}
		}
	}
	else if (m_eCurrentMap == DESERT_THEME::DORORONG_SABER)
	{
		ImGui::Text("DORORONG_SABER Objects");
		_int nSelectedModel = -1;

		const _char* modelNames[] = { "Pad", "Rail", "Beat_Indicator", "DororongBox"};

		if (ImGui::CollapsingHeader("Models"))
		{
			if (ImGui::ListBox("##Models", &nSelectedModel, modelNames, IM_ARRAYSIZE(modelNames), 7))
			{
				if (nSelectedModel == 0)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::PAD;
					m_CurrentLayerName = TEXT("Layer_Pad");
				}
				else if (nSelectedModel == 1)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::RAIL;
					m_CurrentLayerName = TEXT("Layer_Rail");
				}
				else if (nSelectedModel == 2)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::INDICATOR;
					m_CurrentLayerName = TEXT("Layer_Beat_Indicator");
				}
				else if (nSelectedModel == 3)
				{
					m_eCurrentObject = DESESRT_RUIN_OBJECT::DORORONG_BOX;
					m_CurrentLayerName = TEXT("Layer_DororongBox");
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

	static _char szLoadFilePath[256] = "../Bin/DataFiles/MapData_Xion.bin";
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

	ImGui::Spacing();// 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	// 도로롱 세이버 저장 로드

	static _char szSaveDororongFilePath[256] = "../Bin/DataFiles/Dororong_Saber.bin";
	ImGui::InputText("Dororong Save File Path", szSaveDororongFilePath, sizeof(szSaveDororongFilePath));

	// 에디터 세이브 / 로드
	if (ImGui::Button("Save_Dororong"))
	{
		// 맵 오브젝트 저장
		if (FAILED(Save_Dororong_Saber_Objects(szSaveDororongFilePath)))
		{
			MessageBoxW(g_hWnd, L"도로롱 세이버 저장 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"도로롱 세이버 저장 성공.", L"알림", MB_OK);
		}
	}

	ImGui::Spacing(); // 메뉴 사이의 간격
	ImGui::Separator(); // 구분선을 추가
	ImGui::Spacing();

	static _char szLoadDororongFilePath[256] = "../Bin/DataFiles/Dororong_Saber.bin";
	ImGui::InputText("Dororong Load File Path", szLoadDororongFilePath, sizeof(szLoadDororongFilePath));

	if (ImGui::Button("Load_Dororong"))
	{
		if (FAILED(Load_Dororong_Saber_Objects(szLoadDororongFilePath)))
		{
			MessageBoxW(g_hWnd, L"도로롱 세이버 로드 실패", L"알림", MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBoxW(g_hWnd, L"도로롱 세이버 로드 성공.", L"알림", MB_OK);
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
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Iron_Floor")))) return S_OK;
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Deco")))) return S_OK;
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Desert_Tree")))) return S_OK;
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Architecture")))) return S_OK;

	if (FAILED(Save_Interaction_Objects_By_Layer(ofs, TEXT("Layer_Top_Roof")))) return S_OK;
	if (FAILED(Save_Interaction_Objects_By_Layer(ofs, TEXT("Layer_RepairConsole")))) return S_OK;
	if (FAILED(Save_Interaction_Objects_By_Layer(ofs, TEXT("Layer_SciFi_Door")))) return S_OK;
	if (FAILED(Save_Interaction_Objects_By_Layer(ofs, TEXT("Layer_CanBox")))) return S_OK;
	if (FAILED(Save_Interaction_Objects_By_Layer(ofs, TEXT("Layer_Interaction")))) return S_OK;

	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Desert_Grass")))) return S_OK;

	if (FAILED(Save_Lift_Platform_By_Layer(ofs, TEXT("Layer_Lift_Platform")))) return S_OK;
	if (FAILED(Save_Lift_Controller_By_Layer(ofs, TEXT("Layer_Lift_Controller")))) return S_OK;
	if (FAILED(Save_Interaction_Objects_By_Layer(ofs, TEXT("Layer_DisplayBox")))) return S_OK;
	if (FAILED(Save_Objects_By_Layer(ofs, TEXT("Layer_Shutter")))) return S_OK;

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
	if (FAILED(Save_Npcs_By_Layer(ofs, TEXT("Layer_Npc")))) return S_OK;

	ofs.close();

	return S_OK;
}

HRESULT CMapTool_Desert::Save_Dororong_Saber_Objects(const _char* szFilePath)
{
	// 맵 데이터 파일 열기
	std::ofstream ofs(szFilePath, std::ios::binary);
	if (!ofs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Save_Dororong_Saber_By_Layer(ofs, TEXT("Layer_Pad")))) return E_FAIL;
	if (FAILED(Save_Dororong_Saber_By_Layer(ofs, TEXT("Layer_Rail")))) return E_FAIL;
	if (FAILED(Save_Dororong_Saber_By_Layer(ofs, TEXT("Layer_Beat_Indicator")))) return E_FAIL;
	if (FAILED(Save_Dororong_Saber_By_Layer(ofs, TEXT("Layer_DororongBox")))) return E_FAIL;

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

HRESULT CMapTool_Desert::Save_Interaction_Objects_By_Layer(std::ofstream& ofs, const _tchar* pLayerTag)
{
	list<CGameObject*>* pObj = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
	_uint iNumObjs = (pObj) ? (_uint)pObj->size() : 0;

	ofs.write(reinterpret_cast<const char*>(&iNumObjs), sizeof(_uint));

	if (pObj)
	{
		for (auto pObject : *pObj)
		{
			CInteraction* pInteractionObject = dynamic_cast<CInteraction*>(pObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));

			if (pTransform && (pInteractionObject))
			{
				SAVEDINTERACTIONOBJECTINFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);

				const _tchar* pTag = pInteractionObject->Get_ComponentTag();
				wcsncpy_s(info.szComponentTag, 256, pTag, _TRUNCATE);

				const _uint pId = pInteractionObject->Get_ObjectID();
				info.iObjectID = pId;
				info.iInteractionID = pInteractionObject->Get_InteractionID();

				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVEDINTERACTIONOBJECTINFO));
			}
		}
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Save_Lift_Controller_By_Layer(ofstream& ofs, const _tchar* pLayerTag)
{
	list<CGameObject*>* pObj = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
	_uint iNumObjs = (pObj) ? (_uint)pObj->size() : 0;

	ofs.write(reinterpret_cast<const char*>(&iNumObjs), sizeof(_uint));

	if (pObj)
	{
		for (auto pObject : *pObj)
		{
			CLift_Controller* pLiftController = dynamic_cast<CLift_Controller*>(pObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));

			if (pTransform && (pLiftController))
			{
				SAVED_LIFT_CONTROLLER_INFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);

				const _tchar* pTag = pLiftController->Get_ComponentTag();
				wcsncpy_s(info.szComponentTag, 256, pTag, _TRUNCATE);

				const _uint iId = pLiftController->Get_ObjectID();
				const _uint iPlatformId = pLiftController->Get_PlatformID();

				info.iObjectID = iId;
				info.iPlatformID = iPlatformId;
				info.iInteractionID = pLiftController->Get_InteractionID();
				info.bIsControllerType = pLiftController->Get_ControllerType();
				info.iPosition = pLiftController->Get_Position();

				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVED_LIFT_CONTROLLER_INFO));
			}
		}
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Save_Lift_Platform_By_Layer(ofstream& ofs, const _tchar* pLayerTag)
{
	list<CGameObject*>* pObj = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
	_uint iNumObjs = (pObj) ? (_uint)pObj->size() : 0;

	ofs.write(reinterpret_cast<const char*>(&iNumObjs), sizeof(_uint));

	if (pObj)
	{
		for (auto pObject : *pObj)
		{
			CLift_Platform* pLift_Platform = dynamic_cast<CLift_Platform*>(pObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));

			if (pTransform && (pLift_Platform))
			{
				SAVED_LIFT_PLATFORM_INFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);

				const _tchar* pTag = pLift_Platform->Get_ComponentTag();
				wcsncpy_s(info.szComponentTag, 256, pTag, _TRUNCATE);

				const _uint pId = pLift_Platform->Get_ObjectID();
				info.iObjectID = pId;
				info.iPlatformID = pLift_Platform->Get_PlatformID();
				info.fMoveDistance = pLift_Platform->Get_MoveDistance();

				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVED_LIFT_PLATFORM_INFO));
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
			CSpawnBox* pMonster = dynamic_cast<CSpawnBox*>(pObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));

			if (pTransform && (pMonster))
			{
				SAVEDMONSTERINFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);

				const _tchar* pTag = pMonster->Get_ComponentTag();
				wcsncpy_s(info.szComponentTag, 256, pTag, _TRUNCATE);

				const _uint pId = pMonster->Get_MonsterID();
				info.iMonsterID = pId;

				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVEDMONSTERINFO));
			}
		}
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Save_Npcs_By_Layer(ofstream& ofs, const _tchar* pLayerTag)
{
	list<CGameObject*>* pObj = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
	_uint iNumObjs = (pObj) ? (_uint)pObj->size() : 0;

	ofs.write(reinterpret_cast<const char*>(&iNumObjs), sizeof(_uint));

	if (pObj)
	{
		for (auto pObject : *pObj)
		{
			CNpc* pNpc = dynamic_cast<CNpc*>(pObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));

			if (pTransform && (pNpc))
			{
				SAVEDNPCINFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);

				const _tchar* pTag = pNpc->Get_ComponentTag();
				wcsncpy_s(info.szComponentTag, 256, pTag, _TRUNCATE);

				const _uint pId = pNpc->Get_NpcID();
				info.iNpcID = pId;

				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVEDNPCINFO));
			}
		}
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Save_Dororong_Saber_By_Layer(ofstream& ofs, const _tchar* pLayerTag)
{
	list<CGameObject*>* pObj = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::DESERT), pLayerTag);
	_uint iNumObjs = (pObj) ? (_uint)pObj->size() : 0;

	ofs.write(reinterpret_cast<const char*>(&iNumObjs), sizeof(_uint));

	if (pObj)
	{
		for (auto pObject : *pObj)
		{
			CDororong_Saber* pDororongSaberObject = dynamic_cast<CDororong_Saber*>(pObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));

			if (pTransform && (pDororongSaberObject))
			{
				SAVEDDORORONGSABERINFO info;
				const _float4x4* pWorldMatrixFloat4x4 = pTransform->Get_WorldMatrixPtr();
				_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrixFloat4x4);
				XMStoreFloat4x4(&info.worldMatrix, WorldMatrix);

				ofs.write(reinterpret_cast<const char*>(&info), sizeof(SAVEDDORORONGSABERINFO));
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
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Iron_Floor"), TEXT("Layer_Iron_Floor")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Deco"), TEXT("Layer_Deco")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Desert_Tree"), TEXT("Layer_Desert_Tree")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Architecture"), TEXT("Layer_Architecture")))) return S_OK;
	
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Top_Roof"), TEXT("Layer_Top_Roof")))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_RepairConsole"), TEXT("Layer_RepairConsole")))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_SciFi_Door"), TEXT("Layer_SciFi_Door")))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_CanBox"), TEXT("Layer_CanBox")))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Interaction_NonAnim"), TEXT("Layer_Interaction")))) return S_OK;

	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Desert_Grass"), TEXT("Layer_Desert_Grass")))) return S_OK;

	if (FAILED(Load_Lift_Platform_By_Layer(ifs, TEXT("Prototype_GameObject_Lift_Platform"), TEXT("Layer_Lift_Platform")))) return S_OK;
	if (FAILED(Load_Lift_Controller_By_Layer(ifs, TEXT("Prototype_GameObject_Lift_Controller"), TEXT("Layer_Lift_Controller")))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_DisplayBox"), TEXT("Layer_DisplayBox")))) return S_OK;
	if (FAILED(Load_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Shutter"), TEXT("Layer_Shutter")))) return S_OK;

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
	if (FAILED(Load_Npcs_By_Layer(ifs, TEXT("Prototype_GameObject_Npc"), TEXT("Layer_Npc")))) return S_OK;

	ifs.close();

	return S_OK;
}

HRESULT CMapTool_Desert::Load_Dororong_Saber_Objects(const _char* szFilePath)
{
	std::ifstream ifs(szFilePath, std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Load_Dororong_Saber_By_Layer(ifs, TEXT("Prototype_GameObject_Pad"), TEXT("Layer_Pad")))) return E_FAIL;
	if (FAILED(Load_Dororong_Saber_By_Layer(ifs, TEXT("Prototype_GameObject_Rail"), TEXT("Layer_Rail")))) return E_FAIL;
	if (FAILED(Load_Dororong_Saber_By_Layer(ifs, TEXT("Prototype_GameObject_Beat_Indicator"), TEXT("Layer_Beat_Indicator")))) return E_FAIL;
	if (FAILED(Load_Dororong_Saber_By_Layer(ifs, TEXT("Prototype_GameObject_DororongBox"), TEXT("Layer_DororongBox")))) return E_FAIL;

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

HRESULT CMapTool_Desert::Load_Interaction_Objects_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDINTERACTIONOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDINTERACTIONOBJECTINFO));

		CInteraction::PROB_INTERACTION_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iObjectId = info.iObjectID;
		Desc.iInteractionID = info.iInteractionID;
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

HRESULT CMapTool_Desert::Load_Lift_Controller_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVED_LIFT_CONTROLLER_INFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVED_LIFT_CONTROLLER_INFO));

		CLift_Controller::LIFT_CONTROLLER_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iObjectId = info.iObjectID;
		Desc.iInteractionID = info.iInteractionID;
		Desc.pComponentTag = info.szComponentTag;
		Desc.bIsControllerType = info.bIsControllerType;
		Desc.iPlatformID = info.iPlatformID;
		Desc.iPosition = info.iPosition;

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

HRESULT CMapTool_Desert::Load_Lift_Platform_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVED_LIFT_PLATFORM_INFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVED_LIFT_PLATFORM_INFO));

		CLift_Platform::LIFT_PLATFORM_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iObjectId = info.iObjectID;
		Desc.pComponentTag = info.szComponentTag;
		Desc.iPlatFormID = info.iPlatformID;
		Desc.fMoveDistance = info.fMoveDistance;

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

		CSpawnBox::MONSTER_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iMonsterID = info.iMonsterID;
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

HRESULT CMapTool_Desert::Load_Npcs_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDNPCINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDNPCINFO));

		CNpc::NPC_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iNpcID = info.iNpcID;
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

HRESULT CMapTool_Desert::Load_Dororong_Saber_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDDORORONGSABERINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDDORORONGSABERINFO));

		CGameObject::GAMEOBJECT_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;

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

HRESULT CMapTool_Desert::Save_MaskMap(const _char* szFilePath)
{
	if (m_pMaskTexture2D == nullptr)
	{
		return E_FAIL;
	}

	// 1. 와이드 문자열 버퍼 선언
	wchar_t wszFilePath[256] = L"";
	size_t convertedChars = 0;

	// 2. 멀티바이트 문자열 (char*)을 와이드 문자열 (wchar_t*)로 변환
	// szFilePath의 내용을 wszFilePath 버퍼로 안전하게 변환합니다.
	if (mbstowcs_s(&convertedChars, wszFilePath, 256, szFilePath, _TRUNCATE) != 0)
	{
		return E_FAIL;
	}

	if (m_pMaskTexture2D != nullptr)
	{
		if (FAILED(DirectX::SaveWICTextureToFile(m_pContext, m_pMaskTexture2D, GUID_ContainerFormatPng, wszFilePath)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapTool_Desert::Set_LoadMaskMap(const _char* szFilePath)
{
	// 기존 리소스 해제
	Safe_Release(m_pMaskTexture2D);
	Safe_Release(m_pMaskSRV);

	// 1. 와이드 문자열 버퍼 선언 및 변환 (DirectX::Load 함수는 wchar_t* 경로를 받음)
	wchar_t wszFilePath[256] = L"";
	size_t convertedChars = 0;
	if (mbstowcs_s(&convertedChars, wszFilePath, 256, szFilePath, _TRUNCATE) != 0)
	{
		return E_FAIL;
	}

	// 2. 파일에서 텍스처를 로드하여 GPU 렌더링용 ID3D11Texture2D 생성 (기본 Usage: D3D11_USAGE_DEFAULT)
	ID3D11Resource* pLoadResource = nullptr;
	ID3D11Texture2D* pRenderTexture = nullptr;

	// DirectX Tool Kit의 LoadWICTextureFromFileName 함수를 사용하여 텍스처 로드 및 SRV 생성
	// 여기서 pLoadResource는 내부적으로 생성된 ID3D11Texture2D 포인터입니다.
	if (FAILED(DirectX::CreateWICTextureFromFile(m_pDevice, m_pContext, wszFilePath, &pLoadResource, &m_pMaskSRV)))
	{
		return E_FAIL;
	}

	// 3. 로드된 리소스에서 ID3D11Texture2D 포인터 가져오기
	// 로드된 텍스처의 속성을 파악하고, GPU 리소스를 준비합니다.
	if (pLoadResource)
	{
		// ID3D11Resource를 ID3D11Texture2D로 캐스팅
		pRenderTexture = reinterpret_cast<ID3D11Texture2D*>(pLoadResource);
	}
	else
	{
		// 로드 실패 또는 캐스팅 실패 시 SRV와 pLoadResource를 정리하고 반환
		Safe_Release(m_pMaskSRV);
		return E_FAIL;
	}

	// 4. CPU 접근(편집)용 스테이징 Texture2D 생성 (m_pMaskTexture2D)
	D3D11_TEXTURE2D_DESC RenderDesc{};
	pRenderTexture->GetDesc(&RenderDesc); // 로드된 텍스처의 속성을 가져옴

	D3D11_TEXTURE2D_DESC StageDesc = RenderDesc;
	StageDesc.Usage = D3D11_USAGE_STAGING;
	StageDesc.BindFlags = 0;
	StageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	StageDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&StageDesc, nullptr, &m_pMaskTexture2D)))
	{
		Safe_Release(pLoadResource);
		return E_FAIL;
	}

	// 5. GPU 텍스처의 내용을 CPU 스테이징 텍스처로 복사
	// 이제 m_pMaskTexture2D를 Map/Unmap으로 편집할 수 있습니다.
	m_pContext->CopyResource(m_pMaskTexture2D, pRenderTexture);

	// pLoadResource는 로드된 GPU 텍스처이며, SRV 생성에 사용되었으므로 Safe_Release 처리
	Safe_Release(pLoadResource);

	// 로드 성공 알림
	MessageBoxW(g_hWnd, L"마스크 맵 로드 성공.", L"알림", MB_OK);

	return S_OK;
}

HRESULT CMapTool_Desert::Set_NewMaskMap()
{
	Safe_Release(m_pMaskTexture2D);
	Safe_Release(m_pMaskSRV);

	D3D11_TEXTURE2D_DESC		TextureDesc{};
	TextureDesc.Width = 2048;
	TextureDesc.Height = 2048;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_uint* pPixel = new _uint[2048 * 2048];

	for (size_t i = 0; i < 2048; i++)
	{
		for (size_t j = 0; j < 2048; j++)
		{
			_uint iIndex = i * 2048 + j;
			pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	D3D11_SUBRESOURCE_DATA		InitialDesc{};
	InitialDesc.pSysMem = pPixel;
	InitialDesc.SysMemPitch = 2048 * 4;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialDesc, &m_pMaskTexture2D)))
		return E_FAIL;

	// 2. GPU 렌더링용 Texture2D 생성
	D3D11_TEXTURE2D_DESC RenderTextureDesc = TextureDesc;
	RenderTextureDesc.Usage = D3D11_USAGE_DEFAULT;				// GPU 기본 사용
	RenderTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// 쉐이더 리소스로 바인드
	RenderTextureDesc.CPUAccessFlags = 0;						// CPU 접근 불가

	if (FAILED(m_pDevice->CreateTexture2D(&RenderTextureDesc, nullptr, &m_pMaskRenderTexture)))
		return E_FAIL;

	// 3. Shader Resource View 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = RenderTextureDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pMaskRenderTexture, &SRVDesc, &m_pMaskSRV)))
	{
		Safe_Release(m_pMaskRenderTexture);
		return E_FAIL;
	}

	m_pContext->CopyResource(m_pMaskRenderTexture, m_pMaskTexture2D);

	//Safe_Release(pRenderTexture);
	Safe_Delete_Array(pPixel);

	return S_OK;
}

void CMapTool_Desert::Change_MaskMap_Black(_float3 vPickedPoint)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pMaskTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	//const _float fTerrainHalfSize = 256.f;
	const _int iTexelSize = 2048;
	const _float fTexelSize = 2048;

	_int iCenterTexelX = static_cast<_int>(vPickedPoint.x);

	_float fFlippedZ = fTexelSize - (vPickedPoint.z);
	_int iCenterTexelY = static_cast<_int>(fFlippedZ);


	_int iBrushRadius = static_cast<_int>(m_fRadius);

	_uint iRowSize = SubResource.RowPitch / sizeof(_uint);

	_int iStartX = max(0, iCenterTexelX - iBrushRadius);
	_int iEndX = min(iTexelSize, iCenterTexelX + iBrushRadius);
	_int iStartZ = max(0, iCenterTexelY - iBrushRadius);
	_int iEndZ = min(iTexelSize, iCenterTexelY + iBrushRadius);

	for (_int i = iStartZ; i < iEndZ; i++)
	{
		char* pRowStart = static_cast<char*>(SubResource.pData) + (i * SubResource.RowPitch);
		_uint* pRow = reinterpret_cast<_uint*>(pRowStart);

		for (_int j = iStartX; j < iEndX; j++)
		{
			// 픽셀 데이터 쓰기
			pRow[j] = D3DCOLOR_ARGB(255, 0, 0, 0); // (MASK_BLACK일 경우)
		}
	}
	m_pContext->Unmap(m_pMaskTexture2D, 0);

	ID3D11Resource* pGpuResource = { nullptr };
	m_pMaskSRV->GetResource(&pGpuResource);

	if (pGpuResource)
	{
		m_pContext->CopyResource(pGpuResource, m_pMaskTexture2D);
		Safe_Release(pGpuResource);
	}
}

void CMapTool_Desert::Change_MaskMap_Red(_float3 vPickedPoint)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pMaskTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	const _int iTexelSize = 2048;
	const _float fTexelSize = 2048.f;

	_float fFlippedZ = fTexelSize - vPickedPoint.z;

	// 2. 0 ~ 2048 범위로 클램프
	_float fClampedX = max(0.f, min(fTexelSize, vPickedPoint.x));
	_float fClampedY = max(0.f, min(fTexelSize, fFlippedZ));

	// 3. 정수 변환 (안전)
	// 0.5를 더하여 반올림 효과를 줄 수도 있지만, 일단 클램핑된 값을 정수로 변환합니다.
	_int iCenterTexelX = static_cast<_int>(fClampedX);
	_int iCenterTexelY = static_cast<_int>(fClampedY);

	_int iBrushRadius = static_cast<_int>(m_fRadius);

	_tchar szDebug[256];
	wsprintf(szDebug, L"Picked: (X=%.2f, Z=%.2f), Texel: (%d, %d), Radius: %d\n",
		vPickedPoint.x, vPickedPoint.z, iCenterTexelX, iCenterTexelY, iBrushRadius);
	OutputDebugString(szDebug);

	_int iStartX = max(0, iCenterTexelX - iBrushRadius);
	_int iEndX = min(iTexelSize, iCenterTexelX + iBrushRadius);
	_int iStartZ = max(0, iCenterTexelY - iBrushRadius);
	_int iEndZ = min(iTexelSize, iCenterTexelY + iBrushRadius);

	for (_int i = iStartZ; i < iEndZ; i++) // Y (row) 방향 순회
	{
		char* pRowStart = static_cast<char*>(SubResource.pData) + (i * SubResource.RowPitch);
		_uint* pRow = reinterpret_cast<_uint*>(pRowStart); // _uint는 32비트 ARGB (4바이트) 포맷일 때 적절합니다.

		for (_int j = iStartX; j < iEndX; j++) // X (column) 방향 순회
		{
			// pRow는 i번째 행의 시작 주소를 가리키므로, pRow[j]는 j번째 픽셀의 주소입니다.
			// pRow[j]에 직접 픽셀 데이터 쓰기
			pRow[j] = D3DCOLOR_ARGB(255, 255, 0, 0); // Red
		}
	}
	m_pContext->Unmap(m_pMaskTexture2D, 0);

	/*ID3D11Resource* pGpuResource = { nullptr };
	m_pMaskSRV->GetResource(&pGpuResource);*/

	if (m_pMaskRenderTexture)
	{
		m_pContext->CopyResource(m_pMaskRenderTexture, m_pMaskTexture2D);
		//Safe_Release(pGpuResource);
	}
}

void CMapTool_Desert::Change_MaskMap_Green(_float3 vPickedPoint)
{	
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pMaskTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	//const _float fTerrainHalfSize = 256.f;
	const _int iTexelSize = 2048;
	const _float fTexelSize = 2048;

	_int iCenterTexelX = static_cast<_int>(vPickedPoint.x);

	_float fFlippedZ = fTexelSize - (vPickedPoint.z);
	_int iCenterTexelY = static_cast<_int>(fFlippedZ);


	_int iBrushRadius = static_cast<_int>(m_fRadius);

	_uint iRowSize = SubResource.RowPitch / sizeof(_uint);

	_int iStartX = max(0, iCenterTexelX - iBrushRadius);
	_int iEndX = min(iTexelSize, iCenterTexelX + iBrushRadius);
	_int iStartZ = max(0, iCenterTexelY - iBrushRadius);
	_int iEndZ = min(iTexelSize, iCenterTexelY + iBrushRadius);

	for (_int i = iStartZ; i < iEndZ; i++)
	{
		for (_int j = iStartX; j < iEndX; j++)
		{
			//_float fDistSq = (i - iCenterTexelY) * (i - iCenterTexelY) + (j - iCenterTexelX) * (j - iCenterTexelX);

			/*if (fDistSq <= m_fRadius * m_fRadius)
			{
				_uint* pRow = static_cast<_uint*>(SubResource.pData) + (i * iRowSize);
				pRow[j] = D3DCOLOR_ARGB(255, 255, 255, 255);
			}*/
			_uint* pRow = static_cast<_uint*>(SubResource.pData) + (i * iRowSize);
			pRow[j] = D3DCOLOR_ARGB(255, 0, 255, 0);
		}
	}
	m_pContext->Unmap(m_pMaskTexture2D, 0);

	// m_pMaskSRV 생성할떄 사용했던 원본ID3D11Texture2D를 다시 가져오거나
	// MapTool_Desert에서 m_pMaskSRV의 텍스처 포인터를 저장했다면 그것을 사용
	ID3D11Resource* pGpuResource = { nullptr };
	m_pMaskSRV->GetResource(&pGpuResource);

	if (pGpuResource)
	{
		m_pContext->CopyResource(pGpuResource, m_pMaskTexture2D);
		Safe_Release(pGpuResource);
	}
}

void CMapTool_Desert::Change_MaskMap_Blue(_float3 vPickedPoint)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pMaskTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	//const _float fTerrainHalfSize = 256.f;
	const _int iTexelSize = 2048;
	const _float fTexelSize = 2048;

	_int iCenterTexelX = static_cast<_int>(vPickedPoint.x);

	_float fFlippedZ = fTexelSize - (vPickedPoint.z);
	_int iCenterTexelY = static_cast<_int>(fFlippedZ);


	_int iBrushRadius = static_cast<_int>(m_fRadius);

	_uint iRowSize = SubResource.RowPitch / sizeof(_uint);

	_int iStartX = max(0, iCenterTexelX - iBrushRadius);
	_int iEndX = min(iTexelSize, iCenterTexelX + iBrushRadius);
	_int iStartZ = max(0, iCenterTexelY - iBrushRadius);
	_int iEndZ = min(iTexelSize, iCenterTexelY + iBrushRadius);

	for (_int i = iStartZ; i < iEndZ; i++)
	{
		for (_int j = iStartX; j < iEndX; j++)
		{
			_uint* pRow = static_cast<_uint*>(SubResource.pData) + (i * iRowSize);
			pRow[j] = D3DCOLOR_ARGB(255, 0, 0, 255);
		}
	}
	m_pContext->Unmap(m_pMaskTexture2D, 0);

	// m_pMaskSRV 생성할떄 사용했던 원본ID3D11Texture2D를 다시 가져오거나
	// MapTool_Desert에서 m_pMaskSRV의 텍스처 포인터를 저장했다면 그것을 사용
	ID3D11Resource* pGpuResource = { nullptr };
	m_pMaskSRV->GetResource(&pGpuResource);

	if (pGpuResource)
	{
		m_pContext->CopyResource(pGpuResource, m_pMaskTexture2D);
		Safe_Release(pGpuResource);
	}
}

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
		_float fRadius = 10.f;

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
