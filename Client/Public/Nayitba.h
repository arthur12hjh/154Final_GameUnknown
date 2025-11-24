#pragma once
#include "Character.h"

NS_BEGIN(Engine)
class CAISenceComponent;
class CAIController;
NS_END

NS_BEGIN(Client)
class CMonsterFSM;

class CNayitba final : public CCharacter
{
public :
	typedef struct Nayitba_Desc : GAMEOBJECT_DESC
	{
		_uint						iMonsterID = {};
	}NAYITBA_DESC;

private :
	CNayitba(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNayitba(const CNayitba& Prototype);
	virtual ~CNayitba() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

	_uint							GetMonsterID();
	const list<CGameObject*>*		GetTargetList();

	const NAYTIBA_NETWORK_DESC*		GetStaticMonsterData() { return m_pInitMonsterInfo; }
	const NAYTIBA_DESC&				GetMonsterData() { return m_MonsterInfo; }

	//레퍼런스 카운트 증가
	CAIController*					GetController();

private:
	CAISenceComponent*				m_pAISenceCom = { nullptr };
	CAIController*					m_pAIController = { nullptr };

	_uint							m_iMonsterID = {};
	const NAYTIBA_NETWORK_DESC*		m_pInitMonsterInfo = {};
	NAYTIBA_DESC					m_MonsterInfo = {};

	string							m_szEntryAnim = {};

private :
	HRESULT							ADD_Components();
	HRESULT							ADD_PartObjects();

	void							BattleEvent(CGameObject* pTarget, _bool bIsBattle);

public:
	static	CNayitba*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END