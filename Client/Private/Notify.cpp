#include "pch.h"
#include "Notify.h"
#include "GameManager.h"
#include "GameInstance.h"

#include "Model.h"
#include "Character.h"
#include "Effect.h"
#include "AttackHitBox.h"

#include "StringHelper.h"

CNotify::CNotify(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNotify::CNotify(const CNotify& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CNotify::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CNotify::Initialize(void* pArg)
{
	m_pGameManager = CGameManager::GetInstance();

	NOTIFY_DESC* pDesc = static_cast<NOTIFY_DESC*>(pArg);

	m_pModelCom = nullptr;
	m_pCharacter = pDesc->pCharacter;

	return S_OK;
}

void CNotify::AnimationChanged(const _char* szAnimationTag)
{
	// 애니메이션이 변경될 경우, 노티파이 큐를 초기화하고 새로 채워넣는다.
	while(!m_NotifyQueue.empty())
		m_NotifyQueue.pop();

	if(nullptr == m_pGameManager)
		return;

	WCHAR szAnimationTagTmp[MAX_PATH];

	CStringHelper::ConvertUTFToWide(szAnimationTag, szAnimationTagTmp);

	const vector<ANIM_NOTIFY>* pNotifies = m_pGameManager->Find_AnimationNotifyData(szAnimationTagTmp);

	if (nullptr == pNotifies)
		return;

	for (auto& pNotify : *pNotifies)
	{
		m_NotifyQueue.push(pNotify);
	}

}

void CNotify::Set_ModelCom(CModel* pModel)
{
	m_pModelCom = pModel;

	m_pModelCom->AnimationChanged = [this](const _char* szAnimationTag) {
		this->AnimationChanged(szAnimationTag);
		};
}

void CNotify::Update(_float fTimeDelta)
{
	while (!m_NotifyQueue.empty())
	{	
		// 이벤트 호출
		if (m_NotifyQueue.top().iNotifyKeyFrame >= m_pModelCom->Get_AnimationKeyFrameIndex())
		{
			CallNotify(m_NotifyQueue.top());
			m_NotifyQueue.pop();
		}
		else
			return;
	}
	return;
}

HRESULT CNotify::CallNotify(ANIM_NOTIFY AnimNotify)
{
	NOTIFY_TYPE NotifyType = ClassificationNotify(AnimNotify.szNotifyTag);

	if (nullptr == m_pModelCom)
	{
		m_pModelCom = m_pCharacter->Get_BodyModelCom();
		if (nullptr == m_pModelCom)
			return E_FAIL;
	}

	switch (NotifyType)
	{
	case Client::CNotify::PLAY_SFX:
		return Notify_Play_SFX(AnimNotify);
		break;
	case Client::CNotify::PLAY_SOUND:
		return Notify_Play_Sound(AnimNotify);
		break;
	case Client::CNotify::ACTIVE_COLLISION:
		return Notify_Active_Collision(AnimNotify);
		break;
	case Client::CNotify::SET_TRANSFORM:
		return Notify_Set_Transform(AnimNotify);
		break;
	case Client::CNotify::SET_DYNAMICTRANSFORM:
		return Notify_Set_DynamicTransform(AnimNotify);
		break;
	case Client::CNotify::SET_DELTATIMESPEED:
		return Notify_Set_DeltaTimeSpeed(AnimNotify);
		break;
	case Client::CNotify::ADJUST_LIGHT:
		return Notify_Adjust_Light(AnimNotify);
		break;
	case Client::CNotify::PLAY_SCREENSFX:
		return Notify_Play_ScreenSFX(AnimNotify);
		break;
	case Client::CNotify::UNDEFINED:
		return Notify_Undefined(AnimNotify);
		break;
	case Client::CNotify::END:
		return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

CNotify::NOTIFY_TYPE CNotify::ClassificationNotify(const string& szNotifyTag)
{
	if (szNotifyTag == "Play_SFX")             return PLAY_SFX;
	if (szNotifyTag == "Play_Sound")           return PLAY_SOUND;
	if (szNotifyTag == "Active_Collision")     return ACTIVE_COLLISION;
	if (szNotifyTag == "Set_Transform")        return SET_TRANSFORM;
	if (szNotifyTag == "Set_DynamicTransform") return SET_DYNAMICTRANSFORM;
	if (szNotifyTag == "Set_DeltaTimeSpeed")   return SET_DELTATIMESPEED;
	if (szNotifyTag == "Adjust_Light")         return ADJUST_LIGHT;
	if (szNotifyTag == "Play_ScreenSFX")       return PLAY_SCREENSFX;


	return NOTIFY_TYPE::UNDEFINED;
}

HRESULT CNotify::Notify_Play_SFX(ANIM_NOTIFY AnimNotify)
{
	CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
	EffectDesc.fRotationPerSec = 1.f;
	EffectDesc.fSpeedPerSec = 1.f;
	if (AnimNotify.szSocketTag.compare("None") == 0)
	{
		EffectDesc.pRootMatrix = m_pCharacter->GetTransform()->Get_WorldMatrixPtr();
	}
	else
	{
		EffectDesc.pRootMatrix = m_pModelCom->Get_BoneMatrixPtr(AnimNotify.szSocketTag.c_str());
	}
	EffectDesc.vPos = XMVectorSet(AnimNotify.vNotifyPosition.x, AnimNotify.vNotifyPosition.y, AnimNotify.vNotifyPosition.z, 1);
	EffectDesc.fRot = _float3(AnimNotify.vNotifyRotation.x, AnimNotify.vNotifyRotation.y, AnimNotify.vNotifyRotation.z);
	EffectDesc.fSize = 1.f;

	_TCHAR szEffectTag[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg01.c_str(), szEffectTag);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), szEffectTag,
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNotify::Notify_Play_Sound(ANIM_NOTIFY AnimNotify)
{
	_TCHAR szNotifyTag[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg01.c_str(), szNotifyTag);
	m_pGameInstance->Manager_PlaySound(szNotifyTag, CHANNELID::EFFECT, 1.f);

	return S_OK;
}

HRESULT CNotify::Notify_Active_Collision(ANIM_NOTIFY AnimNotify)
{
	// 콜리전을 생성 Or 콜리전 On
	// ANIM_NOTIFY
	// szNotifyTag		=> Notify Event Type
	
	// Create Notify
	// szNotifyArg01	=> ProtoType Name
	// szNotifyArg02	=> Layer Name

	// iNumData1		=>	Skill ID
	// iNumData2		=>	Col Type
	// iNumData3		=>	Hit Box Type
	// iNumData4		=>	Hit Object Type

	// bIsLocalPos		=>	UseNotifyTransform

	// vNotifyScale		=> Hit Box Size
	// vNotifyPosition	=> Hit Box Relative Position
	// vNotifyRotation	=> Hit Box Rotation

	_TCHAR szLayerName[MAX_PATH], szProtoType[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg01.c_str(), szProtoType);
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg02.c_str(), szLayerName);

	CAttackHitBox::HIT_BOX_DESC pHitBoxDesc = {};
	auto pSkillData = m_pGameManager->Find_SkillData(AnimNotify.iNumData1);
	pHitBoxDesc.pData = pSkillData;
	
	pHitBoxDesc.eColType = COLLIDER(AnimNotify.iNumData2);
	pHitBoxDesc.eHitBoxType = HIT_TYPE(AnimNotify.iNumData3);
	pHitBoxDesc.eHitObjectType = HIT_TYPE(AnimNotify.iNumData4);
	pHitBoxDesc.bIsApplyTransform = true;
	pHitBoxDesc.pAttacker = m_pCharacter;

	pHitBoxDesc.vScale = pSkillData->vHitBoxExtents;
	pHitBoxDesc.vRotation = AnimNotify.vNotifyRotation;

	_vector vCharacterPosition = m_pCharacter->GetTransform()->Get_State(STATE::POSITION);
	_vector vCharacterLook = m_pCharacter->GetTransform()->Get_State(STATE::LOOK);
	vCharacterLook = vCharacterLook * pSkillData->fRange;
	XMStoreFloat3(&pHitBoxDesc.vPosition, vCharacterPosition);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), szProtoType,
		ENUM_CLASS(LEVEL::GAMEPLAY), szLayerName, &pHitBoxDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNotify::Notify_Set_Transform(ANIM_NOTIFY AnimNotify)
{
	_vector vCurrentPosition = m_pCharacter->Get_Position();

	_matrix matTransform = XMLoadFloat4x4(m_pCharacter->Get_WorldMatrixPtr());
	matTransform.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_vector vMoveRange = XMVector3TransformNormal(XMLoadFloat3(&AnimNotify.vNotifyPosition), matTransform);

	_vector vNotifyPosition = vCurrentPosition + vMoveRange;

	m_pCharacter->GetTransform()->Set_State(STATE::POSITION, vNotifyPosition);


	/* 미적용
	m_pCharacter->Set_Rotation
	*/

	return S_OK;
}

HRESULT CNotify::Notify_Set_DynamicTransform(ANIM_NOTIFY AnimNotify)
{
	return S_OK;
}

HRESULT CNotify::Notify_Set_DeltaTimeSpeed(ANIM_NOTIFY AnimNotify)
{
	return S_OK;
}

HRESULT CNotify::Notify_Adjust_Light(ANIM_NOTIFY AnimNotify)
{
	return S_OK;
}

HRESULT CNotify::Notify_Play_ScreenSFX(ANIM_NOTIFY AnimNotify)
{
	return S_OK;
}

HRESULT CNotify::Notify_Undefined(ANIM_NOTIFY AnimNotify)
{
	return S_OK;
}

CNotify* CNotify::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNotify* pInstance = new CNotify(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNotify");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNotify::Clone(void* pArg)
{
	CNotify* pInstance = new CNotify(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNotify");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNotify::Free()
{
	__super::Free();

}
