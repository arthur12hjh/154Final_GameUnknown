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

void CSound_Manager::Manager_PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
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

	FMOD_RESULT res = m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	if (res != FMOD_OK) {
		printf("playSound error: %s\n", FMOD_ErrorString(res));
	}
	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::Manager_PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[CHANNELID::BGM]);
	m_pChannelArr[CHANNELID::BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[CHANNELID::BGM]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::Manager_StopSound(CHANNELID eID)
{
	m_pChannelArr[eID]->stop();
}

void CSound_Manager::Manager_StopAll()
{
	for (int i = 0; i < CHANNELID::END; ++i)
		m_pChannelArr[i]->stop();
}

void CSound_Manager::Manager_SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t  fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst64("../Bin/Resources/Sounds/*.*", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resources/Sounds/";	 // 상대 경로
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