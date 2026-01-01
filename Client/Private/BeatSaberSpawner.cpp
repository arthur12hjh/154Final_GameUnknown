#include "pch.h"
#include "BeatSaberSpawner.h"

#include "GameInstance.h"
#include "StringHelper.h"
#include "Note.h"

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
        _float fRatio = m_pGameInstance->Get_ChannelRatio(CHANNELID::BGM);
           
        if (false == m_SpawnList.empty())
        {
            if (m_SpawnList.front().fSpawnRatio <= fRatio)
                Trigger_SpawnEvent();
        }

        if (1 <= fRatio)
            m_bIsPlay = false;
    }
}

void CBeatSaberSpawner::Late_Update(_float fTimeDelta)
{
}

void CBeatSaberSpawner::Load_BeatData(const char* szSpawnNoteFileData)
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
        Data.fSpawnRatio = atof(DataList[i++].c_str());

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
    NoteDesc.vRotation = { 0.f, XMConvertToRadians(180.f), 0.f, 0.f };
    XMStoreFloat3(&NoteDesc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
    NoteDesc.vTargetPoint = { 0.f, 1.f, 0.f };
    
    NoteDesc.fNoteSpeed = 5.f;
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
