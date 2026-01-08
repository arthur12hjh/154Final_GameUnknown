#pragma once
#include "Character.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Engine)
class CBeatSaberSpawner final : public CCharacter
{
public :
	typedef struct BeatSaberSpanwerDesc : public GAMEOBJECT_DESC
	{
		const _float4x4*			pPlayerTransform = { nullptr };
	}BEATSABER_SPAWNER_DESC;

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

	void								Load_BeatData(const char* szSpawnNoteFileData, _float fSongTime, _uint iBPM, _float fDelay);

private:
	CCollider*							m_pColliderCom = { nullptr };
	queue<NOTE_DATA_DESC>				m_SpawnList = {};
	
	const _float4x4*					m_pPlayerTransform = { nullptr };
	_bool								m_bIsPlay = true;

	_float								m_fTimeAcc = 0.f;
	_float								m_fDelay = 3.f;
	_float								m_fSongTime = 0.f;
	_float								m_fNoteToValiTime = 0.f;
	_float								m_fSongLength = 0.f;

private:
	HRESULT								Ready_Components();
	void								Trigger_SpawnEvent();

public:
	static	CBeatSaberSpawner*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};
NS_END