#pragma once

/* 객체를 생성시에 그려져야할 객체라면 오브젝트 매니져에도 추가하고, 렌더러에도 추가한다.(x)*/
/* 매 프레임당 객체안에서 그려져야하는가를 판단하고 그려져야한다라면 렌더러에 등록하는 작업을 수행한다. */

/* 화면에 그려져야할 객체들을 그리는 순서대로 분류하여 보관한다. */
/* 보관하고 잇는 객체들을 보관한 순서대로 렌더함수를 호출해 준다. 컨테이너를 클리어해버린다. */
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
public:
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Set_ScreenSize(_uint iSizeX, _uint iSizeY);
	HRESULT Initialize();
	void	Update(_float fTimeDelta);
	void	Render();

#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugCom);
	HRESULT Add_PhysxGeometry(class PxRigidActor* pActor, class PxShape* pShape);
	void	Render_Debug();

	void	Set_DebugVisible(_bool isVisible) { m_isDebugVisible = isVisible; }
	void   Set_DebugColliderVisible(_bool isVisible);
#endif
	
private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
	list<class CGameObject*>			m_RenderObjects[ENUM_CLASS(RENDER::END)];

	ID3D11DepthStencilView*				m_pShadowDSV = { nullptr };
	ID3D11DepthStencilView*				m_pVolumetricDSV = { nullptr };
private:
	class CShader*						m_pShader = { nullptr };
	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };

private:
	_float4x4							m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	VOLUMETRIC_DESC						m_VolumetricDesc = {};
	_float								m_fDensity = { 0.065f }; 
	_float								m_fStepSize = { 0.055f };
	_float								m_fVolumetricG = { 0.028f };
	_bool								m_isVolumetric = { false };

	_uint2								m_vScreenSize = {};
	_uint2								m_vShadowMapSize = {}; 	//8192, 4608 혹은 16384, 9216
	
	_bool								m_isBloom = { false };
	_bool								m_isFog = { false };
	_bool								m_isHDR = { true };
	_bool								m_isSSAO = { true }; 
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
	void		Render_NonBlend();
	void		Render_LightAcc();
	/* 기		록은 Combined 이전에. */
	void		Render_Combined();
	void		Render_NonLight();
	void		Render_Blend();
	void		Render_Deferred();
	void		Render_ScreenDeferred();


	void		ToneMapping();
	void		Render_BackBuffer();
	void		Render_UI();

private:
	HRESULT		Ready_RenderTargets();
	HRESULT		Ready_MRTs();
	HRESULT		Ready_DepthStencilView(_uint iSizeX, _uint iSizeY);
	HRESULT		Bind_WVP_Matrices();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};

NS_END