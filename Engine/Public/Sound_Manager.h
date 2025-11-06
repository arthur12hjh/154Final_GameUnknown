#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CSound_Manager final : public CBase
{
private :
	CSound_Manager() = default;
	virtual ~CSound_Manager() = default;

public:
	void Initialize();

public:
	void Manager_PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void Manager_PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void Manager_StopSound(CHANNELID eID);
	void Manager_StopAll();
	void Manager_SetChannelVolume(CHANNELID eID, float fVolume);

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD::Sound*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[CHANNELID::END];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System* m_pSystem;

private:
	void LoadSoundFile();

public :
	static	CSound_Manager*		Create();
	virtual void				Free() override;

};
NS_END