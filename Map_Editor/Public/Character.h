#pragma once
#include "MapTool_Defines.h"
#include "../../Client/Public/GameStruct.h"
#include "Entity.h"

NS_BEGIN(Tool_Map)
class CCharacter abstract : public CEntity
{
protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& Prototype);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

	void							SetActionEnable(_bool bFlag);
	const	_bool					GetActionEnable();

	// 캐릭터 잡기 패턴이나 어디다가 붙여야할떄 사용
	void							SocketAttachment(const _float4x4* pAttachedMatrix);
	void							DetachedMatrix();

	virtual HRESULT					Damaged(void* pArg) { return S_OK; }

protected:
	_bool							m_bIsActionEnabled = { true };
	const _float4x4* m_pSocketMatrix = { nullptr };

	class CGameManager* m_pGameManager = { nullptr };
	CCharacterController* m_pCCT = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;

};
NS_END