#pragma once

#include "Animation_Editor_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CGigas final : public CContainerObject
{
private:
	CGigas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGigas(const CGigas& Prototype);
	virtual ~CGigas() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Animation(const _char* szAnimationTag);
	void Set_Animation(_uint iAnimationIndex);

private:
	class CTool_Manager* m_pToolManager = { nullptr };
	class CBody_Gigas* m_pPart_Body = { nullptr };

	CCollider* m_pColliderCom = { nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CGigas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END