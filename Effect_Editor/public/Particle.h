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
		_float4			fTimeDelta;
		_float2			fCircle;
		_float2			fTurnPower;
		_float2			fisSphere;
		_int2			iLoopAndCount;
	};
	typedef struct ParticleData
	{
		string				szMaskTexture;
		string				szDiffuseTexture;
		string				szDissolveTexture;
		string				szCS;
		vector<_float3>		fSizeDiagrams;

		_float4				fGravityDiagram;
		_float4				fPosition;
		_float4				fColor;         

		_float3				fCenter;        
		_float3				fPivot;         
		_float3				fRange;         
		_float3				fRotation;      

		_float2				fSize;
		_float2				fLifeTime;      
		_float2				fSpeed;
		_float2				fTurnPower;


		_float2				fMaskUV;
		_float2				fMaskUVSpeed;
		_float2				fMaskUVSize;
		_float2				fDiffuseUV;
		_float2				fDiffuseUVSpeed;
		_float2				fDiffuseUVSize;
		_float2				fDissolveUV;
		_float2				fDissolveUVSpeed;
		_float2				fDissolveUVSize;
		_float2				fCircle;

		_float				fDelayTime;
		_float				fEndTime;
		_float				fSphereSize;
		_float				fCircleSpeed;

		_int				iBegin;         
		_int				iNumInstance;
		_int				iSelectRender;
		_bool				bisBillboard;
		_bool				bisLoop;
		_bool				bisSphere;
		_bool				bisCircle;
		_bool				bisSpectrum;
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
	HRESULT	Set_Texture(_int iIndex, const char* szPrototype);
	void	Set_ParentMat(const _float4x4* pParentMat) { m_pParentMat = pParentMat; }
	void Stop();
	void Play();
private:
	const _float4x4* m_pParentMat = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CComputeShader* m_pComputeShader = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture[3] = {};
	string    m_szFile[3];

	ID3D11ShaderResourceView* m_pSizeDiagramSRV = { nullptr };

	PointConstBufferData			m_CBData = {};
	ID3D11Buffer* m_pReadSource = { nullptr };
	PARTICLE_DATA	m_tData;
	_float			m_fTime = {};
	_uint			m_iCount = {};
	_uint			m_iBegin = {};
	_uint			m_iSelectRender = {};
	_bool			m_bisLoop = {};
	_bool			m_bisStop = { false };
	_float4x4		m_CombinedWorldMatrix = {};
	RENDER	m_eRender = {};

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