#pragma once

#include "MapTool_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Tool_Map)

class CBody_Player final : public CPartObject
{
public:
	typedef struct tagBody_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
	}BODY_PLAYER_DESC;
private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	_bool isFinish_Att();

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

private:
	const _uint* m_pParentState = { nullptr };
	_bool				m_isAnimFinish = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END