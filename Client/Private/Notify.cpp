#include "pch.h"
#include "Notify.h"
#include "GameManager.h"
#include "GameInstance.h"

#include "Model.h"
#include "Character.h"
#include "Effect.h"
#include "CinematicObject.h"
#include "AttackHitBox.h"

#include "Camera_Action.h"

#include "StringHelper.h"
#include "PartObject.h"

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
	// �ִϸ��̼��� ����� ���, ��Ƽ���� ť�� �ʱ�ȭ�ϰ� ���� ä���ִ´�.
	while (!m_NotifyQueue.empty())
		m_NotifyQueue.pop();

	if (nullptr == m_pGameManager)
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
		// �̺�Ʈ ȣ��
		_uint index = m_pModelCom->Get_AnimationKeyFrameIndex();
		if (m_NotifyQueue.top().iNotifyKeyFrame <= index)
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
	case Client::CNotify::ACTIVE_SFX:
		return Notify_Active_SFX(AnimNotify);
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
	case CNotify::ACTIVE_PARTOBJECT_COLLISION:
		return Notify_Active_PartObject_Collision(AnimNotify);
		break;
	case CNotify::HIT_REACTION:
		return Notify_Hit_Reaction(AnimNotify);
		break;
	case CNotify::SPAWN_OBJECT:
		return Notify_Spawn_Object(AnimNotify);
		break;
	case CNotify::SHOOT_PROJECTILE:
		return Notify_Shoot_Projectile(AnimNotify);
		break;
	case CNotify::ACTIVE_PHYSX_COLLISION:
		return m_pCharacter->CallNotify(CNotify::ACTIVE_PHYSX_COLLISION, &AnimNotify);
		break;
	case CNotify::PLAY_CINEMATIC:
		return Notify_Play_Cinematic(AnimNotify);
		break;
	case CNotify::ATTACK_INTERACTION:
		return m_pCharacter->CallNotify(CNotify::ATTACK_INTERACTION, &AnimNotify);
		break;
	case CNotify::CAMERA_SHAKE:
		return Notify_Camera_Shake(AnimNotify);
	case CNotify::CHANGE_COLOR:
		return m_pCharacter->CallNotify(CNotify::CHANGE_COLOR, &AnimNotify);
		break;
	case CNotify::SET_VISIBLITY:
		return m_pCharacter->CallNotify(CNotify::SET_VISIBLITY, &AnimNotify);
		break;
	case CNotify::SET_CINEMATIC_OBJECT:
		return Set_Cinematic_Object(AnimNotify);
		break;
	case CNotify::PLAY_SCREEN:
		return Notify_Play_Screen(AnimNotify);
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
	if (szNotifyTag == "Play_SFX")						return PLAY_SFX;
	if (szNotifyTag == "Active_SFX")					return ACTIVE_SFX;
	if (szNotifyTag == "Play_Sound")					return PLAY_SOUND;
	if (szNotifyTag == "Active_Collision")				return ACTIVE_COLLISION;
	if (szNotifyTag == "Active_PhysxCollision")			return ACTIVE_PHYSX_COLLISION;
	if (szNotifyTag == "Set_Transform")					return SET_TRANSFORM;
	if (szNotifyTag == "Active_PartObjectCollision")	return ACTIVE_PARTOBJECT_COLLISION; 
	if (szNotifyTag == "Hit_Reaction")					return HIT_REACTION;
	if (szNotifyTag == "Spawn_Object")					return SPAWN_OBJECT;
	if (szNotifyTag == "Attack_Interaction")			return ATTACK_INTERACTION;
	if (szNotifyTag == "Shoot_Projectile")				return SHOOT_PROJECTILE;
	if (szNotifyTag == "Play_Cinematic")				return PLAY_CINEMATIC;
	if (szNotifyTag == "Camera_Shake")					return CAMERA_SHAKE;
	if (szNotifyTag == "Change_Color")					return CHANGE_COLOR;
	if (szNotifyTag == "Set_Visiblity")					return SET_VISIBLITY;
	if (szNotifyTag == "Set_Cinematic_Object")			return SET_CINEMATIC_OBJECT;
	if (szNotifyTag == "Play_Screen")					return PLAY_SCREEN;
	
	return NOTIFY_TYPE::END;
}

