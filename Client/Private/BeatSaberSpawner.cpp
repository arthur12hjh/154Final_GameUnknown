#include "pch.h"
#include "BeatSaberSpawner.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "StringHelper.h"
#include "Note.h"
#include "BeatSaberCharacter.h"

CBeatSaberSpawner::CBeatSaberSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CCharacter(pDevice, pContext)
{
}

CBeatSaberSpawner::CBeatSaberSpawner(const CBeatSaberSpawner& rhs) :
    CCharacter(rhs)
{
}

HRESULT CBeatSaberSpawner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBeatSaberSpawner::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    BEATSABER_SPAWNER_DESC* pDesc = static_cast<BEATSABER_SPAWNER_DESC*>(pArg);
    m_pPlayerTransform = pDesc->pPlayerTransform;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBeatSaberSpawner::Priority_Update(_float fTimeDelta)
{
}

void CBeatSaberSpawner::Update(_float fTimeDelta)
{
    if (m_bIsPlay)
    {
        m_fTimeAcc += fTimeDelta;

        if (m_fTimeAcc >= ((m_fDelay - m_fNoteToValiTime) * 1.5f))
        {
            m_fSongTime += fTimeDelta;
        }

        if (!m_SpawnList.empty())
        {
            float songTime = m_fSongTime;

            if (songTime >= m_SpawnList.front().fSpawnRatio)
            {
                Trigger_SpawnEvent();
            }
        }

        if (m_fSongTime >= m_fSongLength || m_SpawnList.empty())
        {
            CBeatSaberCharacter* pDororong = dynamic_cast<CBeatSaberCharacter*>(CGameManager::GetInstance()->GetBeatSaberCharacter());

            if (!pDororong)
                return;

            if(pDororong->GetBeatSaberCharacterDesc().iComboCnt >= m_iNoteCount)
                pDororong->Set_FullCombo(true);

            m_bIsPlay = false;
            Safe_Release(pDororong);
        }
    }
}

void CBeatSaberSpawner::Late_Update(_float fTimeDelta)
{
}

void CBeatSaberSpawner::Load_BeatData(const char* szSpawnNoteFileData, _float fSongTime, _uint iBPM, _float fDelay)
{
    // 여기서 데이터를 로드해서 가져오기
    NOTE_DATA_DESC Data = {};

    vector<string> DataList;
    CStringHelper::CSVRead(szSpawnNoteFileData, DataList);

    _uint iLastIndex = (_uint)DataList.size();
    for (_uint i = 3; i < iLastIndex;)
    {
        Data.NoteType = NOTE_TYPE(atoi(DataList[i++].c_str()));
        Data.eDirection = DIRECTION(atoi(DataList[i++].c_str()));

        _uint iIdx = atoi(DataList[i++].c_str());
        _float fNoteTimePerBPM = 60.f / (iBPM * 2.f);

        m_fTimeAcc = 0.f;
        m_fNoteToValiTime = sqrtf(50.f * 50.f + 50.6f * 50.6f) / 30.f /*+ (fNoteTimePerBPM * 4.f)*/;
        m_fSongTime = -m_fNoteToValiTime;
        m_fSongLength = fSongTime;
        m_fDelay = fDelay;

        _float fBeatTime = m_fTimeAcc + (fNoteTimePerBPM * (iIdx));

        Data.fSpawnRatio = fBeatTime + m_fSongTime;

        switch (Data.eDirection)
        {
        case DIRECTION::LEFT_FRONT: case DIRECTION::RIGHT_FRONT:
            Data.vBoundAnimFrame = { 38.f, 40.f };
            break;
        case DIRECTION::LEFT_BACK: case DIRECTION::LEFT:
            Data.vBoundAnimFrame = { 18.f, 30.f };
            break;
        case DIRECTION::RIGHT_BACK: case DIRECTION::RIGHT:
            Data.vBoundAnimFrame = { 0.f, 10.f };
            break;
        }

        m_SpawnList.push(Data);
    }

    m_iNoteCount = iLastIndex;
    m_bIsPlay = true;
}

HRESULT CBeatSaberSpawner::Ready_Components()
{
    return S_OK;
}

void CBeatSaberSpawner::Trigger_SpawnEvent()
{
    const NOTE_DATA_DESC& NoteData = m_SpawnList.front();

    CNote::NOTE_DESC NoteDesc = {};
    NoteDesc.bIsApplyTransform = true;
    NoteDesc.vScale = { 1.f, 1.f, 1.f };
    NoteDesc.vRotation = { 0.f, XMConvertToRadians(220.f), 0.f, 0.f };
    XMStoreFloat3(&NoteDesc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat3(& NoteDesc.vTargetPoint, XMLoadFloat4x4(m_pPlayerTransform).r[3]);
    
    NoteDesc.fNoteSpeed = 30.f;
    NoteDesc.NoteType = NoteData.NoteType;
    NoteDesc.eDirection = NoteData.eDirection;
    NoteDesc.fSpawnRatio = NoteData.fSpawnRatio;
    NoteDesc.vBoundAnimFrame = NoteData.vBoundAnimFrame;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_GameObject_BeatNote"),
        ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Layer_Note"), &NoteDesc)))
        return;

    m_SpawnList.pop();
}

CBeatSaberSpawner* CBeatSaberSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBeatSaberSpawner* pBeatSaberSpanwer = new CBeatSaberSpawner(pDevice, pContext);
    if (FAILED(pBeatSaberSpanwer->Initialize_Prototype()))
    {
        Safe_Release(pBeatSaberSpanwer);
        MSG_BOX("Create Fail : Beat Saber Spanwer");
    }
    return pBeatSaberSpanwer;
}

CGameObject* CBeatSaberSpawner::Clone(void* pArg)
{
    CBeatSaberSpawner* pBeatSaberSpanwer = new CBeatSaberSpawner(*this);
    if (FAILED(pBeatSaberSpanwer->Initialize(pArg)))
    {
        Safe_Release(pBeatSaberSpanwer);
        MSG_BOX("Clone Fail : Beat Saber Spanwer");
    }
    return pBeatSaberSpanwer;
}

void CBeatSaberSpawner::Free()
{
    __super::Free();
}
