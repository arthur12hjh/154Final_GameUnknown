#pragma once
#include "Client_Defines.h"
#include "GameStruct.h"
#include "Entity.h"

NS_BEGIN(Engine)
class CModel;
struct AnimNotify;
NS_END

NS_BEGIN(Client)

class CCharacter abstract : public CEntity
{
protected :
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& Prototype);
	virtual ~CCharacter() = default;

public:
	// 파트오브젝트 콜라이더 켜는 노티파이 끌고 들어가는 함수.
	virtual void					Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, ANIM_NOTIFY){};

	// Clone 한 GameObject 켜는 노티파이 끌고 들어가는 함수
	virtual void					Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference) {};
	
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;
	virtual HRESULT					CallNotify(_uint iNotiType, const AnimNotify* pNotify);

	void							SetActionEnable(_bool bFlag);
	const	_bool					GetActionEnable();

	// 캐릭터 잡기 패턴이나 어디다가 붙여야할떄 사용
	void							SocketAttachment(const _float4x4* pAttachedMatrix);
	void							DetachedMatrix();

	// 데미지 함수
	virtual HRESULT					Damaged(void* pArg) { return S_OK; }
	virtual HRESULT					ActionSuccess(void* pArg) { return S_OK; }

	void							Set_Position(_vector vPosition);
	CModel*							Get_BodyModelCom() { return m_pBodyModelCom; }

	_float							Get_ImpactForce();
	void							Set_ImpactForce(_float fImpactForce);

	_vector							Get_Position();

	const _float4x4*				Get_WorldMatrixPtr();

protected :
	_bool							m_bIsActionEnabled = { true };
	const _float4x4*				m_pSocketMatrix = { nullptr };

	class CGameManager*				m_pGameManager = { nullptr };
	class CCollider*				m_pColliderCom = { nullptr };
	class CNotify*					m_pNotifyCom = { nullptr };

	_float							m_fImpactForce = {};
	CCharacterController*			m_pCCT = { nullptr };

public:
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

};
NS_END