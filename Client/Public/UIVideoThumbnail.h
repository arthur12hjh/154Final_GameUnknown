#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Client)
class CVideoPlayer;

class CUIVideoThumbnail final : public CUIBase
{
private:
	CUIVideoThumbnail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIVideoThumbnail(const CUIVideoThumbnail& Prototype);
	virtual ~CUIVideoThumbnail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Play();
	void Pause();
	void Stop();

	void Set_Source(const _tchar* szPath);

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	//virtual HRESULT Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CVideoPlayer* m_pVideoPlayerCom{ nullptr };

public:
	static CUIVideoThumbnail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END