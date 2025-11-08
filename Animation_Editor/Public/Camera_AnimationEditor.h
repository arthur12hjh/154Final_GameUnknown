#pragma once

#include "Animation_Editor_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Animation_Editor)

class CCamera_AnimationEditor final : public CCamera
{
public:
	typedef struct tagCamera_AnimationEditor : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor;		// 마우스 회전 감도
	}CAMERA_DESC;
private:
	CCamera_AnimationEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_AnimationEditor(const CCamera_AnimationEditor& Prototype);
	virtual ~CCamera_AnimationEditor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Initialize_Position();

private:
	class CTool_Manager* m_pTool_Manager = { nullptr };


	_float						m_fMouseSensor = {};		// 마우스 회전 감도


public:
	static CCamera_AnimationEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END