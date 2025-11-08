#pragma once

#include "Client_Defines.h"
#include "HUDLayer.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUIHUDLayer final : public CHUDLayer
{
private:
	CUIHUDLayer();
	virtual ~CUIHUDLayer() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	static CUIHUDLayer* Create();
	virtual void Free() override;
};

NS_END