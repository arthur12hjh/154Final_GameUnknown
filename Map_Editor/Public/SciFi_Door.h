#pragma once

#include "Maptool_Defines.h"
#include "DesertObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)

class CSciFi_Door final : public CDesertObject
{
public:
	enum SCIFI_DOOR_STATE
	{
		OPEN, CLOSE, END
	};

private:
	CSciFi_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSciFi_Door(const CSciFi_Door& Prototype);
	virtual ~CSciFi_Door() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	SCIFI_DOOR_STATE m_eCurState = { SCIFI_DOOR_STATE::END };
	SCIFI_DOOR_STATE m_ePrevState = { SCIFI_DOOR_STATE::END };
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CSciFi_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDesertObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END