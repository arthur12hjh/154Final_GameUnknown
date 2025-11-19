#include "pch.h"
#include "Notify.h"
#include "GameManager.h"

#include "Model.h"

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

	m_pModelCom = pDesc->pModel;

	m_pModelCom->AnimationChanged = [this](const _char* szAnimationTag) {
		WCHAR* szAnimationTagTmp = nullptr;
		CStringHelper::ConvertUTFToWide(szAnimationTag, szAnimationTagTmp);
		this->AnimationChanged(szAnimationTagTmp);
		};

	return S_OK;
}

void CNotify::AnimationChanged(const _wstring& szAnimationTag)
{
	// 애니메이션이 변경될 경우, 노티파이 큐를 초기화하고 새로 채워넣는다.
	while(!m_NotifyQueue.empty())
		m_NotifyQueue.pop();

	if(nullptr == m_pGameManager)
		return;

	const vector<ANIM_NOTIFY>* pNotifies = m_pGameManager->Find_AnimationNotifyData(szAnimationTag);

	for (auto& pNotify : *pNotifies)
	{
		m_NotifyQueue.push(pNotify);
	}

}

void CNotify::Update(_float fTimeDelta)
{
	while (!m_NotifyQueue.empty())
	{
		// 이벤트 호출
		if (m_NotifyQueue.top().iNotifyKeyFrame >= m_pModelCom->Get_AnimationKeyFrameIndex())
		{
			int a = 10;
			m_NotifyQueue.pop();
		}
		else
			return;
	}
	return;
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
