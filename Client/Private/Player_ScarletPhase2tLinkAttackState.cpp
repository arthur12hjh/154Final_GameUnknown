#include "pch.h"

#include "Player_ScarletPhase2tLinkAttackState.h"

#include "Player.h"
#include "Nayitba.h" 
#include "GameInstance.h"
#include "GameManager.h"


CPlayer_ScarletPhase2tLinkAttackState::CPlayer_ScarletPhase2tLinkAttackState()
    : CPlayerState{}
{
}

HRESULT CPlayer_ScarletPhase2tLinkAttackState::Initialize(void* pArg)
{
    SOCKETMATRIX_DESC* pSocketMatrixDesc = static_cast<SOCKETMATRIX_DESC*>(pArg);

    // 고릴라 TransformMatrix를 던져줘야함
    m_pParentTransformMatrix = pSocketMatrixDesc->pParentTransformMatrix;
    m_pSocketMatrix = pSocketMatrixDesc->pSocketMatrix;

    m_eState = PLAYER_STATE::SCARLET_PHASE2_LINKATTACK;

    m_Desc->isSuperArmor = true;
    m_Desc->pPlayerController->Set_Active(false);

    m_pGameInstance->Active_RadialBlur(1.1f, 16, 0.66f);

    CGameInstance::GetInstance()->Set_MoitonBlur_Active(false);

    CGameInstance::GetInstance()->ADD_FrameFinalFunction([&]() {
        CGameInstance::GetInstance()->Set_MoitonBlur_Active(true);
        }, 4);

    return S_OK;
}

void CPlayer_ScarletPhase2tLinkAttackState::Start(void* pArg, _float fBlendRatio)
{    // 클래스의 상태를 변경해준다.
    m_eState = PLAYER_STATE::SCARLET_PHASE2_LINKATTACK;

    DEFAULT_DAMAGE_DESC Desc;
    Desc.pSkillData = m_pGameManager->Find_SkillData(1010);
    m_Desc->pLinkAttackTarget->Damaged(&Desc);

    // 플레이어의 애니메이션을 변경해준다. 
    // 여기서 애니메이션 재생속도 조절가능해.
    m_pPlayer->Set_Animation("Hit_Scarlet_GroggyCounterTry", false, 3.f, 0.f, FALSE, -1.f, 0.f, TRUE);
    m_Desc->isUsingScarletLink2 = true;
}

PLAYER_TRANSITION_DESC CPlayer_ScarletPhase2tLinkAttackState::Update(_float fTimeDelta)
{
    _bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta);

    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    _matrix CombinedMatrix = XMMatrixRotationZ(XMConvertToRadians(270.f)) * XMMatrixRotationX(XMConvertToRadians(180.f)) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformMatrix);

    //45 이상부턴 본 안따라가게 처리.
    if ((5 >= m_pPlayer->Get_iTrackPosition()))
    {
        m_pPlayer->GetTransform()->Set_State(STATE::RIGHT, CombinedMatrix.r[0]);
        m_pPlayer->GetTransform()->Set_State(STATE::UP, CombinedMatrix.r[1]);
        m_pPlayer->GetTransform()->Set_State(STATE::LOOK, CombinedMatrix.r[2]);
        m_pPlayer->GetTransform()->Set_State(STATE::POSITION, CombinedMatrix.r[3]);
    }

    if (true == isAnimationFinished)
    {
        m_tNextState.isChangeMode = true;
        m_tNextState.eMode = PLAYER_MODE::BATTLE;
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
    }

    return m_tNextState;
}

_float CPlayer_ScarletPhase2tLinkAttackState::End()
{
    m_Desc->isSuperArmor = false;
    m_Desc->pPlayerController->Set_Active(true);

    m_Desc->isUsingScarletLink2 = false; 
	return m_fNextBlendRatio;
}

CPlayer_ScarletPhase2tLinkAttackState* CPlayer_ScarletPhase2tLinkAttackState::Create(void* pArg)
{
	CPlayer_ScarletPhase2tLinkAttackState* pState = new CPlayer_ScarletPhase2tLinkAttackState();

	if (FAILED(pState->Initialize(pArg)))
		return nullptr;

	return pState;
}

void CPlayer_ScarletPhase2tLinkAttackState::Free()
{
	__super::Free();
}
