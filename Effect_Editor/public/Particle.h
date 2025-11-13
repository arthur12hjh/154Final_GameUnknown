#pragma once

#include "Tool_Effect_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Point_Instance;
class CComputeShader;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Tool_Effect)

class CParticle final : public CGameObject
{
public:
	struct PointConstBufferData
	{
		_float4x4		matWorld;
		_float4			vPivot;
		_float4			vGravity;
		_float2			fTimeDelta;
		_int2			iLoopAndCount;
	};
	typedef struct ParticleData
	{
		_int	iBegin;
		_int    iNumInstance;
		_float3 fCenter;
		_float3 fPivot;
		_float3 fRange;
		_float2 fSize;
		_float2 fLifeTime;
		_float2 fSpeed;
		_float4 fGravityDiagram = {};
		_float4 fPosition = {0,0,0,1};
		_float3 fRotation = {};
		_bool   bisLoop;
		_uint	iSelectRender = {};
		_float4 fColor = {};
		string szCS;
		vector<_float3> fSizeDiagrams;
	}PARTICLE_DATA;

private:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& Prototype);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Set_Components(PARTICLE_DATA tData);
	void	Update(PARTICLE_DATA tData);
	void	Set_Begin(_int iBegin) { m_iBegin = iBegin; }
	PARTICLE_DATA	Get_Data() { return m_tData; }
	string	Get_TextureName(_int iIndex) { return m_szFile[iIndex]; }
	void	Set_Texture(_int iIndex, CTexture* pTexture, string szFile) { m_pTexture[iIndex] = pTexture; m_szFile[iIndex] = szFile; }
private:
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CComputeShader* m_pComputeShader = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture[3] = {};
	string    m_szFile[3];

	ID3D11ShaderResourceView* m_pSizeDiagramSRV = { nullptr };

	PointConstBufferData			m_CBData = {};
	ID3D11Buffer* m_pReadSource = { nullptr };
	PARTICLE_DATA	m_tData;
	_uint			m_iBegin = {};
	_uint			m_iSelectRender = {};

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

	HRESULT							Ready_ComputeShader();
	void							Spread(_float fTimeDelta);

public:
	static CParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END