#include "PartObject.h"

#include "Model.h"
#include "Shader.h"
#include "Transform.h"
#include "StringHelper.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject& Prototype)
    : CGameObject{ Prototype }
{
}

void CPartObject::Update_PreCombinedMatrix()
{
    m_PreCombinedWorldMatrix = m_CombinedWorldMatrix;
}

HRESULT CPartObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr == pArg)
        return S_OK;

    PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(pArg);
    m_pParentTransformCom = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransformCom);

    return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
}

void CPartObject::Update(_float fTimeDelta)
{
}

void CPartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

const _float4x4* CPartObject::Get_BoneMatrixPtr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneMatrixPtr(pBoneName);;
}

const _float4x4* CPartObject::Get_CombinedMatrixPtr() const
{
    return &m_CombinedWorldMatrix;
}

void CPartObject::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pParentTransformCom);
}
