#pragma once
#include "VIBuffer.h"
#include "Trail.h"

NS_BEGIN(Engine)
class ENGINE_DLL CWaveTrail final :
	public CComponent
{
public:
	typedef struct tagWaveTrailHighLow : public CTrail::TRAILHIGHLOW {
		_float		fPow;
		_float		fSpeed;
	}WAVETRAILHIGHLOW;
protected:
	CWaveTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaveTrail(const CWaveTrail& rhs);
	virtual ~CWaveTrail() = default;

public:
	void Initialize_WaveTrail();
	void Update_WaveTrail(_fmatrix matCurrentWorld, _float fTimeDelta, _bool bMakeWaveTrail = false);
	virtual HRESULT Render() override;
	_bool IsRenderable();

private:
	_float4			m_vHigh = { 0.f, -1.f, 0.f, 1.f };
	_float4			m_vLow = { 0.f, 0.f, 0.f, 1.f };
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };
	VTXTRAIL* m_pVTXTrails = { nullptr };
	_vector* m_pPostions = { nullptr };
	_uint			m_iNumPositionPresent = {};
	_uint			m_iNumPresent = {};
	_uint			m_iEndIndex = {};
	_uint			m_iNumPositions = {};
	_uint			m_iNumVertices = {};
	_uint			m_iNumIndices = {};
	_uint			m_iNumRemove = {};
	_float			m_fTime = {};
	_float			m_fCumulativeTime = {};
	_float			m_fPow = {};
	_float			m_fSpeed = {};

	_float4			m_vPreHighPositions[3]{};
	_float4			m_vPreLowPositions[3]{};
private:
	virtual HRESULT Initialize_Prototype(_int iNum);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CWaveTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iNum = 150);
	virtual CWaveTrail* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END