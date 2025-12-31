#include "pch.h"

#include "Player_GigasLinkAttackState.h"

#include "Player.h"
#include "Nayitba.h" 
#include "GameInstance.h"
#include "GameManager.h"

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
    m_Desc->pPlayerController->Set_Active(false);

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
    m_Desc->pLinkAttackTarget->Damaged(&Desc);

    // 플레이어의 애니메이션을 변경해준다.
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_LinkAttack1_GorillaB_S", false, 1.f, 0.f, FALSE, -1.f, 0.f, TRUE);
}

PLAYER_TRANSITION_DESC CPlayer_GigasLinkAttackState::Update(_float fTimeDelta)
{
    m_isEndList[m_iAnimationIndex] = m_pPlayer->Play_Animation(fTimeDelta);
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
        m_Desc->pLinkAttackTarget->Damaged(&Desc);
    }

    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    _matrix CombinedMatrix = XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixRotationX(XMConvertToRadians(270.f)) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformMatrix);

    m_pPlayer->GetTransform()->Set_State(STATE::RIGHT, CombinedMatrix.r[0]);
    m_pPlayer->GetTransform()->Set_State(STATE::UP, CombinedMatrix.r[1]);
    m_pPlayer->GetTransform()->Set_State(STATE::LOOK, CombinedMatrix.r[2]);
    m_pPlayer->GetTransform()->Set_State(STATE::POSITION, CombinedMatrix.r[3]);


    return m_tNextState;
}

_float CPlayer_GigasLinkAttackState::End()
{
    m_Desc->isSuperArmor = false;
    m_Desc->pPlayerController->Set_Active(true);
    m_Desc->pLinkAttackTarget = nullptr;

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