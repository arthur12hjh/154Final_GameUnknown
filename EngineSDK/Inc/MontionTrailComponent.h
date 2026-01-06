#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)
class CModel;
class CShader;

class ENGINE_DLL CMontionTrailComponent final : public CVIBuffer_Instance
{
public :
	typedef struct MotionTrailComDesc
	{
		CModel*					pModel;
		const _float4x4*		pTransform;
		_float					fLifeTime;
	}MOTION_TRAIL_COMPONENT_DESC;

private :
	CMontionTrailComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMontionTrailComponent(const CMontionTrailComponent& rhs);
	virtual ~CMontionTrailComponent() = default;

public:
	HRESULT								Initialize_Prototype(_int iNum);
	virtual HRESULT						Initialize(void* pArg) override;

public:
	void								EnableMotionTrail(_bool bIsEnable);
	_bool								IsEnableMotionTrail();

	void								Update_Trail(_float fTimeDelta);
	virtual HRESULT						Render();

private:
	_bool								m_bEnableMotionTrail = { false };
	_uint								m_iNumBones = {};
	_uint								m_TrailCount = {};

	CModel*								m_pModel = { nullptr };
	const _float4x4*					m_pTransformMatrix = {};
	VTX_MOTION_TRAIL_INSTANCE_MODEL*	m_pInstanceVertices = { nullptr };

	CShader*							m_pShader = { nullptr };
	D3D11_BOX							m_CopyBoxSize;
	ID3D11Buffer*						m_pMotionTrailBuffers = { nullptr };
	ID3D11ShaderResourceView*			m_pMotionTrailSRV = { nullptr };

	// m_vColor[0] =>	트레일 시작 컬러
	// m_vColor[1] =>	트레일 끝 컬러
	_float4								m_vColor[2] = {};

private :
	HRESULT								Bind_Resource(_uint iMeshIndex);
	HRESULT								Ready_TarilBuffer();
	HRESULT								Ready_Shader();

public:
	static CMontionTrailComponent*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iNum = 50);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};
NS_END