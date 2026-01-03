#pragma once

#include "Maptool_Defines.h"
#include "DesertObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Tool_Map)

class CNpc final : public CDesertObject
{
public:
	typedef struct Npc_Desc : public DESERT_OBJECT_DESC
	{
		_uint				iNpcID = 0;
	}NPC_DESC;

private:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc(const CNpc& Prototype);
	virtual ~CNpc() = default;

public:
	_uint Get_NpcID() { return m_iNpcID; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();
	void	SetCullingCollider(_uint iObjectID);
	_uint	Object_Number(const _tchar* pComponentTag);

private:
	_uint	m_iNpcID = 0;

public:
	static CNpc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDesertObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END