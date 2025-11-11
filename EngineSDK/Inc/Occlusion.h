#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CQuery;

class COcclusion : public CBase
{
private:
	COcclusion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COcclusion() = default;

public:
	HRESULT							Initialize();

	HRESULT							OccluseionBegin();
	HRESULT							OccluseionEnd();

	UINT64							GetNumSamplePassed();
	_bool							IsDataReady() const;

private :
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	_bool							m_bIsDataReady = false;
	CQuery*							m_pOCclusionQuery = { nullptr };

public:
	static		COcclusion*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		void				Free() override;

};
NS_END