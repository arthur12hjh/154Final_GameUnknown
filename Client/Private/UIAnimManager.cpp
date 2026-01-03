#include "pch.h"
#include "UIAnimManager.h"

#include "JsonParser.h"
#include "StringHelper.h"

#include "GameInstance.h"
#include "UIBase.h"
#include "UIAnimInstance.h"

/*
이벤트, 애니메이션 매니저 만들기(싱글톤일 필요X)
	데이터를 읽고 소유

UIBaseDesc에 이 UI가 실행할 이벤트, 애니메이션 목록 추가하기

모든 UI들은 HUD를 통해 이벤트, 애니메이션 수행
*/

CUIAnimManager::CUIAnimManager()
	: CBase{}
{
}

HRESULT CUIAnimManager::Initialize()
{
	return S_OK;
}

void CUIAnimManager::Update(_float fTimeDelta)
{
	for (auto iter = m_AnimInstances.begin(); iter != m_AnimInstances.end();)
	{
		CUIAnimInstance* pAnimInstance = *iter;

		if (pAnimInstance->Update(fTimeDelta))
		{
			if (pAnimInstance == m_pTargetAnim)
				m_bTargetAnimFinish = true;

			Safe_Release(pAnimInstance);
			iter = m_AnimInstances.erase(iter);
		}
		else
			++iter;
	}
}

HRESULT CUIAnimManager::Export_Anim_Prefab(_wstring szAnimTag, void* pDesc)
{
	UI_ANIM_DESC AnimDesc = *static_cast<UI_ANIM_DESC*>(pDesc);

	Json jAnim;

	_char szText[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szAnimTag.c_str(), szText);
	jAnim["szAnimTag"] = szText;
	jAnim["fDuration"] = AnimDesc.fDuration;
	jAnim["isLoop"] = AnimDesc.isLoop;
	jAnim["isBeapBeap"] = AnimDesc.isBeapBeap;
	jAnim["isInfluenceChildren"] = AnimDesc.isInfluenceChildren;

	for (auto& TrackDesc : AnimDesc.m_Tracks)
	{
		Json jTrack;
		
		CStringHelper::ConvertWideToUTF(TrackDesc.first.c_str(), szText);
		jTrack["szTrackTag"] = szText;
		jTrack["vStartParam"] = {
			TrackDesc.second.vStartParam.x,
			TrackDesc.second.vStartParam.y,
			TrackDesc.second.vStartParam.z,
			TrackDesc.second.vStartParam.w
		};
		jTrack["vEndParam"] = {
			TrackDesc.second.vEndParam.x,
			TrackDesc.second.vEndParam.y,
			TrackDesc.second.vEndParam.z,
			TrackDesc.second.vEndParam.w
		};

		jAnim["Tracks"].push_back(jTrack);
	}

	_wstring szFilePath{};
	szFilePath = TEXT("../Bin/DataFiles/UI/UI_Anims/") + szAnimTag + TEXT(".json");

	_char szPath[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

	CJsonParser::SaveJsonData(szPath, jAnim);

	MSG_BOX("저장 성공");

	return S_OK;
}

HRESULT CUIAnimManager::Load_Anim_Files()
{
	Clear_AnimInstances();

	WIN32_FIND_DATAW fd;
	_wstring search = L"../Bin/DataFiles/UI/UI_Anims/*.json";

	HANDLE hFind = FindFirstFileW(search.c_str(), &fd);

	if (hFind == INVALID_HANDLE_VALUE)
		return E_FAIL;
	do
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_wstring szfileName = fd.cFileName;

			size_t pos = szfileName.find_last_of(L'.');

			UI_ANIM_DESC AnimDesc{};

			Import_Anim_Prefab(szfileName.substr(0, pos), &AnimDesc);
			m_AnimDatas.emplace(szfileName.substr(0, pos), AnimDesc);
		}
	} while (FindNextFileW(hFind, &fd));

	FindClose(hFind);

	return S_OK;
}

