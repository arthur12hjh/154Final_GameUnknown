#pragma once

#include "Maptool_Defines.h"
#include "DesertObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CTop_Roof final : public CDesertObject
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
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CTop_Roof* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDesertObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END