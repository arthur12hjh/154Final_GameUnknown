#include "pch.h"
#include "Player_TestState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_TestState::CPlayer_TestState()
    : CPlayerState{}
{
}

//상태가 바뀔때 무조건 실행되는 함수.
//1. 플레이어의 애니메이션을 세팅해주고, 
//2. 클래스의 상태를 변경해줘야 한다.

HRESULT CPlayer_TestState::Initialize(void* pArg)
{
    SOCKETMATRIX_DESC* pSocketMatrixDesc = static_cast<SOCKETMATRIX_DESC*>(pArg);

    // 고릴라 TransformMatrix를 던져줘야함
    m_pParentTransformMatrix = pSocketMatrixDesc->pParentTransformMatrix;
    m_pSocketMatrix = pSocketMatrixDesc->pSocketMatrix;

    return S_OK;
}

void CPlayer_TestState::Start(void* pArg, _float fBlendRatio)
{
    m_isEndList.push_back(FALSE);
    m_isEndList.push_back(FALSE);
    m_iAnimationIndex = 0;


    // 클래스의 상태를 변경해준다.
    m_eState = PLAYER_STATE::TEST_STATE;

    // 플레이어의 애니메이션을 변경해준다.
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_LinkAttack1_GorillaB_S", false, 1.f, 0.f, FALSE, -1.f, 0.f, TRUE);
    //m_pPlayer->AttachBone(Gorilla, "SC_LinkTarget");
}

//상태 업데이트 함수.
// 1. 플레이어의 애니메이션을 재생하고 (이건 상황따라 안할떄도 드물게 있어.)
// 2. 입력에 따른 상태를 처리하고,
// 3. 반환값에 따라 다음 상태로의 반환을 담당한다.

// 일단 지금은 애니 재생이 끝나면 idle로 돌아가게 처리해놨어.
// 키 입력에 따라서도 너 마음대로 커스텀 가능해.
// 이건 잘 고민해봐
PLAYER_TRANSITION_DESC CPlayer_TestState::Update(_float fTimeDelta)
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

// 다음 상태에서 재생할 애니메이션의 Blend 시간을 임의로 조정해줌.
// 기본적으로 0.12 (기본값) 세팅해놨어
_float CPlayer_TestState::End()
{
    return m_fNextBlendRatio;
}

// 생성자 함수
CPlayer_TestState* CPlayer_TestState::Create(void* pArg)
{
    CPlayer_TestState* pState = new CPlayer_TestState();

    if (FAILED(pState->Initialize(pArg)))
        return nullptr;

    return pState;
}
// 소멸자 함수
void CPlayer_TestState::Free()
{
    __super::Free();
}
