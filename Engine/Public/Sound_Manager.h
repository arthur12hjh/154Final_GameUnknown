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
	void Manager_PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume, _uint iLoopCount, function<void(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)> pFinishedCallBack);
	void Manager_PlayBGM(const TCHAR* pSoundKey, float fVolume, _uint iLoopCount, function<void(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)> pFinishedCallBack);
	void Manager_StopSound(CHANNELID eID);
	void Manager_StopAll();
	void Manager_SetChannelVolume(CHANNELID eID, float fVolume);

	void	Tick(_float fTimeDelta);
	_uint	Get_BGMLength(const TCHAR* pSoundKey);

	_uint	Get_ChannelLength(CHANNELID eChannelID);
	_float	Get_ChannelRatio(CHANNELID eChannelID);

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD::Sound*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[CHANNELID::END];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System* m_pSystem;

	function<void(FMOD_CHANNELCONTROL* channelcontrol,
		FMOD_CHANNELCONTROL_TYPE controltype,
		FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
		void* commanddata1, void* commanddata2)> m_pFinishedFunction[CHANNELID::END];
private:
	void LoadSoundFile();

public :
	static	CSound_Manager*		Create();
	virtual void				Free() override;

};
NS_END