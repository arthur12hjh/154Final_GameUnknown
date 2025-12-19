#include "Occlusion.h"
#include "GameObject.h"

COcclusion::COcclusion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT COcclusion::Initialize()
{
    D3D11_QUERY_DESC desc{};
    desc.Query = D3D11_QUERY_OCCLUSION;
    desc.MiscFlags = 0;

    for (_uint i = 0; i < 2; ++i)
    {
        if (FAILED(m_pDevice->CreateQuery(&desc, &m_pQueries[i])))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT COcclusion::Begin_Object_Query(CGameObject* pObject)
{
    ID3D11Query* pQuery = Create_Query(pObject);
    if (nullptr == pQuery)
        return E_FAIL;

    m_pContext->Begin(pQuery);

    return S_OK;
}

HRESULT COcclusion::End_Obejct_Query(CGameObject* pObject)
{
    ID3D11Query* pQuery = m_QueryData[m_iFrameIndex].m_mapQueries[pObject];
    if (nullptr == pQuery)
        return E_FAIL;

    m_pContext->End(pQuery);

    return S_OK;
}

HRESULT COcclusion::Get_Result(CGameObject* pObject, _bool* pIsVisible)
{
    const _uint iPrevFrame = (m_iFrameIndex + 1) % 2;

    auto iter = m_QueryData[iPrevFrame].m_mapQueries.find(pObject);
    
    if (iter == m_QueryData[iPrevFrame].m_mapQueries.end())
    {
        *pIsVisible = true;
        //pObject->Set_Occlusion_CoolDown(0);
        return S_OK;
    }

    ID3D11Query* pQuery = iter->second;
    UINT64 numSamples = 0;

    HRESULT hr = m_pContext->GetData(
        pQuery,
        &numSamples,
        sizeof(UINT64),
        D3D11_ASYNC_GETDATA_DONOTFLUSH
    );
       
    if (S_OK == hr)
    {
        if (numSamples > 0)
        {
            *pIsVisible = true;
            pObject->Set_Occlusion_CoolDown(0);

            //return S_OK;
        }
        else
        {
            _int iCurrentCooldown = pObject->Get_Occlusion_CoolDown() + 1;
            pObject->Set_Occlusion_CoolDown(iCurrentCooldown);
            
            *pIsVisible = (iCurrentCooldown >= 10) ? false : true;
            /*const _int COOLDOWN_THRESHOLD = 10; 

            if (iCurrentCooldown >= COOLDOWN_THRESHOLD)
            {
                *pIsVisible = false;
            }
            else
            {
                *pIsVisible = true; 
            }*/
            //return S_OK;
            
        }
        return S_OK;
    }

    else if (S_FALSE == hr)
    {
        //*pIsVisible = true;
        *pIsVisible = (pObject->GetVisibility() == VISIBILITY::VISIBLE);
        return S_FALSE;
    }

    *pIsVisible = true;
    pObject->Set_Occlusion_CoolDown(0);

    return E_FAIL;
}

void COcclusion::SwapFrame()
{
    const _uint iNextFrame = (m_iFrameIndex + 1) % 2;

    m_QueryData[iNextFrame].m_mapQueries.clear();

    m_iFrameIndex = iNextFrame;
}

ID3D11Query* COcclusion::Create_Query(CGameObject* pObject)
{
    OCCLUSION_QUERY_DATA& currentData = m_QueryData[m_iFrameIndex];

    // 1. 영구 맵에서 이미 생성된 쿼리가 있는지 확인
    auto permanent_iter = m_mapPermanentQueries.find(pObject);
    ID3D11Query* pQuery = nullptr;

    if (permanent_iter == m_mapPermanentQueries.end())
    {
        // 쿼리가 없다면 새로 생성하고 영구 맵에 저장
        D3D11_QUERY_DESC Desc;
        ZeroMemory(&Desc, sizeof(D3D11_QUERY_DESC));
        Desc.Query = D3D11_QUERY_OCCLUSION;
        Desc.MiscFlags = 0;

        if (FAILED(m_pDevice->CreateQuery(&Desc, &pQuery)))
        {
            return nullptr;
        }

        m_mapPermanentQueries.insert(make_pair(pObject, pQuery));
    }
    else
    {
        // 이미 생성된 쿼리 객체를 사용
        pQuery = permanent_iter->second;
    }

    // 2. 현재 프레임 맵에 객체와 쿼리 객체 포인터를 연결 (다음 프레임 Get_Result에서 사용)
    currentData.m_mapQueries.insert(make_pair(pObject, pQuery));

    return pQuery;
}

COcclusion* COcclusion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    COcclusion* pOcclusion = new COcclusion(pDevice, pContext);
    if (FAILED(pOcclusion->Initialize()))
    {
        Safe_Release(pOcclusion);
        MSG_BOX("Create Fail : Occlusion");
    }
    return pOcclusion;
}

void COcclusion::Free()
{
    __super::Free();

    for (auto& pair : m_mapPermanentQueries)
        Safe_Release(pair.second);
    m_mapPermanentQueries.clear();

    for (int i = 0; i < 2; ++i)
        Safe_Release(m_pQueries[i]);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pQuery);
}
