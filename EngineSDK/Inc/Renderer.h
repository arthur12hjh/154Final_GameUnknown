#pragma once

/* 객체를 생성시에 그려져야할 객체라면 오브젝트 매니져에도 추가하고, 렌더러에도 추가한다.(x)*/
/* 매 프레임당 객체안에서 그려져야하는가를 판단하고 그려져야한다라면 렌더러에 등록하는 작업을 수행한다. */

/* 화면에 그려져야할 객체들을 그리는 순서대로 분류하여 보관한다. */
/* 보관하고 잇는 객체들을 보관한 순서대로 렌더함수를 호출해 준다. 컨테이너를 클리어해버린다. */
#include "Base.h"

NS_BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum class SHADER_DEFERRED_IDX {
		//여기까진 조명처리
		DEBUG, DIRECTIONAL, POINT, COMBINED,
		//후처리 셰이딩. 일단 블러만 추가
		BLUR_X, BLUR_FINAL, DISTORTION,

		// 최종적으로 백버퍼에 렌더타겟 넘기는 과정.
		SCENE
	};
public:
	/* 블러 데스크. 블러 사이즈 세팅 */
	typedef struct tagBlurDesc {
		_float fSizeX = {};
	} BLUR_DESC;

	/* 디스토션 데스크. 강도 세팅 */
	typedef struct tagDistortionDesc {
		_float fIntensity = {};
	} DISTORTION_DESC;

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Ready_RenderTargets();
	HRESULT Ready_MRTs();
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
	void Render();

#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugCom);
#endif

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
	list<class CGameObject*>			m_RenderObjects[ENUM_CLASS(RENDER::END)];

	ID3D11DepthStencilView* m_pShadowDSV = { nullptr };

#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugComponents;
#endif

private:
	class CShader*						m_pShader = { nullptr };
	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };

private:
	_bool								m_isDebugVisible = { false };
	_float4x4							m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	_uint2								m_vScreenSize = {};
	//8192, 4608 혹은 16384, 9216
	_uint2								m_vShadowMapSize = {};

private:
	void Render_Priority();
	void Render_Shadow();
	void Render_NonBlend();
	void Render_LightAcc();
	/* 기록은 Combined 이전에. */
	void Render_Blur();
	void Render_Distortion();
	void Render_Combined();
	void Render_NonLight();
	void Render_Blend();
	void Apply_Deferred();
	void Render_UI();
	void Composite_RT_ToBackBuffer();
private:
	HRESULT Ready_DepthStencilView(_uint iSizeX, _uint iSizeY);

#ifdef _DEBUG
private:
	void Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};

NS_END