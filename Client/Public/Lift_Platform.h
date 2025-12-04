#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"
#include "Actor.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)
class CLift_Platform final : public CActor
{
public :
	enum class LIFT_PLATFORM_STATE { UPPER, DWON, END };

private:
	CLift_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLift_Platform(const CLift_Platform& Prototype);
	virtual ~CLift_Platform() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

	_bool							SetPlatformMove(LIFT_PLATFORM_STATE eState);

private :
	LIFT_PLATFORM_STATE				m_ePlatform_State = { LIFT_PLATFORM_STATE::END };
	CModel*							m_pModelCom = { nullptr };

	_bool							m_bIsPaltformMove = {};

	_float3							m_vTargetPoint = {};
	_float							m_vLerpSpeed = { 1.f };
	
	_float3							m_vRootPos = {};
	_float							m_fMoveDistance = { 55.f };

private :
	void							LerpTargetPoint(_float fTimeDelta);



private:
	HRESULT							Ready_Components(const _tchar* pComponentTag);
	HRESULT							Bind_ShaderResources();

public:
	static CLift_Platform*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END