HRESULT CNotify::Notify_Play_SFX(const ANIM_NOTIFY& AnimNotify)
{
	const _float4x4* pWorldMatrix = m_pCharacter->GetTransform()->Get_WorldMatrixPtr();

  	CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
	EffectDesc.fRotationPerSec = 1.f;
	EffectDesc.fSpeedPerSec = 1.f;
	EffectDesc.fSpeed = AnimNotify.fNumData01;
	if (!AnimNotify.szNotifyArg08.empty())
	{
		_TCHAR szPartObjectName[MAX_PATH];
		CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg08.c_str(), szPartObjectName);
		pWorldMatrix = m_pCharacter->Get_PartObject(szPartObjectName)->Get_CombinedMatrixPtr();
	}

	if (AnimNotify.szSocketTag.compare("None") == 0)
	{
		EffectDesc.pRootMatrix = nullptr;
		EffectDesc.pWorldMatrix = nullptr;
	}
	else if (AnimNotify.szSocketTag.compare("Transform") == 0)
	{
		EffectDesc.pRootMatrix = pWorldMatrix;
		EffectDesc.pWorldMatrix = nullptr;
	}
	else if (AnimNotify.szSocketTag.compare("NoneBone") != 0)
	{
		EffectDesc.pRootMatrix = m_pModelCom->Get_BoneMatrixPtr(AnimNotify.szSocketTag.c_str());
		EffectDesc.pWorldMatrix = pWorldMatrix;
	}

	EffectDesc.vPos = XMVectorSet(AnimNotify.vNotifyPosition.x, AnimNotify.vNotifyPosition.y, AnimNotify.vNotifyPosition.z, 1);
	EffectDesc.fRot = _float3(XMConvertToRadians(AnimNotify.vNotifyRotation.x), XMConvertToRadians(AnimNotify.vNotifyRotation.y), XMConvertToRadians(AnimNotify.vNotifyRotation.z));
	EffectDesc.fSize = AnimNotify.vNotifyScale.x;

	_float4x4 matTransform;
	if (AnimNotify.iNumData01 == 1 && nullptr != EffectDesc.pRootMatrix) {
		if (nullptr != EffectDesc.pWorldMatrix)
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix) * XMLoadFloat4x4(EffectDesc.pWorldMatrix));
		else
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix));
		_matrix mat = XMLoadFloat4x4(&matTransform);
		EffectDesc.pRootMatrix = nullptr;
		EffectDesc.pWorldMatrix = nullptr;
		EffectDesc.vPos = mat.r[3];
		EffectDesc.vPos += mat.r[0] * AnimNotify.vNotifyPosition.x;
		EffectDesc.vPos += mat.r[1] * AnimNotify.vNotifyPosition.y;
		EffectDesc.vPos += mat.r[2] * AnimNotify.vNotifyPosition.z;
	}
	else if (AnimNotify.iNumData01 == 2 && nullptr != EffectDesc.pRootMatrix) {
		if(nullptr != EffectDesc.pWorldMatrix)
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix) * XMLoadFloat4x4(EffectDesc.pWorldMatrix));
		else
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix));
		EffectDesc.pRootMatrix = nullptr;
		EffectDesc.pWorldMatrix = nullptr;
		EffectDesc.vPos = XMVectorSet(XMVectorGetX(EffectDesc.vPos) + matTransform._41, XMVectorGetY(EffectDesc.vPos) + pWorldMatrix->_42, XMVectorGetZ(EffectDesc.vPos) + matTransform._43, 1);
	}
	else if (AnimNotify.iNumData01 == 3 && nullptr != EffectDesc.pRootMatrix) {
		if (nullptr != EffectDesc.pWorldMatrix)
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix) * XMLoadFloat4x4(EffectDesc.pWorldMatrix));
		else
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix));
		_matrix mat = XMLoadFloat4x4(&matTransform);
		EffectDesc.pRootMatrix = nullptr;
		EffectDesc.pWorldMatrix = nullptr;
		EffectDesc.fRot = _float3(0,0,0);
		EffectDesc.vPos = mat.r[3];
		EffectDesc.vPos += mat.r[0] * AnimNotify.vNotifyPosition.x;
		EffectDesc.vPos += mat.r[1] * AnimNotify.vNotifyPosition.y;
		EffectDesc.vPos += mat.r[2] * AnimNotify.vNotifyPosition.z;
		//EffectDesc.vPos = XMVectorSet(XMVectorGetX(EffectDesc.vPos) + matTransform._41, XMVectorGetY(EffectDesc.vPos) + matTransform._42, XMVectorGetZ(EffectDesc.vPos) + matTransform._43, 1);
	}
	else if (AnimNotify.iNumData01 == 4 && nullptr != EffectDesc.pRootMatrix) {
		if (nullptr != EffectDesc.pWorldMatrix)
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix) * XMLoadFloat4x4(EffectDesc.pWorldMatrix));
		else
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix));
		_matrix mat = XMLoadFloat4x4(&matTransform);
		EffectDesc.pRootMatrix = nullptr;
		EffectDesc.pWorldMatrix = nullptr;
		EffectDesc.vPos = mat.r[3];
		EffectDesc.vPos += mat.r[0] * AnimNotify.vNotifyPosition.x;
		EffectDesc.vPos += mat.r[1] * AnimNotify.vNotifyPosition.y;
		EffectDesc.vPos += mat.r[2] * AnimNotify.vNotifyPosition.z;
	}
	else if (AnimNotify.iNumData01 == 5 && nullptr != EffectDesc.pRootMatrix) {
		if (nullptr != EffectDesc.pWorldMatrix)
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix) * XMLoadFloat4x4(EffectDesc.pWorldMatrix));
		else
			XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(EffectDesc.pRootMatrix));
		EffectDesc.pRootMatrix = nullptr;
		EffectDesc.pWorldMatrix = nullptr;

		_TCHAR szPartObjectName[MAX_PATH];
		CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg07.c_str(), szPartObjectName);
		const _float4x4* pBodyMatrix = m_pCharacter->Get_PartObject(szPartObjectName)->Get_CombinedMatrixPtr();
		EffectDesc.vPos = XMVectorSet(XMVectorGetX(EffectDesc.vPos) + matTransform._41, XMVectorGetY(EffectDesc.vPos) + pBodyMatrix->_42, XMVectorGetZ(EffectDesc.vPos) + matTransform._43, 1);
	}

	_TCHAR szEffectTag[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg01.c_str(), szEffectTag);

	CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), szEffectTag,
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));
	if (AnimNotify.iNumData01 == 3) {
		_TCHAR szPartObjectName[MAX_PATH];
		CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg08.c_str(), szPartObjectName);
		_matrix matDir = XMLoadFloat4x4(m_pCharacter->Get_PartObject(szPartObjectName)->Get_CombinedMatrixPtr());
		pEffect->GetTransform()->Set_State(STATE::RIGHT, matDir.r[0] * EffectDesc.fSize);
		pEffect->GetTransform()->Set_State(STATE::UP, matDir.r[1] * EffectDesc.fSize);
		pEffect->GetTransform()->Set_State(STATE::LOOK, matDir.r[2] * EffectDesc.fSize);
		if (0 != AnimNotify.vNotifyRotation.x)
			pEffect->GetTransform()->Turn(pEffect->GetTransform()->Get_State(STATE::UP), XMConvertToRadians(AnimNotify.vNotifyRotation.x));
		if (0 != AnimNotify.vNotifyRotation.y)
			pEffect->GetTransform()->Turn(pEffect->GetTransform()->Get_State(STATE::RIGHT), XMConvertToRadians(AnimNotify.vNotifyRotation.y));
		if (0 != AnimNotify.vNotifyRotation.z)
			pEffect->GetTransform()->Turn(pEffect->GetTransform()->Get_State(STATE::LOOK), XMConvertToRadians(AnimNotify.vNotifyRotation.z));
	}
	else if (AnimNotify.iNumData01 == 4) {
		_matrix matDir = XMLoadFloat4x4(&matTransform);
		pEffect->GetTransform()->Set_State(STATE::RIGHT, matDir.r[0] * EffectDesc.fSize);
		pEffect->GetTransform()->Set_State(STATE::UP, matDir.r[1] * EffectDesc.fSize);
		pEffect->GetTransform()->Set_State(STATE::LOOK, matDir.r[2] * EffectDesc.fSize);
	}
	if (nullptr == pEffect)
		return E_FAIL;

	if (AnimNotify.szSocketTag.compare("None") == 0)
	{
		pEffect->GetTransform()->Set_State(STATE::POSITION, m_pCharacter->GetTransform()->Get_State(STATE::POSITION) + XMVectorSet(AnimNotify.vNotifyPosition.x, AnimNotify.vNotifyPosition.y, AnimNotify.vNotifyPosition.z, 0));
	}

	m_pCharacter->CallNotify(PLAY_SFX, &AnimNotify);
	return S_OK;
}

