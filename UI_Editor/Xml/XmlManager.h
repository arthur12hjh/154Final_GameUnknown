#pragma once

#include "Tool_UI_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Tool_UI)
class CXmlManager : public CBase
{
private:
	CXmlManager();
	virtual ~CXmlManager() = default;

public:
	HRESULT Load_Default_Layer(const _char* pFilePath);

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

private:
	CGameInstance* m_pGameInstance = { nullptr };

	

private:

public:
	virtual void Free() override;
};

NS_END
