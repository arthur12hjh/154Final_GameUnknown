#include "pch.h"

#include "Player_GigasLinkAttackState.h"

#include "Player.h"
#include "Nayitba.h" 
#include "GameInstance.h"
#include "GameManager.h"
#include "PonyTail_Player.h"

CPlayer_GigasLinkAttackState::CPlayer_GigasLinkAttackState()
    : CPlayerState{}
{

}

HRESULT CPlayer_GigasLinkAttackState::Initialize(void* pArg)
{
    SOCKETMATRIX_DESC* pSocketMatrixDesc = static_cast<SOCKETMATRIX_DESC*>(pArg);

    // 고릴라 TransformMatrix를 던져줘야함
    m_pParentTransformMatrix = pSocketMatrixDesc->pParentTransformMatrix;
    m_pSocketMatrix = pSocketMatrixDesc->pSocketMatrix;

    m_eState = PLAYER_STATE::GIGAS_LINKATTACK;

    m_Desc->isSuperArmor = true;
    
    m_Desc->pPlayerController->Set_Active(true);
    m_Desc->pPlayerController->Set_CCTCollision(false);

    m_pGameInstance->Active_RadialBlur(1.1f, 16, 0.66f);

    CGameInstance::GetInstance()->Set_MoitonBlur_Active(false);

    CGameInstance::GetInstance()->ADD_FrameFinalFunction([&]() {
        CGameInstance::GetInstance()->Set_MoitonBlur_Active(true);
        }, 4);

    return S_OK;
}

void CPlayer_GigasLinkAttackState::Start(void* pArg, _float fBlendRatio)
{
    m_isEndList.push_back(FALSE);
    m_isEndList.push_back(FALSE);
    m_iAnimationIndex = 0;

    // 클래스의 상태를 변경해준다.
    //m_eState = PLAYER_STATE::TEST_STATE;
    m_eState = PLAYER_STATE::GIGAS_LINKATTACK;

    DEFAULT_DAMAGE_DESC Desc;
    Desc.pSkillData = m_pGameManager->Find_SkillData(1010);

    if(m_Desc->pLinkAttackTarget)
        m_Desc->pLinkAttackTarget->Damaged(&Desc);

    // 플레이어의 애니메이션을 변경해준다.
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_LinkAttack1_GorillaB_S", false, 1.f, 0.f, FALSE, -1.f, 0.f, TRUE);
}

PLAYER_TRANSITION_DESC CPlayer_GigasLinkAttackState::Update(_float fTimeDelta)
{
    m_isEndList[m_iAnimationIndex] = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (true == m_isEndList[0])
    {
        m_pPlayer->Set_Animation("P_Eve_Sword_Normal_LinkAttack1_GorillaB_E", false, 1.f, 0.f, FALSE, -1.f, 0.f, TRUE);
        m_iAnimationIndex = 1;
    }

    //애니 재생이 끝났다면, Idle로 전환하는 코드
    if (true == m_isEndList[1])
    {
        m_tNextState.isChangeMode = true;
        m_tNextState.eMode = PLAYER_MODE::BATTLE;
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
        //여기서 기가스 데미지 주면 됨.
        DEFAULT_DAMAGE_DESC Desc;
        Desc.pSkillData = m_pGameManager->Find_SkillData(1011);
        if(m_Desc->pLinkAttackTarget)
            m_Desc->pLinkAttackTarget->Damaged(&Desc);
    }

    _matrix	SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    _matrix CombinedMatrix = XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixRotationX(XMConvertToRadians(270.f)) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformMatrix);

    //현재 포지션보다 높다면
    if (0 == m_iAnimationIndex && 0.75f <= fAnimationRatio)
        m_isFlagActivated = true;

    if (true == m_isFlagActivated)
        CombinedMatrix.r[3].m128_f32[1] = m_fStartY;

    if (true == m_isStartFrame)
    {
        CPonyTail_Player* pPonytail = static_cast<CPonyTail_Player*>(m_pPlayer->Get_PartObject(TEXT("Part_PonyTail")));
        pPonytail->Teleport_JointChains(CombinedMatrix);
        m_isStartFrame = false;
    }

    m_Desc->pPlayerTransform->Set_WorldMatrix(CombinedMatrix);

    return m_tNextState;
}

_float CPlayer_GigasLinkAttackState::End()
{
    m_Desc->isSuperArmor = false;
    m_Desc->pLinkAttackTarget = nullptr;

    m_Desc->pPlayerController->Set_Active(true);
    m_Desc->pPlayerController->Set_CCTCollision(true);

    return m_fNextBlendRatio;
}

CPlayer_GigasLinkAttackState* CPlayer_GigasLinkAttackState::Create(void* pArg)
{
    CPlayer_GigasLinkAttackState* pState = new CPlayer_GigasLinkAttackState();

    if (FAILED(pState->Initialize(pArg)))
        return nullptr;

    return pState;
}

void CPlayer_GigasLinkAttackState::Free()
{
    __super::Free();
}