HRESULT CUIAnimManager::Import_Anim_Prefab(_wstring szAnimTag, void* pAnimOut)
{
	Json jAnim;

	_wstring szFilePath{};
	szFilePath = TEXT("../Bin/DataFiles/UI/UI_Anims/") + szAnimTag + TEXT(".json");

	_char szPath[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

	CJsonParser::ReadJsonData(szPath, jAnim);

	WCHAR szText[MAX_PATH]{};

	UI_ANIM_DESC AnimDesc{};

	CStringHelper::ConvertUTFToWide(jAnim["szAnimTag"].get<string>().c_str(), szText);
	AnimDesc.szAnimTag = szText;
	AnimDesc.isLoop = jAnim["isLoop"].get<_bool>();
	AnimDesc.fDuration = jAnim["fDuration"].get<_float>();
	AnimDesc.isBeapBeap = jAnim["isBeapBeap"].get<_bool>();
	AnimDesc.isInfluenceChildren = jAnim["isInfluenceChildren"].get<_bool>();

	for (auto& Track : jAnim["Tracks"])
	{
		UI_ANIM_TRACK_DESC TrackDesc{};

		CStringHelper::ConvertUTFToWide(Track["szTrackTag"].get<string>().c_str(), szText);
		TrackDesc.szTrackTag = szText;
		TrackDesc.vStartParam = {
			Track["vStartParam"][0].get<_float>(),
			Track["vStartParam"][1].get<_float>(),
			Track["vStartParam"][2].get<_float>(),
			Track["vStartParam"][3].get<_float>()
		};
		TrackDesc.vEndParam = {
			Track["vEndParam"][0].get<_float>(),
			Track["vEndParam"][1].get<_float>(),
			Track["vEndParam"][2].get<_float>(),
			Track["vEndParam"][3].get<_float>()
		};

		AnimDesc.Add_UI_Track_Desc(szText, TrackDesc);
	}

	*static_cast<UI_ANIM_DESC*>(pAnimOut) = AnimDesc;

	return S_OK;
}

UI_ANIM_DESC* CUIAnimManager::Get_AnimData(_wstring szAnimTag)
{
	auto iter = m_AnimDatas.find(szAnimTag);

	if(iter == m_AnimDatas.end())
		return nullptr;

	return &iter->second;
}

HRESULT CUIAnimManager::Create_Prefab(_wstring szAnimTag)
{
	auto iter = m_AnimDatas.find(szAnimTag);

	if (iter != m_AnimDatas.end())
		return E_FAIL;

	UI_ANIM_DESC AnimDesc{};
	AnimDesc.szAnimTag = szAnimTag;

	m_AnimDatas.emplace(szAnimTag, AnimDesc);
	
	return S_OK;
}

HRESULT CUIAnimManager::Delete_Prefab(_wstring szAnimTag)
{
	auto Data = m_AnimDatas.find(szAnimTag);

	if (Data == m_AnimDatas.end())
		return E_FAIL;

	m_AnimDatas.erase(Data);

	return S_OK;
}

void CUIAnimManager::Anim_Play(CUIBase* pUI, _wstring szAnimTag, _wstring szPrefabTag, _float fDelay)
{
	auto AnimDesc = m_AnimDatas.find(szPrefabTag);

	if (AnimDesc == m_AnimDatas.end())
		return;

	auto pAnimInstance = CUIAnimInstance::Create(pUI, &AnimDesc->second, szAnimTag, fDelay);
	m_AnimInstances.push_back(pAnimInstance);
}

void CUIAnimManager::Anim_Stop(CUIBase* pUI)
{
	for (auto iter = m_AnimInstances.begin(); iter != m_AnimInstances.end();)
	{
		CUIAnimInstance* pAnimInstance = *iter;

		if (pAnimInstance->Get_TargetUI() == pUI)
		{
			pAnimInstance->Stop_Anim();
			Safe_Release(pAnimInstance);
			iter = m_AnimInstances.erase(iter);
		}
		else
			++iter;
	}
}

_bool CUIAnimManager::Check_Anim_Finish(CUIBase* pUI, _wstring szAnimTag)
{
	/*if(m_bTargetAnimFinish)
		Safe_Release(m_pTargetAnim);*/
	
	return m_bTargetAnimFinish;
}

void CUIAnimManager::Set_TargetAnim(CUIBase* pUI, _wstring szAnimTag)
{
	for (auto& pAnimInstance : m_AnimInstances)
	{
		if (pAnimInstance->Get_TargetUI() == pUI
			&& pAnimInstance->Get_UI_Anim_Desc()->szAnimTag == szAnimTag)
		{
			m_pTargetAnim = pAnimInstance;
			m_bTargetAnimFinish = false;
			//Safe_AddRef(m_pTargetAnim);
		}
	}
}

void CUIAnimManager::Clear_AnimInstances()
{
	for (auto& AnimInstance : m_AnimInstances)
		Safe_Release(AnimInstance);
	m_AnimInstances.clear();
}

CUIAnimManager* CUIAnimManager::Create()
{
	CUIAnimManager* pInstance = new CUIAnimManager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIAnimManager::Free()
{
	__super::Free();

	Clear_AnimInstances();

	//Safe_Release(m_pTargetAnim);
}

