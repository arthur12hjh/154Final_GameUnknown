#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{
private:
	CShadow();
	virtual ~CShadow() = default;

public:
	HRESULT Initialize(void* pArg);
	HRESULT Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc);
	HRESULT Bind_Shader_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType);
	HRESULT Bind_Shader_Resource_Cascade(class CShader* pShader, const _char* pConstantName, D3DTS eType);
	HRESULT Bind_Cascade_Ends(class CShader* pShader, const _char* pConstantName, const _char* pConstantName2);
	_float* Get_CascadeEnds() { return m_fCascadeEnds; }

	void Update(_float fTimeDelta);
	void Seperate_CascadeFrustum();
	void Calc_CascadeMatrices();
public:


private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	
	_float4x4				m_TransformationMatrices[ENUM_CLASS(D3DTS::END)] = {};

	//캐스케이드 관련 정보들
	_float					m_fCascadeEnds[CASCADE_LEVEL + 1];
	_float4					m_vCascadeNearCorner[CASCADE_LEVEL][4];
	_float4					m_vCascadeFarCorner[CASCADE_LEVEL][4];
	//캐스케이드 갯수만큼 들고 있게 처리.
	_float4x4				m_CasCadeTransformationViewMatrices[CASCADE_LEVEL] = {};
	_float4x4				m_CasCadeTransformationProjMatrices[CASCADE_LEVEL] = {};
	_float4					m_vCascadePositions[CASCADE_LEVEL] = {};

	SHADOW_LIGHT_DESC		m_tShadowLightDesc = {};
	
public:
	static CShadow* Create();
	virtual void Free() override;
};

NS_END