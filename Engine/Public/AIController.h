#pragma once
#include "GameObject.h"

// 이친구는 FSM제어 하는 녀석입니다.
// BT복잡한 애들만 사용할거임
NS_BEGIN(Engine)
class ENGINE_DLL CAIController abstract : public CGameObject
{
public :
	typedef struct AI_ControllerDesc
	{
		CGameObject*				pOwner;
	}AI_CONTROLLER_DESC;

protected:
	CAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAIController(const CAIController& Prototype);
	virtual ~CAIController() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

protected :
	CGameObject*					m_pOwner = { nullptr };

public:
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

};
NS_END