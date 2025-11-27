#include "Entity.h"
#include "PartObject.h"

#include "GameInstance.h"
#include "Model.h"

CEntity::CEntity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject { pDevice, pContext }
{
}

CEntity::CEntity(const CEntity& Prototype)
    : CContainerObject { Prototype }
{
}

HRESULT CEntity::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEntity::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CEntity::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEntity::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CEntity::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CEntity::Render()
{


    return S_OK;
}

_bool CEntity::Play_Animation(_float fTimeDelta, CTransform* pTargetTransform, _float fRootMotionRatio)
{
    /// "          "가 비어있을 경우, 아래에 나열된 작업을 수행합니다.
    /// Find_PartObject(TEXT("Part_Body")) 를 통해 PartObject를 받습니다.
    /// PartObject의 Find_Component(TEXT("Com_Model"))을 통해 CModel*을 받습니다.
    /// 
    /// 이후, CModel의 Play_Animation함수를 return 해줍니다.
    /// 
    if (nullptr == m_pBodyModelCom)
        Import_ModelPtr();

    return m_pBodyModelCom->Play_Animation(fTimeDelta, pTargetTransform, fRootMotionRatio);
}

_bool CEntity::IsAnmiationFinished()
{
    return m_pBodyModelCom->IsAnimationFinished();
}

void CEntity::Set_AnimationIndex(_int iAnimIndex, _bool isLoop)
{
    if (nullptr == m_pBodyModelCom)
        Import_ModelPtr();

    m_pBodyModelCom->Set_AnimationIndex(iAnimIndex, isLoop);
}

void CEntity::Set_Animation(const _char* szAnimationTag, _bool isLoop, _float fAnimationPlayRate)
{
    if (nullptr == m_pBodyModelCom)
        Import_ModelPtr();

    m_pBodyModelCom->Set_Animation(szAnimationTag, isLoop, fAnimationPlayRate);
}

void CEntity::Set_Animation(const _wstring& strAnimationTag, _bool isLoop, _float fAnimationPlayRate)
{
    if (nullptr == m_pBodyModelCom)
        Import_ModelPtr();

    m_pBodyModelCom->Set_Animation(strAnimationTag, isLoop, fAnimationPlayRate);
}
_float CEntity::Get_AnimationRatio()
{
    return m_pBodyModelCom->Get_AnimationRatio();
}

HRESULT CEntity::Import_ModelPtr()
{

    m_pBodyModelCom = static_cast<CModel*>(Find_PartObject(TEXT("Part_Body"))->Find_Component(TEXT("Com_Model")));

    Safe_AddRef(m_pBodyModelCom);

    return S_OK;
}

void CEntity::Free()
{
    __super::Free();

    Safe_Release(m_pBodyModelCom);
}

