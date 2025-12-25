#pragma once

#include "Maptool_Defines.h"
#include "Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CCanBox final : public CInteraction
{
public:
	enum CANBOX_STATE
	{
		OPEN, END
	};

private:
	CCanBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanBox(const CCanBox& Prototype);
	virtual ~CCanBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CANBOX_STATE m_eCurState = { CANBOX_STATE::END };
	CANBOX_STATE m_ePrevState = { CANBOX_STATE::END };
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CCanBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END