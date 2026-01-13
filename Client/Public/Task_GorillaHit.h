#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CBossBlackBoard;

struct Default_Damage_Desc;
struct Character_Skill_Desc;

class CTask_GorillaHit final : public CTask
{
private :
	CTask_GorillaHit();
	virtual ~CTask_GorillaHit() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private:
	CNaytiba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };

	string								m_szAnimationName = {};
	const Default_Damage_Desc*			m_pHit_Data = { nullptr };

	_bool								m_bIsHitRepulse = { false };
	_float3								m_vImpactDir = {};
	_float								m_fImpactForce = {};

private:
	void								Refresh_HitMotion();
	void								Damaged_Attack(const Character_Skill_Desc* pData, _vector vDir);
	void								Play_HitSound(_uint iHitIndex);

public:
	static	CTask_GorillaHit*			Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END