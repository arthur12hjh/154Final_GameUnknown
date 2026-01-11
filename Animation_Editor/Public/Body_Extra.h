#pragma once

#include "Animation_Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CBody_Extra final : public CPartObject
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

	typedef struct tagBody_Extra_Desc : public CPartObject::PARTOBJECT_DESC
	{
		_wstring szModelTag;
		void* pBodyPtr = { nullptr };
	}BODY_EXTRA_DESC;
private:
	CBody_Extra(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Extra(const CBody_Extra& Prototype);
	virtual ~CBody_Extra() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);

	HRESULT								Mapping_Shader_Material(_uint iIdx);

	_bool isFinish_Att();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:

private:
	CModel* m_pBodyModelCom = { nullptr };

	_wstring			m_szModelTag;
	_float m_fWeightTest;
	
	_uint				m_iAnimationIndex;

	_bool				m_isAnimFinish = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Bind_BoneToPartBody(void* pArg);

public:
	static CBody_Extra* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END