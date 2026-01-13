#pragma once
#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
class CShader;
NS_END

NS_BEGIN(Tool_Map)
class CCameraTriggerBox : public CGameObject
{
public:
	typedef struct TriggerBoxDesc : public GAMEOBJECT_DESC
	{
		CAMERA_TYPE eCameraType;
	}CAMERA_TRIGGER_BOX_DESC;

private:
	CCameraTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraTriggerBox(const CCameraTriggerBox& Prototype);
	virtual ~CCameraTriggerBox() = default;

public:
	CAMERA_TYPE Get_CameraType() const { return m_eCameraType; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

private:
	CCollider* m_pCollider = { nullptr };
	CAMERA_TYPE m_eCameraType = {CAMERA_TYPE::END};

	_bool					m_bIsRender = { false };
private:
	HRESULT					Ready_Components();

public:
	static CCameraTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END