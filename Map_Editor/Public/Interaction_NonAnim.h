#pragma once

#include "Maptool_Defines.h"
#include "Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CInteraction_NonAnim final : public CInteraction
{
private:
	CInteraction_NonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_NonAnim(const CInteraction_NonAnim& Prototype);
	virtual ~CInteraction_NonAnim() = default;

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
	//void	SetCullingCollider(_uint iObjectID);
	//_uint	Object_Number(const _tchar* pComponentTag);

public:
	static CInteraction_NonAnim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END