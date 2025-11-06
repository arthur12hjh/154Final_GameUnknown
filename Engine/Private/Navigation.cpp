#include "Navigation.h"

#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"

_float4x4 CNavigation::m_WorldMatrix = { };

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{

}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
	, m_Cells { Prototype.m_Cells }
#ifdef _DEBUG
	, m_pShader { Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFiles)
{
	_ulong		dwByte = { };
	HANDLE		hFile = CreateFile(pNavigationDataFiles, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[ENUM_CLASS(NAVI_POINT::END)] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * ENUM_CLASS(NAVI_POINT::END), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}	

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	SetUp_Neighbors();

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

	return S_OK;
}

_bool CNavigation::isMove(_fvector vPosition)
{
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int		iNeighborIndex = { -1 };

	_bool		isIn = m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex);

	if (true == isIn)
	{
		return true;
	}
	else
	{
		/* 나간 방향에 이웃이 있냐? */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex))
					break;
			}
			

			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}
		else	
			return false;
	}	
}

void CNavigation::Compute_Height(CTransform* pTransform)
{
	_vector		vLocalPos = XMVector3TransformCoord(pTransform->Get_State(STATE::POSITION), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));


	_float		fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos);

	vLocalPos = XMVectorSetY(vLocalPos, fHeight);


	pTransform->Set_State(STATE::POSITION, XMVector3TransformCoord(vLocalPos, XMLoadFloat4x4(&m_WorldMatrix)));
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_float4		vColor = { };
	_float4x4	WorldMatrix = m_WorldMatrix;

	if(-1 == m_iCurrentCellIndex)	
		vColor = _float4(0.f, 1.f, 0.f, 1.f);
	else
	{
		WorldMatrix._42 += 0.05f;
		vColor = _float4(1.f, 0.f, 0.f, 1.f);	
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	m_pShader->Begin(0);

	if (-1 != m_iCurrentCellIndex)
	{
		return m_Cells[m_iCurrentCellIndex]->Render();
	}

	for (auto& pCell : m_Cells)
 		pCell->Render();

	return S_OK;
}
#endif

void CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_Point(NAVI_POINT::A), pSourCell->Get_Point(NAVI_POINT::B)))
			{
				pSourCell->Set_Neighbor(NAVI_LINE::AB, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(NAVI_POINT::B), pSourCell->Get_Point(NAVI_POINT::C)))
			{
				pSourCell->Set_Neighbor(NAVI_LINE::BC, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(NAVI_POINT::C), pSourCell->Get_Point(NAVI_POINT::A)))
			{
				pSourCell->Set_Neighbor(NAVI_LINE::CA, pDestCell);
			}
		}
	}
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFiles)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFiles)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
