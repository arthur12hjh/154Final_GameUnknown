#pragma once

#include "Client_Defines.h"
#include "GameHUD.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUIHUD final : public CGameHUD
{
private:
	CUIHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUIHUD() = default;

public:
	unordered_map<_wstring, CHUDLayer*> Get_Layers() { return m_pLayers; }

public:
	static CUIHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END