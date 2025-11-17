#pragma once

#include "Tool_Effect_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool_Effect)
class CImgBase : public CBase
{
protected:
	CImgBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImgBase() = default;

public:
	virtual HRESULT			Initialize();
	virtual void			Update(_float fDeletaTime);
	virtual HRESULT Render();
protected:
	CGameInstance* m_pGameInstance = { };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
public:
	virtual		CImgBase* Clone(void* pArg);
	virtual		void			Free() override;
};
NS_END