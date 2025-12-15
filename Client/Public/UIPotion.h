#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CUIPotion final : public CUIBase
{
private:
	CUIPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPotion(const CUIPotion& Prototype);
	virtual ~CUIPotion() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_int Get_PotionCount() { return *m_iPotions; }
	_int Get_MaxPotionCount() { return *m_iMaxPotions; }

	_float Get_PotionPercent();

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
#ifdef _DEBUG
	_int m_MaxGara = 5;
	_int m_Gara = 3;
#endif // DEBUG

	_int* m_iMaxPotions = nullptr;
	_int* m_iPotions = nullptr;

public:
	static CUIPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END