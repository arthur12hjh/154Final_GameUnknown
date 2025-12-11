#pragma once

#include "Maptool_Defines.h"
#include "DesertObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CDesert_Grass final : public CDesertObject
{
private:
	CDesert_Grass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDesert_Grass(const CDesert_Grass& Prototype);
	virtual ~CDesert_Grass() = default;

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

public:
	static CDesert_Grass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDesertObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END