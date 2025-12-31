#include "pch.h"

#include "Player_ScarletLinkAttackState.h"

#include "Player.h"
#include "Nayitba.h" 
#include "GameInstance.h"
#include "GameManager.h"

CPlayer_ScarletLinkAttackState::CPlayer_ScarletLinkAttackState()
	: CPlayerState{}
{
}

HRESULT CPlayer_ScarletLinkAttackState::Initialize(void* pArg)
{
    SOCKETMATRIX_DESC* pSocketMatrixDesc = static_cast<SOCKETMATRIX_DESC*>(pArg);

    // 고릴라 TransformMatrix를 던져줘야함
    m_pParentTransformMatrix = pSocketMatrixDesc->pParentTransformMatrix;
    m_pSocketMatrix = pSocketMatrixDesc->pSocketMatrix;

    m_eState = PLAYER_STATE::SCARLET_LINKATTACK;

    m_Desc->isSuperArmor = true;
    m_Desc->pPlayerController->Set_Active(false);

    return S_OK;
}

void CPlayer_ScarletLinkAttackState::Start(void* pArg, _float fBlendRatio)
{
    // 클래스의 상태를 변경해준다.
    m_eState = PLAYER_STATE::SCARLET_LINKATTACK;

    DEFAULT_DAMAGE_DESC Desc;
    Desc.pSkillData = m_pGameManager->Find_SkillData(1010);
    m_Desc->pLinkAttackTarget->Damaged(&Desc);

    // 플레이어의 애니메이션을 변경해준다. 
    // 여기서 애니메이션 재생속도 조절가능해.
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_LinkAttack1_Scarlet", false, 1.f, 0.f, FALSE, -1.f, 0.f, TRUE);
}

PLAYER_TRANSITION_DESC CPlayer_ScarletLinkAttackState::Update(_float fTimeDelta)
{
    _bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta);

    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    _matrix CombinedMatrix = XMMatrixRotationZ(XMConvertToRadians(270.f)) * XMMatrixRotationX(XMConvertToRadians(180.f)) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformMatrix);

    m_pPlayer->GetTransform()->Set_State(STATE::RIGHT, CombinedMatrix.r[0]);
    m_pPlayer->GetTransform()->Set_State(STATE::UP, CombinedMatrix.r[1]);
    m_pPlayer->GetTransform()->Set_State(STATE::LOOK, CombinedMatrix.r[2]);
    m_pPlayer->GetTransform()->Set_State(STATE::POSITION, CombinedMatrix.r[3]);


    return m_tNextState;
}

_float CPlayer_ScarletLinkAttackState::End()
{
    m_Desc->isSuperArmor = false;
    m_Desc->pPlayerController->Set_Active(true);
    m_Desc->pLinkAttackTarget = nullptr;

    return m_fNextBlendRatio;
}

CPlayer_ScarletLinkAttackState* CPlayer_ScarletLinkAttackState::Create(void* pArg)
{
	CPlayer_ScarletLinkAttackState* pState = new CPlayer_ScarletLinkAttackState();

	if (FAILED(pState->Initialize(pArg)))
		return nullptr;

	return pState;
}

void CPlayer_ScarletLinkAttackState::Free()
{
	__super::Free();
}
