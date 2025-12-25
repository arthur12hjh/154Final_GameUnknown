#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 1. 다음 레벨에 대한 자원을 로드한다. */

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUIResourceManager;

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();
	void Output();

public:
	_bool isFinished() const {
		return m_isFinished;
	}

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL				m_eNextLevelID = { LEVEL::END };

	HANDLE				m_hThread = {};

	_wstring			m_strMessage = {};
	_bool				m_isFinished = { false };

	CRITICAL_SECTION	m_CriticalSection = {};

	CGameInstance*		m_pGameInstance = { nullptr };
	class CGameManager*		m_pGameManager = { nullptr }; 
private:
	HRESULT Loading_For_Logo();

	HRESULT Loading_For_Scarlet(void* pArg);

#pragma region Scarlet
	HRESULT	Loading_For_Scarlet();

	HRESULT	Loading_For_Scarlet_SKY(void* pArg);
	HRESULT Loading_For_Map_Scarlet_Building(void* pArg);
	HRESULT Loading_For_Map_Scarlet_Environment(void* pArg);
	HRESULT Loading_For_Map_Scarlet_Environment2(void* pArg);
#pragma endregion

#pragma region GamePlay
	HRESULT Loading_For_GamePlay();
	HRESULT Loading_For_GamePlay_Player(void* pArg);
	HRESULT Loading_For_GamePlay_Mesh(void* pArg);
	HRESULT Loading_For_GamePlay_Shader(void* pArg);
	HRESULT Loading_For_GamePlay_Effect(void* pArg);
	HRESULT Loading_For_GamePlay_InstanceMesh(void* pArg);
	HRESULT Loading_For_GamePlay_Components(void* pArg);

	HRESULT Loading_For_GamePlay_Map(void* pArg);

	HRESULT Loading_For_GamePlay_Map_DesertA(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertB(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertC(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertD(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertE(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertF(void* pArg);

	HRESULT Loading_For_Desert_Deco(void* pArg);
	HRESULT Loading_For_Desert_Building_Ruin(void* pArg);
	HRESULT Loading_For_Desert_Building_Ruin_Col(void* pArg);
	HRESULT Loading_For_Desert_Building_Ruin_B(void* pArg);
	HRESULT Loading_For_Desert_Building_Ruin_B_Col(void* pArg);
	HRESULT Loading_For_Desert_Deco_Camp(void* pArg);
	HRESULT Loading_For_Desert_Deco_Corpse_And_Container(void* pArg);
	HRESULT Loading_For_Desert_Deco_Container_Col(void* pArg);
	HRESULT Loading_For_Desert_Deco_Fence_And_Ruin(void* pArg);
	HRESULT Loading_For_Desert_Deco_Fence_And_Ruin_Col(void* pArg);
	HRESULT Loading_For_Desert_Deco_Vehicle_And_Wheel(void* pArg);
	HRESULT Loading_For_Desert_Deco_Building_And_Trash(void* pArg);
	HRESULT Loading_For_Desert_Deco_Sign_And_Crane(void* pArg);

	HRESULT Loading_For_Desert_Environment_Tree1(void* pArg);
	HRESULT Loading_For_Desert_Environment_Tree2(void* pArg);
	HRESULT Loading_For_Desert_Environment_Grass1(void* pArg);

	HRESULT Loading_For_Desert_Archi(void* pArg);
	HRESULT Loading_For_Desert_Archi_Col(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertA_Col(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertB_Col(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertC_Col(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertD_Col(void* pArg);
	HRESULT Loading_For_GamePlay_Map_DesertF_Col(void* pArg);
	HRESULT Loading_For_Desert_Bridge(void* pArg);
	HRESULT Loading_For_Desert_Bridge_Col(void* pArg);
#pragma endregion


//#pragma region Level_Scarlet
//	HRESULT Loading_For_Scarlet_Mesh(void* pArg);
//	HRESULT Loading_For_Scarlet_InstanceMesh(void* pArg);
//	HRESULT Loading_For_Scarlet_Shader(void* pArg);
//	HRESULT Loading_For_Scarlet();
//	HRESULT Loading_For_Scarlet_Map(void* pArg);
//	HRESULT Loading_For_Scarlet_Map_Environment(void* pArg);
//	HRESULT Loading_For_Scarlet_Map_Building(void* pArg);
//#pragma endregion

	HRESULT Loading_UI_For_Logo_Level();
	HRESULT Loading_UI_For_GamePlay_Level(void* pArg);
	HRESULT Loading_UI_For_Combat_HUD_Vitals(void* pArg);
	HRESULT Loading_UI_For_Combat_HUD_Skills(void* pArg);
	HRESULT Loading_UI_For_World(void* pArg);
	HRESULT Loading_UI_For_Popup(void* pArg);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END