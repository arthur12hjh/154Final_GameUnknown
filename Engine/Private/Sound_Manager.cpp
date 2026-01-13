#include "Sound_Manager.h"

void CSound_Manager::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_RESULT result = FMOD::System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();
	m_pSystem->setDriver(0);
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();
	pInstance->Initialize();
	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		Mypair.second->release();
	}
	m_mapSound.clear();

	for (_uint i = 0; i < CHANNELID::END; ++i)
	{
		for (auto& iter : m_ChannelEndCallBacks[i])
			Safe_Delete(iter);

		m_ChannelEndCallBacks[i].clear();
	}
	
	m_pSystem->release();
	m_pSystem->close();
}

// 함수 호출규약 STDCALL 형식 호출한 녀석이 책임진다는
// 규약떄문에 클라이언트에서 전역함수를 선언하고 포인터를 사용하려고 하면
// 계속 nullptr이 나오는거였음 조심하자
FMOD_RESULT F_CALL Finished_BGMSoundCallBack(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)
{
	switch (controltype)
	{
	case FMOD_CHANNELCONTROL_CHANNEL:
	{
		auto pChannel = (FMOD::Channel*)(channelcontrol);
		void* pUserData = nullptr;
		FMOD::Sound* pSound = {};
		pChannel->getCurrentSound(&pSound);
		pChannel->getUserData(&pUserData);

		_uint iSoundTotalLength{};
		pSound->getLength(&iSoundTotalLength, FMOD_TIMEUNIT_MS);

		if (pUserData)
		{
			auto pUserCalllBack = static_cast<SOUND_CALLBACK_DESC*>(pUserData);
			if (pUserCalllBack->pChannelTrackPosition >= iSoundTotalLength)
				pUserCalllBack->EndCallBackFunc(channelcontrol, controltype, callbacktype, commanddata1, commanddata2);
		}
	}
	break;
	case FMOD_CHANNELCONTROL_CHANNELGROUP:
		break;
	case FMOD_CHANNELCONTROL_MAX:
		break;
	case FMOD_CHANNELCONTROL_FORCEINT:
		break;
	default:
		return FMOD_RESULT::FMOD_ERR_BADCOMMAND;
	}
	return  FMOD_RESULT::FMOD_OK;
}

void CSound_Manager::Manager_PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume, _uint iLoopCount, function<void(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)> pFinishedCallBack)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	bool bPlay = FALSE;

	SOUND_CALLBACK_DESC* SoundDesc = new SOUND_CALLBACK_DESC;
	FMOD::Channel* pBGMChannel = nullptr;
	FMOD_RESULT res = m_pSystem->playSound(iter->second, nullptr, FALSE, &pBGMChannel);
	if (res != FMOD_OK) {
		printf("playSound error: %s\n", FMOD_ErrorString(res));
	}
	pBGMChannel->setVolume(fVolume);
	m_pChannelVolume[eID] = fVolume;

	SoundDesc->EndCallBackFunc = pFinishedCallBack;
	m_ChannelEndCallBacks[eID].push_back(SoundDesc);
	m_pChannelArr[eID].push_back(pBGMChannel);
	if (INFINITE != iLoopCount)
	{
		pBGMChannel->setLoopCount(iLoopCount);
		if (pFinishedCallBack)
		{
			pBGMChannel->setCallback(Finished_BGMSoundCallBack);
			pBGMChannel->setUserData((void*)m_ChannelEndCallBacks[eID].back());
		}
	}

}

void CSound_Manager::Manager_PlayBGM(const TCHAR* pSoundKey, float fVolume, _uint iLoopCount, function<void(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)> pFinishedCallBack)
{
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	SOUND_CALLBACK_DESC* SoundDesc = new SOUND_CALLBACK_DESC;
	FMOD::Channel* pBGMChannel = nullptr;
	m_pSystem->playSound(iter->second, nullptr, FALSE, &pBGMChannel);

	pBGMChannel->setVolume(fVolume);
	m_pChannelVolume[CHANNELID::BGM] = fVolume;

	SoundDesc->EndCallBackFunc = pFinishedCallBack;
	m_ChannelEndCallBacks[CHANNELID::BGM].push_back(SoundDesc);
	m_pChannelArr[CHANNELID::BGM].push_back(pBGMChannel);
	if (INFINITE != iLoopCount)
	{
		pBGMChannel->setMode(FMOD_LOOP_OFF);
		pBGMChannel->setLoopCount(iLoopCount);
		if (pFinishedCallBack)
		{
			pBGMChannel->setCallback(Finished_BGMSoundCallBack);
			pBGMChannel->setUserData((void*)m_ChannelEndCallBacks[CHANNELID::BGM].back());
		}
	}
	else
		pBGMChannel->setMode(FMOD_LOOP_NORMAL);
}

void CSound_Manager::Manager_StopSound(CHANNELID eID)
{
	_bool bIsChannelPlaying;
	auto pBGMChannel = m_pChannelArr[eID];

	_uint iIndex = 0;
	for (auto& iter : m_pChannelArr[eID])
	{
		iter->isPlaying(&bIsChannelPlaying);
		iter->stop();

		Safe_Delete(m_ChannelEndCallBacks[eID][iIndex]);
		iIndex++;
	}

	m_ChannelEndCallBacks[eID].clear();
	m_pChannelArr[eID].clear();
}