HRESULT CNotify::Notify_Active_SFX(const ANIM_NOTIFY& AnimNotify)
{
	_TCHAR szPartTag[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg08.c_str(), szPartTag);

	_TCHAR szObjectTag[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg01.c_str(), szObjectTag);

	m_pCharacter->Active_SFX(szPartTag, szObjectTag, AnimNotify);

	return S_OK;
}

HRESULT CNotify::Notify_Play_Sound(const ANIM_NOTIFY& AnimNotify)
{
	_TCHAR szNotifyTag[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg01.c_str(), szNotifyTag);
	m_pGameInstance->Manager_PlaySound(szNotifyTag, CHANNELID::EFFECT, 1.f);

	return S_OK;
}

HRESULT CNotify::Notify_Play_Cinematic(const ANIM_NOTIFY& AnimNotify)
{
	_float4x4 PrePosMatrix = {};

	m_pGameInstance->SetMainCamera(TEXT("ActionCamera"), &PrePosMatrix);
	CCamera* pCamera = m_pGameInstance->GetMainCamera();

	dynamic_cast<CCamera_Action*>(pCamera)->Initialize_CameraAnimationData(AnimNotify.iNumData01);

	Safe_Release(pCamera);

	return S_OK;
}

HRESULT CNotify::Set_Cinematic_Object(const ANIM_NOTIFY& AnimNotify)
{
	if (dynamic_cast<CCinematicObject*>(m_pCharacter) == nullptr)
		return S_OK;	// E_FAIL로 플래그 받고싶으면 나중에 설정 ㄱㄱ
	
	dynamic_cast<CCinematicObject*>(m_pCharacter)->Set_Cinematic_Object(AnimNotify);

	return S_OK;
}

