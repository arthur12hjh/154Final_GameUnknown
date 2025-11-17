#include "pch.h"
#include "ImgBase.h"
#include "GameInstance.h"

CImgBase::CImgBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CImgBase::Initialize()
{
    return S_OK;
}

void CImgBase::Update(_float fDeletaTime)
{

}

HRESULT CImgBase::Render()
{
    return S_OK;
}

CImgBase* CImgBase::Clone(void* pArg)
{
    return nullptr;
}

void CImgBase::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}