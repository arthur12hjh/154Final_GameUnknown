#pragma once

#include "Client_Defines.h"
#include "Prob_Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CTop_Roof final : public CProb_Interaction
{
public:
	enum TOP_ROOF_STATE
	{
		IDLE, START, END
	};

private:
	CTop_Roof(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTop_Roof(const CTop_Roof& Prototype);
	virtual ~CTop_Roof() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	TOP_ROOF_STATE m_eCurState = { TOP_ROOF_STATE::END };
	TOP_ROOF_STATE m_ePrevState = { TOP_ROOF_STATE::END };
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

	virtual HRESULT					    Begin_OverlapCallBack() override;
	virtual HRESULT					    End_OverlapCallBack() override;
	virtual void					    Excute_CallBack(CGameObject* pActionObject) override;

public:
	static CTop_Roof* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END