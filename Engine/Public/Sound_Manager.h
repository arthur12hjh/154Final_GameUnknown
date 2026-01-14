#pragma once
#include "Base.h"

NS_BEGIN(Engine)

typedef struct SoundCallBackDesc
{
	function<void(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)> EndCallBackFunc;
	_uint							pChannelTrackPosition;
}SOUND_CALLBACK_DESC;

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
	void Manager_SetChannelVolume(CHANNELID eID, float fVolume, _bool bIsLerp);

	void	Tick(_float fTimeDelta);
	_uint	Get_BGMLength(const TCHAR* pSoundKey);

	_uint	Get_ChannelLength(CHANNELID eChannelID, _uint iIndex);
	_float	Get_ChannelRatio(CHANNELID eChannelID, _uint iIndex);
	
	void	Get_BGMMinVolume(_float	fBGMMinVolume);
private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD::Sound*>	m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	vector<FMOD::Channel*>			m_pChannelArr[CHANNELID::END];
	vector<SOUND_CALLBACK_DESC*>	m_ChannelEndCallBacks[CHANNELID::END];

	_float							m_pChannelVolume[CHANNELID::END];
	_bool							m_bIsLerpBGMVolume;
	_float							m_fBGMMinVolume = { 2.f };

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System*					m_pSystem;



private:
	void						LoadSoundFile();
	void						Remove_EndSound();

public :
	static	CSound_Manager*		Create();
	virtual void				Free() override;

};
NS_END