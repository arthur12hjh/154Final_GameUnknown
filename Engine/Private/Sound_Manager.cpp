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

		_uint iSoundTotalLength{}, iPosition;
		pSound->getLength(&iSoundTotalLength, FMOD_TIMEUNIT_MS);
		pChannel->getPosition(&iPosition, FMOD_TIMEUNIT_MS);

		if (iPosition >= iSoundTotalLength)
		{
			if (pUserData)
			{
				auto pUserCalllBack = static_cast<function<void(FMOD_CHANNELCONTROL * channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)>*>(pUserData);
				(*pUserCalllBack)(channelcontrol, controltype, callbacktype, commanddata1, commanddata2);
			}
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

	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->stop();
		m_pChannelArr[eID] = nullptr;
	}

	FMOD_RESULT res = m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	if (res != FMOD_OK) {
		printf("playSound error: %s\n", FMOD_ErrorString(res));
	}
	m_pChannelArr[eID]->setVolume(fVolume);
	if (INFINITE != iLoopCount)
	{
		m_pChannelArr[eID]->setLoopCount(iLoopCount);
		if (pFinishedCallBack)
		{
			m_pFinishedFunction[eID] = pFinishedCallBack;
			m_pChannelArr[eID]->setCallback(Finished_BGMSoundCallBack);
			m_pChannelArr[eID]->setUserData(&m_pFinishedFunction[eID]);
		}
	}

	m_pSystem->update();
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

	if (m_pChannelArr[CHANNELID::BGM])
	{
		m_pChannelArr[CHANNELID::BGM]->stop();
		m_pChannelArr[CHANNELID::BGM] = nullptr;
	}

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[CHANNELID::BGM]);
	m_pChannelArr[CHANNELID::BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[CHANNELID::BGM]->setVolume(fVolume);
	if (INFINITE != iLoopCount)
	{
		m_pChannelArr[CHANNELID::BGM]->setLoopCount(iLoopCount);
		if (pFinishedCallBack)
		{
			m_pFinishedFunction[CHANNELID::BGM] = pFinishedCallBack;
			m_pChannelArr[CHANNELID::BGM]->setCallback(Finished_BGMSoundCallBack);
			m_pChannelArr[CHANNELID::BGM]->setUserData(&m_pFinishedFunction[CHANNELID::BGM]);
		}
			
	}

	m_pSystem->update();
}

void CSound_Manager::Manager_StopSound(CHANNELID eID)
{
	_bool bIsChannelPlaying;
	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->isPlaying(&bIsChannelPlaying);
		if (bIsChannelPlaying)
		{
			m_pChannelArr[eID]->stop();
			m_pChannelArr[eID] = nullptr;
		}
	}
}

void CSound_Manager::Manager_StopAll()
{
	_bool bIsChannelPlaying;
	for (int i = 0; i < CHANNELID::END; ++i)
	{
		if (m_pChannelArr[i])
		{
			m_pChannelArr[i]->isPlaying(&bIsChannelPlaying);
			if (bIsChannelPlaying)
			{
				m_pChannelArr[i]->stop();
				m_pChannelArr[i] = nullptr;
			}
		}
	}
}

void CSound_Manager::Manager_SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::Tick(_float fTimeDelta)
{
	m_pSystem->update();
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

_uint CSound_Manager::Get_ChannelLength(CHANNELID eChannelID)
{
	FMOD::Sound* pCurSound = nullptr;
	_uint iLength{};

	m_pChannelArr[ENUM_CLASS(eChannelID)]->getCurrentSound(&pCurSound);
	pCurSound->getLength(&iLength, FMOD_TIMEUNIT_MS);
	return iLength;
}

_float CSound_Manager::Get_ChannelRatio(CHANNELID eChannelID)
{
	FMOD::Sound* pCurSound = nullptr;
	_uint iPosition{}, iLength{};

	m_pChannelArr[ENUM_CLASS(eChannelID)]->getCurrentSound(&pCurSound);
	pCurSound->getLength(&iLength, FMOD_TIMEUNIT_MS);

	m_pChannelArr[ENUM_CLASS(eChannelID)]->getPosition(&iPosition, FMOD_TIMEUNIT_MS);
	return (_float)iPosition / (_float)iLength;
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