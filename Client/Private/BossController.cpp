#include "pch.h"
#include "BossController.h"

#include "GameInstance.h"
#include "BehaviorTree.h"
#include "GameManager.h"
#include "Nayitba.h"
#include "Player.h"
#include "BossBlackBoard.h"

CBossController::CBossController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CAIController(pDevice, pContext)
{
}

CBossController::CBossController(const CBossController& Prototype) :
    CAIController(Prototype)
{
}

HRESULT CBossController::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossController::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    BOSS_CONTROLLER_DESC* pControllerDesc = static_cast<BOSS_CONTROLLER_DESC*>(pArg);
    if (FAILED(Ready_Behavior(*pControllerDesc)))
        return E_FAIL;

    m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pBehaviorTree->GetBlackBoard());
    m_pBlackBoard->SetAttackDelay(0.7f);
    return S_OK;
}

void CBossController::Priority_Update(_float fTimeDelta)
{
}

void CBossController::Update(_float fTimeDelta)
{
    auto pNayitba = static_cast<CNayitba*>(m_pParent);

    if (NAYTIBA_STATE::BATTLE == pNayitba->GetMonsterData().eNaytibaState)
    {
        m_pBlackBoard->SetTarget(pNayitba->GetTarget());
        m_pBlackBoard->AccAttackDelay(fTimeDelta);
    }

    m_pBehaviorTree->Update(fTimeDelta);
}

void CBossController::Late_Update(_float fTimeDelta)
{
}

HRESULT CBossController::Render()
{
    return S_OK;
}

void CBossController::Damage(void* pArg)
{
    DEFAULT_DAMAGE_DESC* pDamageDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);

    auto pNayitba = static_cast<CNayitba*>(m_pParent);
    const Character_Skill_Desc* pAttackData = m_pBlackBoard->GetAttackData();
    const CHARACTER_SKILL_DESC* pDamageSKillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDamageDesc->pSkillData);

    _float fLimitPercent = m_pBlackBoard->Get_CurrentPhaseLitmitPercent();
    _float fCurHealthRatio = (_float)pNayitba->GetMonsterData().iCurrentHealth / (_float)pNayitba->GetStaticMonsterData()->iMaxHealth;

    if (fLimitPercent > fCurHealthRatio)
    {
        _float RecoveryHealth = (pNayitba->GetStaticMonsterData()->iMaxHealth * fLimitPercent) - pNayitba->GetMonsterData().iCurrentHealth;
        pNayitba->RecoveryPoint(RECOVERY_TYPE::RECOVERY_HP, (long long)RecoveryHealth);
    }

    if (10 >= m_pBlackBoard->GetBossInfo()->iCurrentHealth)
    {
        // 이거 죽는모션 나옴 죽으면 
        // 디졸브 이런 느낌의 이펙트 실행되고 삭제되게끔 제어할 예정
        if (SKILL_PROPERTY::EXCUTION & pDamageSKillDesc->eProPerty)
        {
            if (0 < m_pBlackBoard->GetBossInfo()->iCurrentHealth)
            {
                auto pGameManger = CGameManager::GetInstance();
                pGameManger->Play_Cinematic(125, [&]() {
                    auto pNayitba = static_cast<CNayitba*>(m_pParent);
                    pNayitba->Excution();
                });

                m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::DEAD);
            }
        }
        else
            m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::THESHOLD);
    }
    else if(false == m_pBlackBoard->IsPhaseLastAttack())
    {
        // 여기서 피격을 입력으로 피격 무조건 실행하게 하고 데미지도 들어가는데
        // 일단 입력을 넘기고 어떤 상태이냐에 대한 예외처리를 하자
        _bool bIsHitAble = true;
        if (CBossBlackBoard::BOSS_STATE::ATTACK == m_pBlackBoard->GetCurState())
        {
            // 나중에 여러 속성 추가할 예정
            if (nullptr == pDamageSKillDesc || nullptr == pAttackData)
                return;

            if (SKILL_PROPERTY::PARRY & pDamageSKillDesc->eProPerty)
            {
                if (0 >= m_pBlackBoard->GetBossInfo()->iCurrentStamina)
                {
                    // 여기서 그로기 타임 주고 설정
                    // 그로기 들어가기전에 패링 히트 애니메이션 재생후에 들어감
                    // 원작은 뒤로 물러나면서 들어가는거 같음
                    bIsHitAble = false;
                    m_pBlackBoard->EnterGroggy();
                    pNayitba->SetThesholdAction(EXCUTION_TYPE::LINK_ATTACK);
                }
                else
                {
                    if (false == pNayitba->bIsHitReaction())
                        bIsHitAble = false;
                }
            }
            else
            {
                if (SKILL_PROPERTY::SUPERARMOR & pAttackData->eProPerty)
                {
                    if (SKILL_TYPE::BETA_SKILL != pDamageSKillDesc->eSkillType)
                    {
                        bIsHitAble = false;
                    }
                }

                if (SKILL_TYPE::BETA_SKILL == pDamageSKillDesc->eSkillType)
                {
                    if (SKILL_PROPERTY::IGNORE_GUARDBREAK & pAttackData->eProPerty)
                    {
                        bIsHitAble = false;
                    }
                }
            }
        }
        else if (CBossBlackBoard::BOSS_STATE::GROGGY == m_pBlackBoard->GetCurState())
        {
            if (SKILL_PROPERTY::EXCUTION & pDamageSKillDesc->eProPerty)
                m_pBlackBoard->EnterExcution(EXCUTION_TYPE::LINK_ATTACK);
            else
            {
                bIsHitAble = false;
            }
        }
          

        if (bIsHitAble)
        {
            m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::HIT);
            m_pBlackBoard->SetHitData(pDamageDesc);
        }
    }
}

void CBossController::ActionSuccess(void* pArg)
{
}

HRESULT CBossController::Ready_Behavior(const BOSS_CONTROLLER_DESC& pDesc)
{
    CBehaviorTree::BEHAVIORTREE_DESC Desc = {};
    Desc.pOwner = m_pParent;

    auto pClone = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::GAMEPLAY), pDesc.szBehaviorProtoType, &Desc);
    if (nullptr == pClone)
        return E_FAIL;

    m_pBehaviorTree = static_cast<CBehaviorTree*>(pClone);
    return S_OK;
}

CBossController* CBossController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossController* pBossController = new CBossController(pDevice, pContext);
    if (FAILED(pBossController->Initialize_Prototype()))
    {
        Safe_Release(pBossController);
        MSG_BOX("Create Fail : Boss Controller");
    }
    return pBossController;
}

CGameObject* CBossController::Clone(void* pArg)
{
    CBossController* pBossController = new CBossController(*this);
    if (FAILED(pBossController->Initialize(pArg)))
    {
        Safe_Release(pBossController);
        MSG_BOX("Clone Fail : Boss Controller");
    }
    return pBossController;
}

void CBossController::Free()
{
    __super::Free();

    Safe_Release(m_pBehaviorTree);
    Safe_Release(m_pBlackBoard);
}