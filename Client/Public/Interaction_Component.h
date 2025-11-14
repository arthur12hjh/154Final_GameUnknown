#pragma once
#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
class CCollider;
class COBBCollider;
NS_END

NS_BEGIN(Client)
class CInteraction_Component final : public CComponent
{
public:
	typedef struct InterractionDesc
	{
		_float4			vRoation;
		_float3			vSize;

		//	업데이트 해주면 이거 계속 불립니다.
		//	그안에서 로직구성하세요
		function<void(_float fTimeDeleta)>  CallBackFunc;
	}INTERACTION_DESC;

private:
	CInteraction_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Component(const CInteraction_Component& rhs);
	virtual ~CInteraction_Component() = default;

public:
	virtual HRESULT							Initialize_Prototype() override;
	virtual HRESULT							Initialize(void* pArg) override;

	void									Update_Intraction(_float fTimeDeleta);

	_bool									Is_Overlap(CCollider* pCollider);

	// 콜라이더의 Hit Desc 넣으면 Hit정보를 담아서 준다.
	_bool									Is_RayHit(_vector vTargetPos, _vector vDir, void* OutDesc = nullptr);

private :
	COBBCollider*							m_pOBBColiider = nullptr;
	function<void(_float fTimeDeleta)>		m_UpdateFunction = nullptr;


public:
	static		CInteraction_Component*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*					Clone(void* pArg) override;
	virtual		void						Free()  override;

};
NS_END