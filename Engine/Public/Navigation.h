#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigaionDesc
	{
		_int				iCurrentCellIndex = { -1 };
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFiles);
	virtual HRESULT Initialize(void* pArg);
	void Update(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_bool isMove(_fvector vPosition);
	void Compute_Height(class CTransform* pTransform);

	HRESULT Add_Cell_From_Editor(const _float3* vPoints);

	//  현재 로드된 모든 셀 목록 반환
	const vector<class CCell*>* Get_Cells() const { return &m_Cells; }

	// 현재 셀의 개수를 반환
	_uint Get_CellCount() const { return (_uint)m_Cells.size(); }

	_bool Find_Closest_Point(_fvector vPickedPos, _float fRadius, _vector* vOutPoint);

	void Reset_Line();
	void Delete_Line();

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	_int							m_iCurrentCellIndex = { -1 };
	vector<class CCell*>			m_Cells;
	static _float4x4				m_WorldMatrix;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	void SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFiles);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END