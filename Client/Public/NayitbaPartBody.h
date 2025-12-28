#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CEffect;
class CTrailEffect;
class CRimLight;

class CNayitbaPartBody final : public CPartObject
{
public :
	typedef struct NayitbaPartBodyDesc : public PARTOBJECT_DESC
	{
		const WCHAR*				szBodyModel;
	}NAYITBA_PART_BODY_DESC;
	typedef struct NayitbaTrailDesc
	{
		const _float4x4* pRootMatrix = nullptr;
		CTrailEffect*	pTrailEffect = {};
		_bool			bisPlay = false;
	}NAYITBA_TRAIL_DESC;
	typedef struct NayitbaLineTrailDesc
	{
		const _float4x4* pRootMatrix = nullptr;
		CTrailEffect* pTrailEffect = {};
		_float			fSpeed;
		_float			fPow;
		_int			iDir;
		_bool			bisPlay = false;
	}NAYITBA_LINE_TRAIL_DESC;

private:
	CNayitbaPartBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNayitbaPartBody(const CNayitbaPartBody& Prototype);
	virtual ~CNayitbaPartBody() = default;

public:
	virtual HRESULT										Initialize_Prototype() override;
	virtual HRESULT										Initialize(void* pArg) override;

	virtual void										Priority_Update(_float fTimeDelta) override;
	virtual void										Update(_float fTimeDelta) override;
	virtual void										Late_Update(_float fTimeDelta) override;

	virtual HRESULT										Render() override;
	virtual HRESULT										Render_Shadow() override;
	virtual HRESULT										Render_MotionBlur() override;
	virtual void										Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference) override;
	void												Play_DeadEffect();

	void												SetPart_BodyColor(_bool bIsEnable, _bool bIsDissolve = false, _float4 vColor = {});

private:
	CCollider*											m_pColliderCom = { nullptr };
	CRimLight*											m_pRimLight = { nullptr };

	vector<pair<CEffect*, _int>>						m_pEffects = {};
	vector<pair<NAYITBA_TRAIL_DESC*, _int>>				m_pTrailEffects = {};
	vector<pair<NAYITBA_LINE_TRAIL_DESC*, _int>>				m_pLineTrailEffects = {};
	
	CTexture*											m_pTexture = {};
	_bool												m_isDeadEffect = { false };

	_float												m_fDeadTime = {};
	_float												m_fLineTime = {};
	_bool												m_bisSetDeadEffect = { true };

	_float												m_vDissolveRadius = { 30.f };
	_bool												m_bIsDissolveFade = {};

	_float4												m_vColCenterPos = {};
	_float4												m_vPatternColor = {};
	const _float4x4*									m_pSpineMatrix = {};

	_bool												m_bIsChangeBodyColor = { false };
	_bool												m_bIsChangeColorDissolve = { false };
	RIMLIGHT_DESC										m_MonsterLimLightDesc = {};

private:
	HRESULT												Ready_Components(const NAYITBA_PART_BODY_DESC& pDesc);
	HRESULT												Bind_ShaderResources();
	HRESULT												End_ShaderResources();

public:
	static		CNayitbaPartBody*						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*							Clone(void* pArg) override;
	virtual		void									Free() override;

};
NS_END