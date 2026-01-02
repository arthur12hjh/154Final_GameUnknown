#pragma once
#include "Character.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Engine)
class CBeatSaberSpawner final : public CCharacter
{
private:
	CBeatSaberSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeatSaberSpawner(const CBeatSaberSpawner& rhs);
	virtual ~CBeatSaberSpawner() = default;

public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize(void* pArg);

	virtual void						Priority_Update(_float fTimeDelta);
	virtual void						Update(_float fTimeDelta);
	virtual void						Late_Update(_float fTimeDelta);

	void								Load_BeatData(const char* szSpawnNoteFileData);

private:
	CCollider*							m_pColliderCom = { nullptr };
	queue<NOTE_DATA_DESC>				m_SpawnList = {};
	
	_bool								m_bIsPlay = true;

private:
	HRESULT								Ready_Components();
	void								Trigger_SpawnEvent();

public:
	static	CBeatSaberSpawner*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};
NS_END