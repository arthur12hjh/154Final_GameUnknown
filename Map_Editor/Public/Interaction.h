#pragma once

#include "Maptool_Defines.h"
#include "DesertObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Model;
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CInteraction : public CDesertObject
{
public:
	typedef struct Prob_Interaction_Desc : public DESERT_OBJECT_DESC
	{
		_uint iInteractionID = 0;
	}PROB_INTERACTION_DESC;

public:
	_uint Get_InteractionID() const { return m_iInteractionID; }

protected:
	CInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction(const CInteraction& Prototype);
	virtual ~CInteraction() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_uint m_iInteractionID = 0;

protected:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CInteraction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END