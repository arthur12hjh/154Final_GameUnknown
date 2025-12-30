#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CCinematicPartBody final : public CPartObject
{
public:
	typedef struct CinematicPartBodyDesc : public PARTOBJECT_DESC
	{
		_wstring szModelTag;
	}BODY_CINEMATIC_DESC;

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
	
private:
	_wstring											m_szModelTag;

private:
	HRESULT												Bind_ShaderResources();
	HRESULT												Ready_Components();

public:
	static		CCinematicPartBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject* Clone(void* pArg) override;
	virtual		void									Free() override;

};
NS_END