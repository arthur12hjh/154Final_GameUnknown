#pragma once

#include "Maptool_Defines.h"
#include "Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CLift_Controller final : public CInteraction
{
public:
	typedef struct Lift_Controller_Desc : public Prob_Interaction_Desc
	{
		_bool bIsControllerType = 0;
		_uint iPlatformID = 0;
	}LIFT_CONTROLLER_DESC;

	enum LIFT_STATE
	{
		LIFT_PULL, LIFT_PUSH, LIFT_END
	};

public:
	_bool Get_ControllerType() const { return m_bIsControllerType; }
	_uint Get_PlatformID() const { return m_iPlatformID; }

private:
	CLift_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLift_Controller(const CLift_Controller& Prototype);
	virtual ~CLift_Controller() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LIFT_STATE m_eCurState = { LIFT_STATE::LIFT_END };
	LIFT_STATE m_ePrevState = { LIFT_STATE::LIFT_END };

	_uint m_iPlatformID = 0;
	_bool m_bIsControllerType = { false };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CLift_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END