#pragma once
#include "Client_Defines.h"
#include "Prob_Interaction.h"

// 일단은 Final인데 아이템 추가되고 많아지면
// Final에서 바꾸고 작업해도 됩니다.
// 이거는 지금 땅에떨어지는 데이터 표현만 가능하게 할거임

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)
class CEffect;
class CItem final : public CProb_Interaction
{
public:
	typedef struct Item_Desc : public PROB_INTERACTION_DESC
	{
		_uint						iItemID;
		_int						fAmount;

		_float3						fDropPoint;
		_float						fDropForce;
		_float3						vParentLook;
		_bool						isHemiSphere = { false };
	}ITEM_DESC;

protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& Prototype);
	virtual ~CItem() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

protected:
	CModel*							m_pModelCom = { nullptr };
	CEffect*						m_pEffect = { nullptr };

	ITEM_DSEC						m_ItemData = {};
	_float							m_fAmount = {};
	_float							m_fDropForce = {};
	_float3							m_CurvePoins[5] = {};

	_bool							m_bIsLerpAnimation = { false };
	_float2							m_fLerpTime = { 0.f, 1.f };
	_bool							m_isHemiSphere = { false };
	_float3							m_vParentLook = {};

protected:
	virtual HRESULT					Begin_OverlapCallBack() override;
	virtual HRESULT					End_OverlapCallBack() override;
	virtual void					Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject) override;

private :
	HRESULT							ADD_Components(const ACTOR_DESC& Desc);
	HRESULT							Bind_ShaderResources();

public:
	static		CItem*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;
};
NS_END