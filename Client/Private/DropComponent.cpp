#include "pch.h"
#include "DropComponent.h"

#include "GameInstance.h"
#include "Item.h"

const WCHAR* CDropComponent::m_szProtoTypeName = TEXT("Prototype_GamePlay_ItemObject");
const WCHAR* CDropComponent::m_szLayerName = TEXT("GamePlay_Layer_DropItem");

CDropComponent::CDropComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{
}

HRESULT CDropComponent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDropComponent::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    DROP_COMPONENT_DESC* pDesc = static_cast<DROP_COMPONENT_DESC*>(pArg);
    m_fDropRange = pDesc->fDropRange;
    m_fDropForce = pDesc->fForce;
    return S_OK;
}

void CDropComponent::ItemDrop(_uint iDropItemCount)
{
    for(_uint i = 0; i< iDropItemCount; ++i)
        CalculationItemDrop();

    CreateObjectToLayer();
}

HRESULT CDropComponent::ADD_DropItem(const pair<_uint, _float>& ItemData, _float fAmount)
{
    auto iter = find_if(m_DoprItemList.begin(), m_DoprItemList.end(), [&](const auto& src)
        {
            return src.first == ItemData.first ? true : false;
        });

    if (iter == m_DoprItemList.end())
    {
        m_DoprItemList.push_back(ItemData);
        m_AmountItemList.push_back(fAmount);

        m_fTotalWeight += ItemData.second;
        m_iNumItemCount++;
    }
    else
        return E_FAIL;

    return S_OK;
}

void CDropComponent::CalculationItemDrop()
{
    _float fRandomWeight = m_pGameInstance->Random(1, m_fTotalWeight);
    sort(m_DoprItemList.begin(), m_DoprItemList.end(), [&](const auto& src, const auto& dest)
        {
            return src.second > dest.second ? true : false;
        });
    
    for (_uint i = 0; i < m_iNumItemCount; ++i)
    {
        fRandomWeight -= m_DoprItemList[i].second;
        if (0 >= fRandomWeight)
        {
            DROP_RESULT_DESC ResultDesc = {};
            ResultDesc.iDropItemID = m_DoprItemList[i].first;
            ResultDesc.fAmountVal = m_pGameInstance->Random(1, m_AmountItemList[i]);

            m_DropResultList.push_back(move(ResultDesc));
            return;
        }
    }
}

void CDropComponent::CreateObjectToLayer()
{
    // 여기서 객체를 생성해준다
    _uint iLevelID = m_pGameInstance->GetCurrentLevelID();
  
    // 아이템에 대한 DESC
    // 아이템 ID 습득량 이정보 정도면 될듯
    for (auto& iter : m_DropResultList)
    {
        CItem::ITEM_DESC ItemDesc = {};
        ItemDesc.iInteractionID = 0;
        ItemDesc.bIsApplyTransform = true;
        ItemDesc.vScale = { 1.f, 1.f, 1.f };
        ItemDesc.fDropForce = m_pGameInstance->Random(m_fDropForce - 5.f, m_fDropForce + 5.f);
        //ItemDesc.vRotation = { XMConvertToRadians(m_pGameInstance->Random(0, 360.f)),
        //                       XMConvertToRadians(m_pGameInstance->Random(0, 360.f)),
        //                       XMConvertToRadians(m_pGameInstance->Random(0, 360.f)), 0.f };

        _float fRange = m_pGameInstance->Random(m_fDropRange * 0.7f, m_fDropRange * 1.3f);
        _float fRadius = m_pGameInstance->Random(0.f, 360.f);
        
        _vector vDorpPoint = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
        XMStoreFloat3(&ItemDesc.vPosition, vDorpPoint);
        vDorpPoint.m128_f32[0] += cosf(XMConvertToRadians(fRadius)) * fRange;
        vDorpPoint.m128_f32[1] += 1.f;
        vDorpPoint.m128_f32[2] += sinf(XMConvertToRadians(fRadius)) * fRange;

        XMStoreFloat3(&ItemDesc.fDropPoint, vDorpPoint);
        memcpy(&ItemDesc.iItemID, &iter.iDropItemID, sizeof(_uint));
        memcpy(&ItemDesc.fAmount, &iter.fAmountVal, sizeof(_float));
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iLevelID, m_szProtoTypeName, iLevelID, m_szLayerName, &ItemDesc)))
            return;
    }
    m_DropResultList.clear();
}

CDropComponent* CDropComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDropComponent* pDropComponent = new CDropComponent(pDevice, pContext);
    if (FAILED(pDropComponent->Initialize_Prototype()))
    {
        Safe_Release(pDropComponent);
        MSG_BOX("Create Fail : Drop Component");
    }
    return pDropComponent;
}

CComponent* CDropComponent::Clone(void* pArg)
{
    CDropComponent* pDropComponent = new CDropComponent(*this);
    if (FAILED(pDropComponent->Initialize(pArg)))
    {
        Safe_Release(pDropComponent);
        MSG_BOX("Clone Fail : Drop Component");
    }
    return pDropComponent;
}

void CDropComponent::Free()
{
    __super::Free();
}
