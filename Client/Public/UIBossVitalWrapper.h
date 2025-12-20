#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUIBossVitalWrapper final : public CUIBase
{
private:
	CUIBossVitalWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIBossVitalWrapper(const CUIBossVitalWrapper& Prototype);
	virtual ~CUIBossVitalWrapper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Boss_Desc(const NAYTIBA_NETWORK_DESC* pNetworkDesc, const NAYTIBA_DESC* pNaytibaDesc)
	{
		m_pNetworkDesc = pNetworkDesc;
		m_pNaytibaDesc = pNaytibaDesc;
		m_eVisibility = VISIBILITY::VISIBLE;
	}

	const NAYTIBA_NETWORK_DESC* Get_NetworkDesc() {return m_pNetworkDesc;}
	const NAYTIBA_DESC* Get_NaytibaDesc() {return m_pNaytibaDesc;}

private:
	const NAYTIBA_NETWORK_DESC* m_pNetworkDesc{ nullptr };
	const NAYTIBA_DESC* m_pNaytibaDesc{ nullptr };

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

public:
	static CUIBossVitalWrapper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

/*
여기서 스킬 4개, Rush 컨트롤
*/