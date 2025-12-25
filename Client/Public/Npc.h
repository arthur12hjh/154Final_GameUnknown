#pragma once
#include "Character.h"

NS_BEGIN(Engine)
class CCollider;
class CGameObject;
class CCharacterController;
class CAIController;
class CRigidBody;
NS_END

NS_BEGIN(Client)
struct Npc_Data_Desc;
class CInteractionUIBinder;

class CNpc final : public CCharacter
{
public :
	typedef struct Npc_Desc : public GAMEOBJECT_DESC
	{
		_uint				iNpcID;
	}NPC_DESC;

private :
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc(const CNpc& Prototype);
	virtual ~CNpc() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() override;

private:
	const Npc_Data_Desc*		m_NpcDesc = {};

	CInteractionUIBinder*		m_pInteractionCom = { nullptr };
	CAIController*				m_pAIController = { nullptr };

private :
	HRESULT						Ready_PartObjects();
	HRESULT						Ready_Components();

	void						Begin_Interaction();
	void						Excute_Interaction(_float fTimeDelta, CGameObject* pActionObject);
	void						End_Interaction();

public:
	static CNpc*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END