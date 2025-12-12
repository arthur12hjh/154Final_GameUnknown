#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)
class CEffectSRV : public CBase
{
	DECLARE_SINGLETON(CEffectSRV);
private:
	CEffectSRV() = default;
	virtual ~CEffectSRV() = default;
public:
	HRESULT									Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Reset();
	ID3D11ShaderResourceView* Get_SRV();
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11DepthStencilView* m_pOriginalDSV = { nullptr };
	ID3D11Resource* m_pResourse = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	_bool					m_bisSRV = { false };
public:
	virtual void					Free() override;
};
NS_END