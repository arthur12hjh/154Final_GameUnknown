#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "UIStruct.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CVIBuffer_Point;
class CTexture;
class CShader;
class CEventHandle;
NS_END

NS_BEGIN(Client)
class CUIAnimationCom;
class CUIHUD;

class CUIBase abstract : public CUIObject
{
protected:
	CUIBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); 
	CUIBase(const CUIBase& Prototype);
	virtual ~CUIBase() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	UIBASE_DESC& Get_UIBase_Desc() { return m_tUIDesc; }
	void Set_UIBase_Desc(UIBASE_DESC pDesc) {
		m_tUIDesc = pDesc;
	}

	UIBASE_DESC Get_UIBase_OriginDesc() { return m_tOriginUIDesc; }
	void Set_UIBase_OriginDesc(UIBASE_DESC tDesc) {
		m_tOriginUIDesc = tDesc;
		m_tUIDesc = m_tOriginUIDesc;
	}

	_uint Get_Depth() { return m_tUIDesc.iDepth; }
	void Set_Depth(_uint iDepth) {
		m_tUIDesc.iDepth = iDepth;
		m_iZOrder = iDepth;
	}

	/*void Set_Parent(CGameObject* pParent) {
		m_pParent = pParent;
	}
	CUIBase* Get_Parent() {
		return dynamic_cast<CUIBase*>(m_pParent);
	}*/

	void Set_DrawType(DRAW_TYPE eDrawType) {
		m_tUIDesc.iDrawType = ENUM_CLASS(eDrawType);
		m_eDrawType = eDrawType;
	}

	HRESULT Add_Child(CGameObject* pObj);

	const vector<CUIBase*>* Get_Children() {
		return &m_Children;
	}

	void Set_Position(_float fX, _float fY);
	void Set_Rotation(_float fRotation);
	void Set_Size(_float fSizeX, _float fSizeY);
	void Set_Alpha(_float fAlpha);
	void Set_Pass(_uint iPass);
	void Set_Text_Color(_float4 vColor);
	void Set_TextureUV(_float4 vUV);
	void Set_FillAmount(_float fFillAmount);
	void Set_TintColor(_float4 vColor);
	void Set_Texture_Index(_uint iTextureIndex);
	void Set_GlowIntensity(_float fIntensity);
	HRESULT Set_TextureCom(_wstring szTextureTag, _wstring szProtoTag, _uint iTextureIndex);

	void Trigger_Event(const _wstring& TriggerTag, void* pArg); // ActionTag에 의해 이벤트 동작 수행(Execute) 및 이벤트 전달(Notify)
	void Bind_Event(vector<_wstring> SubEvents, vector<CEventHandle*> Events); // 내가 구독할 이벤트 등록
	void UnBind_Event(); // 이벤트 구독 취소

	_bool Get_Follow_Parent() { return m_bFollowParent; }
	void Set_Follow_Parent(_bool bFollow) { m_bFollowParent = bFollow; }

	void Set_Anim_Playing(_bool bActive) { m_bPlayingAnim = bActive; }
	_bool Get_Anim_Playing() { return m_bPlayingAnim; }

	void Set_TargetPos(const _float3* pPos) { m_pTargetPos = pPos; }

	CUIBase* Clone_UI(CUIHUD* pHUD, _uint iIdx);
	void Update_Children(CUIBase* pChild);

#ifdef _DEBUG
	void Render_Debug_Rect();
#endif

protected:
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	UIBASE_DESC m_tUIDesc{};
	UIBASE_DESC m_tOriginUIDesc{};

	vector<CUIBase*>						m_Children = {};
	map<_wstring, vector<CEventHandle*>>	m_pEventHandles{}; // 내가 들고있을 이벤트 핸들 목록
	vector<_wstring>						m_SubscribeEvents{}; // 내가 구독할 이벤트 목록

	_bool					m_bFollowParent{ true };
	_bool					m_bPlayingAnim{ false };

	const _float3*					m_pTargetPos{nullptr};

private:
	HRESULT Ready_Texture();
	HRESULT Ready_Events();
	HRESULT Initialize_ShaderResources();

	// ★ 중앙 브로드캐스트: 파생형에서 더 이상 오버라이드 필요 없음
	virtual HRESULT Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg);

	// ★ 공통 유효성 검사: 항상 UI_EVENT_ARG_DESC* 만 허용
	bool ValidateEventArg(void* pArg) const;

#ifdef _DEBUG
	HRESULT Ready_Components_For_Debug();
	HRESULT Bind_Debug_ShaderResources();

	CVIBuffer_Point* m_pVIDebugBufferCom = { nullptr };
#endif

protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();
	//virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) PURE; // 이벤트 동작 수행
	//virtual HRESULT Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg) PURE; // 어떤 데이터를 감지할지
	//virtual void CallbackEvent(void* pArg) PURE; // 콜백 함수

	// 각 객체가 [본인]의 이벤트를 ‘수행’하는 로직(애니메이션/액션 등)
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) PURE;

	// 각 객체가 [구독하는 이벤트가 Notify하는 값에 의해] 콜백을 받는 지점(타입별 분기는 여기서)
	virtual void CallbackEvent(void* pArg) PURE;

public:
	virtual void Free() override;
};

NS_END