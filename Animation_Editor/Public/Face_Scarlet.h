#pragma once

#include "Animation_Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBone;
NS_END

NS_BEGIN(Animation_Editor)

class CFace_Scarlet final : public CPartObject
{
public:
	enum class FACE_MATERIAL {
		//이 4개는 그냥 둬야함
		DEFAULT, SHADOW, RIMLIGHT, MOTIONBLUR,

		MI_CH_M_NA_961_Head, MI_CH_M_NA_961_Eyebrow, MI_CH_M_NA_961_Eyes,
		MI_CH_M_NA_961_Lens, MI_CH_M_NA_961_Eyelashes, MI_CH_M_NA_961_Tearline,
		MI_CH_M_NA_961_Eyeshadow, MI_CH_M_NA_961_EyeBlend, MI_CH_M_NA_961_NoseShadow,
		MI_CH_M_NA_961_Teeth
	};

public:
	typedef struct tagFace_Scarlet_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		void* pBodyPtr = { nullptr };
	}FACE_SCARLET_DESC;

private:
	CFace_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFace_Scarlet(const CFace_Scarlet& Prototype);
	virtual ~CFace_Scarlet() = default;

public:
	HRESULT								Mapping_Shader_Material(_uint iIdx);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	CModel* m_pBodyModelCom = { nullptr };

private:
	const				_uint* m_pParentState = { nullptr };



private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Bind_BoneToPartBody(void* pArg);

public:
	static CFace_Scarlet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END