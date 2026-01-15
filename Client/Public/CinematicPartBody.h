#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)
class CEffect;
class CTrailEffect;
class CCinematicPartBody final : public CPartObject
{
public:
	typedef struct CinematicPartBodyDesc : public PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = nullptr;
		_wstring szModelTag;
		_bool isAnim = TRUE;
		_bool isSetTransform = FALSE;
		_float3 vPartPosition = {};
		_float3 vPartRotation = {};
		_float3 vPartScale = { 1.f, 1.f, 1.f };
	}BODY_CINEMATIC_DESC;
	typedef struct ScarletTrailDesc
	{
		const _float4x4* pRootMatrix = nullptr;
		CTrailEffect* pTrailEffect = {};
		_bool			bisPlay = false;
	}SCARLET_TRAIL_DESC;
	typedef struct ScarletLineTrailDesc
	{
		const _float4x4* pRootMatrix = nullptr;
		CTrailEffect* pTrailEffect = {};
		_vector			vPos = {};
		_float			fSpeed;
		_float			fLength = {};
		_float			fPow;
		_int			iDir;
		_bool			bisPlay = false;
	}SCARLET_LINE_TRAIL_DESC;

private:
	CCinematicPartBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicPartBody(const CCinematicPartBody& Prototype);
	virtual ~CCinematicPartBody() = default;

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

	void												Reset_SocketMatrix(_float4x4* pSocketMatrix = nullptr);
	void												Set_Render(_bool isActive) { SetActive(isActive); }
	//골반 본을 기준으로 한 시네마틱 객체의 위치를 가져온다.
	_matrix												Get_CinematicWorldPos();

	void								Stop_All_Effect();

private:
	_wstring											m_szModelTag;
	_float4x4*											m_pSocketMatrix = { nullptr };


	_float												m_fLineTime = {};

	vector<pair<CEffect*, _int>>						m_pEffects = {};
	vector<pair<SCARLET_TRAIL_DESC*, _int>>				m_pTrailEffects = {};
	vector<pair<SCARLET_LINE_TRAIL_DESC*, _int>>				m_pLineTrailEffects = {};

	_bool												m_bIsAnim = TRUE;

private:
	HRESULT												Bind_ShaderResources();
	HRESULT												Ready_Components();

public:
	static		CCinematicPartBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject* Clone(void* pArg) override;
	virtual		void									Free() override;

};
NS_END