HRESULT CNotify::Notify_Play_Screen(const ANIM_NOTIFY& AnimNotify)
{
	_TCHAR szNotifyTag[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szSocketTag.c_str(), szNotifyTag);

	m_pGameManager->Set_Active_ReserveDeferred(szNotifyTag, true);
	return S_OK;
}

HRESULT CNotify::Notify_Camera_Shake(const ANIM_NOTIFY& AnimNotify)
{
	m_pGameInstance->Shake_Camera(AnimNotify.fNumData01, AnimNotify.fNumData02);

	return S_OK;
}

HRESULT CNotify::Notify_Active_Collision(const ANIM_NOTIFY& AnimNotify)
{
	m_pCharacter->CallNotify(ACTIVE_COLLISION, &AnimNotify);

	return S_OK;
}

HRESULT CNotify::Notify_Set_Transform(const ANIM_NOTIFY& AnimNotify)
{
	_vector vCurrentPosition = m_pCharacter->Get_Position();

	_matrix matTransform = XMLoadFloat4x4(m_pCharacter->Get_WorldMatrixPtr());
	matTransform.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_vector vMoveRange = XMVector3TransformNormal(XMLoadFloat3(&AnimNotify.vNotifyPosition), matTransform);

	_vector vNotifyPosition = vCurrentPosition + vMoveRange;

	m_pCharacter->GetTransform()->Set_State(STATE::POSITION, vNotifyPosition);


	/* ������
	m_pCharacter->Set_Rotation
	*/

	return S_OK;
}

HRESULT CNotify::Notify_Hit_Reaction(const ANIM_NOTIFY& AnimNotify)
{
	m_pCharacter->CallNotify(HIT_REACTION, &AnimNotify);
	return S_OK;
}

HRESULT CNotify::Notify_Spawn_Object(const ANIM_NOTIFY& AnimNotify)
{
	m_pCharacter->CallNotify(SPAWN_OBJECT, &AnimNotify);
	return S_OK;
}

HRESULT CNotify::Notify_Shoot_Projectile(const ANIM_NOTIFY& AnimNotify)
{
	m_pCharacter->CallNotify(SHOOT_PROJECTILE, &AnimNotify);
	return S_OK;
}

HRESULT CNotify::Notify_Active_PartObject_Collision(const ANIM_NOTIFY& AnimNotify)
{
	_TCHAR szPartObjectName[MAX_PATH], szComponentName[MAX_PATH];
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg01.c_str(), szPartObjectName);
	CStringHelper::ConvertUTFToWide(AnimNotify.szNotifyArg02.c_str(), szComponentName);

	m_pCharacter->Activate_PartObject_Collider(szPartObjectName, szComponentName, AnimNotify);

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
