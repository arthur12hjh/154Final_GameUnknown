#pragma once

#include "Maptool_Defines.h"
#include "DesertObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Tool_Map)

class CSpawnBox final : public CDesertObject
{
public:
	typedef struct Monster_Desc : public DESERT_OBJECT_DESC
	{
		_uint				iMonsterID = 0;
	}MONSTER_DESC;

private:
	CSpawnBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawnBox(const CSpawnBox& Prototype);
	virtual ~CSpawnBox() = default;

public:
	_uint Get_MonsterID() { return m_iMonsterID; }

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

private:
	_uint	m_iMonsterID = 0;

public:
	static CSpawnBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDesertObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END