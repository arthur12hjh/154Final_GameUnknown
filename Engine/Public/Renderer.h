#pragma once

/* 객체를 생성시에 그려져야할 객체라면 오브젝트 매니져에도 추가하고, 렌더러에도 추가한다.(x)*/
/* 매 프레임당 객체안에서 그려져야하는가를 판단하고 그려져야한다라면 렌더러에 등록하는 작업을 수행한다. */

/* 화면에 그려져야할 객체들을 그리는 순서대로 분류하여 보관한다. */
/* 보관하고 잇는 객체들을 보관한 순서대로 렌더함수를 호출해 준다. 컨테이너를 클리어해버린다. */
#include "Base.h"

NS_BEGIN(Engine)
class COcclusion;

class CRenderer final : public CBase
{
public:
	enum class SHADER_DEFERRED_IDX {
		//여기까진 조명처리
		DEBUG, DIRECTIONAL, POINT, COMBINED,
		OUTLINE,

		//후처리 셰이딩.
		BLUR_X, BLUR_FINAL, 
		GLOW_X, GLOW_FINAL,
		DISTORTION,

		// 최종적으로 백버퍼에 렌더타겟 넘기는 과정.
		SCENE,
		// 디버깅용 패스. 당장은 릴리즈모드에서도 둠
		PHYSX
	};
public:
	/* 블러 데스크. 블러 사이즈 세팅 */
	typedef struct tagBlurDesc {
		_float fSizeX = {};
	} BLUR_DESC;

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Ready_RenderTargets();
	HRESULT Ready_MRTs();
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
	
	//void	UpdateOcclusion();
	void	Render();

#ifdef _DEBUG
	HRESULT								Add_DebugComponent(class CComponent* pDebugCom);
	HRESULT								Add_PhysxGeometry(class PxRigidActor* pActor, class PxShape* pShape);

	unique_ptr<GeometricPrimitive>		CreateHemisphere(ID3D11DeviceContext* pContext, _float fRadius, _int iTessellation, _bool isTop);
#endif

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };

	/*list<class CGameObject*>								m_NonCulledObjects[ENUM_CLASS(RENDER::END)];
	unordered_map<class CGameObject*, class COcclusion*>	m_pOcclusionDatas[2];*/

	/* 사실적인 림라이트를 구현하려면 조명 연산 도중에 들어가는게 맞긴한데.. */
	/* NonBlend에 대해서 Desc하나 만들것 */
	/* 우선 선행적으로 Query를 통해서 검사할 오브젝트들 */
	list<class CGameObject*>			m_RenderObjects[ENUM_CLASS(RENDER::END)];

	ID3D11DepthStencilView*				m_pShadowDSV = { nullptr };
	ID3D11DepthStencilView*				m_pOutlineDSV = { nullptr };

	/*ID3D11RasterizerState*				m_pOcclusionRSState = { nullptr };
	ID3D11DepthStencilState*			m_pOcclusionDSState = { nullptr };*/

private:
	class CShader*						m_pShader = { nullptr };
	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };

private:
	_float4x4							m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	_uint2								m_vScreenSize = {};
	//8192, 4608 혹은 16384, 9216
	_uint2								m_vShadowMapSize = {};
	

#ifdef _DEBUG
private:
	list<pair<class PxRigidActor*, class PxShape*>> m_PxShapes = {};
	class CShader*									m_pPhysxDebugShader = { nullptr };
	list<class CComponent*>							m_DebugComponents = {};
	BasicEffect*									m_pEffect = {};
	ID3D11InputLayout*								m_pInputLayout = {};

	// 외부 제공 모듈이라 어쩔수 없이 unique ptr 써야함..
	unique_ptr<GeometricPrimitive>		m_pBoxShape = { nullptr };
	unique_ptr<GeometricPrimitive>		m_pSphereShape = { nullptr };
	unique_ptr<GeometricPrimitive>		m_pCapsuleCylinderShape = { nullptr };
	unique_ptr<GeometricPrimitive>		m_pCapsuleHemiSphereShape = { nullptr };

	_bool								m_isDebugVisible = { false };
	_bool								m_isColliderVisible = { false };
#endif
private:
	void Render_Priority();
	void Render_Shadow();
	void Render_NonBlend();
	void Render_LightAcc();
	/* 기록은 Combined 이전에. */
	void Render_Blur();
	void Render_Glow();
	void Render_Distortion();
	void Render_Combined();
	void Render_NonLight();
	void Render_Blend();
	void Apply_Deferred();
	void Render_UI();
	void Composite_RT_ToBackBuffer();

private:
	HRESULT								Ready_DepthStencilView(_uint iSizeX, _uint iSizeY);
	//HRESULT								Ready_OcclusionDepthStencil();

#ifdef _DEBUG
private:
	void Render_Debug();
	void Render_PhysxDebug();
public:
	_float4 Convert_PxColor_ToVector(PxU32 iColor);
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};

NS_END