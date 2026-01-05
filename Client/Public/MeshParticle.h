#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_MeshParticle;
class CComputeShader;
class CModel;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)
class CEffectSRV;

class CMeshParticle final : public CGameObject
{
public:
	struct MeshConstBufferData
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
	typedef struct MeshParticleData
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
		_bool				bisMeshTexture;
		_bool				bisSphere;
		_bool				bisCircle;
		_bool				bisSpectrum;
	}MESH_PARTICLE_DATA;

private:
	CMeshParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshParticle(const CMeshParticle& Prototype);
	virtual ~CMeshParticle() = default;

public:
	virtual HRESULT Initialize_Prototype(const MESH_PARTICLE_DATA* pMeshParticleData);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_MotionBlur() override;
	void	Set_ParentMat(const _float4x4* pParentMat) { m_pParentMat = pParentMat; }
	void    Stop();
	void    Play();
	void    End();
private:
	const _float4x4* m_pParentMat = { nullptr };
	CVIBuffer_Instance_MeshParticle* m_pVIBufferCom = { nullptr };
	CComputeShader* m_pComputeShader = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture[3] = {};

	ID3D11ShaderResourceView* m_pSizeDiagramSRV = { nullptr };

	MeshConstBufferData			m_CBData = {};
	ID3D11Buffer* m_pReadSource = { nullptr };
	CModel* m_pModelCom = { nullptr };


	CEffectSRV* m_pEffectSRV = { nullptr };

	MESH_PARTICLE_DATA	m_tData;
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
	static CMeshParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MESH_PARTICLE_DATA* pMeshParticleData);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END