#pragma once

/* ��ü�� �����ÿ� �׷������� ��ü��� ������Ʈ �Ŵ������� �߰��ϰ�, ���������� �߰��Ѵ�.(x)*/
/* �� �����Ӵ� ��ü�ȿ��� �׷������ϴ°��� �Ǵ��ϰ� �׷������Ѵٶ�� �������� ����ϴ� �۾��� �����Ѵ�. */

/* ȭ�鿡 �׷������� ��ü���� �׸��� ������� �з��Ͽ� �����Ѵ�. */
/* �����ϰ� �մ� ��ü���� ������ ������� �����Լ��� ȣ���� �ش�. �����̳ʸ� Ŭ�����ع�����. */
#include "Base.h"

NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	const _float4x4* Get_Renderer_Matrix(D3DTS eType = D3DTS::END);

	void*	Get_DoF_Desc();
	void*   Get_Bloom_Desc();
	void*   Get_Fog_Desc();
	void*   Get_SSAO_Desc();
	void*	Get_MotionBlur_Desc();
	void*   Get_Volumetric_Desc();
	void*	Get_HDR_Desc();

public:
	void Active_RadialBlur(_float fLifeTime, _uint iSampleCount, _float fSamplePower);
public:
	HRESULT				Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
	HRESULT				Set_ScreenSize(_uint iSizeX, _uint iSizeY);
	HRESULT				Initialize();
	void				Update(_float fTimeDelta);
	void				Render();

#ifdef _DEBUG
	HRESULT				Add_DebugComponent(class CComponent* pDebugCom);
	HRESULT				Add_PhysxGeometry(class CGameObject* pGameObject, class PxRigidActor* pActor, class PxShape* pShape);
	void				Render_Debug();
	
	void				Set_DebugVisible(_bool isVisible) { m_isDebugVisible = isVisible; }
	void				Set_DebugColliderVisible(_bool isVisible);
#endif
	
private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
	list<class CGameObject*>			m_RenderObjects[ENUM_CLASS(RENDER::END)];

	ID3D11DepthStencilView*				m_pShadowDSV = { nullptr };
	ID3D11DepthStencilView*				m_pCascadeShadowDSV = { nullptr };
private:
	class CShader*						m_pShader = { nullptr };
	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };

	class CShader*						m_pOcclusionShader = { nullptr };
	class CVIBuffer_Cube*				m_pOcclusionVIBuffer = { nullptr };

private:
	_float4x4							m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	VOLUMETRIC_DESC						m_VolumetricDesc = {};
	_float								m_fDensity = { 0.065f }; 
	_float								m_fStepSize = { 0.055f };
	_float								m_fVolumetricG = { 0.028f };
	_bool								m_isVolumetric = { false };

	_uint2								m_vScreenSize = {};
	_uint2								m_vShadowMapSize = {}; 	//8192, 4608 Ȥ�� 16384, 9216
	_uint2								m_vCascadeShadowMapSize = { 2048, 2048 };		
	
	_bool								m_isBloom = { true };
	_bool								m_isFog = { true };
	HDR_DESC							m_HDRDesc = {};
	_bool								m_isHDR = { true };
	_float								m_fHDRExposure = { 1.52f };
	_bool								m_isSSAO = { true }; 

	ID3D11RasterizerState*				m_pRS_OcclusionQuery = { nullptr };

private:
	class CBlur*						m_pBlur = { nullptr };
	class CGlow*						m_pGlow = { nullptr };
	class CDistortion*					m_pDistortion = { nullptr };
	class CBloom*						m_pBloom = { nullptr };
	class CFog*							m_pFog = { nullptr };
	class CRadialBlur*					m_pRadialBlur = { nullptr }; 
	class CDepthofField*				m_pDepthofField = { nullptr };
	class CMotionBlur*					m_pMotionBlur = { nullptr };
	class CSSAO*						m_pSSAO = { nullptr }; 
	class CEmissive*					m_pEmissive = { nullptr };

#ifdef _DEBUG
	class CColliderRenderer*			m_pColliderRenderer = { nullptr };
	_bool								m_isDebugVisible = { false };
#endif

private:
	void		Render_Priority();
	void		Render_Shadow();
	void		Render_MotionBlur();
	void		Render_Occlusion();
	void		Render_NonBlend();
	void		Render_LightAcc();
	void		Render_Combined();
	void		Render_NonLight();
	void		Render_Blend();
	void		Render_Deferred();
	void		Render_ScreenDeferred();


	void		ToneMapping();
	void		Render_BackBuffer();
	void		Render_UI();

	void		Update_Occlusion_Visibility();

private:
	HRESULT		Ready_RenderTargets();
	HRESULT		Ready_MRTs();
	HRESULT		Ready_Shadow_DepthStencilView(_uint iSizeX, _uint iSizeY);
	HRESULT		Ready_CascadeShadow_DepthStencilView(_uint iSizeX, _uint iSizeY, _uint iCSMLevel);
	HRESULT		Bind_WVP_Matrices();

	void		BeginMarker(ID3D11DeviceContext* pContext, const wchar_t* name);

	void		EndMarker(ID3D11DeviceContext* pContext);

	ID3D11DepthStencilState* m_pDSS_DepthNonWrite = { nullptr };
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};

NS_END