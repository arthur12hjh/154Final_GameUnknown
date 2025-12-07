#pragma once

#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Model;
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CDesertObject : public CGameObject
{
public:
	typedef struct DesertObjectDesc : public GAMEOBJECT_DESC
	{
		const wchar_t* pComponentTag;
		unsigned int	iObjectId = 0;

	}DESERT_OBJECT_DESC;

public:
	const _tchar* Get_ComponentTag() const { return m_ComponentTag; }
	_uint Get_ObjectID() const { return m_iObjectID; }

protected:
	CDesertObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDesertObject(const CDesertObject& Prototype);
	virtual ~CDesertObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Instance_Model* m_pInstanceModelCom = { nullptr };

	_uint	m_iObjectID = 0;
	_tchar m_ComponentTag[256] = {};

protected:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CDesertObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END