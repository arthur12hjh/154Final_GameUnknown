#pragma once

#include "Maptool_Defines.h"
#include "Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CDisplayBox final : public CInteraction
{
public:
	enum Box_STATE
	{
		OPEN, IDLE
	};

private:
	CDisplayBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDisplayBox(const CDisplayBox& Prototype);
	virtual ~CDisplayBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	Box_STATE m_eCurState = { Box_STATE::IDLE };
	Box_STATE m_ePrevState = { Box_STATE::IDLE };
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CDisplayBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END