#include "pch.h"

#include "Player_ScarletLinkAttackState.h"

#include "Player.h"
#include "Nayitba.h" 
#include "GameInstance.h"
#include "GameManager.h"
#include "PonyTail_Player.h"

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

    m_pGameInstance->Active_RadialBlur(1.1f, 16, 0.66f);

    CGameInstance::GetInstance()->Set_MoitonBlur_Active(false);

    CGameInstance::GetInstance()->ADD_FrameFinalFunction([&]() {
        CGameInstance::GetInstance()->Set_MoitonBlur_Active(true);
        }, 4);

    m_pGameInstance->Manager_PlaySound(TEXT("sfx_low01_01.wav"), CHANNELID::EFFECT, 5.f, 1.f);


    m_Desc->isLinkAttackAvailable = false;

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
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_LinkAttack1_Scarlet", false, 3.f, 0.f, FALSE, -1.f, 0.f, TRUE);
}

PLAYER_TRANSITION_DESC CPlayer_ScarletLinkAttackState::Update(_float fTimeDelta)
{
    _bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    Play_Sound(fAnimationRatio);

    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    _matrix CombinedMatrix = XMMatrixRotationZ(XMConvertToRadians(270.f)) * XMMatrixRotationX(XMConvertToRadians(180.f)) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformMatrix);

    if (true == m_isStartFrame)
    {
        CPonyTail_Player* pPonytail = static_cast<CPonyTail_Player*>(m_pPlayer->Get_PartObject(TEXT("Part_PonyTail")));
        pPonytail->Teleport_JointChains(CombinedMatrix);
        m_isStartFrame = false;
    }

    //240 프레임 이상은 본 안따라가게 처리?
    if (!(240 <= m_pPlayer->Get_iTrackPosition()))
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

_float CPlayer_ScarletLinkAttackState::End()
{
    m_Desc->isSuperArmor = false;
    m_Desc->pPlayerController->Set_Active(true);
    m_Desc->pLinkAttackTarget = nullptr;
    m_Desc->isLinkAttackAvailable = true;

    return m_fNextBlendRatio;
}

void CPlayer_ScarletLinkAttackState::Play_Sound(_float fAnimRatio)
{
    switch (m_iSoundCount)
    {
    case 0:
        if (fAnimRatio >= 0.069f)
        {
            m_pGameInstance->Manager_PlaySound(TEXT("sfx_low_hit1_01_MONO.wav"), CHANNELID::EFFECT, 5.f, 1.f);
            m_iSoundCount++;
        }
        break;
    case 1:
        if (fAnimRatio >= 0.220f)
        {
            m_pGameInstance->Manager_PlaySound(TEXT("Hit_sword_nomarl_01.wav"), CHANNELID::EFFECT, 5.f, 1.f);
            m_iSoundCount++;
        }
        break;
    case 2:
        if (fAnimRatio >= 0.318f)
        {
            m_pGameInstance->Manager_PlaySound(TEXT("Hit_sword_nomarl_04.wav"), CHANNELID::EFFECT, 5.f, 1.f);
            m_iSoundCount++;
        }
        break;
    case 3:
        if (fAnimRatio >= 0.481f)
        {
            m_pGameInstance->Manager_PlaySound(TEXT("Hit_sword_nomarl_03.wav"), CHANNELID::EFFECT, 5.f, 1.f);
            m_iSoundCount++;
        }
        break;
    case 4:
        if (fAnimRatio >= 0.742f)
        {
            m_pGameInstance->Manager_PlaySound(TEXT("M_Warp_fx.wav"), CHANNELID::EFFECT, 5.f, 1.f);
            m_iSoundCount++;
        }

        break;
    default:
        break;
    }
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
