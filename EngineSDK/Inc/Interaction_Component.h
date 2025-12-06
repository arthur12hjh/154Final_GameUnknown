#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CCollider;
class CGameObject;
class COBBCollider;

class ENGINE_DLL CInteraction_Component final : public CComponent
{
public:
	typedef struct InterractionDesc
	{
		_float4			vRoation;
		_float3			vSize;

		//	업데이트 해주면 이거 계속 불립니다.
		//	그안에서 로직구성하세요
		function<void(CGameObject*)>	InteractionEvent;
		function<void()>				BeginCallBackFunc;
		function<void()>				EndCallBackFunc;
	}INTERACTION_DESC;

private:
	CInteraction_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Component(const CInteraction_Component& rhs);
	virtual ~CInteraction_Component() = default;

public:
	virtual HRESULT								Initialize_Prototype() override;
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								SetOwner(CGameObject* pGameObject);

	void										Update_Com();
	void										Action_InteractionEvent(CGameObject* pGameObject);

	void										SetInteractionHitType(HIT_TYPE eHitType);
	void										ADD_InteractionIgnoreObject(HIT_TYPE eHitType);
	void										ADD_InteractionOnlyHitObject(HIT_TYPE eHitType);

	const _float3&								Get_CenterPos();

#ifdef _DEBUG
	virtual HRESULT								Render() override;
#endif

	_bool										Is_Overlap(CCollider* pCollider);

	// 콜라이더의 Hit Desc 넣으면 Hit정보를 담아서 준다.
	_bool										Is_RayHit(_vector vTargetPos, _vector vDir, void* OutDesc = nullptr);

private :
	COBBCollider*								m_pOBBColiider = nullptr;

	function<void()>							m_BeginCallBackFunc = nullptr;
	function<void(CGameObject* pGameObject)>	m_InteractionFunc = nullptr;
	function<void()>							m_EndCallBackFunc = nullptr;

private :
	HRESULT										Ready_Components(const INTERACTION_DESC& Desc);

public:
	static		CInteraction_Component*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*						Clone(void* pArg) override;
	virtual		void							Free()  override;

};
NS_END