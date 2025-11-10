#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CVillage_Mou final : public CGameObject
{
public:
	class CNavigation* Get_Navigation() const
	{
		return m_pNavigationCom;
	}

private:
	CVillage_Mou(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVillage_Mou(const CVillage_Mou& Prototype);
	virtual ~CVillage_Mou() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CVillage_Mou* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END