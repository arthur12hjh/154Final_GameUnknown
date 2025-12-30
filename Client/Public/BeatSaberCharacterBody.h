#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)
class CEffect;
class CTrailEffect;
class CRimLight;

class CBeatSaberCharacterBody final : public CPartObject
{
public:
	typedef struct BeatSaberCharacterBodyDesc : public PARTOBJECT_DESC
	{
		const WCHAR* szBodyModel;
	}BEATSABER_CHARACTER_DESC;

private:
	CBeatSaberCharacterBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeatSaberCharacterBody(const CBeatSaberCharacterBody& Prototype);
	virtual ~CBeatSaberCharacterBody() = default;

public:
	virtual HRESULT										Initialize_Prototype() override;
	virtual HRESULT										Initialize(void* pArg) override;

	virtual void										Priority_Update(_float fTimeDelta) override;
	virtual void										Update(_float fTimeDelta) override;
	virtual void										Late_Update(_float fTimeDelta) override;

	virtual HRESULT										Render() override;
	virtual HRESULT										Render_Shadow() override;
	virtual HRESULT										Render_MotionBlur() override;
	
private:
	CRimLight*											m_pRimLight = { nullptr };
	RIMLIGHT_DESC										m_LimLightDesc = {};

private:
	HRESULT												Bind_ShaderResources();
	HRESULT												Ready_Components();

public:
	static		CBeatSaberCharacterBody*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*							Clone(void* pArg) override;
	virtual		void									Free() override;

};
NS_END