#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Model;
class CComputeShader;
class CModel;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CPointParticle final : public CGameObject
{
public:
	struct PointConstBufferData
	{
		_float4x4		matWorld;
		_float4			vPivot;
		_float4			vGravity;
		_float4			vRotation;
		_float4			fTimeDelta;
		_float2			fCircle;
		_float2			fTurnPower;
		_float2			fisSphere;
		_int2			iLoopAndCount;
	};
	typedef struct PointParticleData
	{
		string				szModel = {};
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
		_float3				fMeshRotation;

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
		_bool				bisLoop;
		_bool				bisSphere;
		_bool				bisCircle;
		_bool				bisSpectrum;
	}POINT_PARTICLE_DATA;

private:
	CPointParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPointParticle(const CPointParticle& Prototype);
	virtual ~CPointParticle() = default;

public:
	virtual HRESULT Initialize_Prototype(const POINT_PARTICLE_DATA* pPointParticleData);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Set_ParentMat(const _float4x4* pParentMat) { m_pParentMat = pParentMat; }
	void    Stop();
	void    Play();
	void    End();
private:
	const _float4x4* m_pParentMat = { nullptr };
	CVIBuffer_Instance_Model* m_pVIBufferCom = { nullptr };
	CComputeShader* m_pComputeShader = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture[3] = {};

	ID3D11ShaderResourceView* m_pSizeDiagramSRV = { nullptr };

	PointConstBufferData			m_CBData = {};
	ID3D11Buffer* m_pReadSource = { nullptr };
	CModel* m_pModelCom = { nullptr };
	POINT_PARTICLE_DATA	m_tData;
	_uint			m_iCount = {};
	_uint			m_iBegin = {};
	_uint			m_iSelectRender = {};
	_int			m_iRenderCount = {};
	_float			m_fTime = {};
	_float			m_fLength = {};
	_float4x4		m_CombinedWorldMatrix = {};
	RENDER			m_eRender;
	_bool			m_bisStop = { false };

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

	HRESULT							Ready_ComputeShader();
	void							Spread(_float fTimeDelta);

public:
	static CPointParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const POINT_PARTICLE_DATA* pPointParticleData);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END