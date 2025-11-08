#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)
class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager);

private:
	CGameManager() = default;
	virtual ~CGameManager() = default;

public :
	HRESULT						Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	// 캐릭터 바인딩
	// 어떤 캐릭터를 플레이어가 지금 조종중인지를 바인딩
	// 이전에 플레이어 바인딩되어있던건 참조 카운트 감소하고
	// 새로운 플레이어 바인딩되면 참조카운트 증가함
	void						Bind_GameCharacter(CGameObject* pCharacter);

	// 플레이어블 캐릭터 꺼내오기
	// 캐릭터 꺼내오면 레퍼런스 증가함 
	// 다 쓰면 감소시켜주세요
	CGameObject*				GetGameCharacter();

private :
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;

	CGameObject*				m_pPlayer = nullptr;

private :
	HRESULT						Setting_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);


public:
	virtual void				Free() override;

};
NS_END