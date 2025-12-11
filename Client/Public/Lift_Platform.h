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
	// 플렛폼이 위로 올라가있는지
	// 아래로 내려와 있는지를 판단하는 상태값입니다.
	// 이거 진성햄이 바꿔서 써도됨
	enum class LIFT_PLATFORM_STATE { UPPER, DWON, END };

	typedef struct Lift_Platform_Desc : public ACTOR_DESC
	{
		_uint	iPlatFormID = 0;
		_float	fMoveDistance = 0.f;
	}LIFT_PLATFORM_DESC;

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
	_bool							GetPlatformMove() const { return m_bIsPaltformMove; }

	_uint							GetPlatformID() { return m_iPlatformId; }

private :
	LIFT_PLATFORM_STATE				m_ePlatform_State = { LIFT_PLATFORM_STATE::END };
	CModel*							m_pModelCom = { nullptr };

	_bool							m_bIsPaltformMove = { false };

	_float3							m_vTargetPoint = {};
	_float							m_vLerpSpeed = { 0.5f };
	
	_float3							m_vRootPos = {};

	// 이거는 리프트가 올라가는 거리입니다.
	// 이거도 형이 툴에서 수정해서 같이 넘겨서 받으면될거같음
	_uint							m_iPlatformId = 0;
	_float							m_fMoveDistance = { 45.f };
	
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