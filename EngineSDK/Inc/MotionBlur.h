#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class CMotionBlur final : public CComponent
{
private:
	CMotionBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMotionBlur(const CMotionBlur& rhs);
	virtual ~CMotionBlur() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT	Initialize(void* pArg);

private:
	


public:
	static CMotionBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
