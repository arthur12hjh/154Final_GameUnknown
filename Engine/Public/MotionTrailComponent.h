#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)
class CModel;
class CShader;

class ENGINE_DLL CMotionTrailComponent final : public CVIBuffer_Instance
{
public :
	typedef struct MotionTrailComDesc
	{
		CModel*					pModel;
		CModel*					pPreBoneModel;

		const _float4x4*		pTransform;
		_float					fUpdateTime = { 0.4f };
		_float					fLifeTime;
	}MOTION_TRAIL_COMPONENT_DESC;

private :
	CMotionTrailComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMotionTrailComponent(const CMotionTrailComponent& rhs);
	virtual ~CMotionTrailComponent() = default;

public:
	HRESULT								Initialize_Prototype(_int iNum);
	virtual HRESULT						Initialize(void* pArg) override;

public:
	void								EnableMotionTrail(_bool bIsEnable);
	_bool								IsEnableMotionTrail();

	void								Update_Trail(_float fTimeDelta);
	virtual HRESULT						Render();

	void								SetMotionTrailColor(_float4 vColor);
	void								SetRimLight(_float fRimLightPower, _float fRimLightIntensity);
	void								SetTrailCreateCoolDown(_float fCoolDown) { m_vUpdateTime.y = fCoolDown; }
private:
	_bool								m_bEnableMotionTrail = { false };
	_uint								m_iNumBones = {};
	_uint								m_TrailCount = {};
	_uint								m_iActiveTrailCount = {};
	_float2								m_vUpdateTime = {};

	CModel*								m_pModel = { nullptr };
	CModel*								m_pPreBoneModel = { nullptr };

	const _float4x4*					m_pTransformMatrix = {};
	VTX_MOTION_TRAIL_INSTANCE_MODEL*	m_pInstanceVertices = { nullptr };

	CShader*							m_pShader = { nullptr };
	D3D11_BOX							m_CopyBoxSize;
	ID3D11Buffer*						m_pMotionTrailBuffers = { nullptr };
	ID3D11ShaderResourceView*			m_pMotionTrailSRV = { nullptr };

	// m_vColor[0] =>	트레일 시작 컬러
	// m_vColor[1] =>	트레일 끝 컬러
	_float								m_fRimLightPower = 2.f;
	_float								m_fRimLightIntensity = 1.f;
	_float4								m_vColor[2] = {};

private :
	HRESULT								Bind_ShaderResource();
	HRESULT								Bind_Resource(_uint iMeshIndex);

	HRESULT								Ready_TarilBuffer();
	HRESULT								Ready_Shader();

public:
	static CMotionTrailComponent*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iNum = 50);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};
NS_END