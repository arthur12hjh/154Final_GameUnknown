#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;

class CQuery : public CBase
{
private :
	CQuery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CQuery() = default;

public :
	HRESULT					Initialize(const D3D11_QUERY_DESC& Desc);

	void					QueryBegin();
	void					QueryEnd();

	UINT64					GetNumSamplePassed();
	_bool					IsDataReady() const;

private :
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	ID3D11Query*			m_pQuery = {nullptr};

	CGameInstance*			m_pGameInstance = { nullptr };

public :
	static		CQuery*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const D3D11_QUERY_DESC& Desc);
	virtual		void		Free() override;

};
NS_END