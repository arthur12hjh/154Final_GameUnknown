#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Point_Instance;
class CComputeShader;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)
class CEffectSRV;
class CSpriteParticle final : public CGameObject
{
public:
	struct SpriteConstBufferData
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
	typedef struct SpriteParticleData
	{
		string				szMaskTexture;
		string				szDiffuseTexture;
		string				szNormalTexture;
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
		_float2				fParticleSize;
		_float2				fCircle;
		_int2				iUV;

		_float				fDelayTime;
		_float				fEndTime;
		_float				fSphereSize;
		_float				fAngle;
		_float				fCircleSpeed;



		_int				iBegin;
		_int				iNumInstance;
		_int				iSelectRender;
		_bool				bisBillboard;
		_bool				bisAngleBillboard;
		_bool				bisStart;
		_bool				bisAnimation;
		_bool				bisLoop;
		_bool				bisSphere;
		_bool				bisCircle;
		_bool				bisSpectrum;
	}SPRITE_PARTICLE_DATA;

private:
	CSpriteParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpriteParticle(const CSpriteParticle& Prototype);
	virtual ~CSpriteParticle() = default;

public:
	virtual HRESULT Initialize_Prototype(const SPRITE_PARTICLE_DATA* pSpriteParticleData);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Set_ParentMat(const _float4x4* pParentMat) { m_pParentMat = pParentMat; }
	void    Stop();
	void    Play();
	void    End(_float fTime);
private:
	const _float4x4* m_pParentMat = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CComputeShader* m_pComputeShader = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture[3] = {};
	ID3D11Buffer* m_pReadSource = { nullptr };
	ID3D11ShaderResourceView* m_pSizeDiagramSRV = nullptr;

	CEffectSRV* m_pEffectSRV = { nullptr };

	SpriteConstBufferData			m_CBData = {};
	SPRITE_PARTICLE_DATA	m_tData;
	_float			m_fTime = {};
	_float			m_fLength = {};
	_uint			m_iRenderCount = {};
	_bool			m_bisStop = { false };
	_float4x4		m_CombinedWorldMatrix = {};
	RENDER			m_eRender;

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

	HRESULT							Ready_ComputeShader();
	void							Spread(_float fTimeDelta);

public:
	static CSpriteParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const SPRITE_PARTICLE_DATA* pSpriteParticleData);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END