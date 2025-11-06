#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

public:
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool isIn_WorldFrustum(_fvector vWorldPos, _float fRange);
	_bool isIn_LocalFrustum(_fvector vLocalPos, _float fRange);

private:
	_float4				m_vOriginalPoints[8] = {};
	_float4				m_vWorldPoints[8] = {};
	

	_float4				m_vWorldPlanes[6] = {};
	_float4				m_vLocalPlanes[6] = {};

private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	void Make_Planes(const _float4* pPoints, _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

NS_END