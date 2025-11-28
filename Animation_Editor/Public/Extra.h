#pragma once

#include "Animation_Editor_Defines.h"
#include "Entity.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CExtra final : public CEntity
{
public:
	typedef struct tagExtra_Desc
	{
		_wstring szModelTag;
	}EXTRA_DESC;

private:
	CExtra(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExtra(const CExtra& Prototype);
	virtual ~CExtra() = default;

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
	class CBody_Extra* m_pPart_Body = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

	_wstring m_szModelTag;


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CExtra* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END