void CSound_Manager::Manager_StopAll()
{
	_bool bIsChannelPlaying;
	for (int i = 0; i < CHANNELID::END; ++i)
	{
		_uint iIndex = 0;
		for (auto& iter : m_pChannelArr[i])
		{
			iter->isPlaying(&bIsChannelPlaying);
			iter->stop();

			Safe_Delete(m_ChannelEndCallBacks[i][iIndex]);
			iIndex++;
		}
		m_ChannelEndCallBacks[i].clear();
		m_pChannelArr[i].clear();
	}
}

void CSound_Manager::Manager_SetChannelVolume(CHANNELID eID, float fVolume)
{
	for (auto& iter : m_pChannelArr[eID])
		iter->setVolume(fVolume);
	
	m_pChannelVolume[eID] = fVolume;
}

void CSound_Manager::Tick(_float fTimeDelta)
{
	_bool IsPlay = { false };
	

	for (_uint i = 0; i < CHANNELID::END; ++i)
	{
		size_t iChannelSize = m_pChannelArr[i].size();
		for (_uint j = 0; j < iChannelSize; ++j)
		{
			m_pChannelArr[i][j]->getPosition(&m_ChannelEndCallBacks[i][j]->pChannelTrackPosition, FMOD_TIMEUNIT_MS);

			if(CHANNELID::BGM < i)
				m_pChannelArr[i][j]->isPlaying(&IsPlay);
		}
	}
	
	for (auto& iter : m_pChannelArr[CHANNELID::BGM])
	{
		_float fRatio = {};
		iter->getVolume(&fRatio);

		if (!IsPlay)
		{
			fRatio = Lerp<_float>(fRatio, m_pChannelVolume[CHANNELID::BGM], fTimeDelta);
			iter->setVolume(fRatio);
		}
		else
		{
			fRatio = Lerp<_float>(fRatio, m_fBGMMinVolume, fTimeDelta * 5.f);
			iter->setVolume(fRatio);
		}
	}

	m_pSystem->update();
	Remove_EndSound();
}

_uint CSound_Manager::Get_BGMLength(const TCHAR* pSoundKey)
{
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return 0;
	
	_uint	iLength = {};

	iter->second->getLength(&iLength, FMOD_TIMEUNIT_MS);
	return iLength;
}

_uint CSound_Manager::Get_ChannelLength(CHANNELID eChannelID, _uint iIndex)
{
	FMOD::Sound* pCurSound = nullptr;
	_uint iLength{};

	if (m_pChannelArr[ENUM_CLASS(eChannelID)].size() <= iIndex && 0 > iIndex)
		return 0;

	m_pChannelArr[ENUM_CLASS(eChannelID)][iIndex]->getCurrentSound(&pCurSound);
	pCurSound->getLength(&iLength, FMOD_TIMEUNIT_MS);
	return iLength;

}

_float CSound_Manager::Get_ChannelRatio(CHANNELID eChannelID, _uint iIndex)
{
	FMOD::Sound* pCurSound = nullptr;
	_uint iPosition{}, iLength{};

	if (m_pChannelArr[ENUM_CLASS(eChannelID)].size() <= iIndex && 0 > iIndex)
		return 0.f;

	auto pChannelSound = m_pChannelArr[ENUM_CLASS(eChannelID)][iIndex];
	pChannelSound->getCurrentSound(&pCurSound);
	pCurSound->getLength(&iLength, FMOD_TIMEUNIT_MS);

	pChannelSound->getPosition(&iPosition, FMOD_TIMEUNIT_MS);
	return (_float)iPosition / (_float)iLength;
}

void CSound_Manager::Get_BGMMinVolume(_float fBGMMinVolume)
{
	m_fBGMMinVolume = fBGMMinVolume;
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t  fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst64("../Bin/SoundResources/Sounds/*.*", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Bin/SoundResources/Sounds/";	 // 상대 경로
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		FMOD::Sound* pSound = nullptr;
		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = int(strlen(fd.name) + 1);

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext64(handle, &fd);
	}

	m_pSystem->update();
	_findclose(handle);
}

void CSound_Manager::Remove_EndSound()
{
	_bool IsPlay = { false };
	FMOD_MODE IsMode = FMOD_LOOP_NORMAL;
	for (_uint i = 0; i < CHANNELID::END; ++i)
	{
		_uint iIndex = {};
		for (auto iter = m_pChannelArr[i].begin(); iter != m_pChannelArr[i].end();)
		{

			(*iter)->getMode(&IsMode);
			(*iter)->isPlaying(&IsPlay);
			if (!IsPlay && (FMOD_LOOP_OFF == IsMode || FMOD_DEFAULT == IsMode))
			{
				auto Funciter = m_ChannelEndCallBacks[i].begin() + iIndex;

				Safe_Delete(*Funciter);
				m_ChannelEndCallBacks[i].erase(Funciter);
				iter = m_pChannelArr[i].erase(iter);
			}
			else
			{
				iter++;
				iIndex++;
			}
		}
	}
}
