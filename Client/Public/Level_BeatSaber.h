#pragma once
#include "Client_Defines.h"
#include "ClientStruct.h"
#include "Level.h"

NS_BEGIN(Client)
class CPlaySongEvent;

class CLevel_BeatSaber : public CLevel
{
public:
	typedef struct SavedDororongSaberInfo
	{
		_float4x4	    worldMatrix;
	}SAVEDDORORONGSABERINFO;

	typedef struct NoteDataFile
	{
		WCHAR			SongFileName[MAX_PATH];
		char			NoteFileName[MAX_PATH];
		_float			fSongTime;
		_uint			iBPM;
	}NOTE_DATA_NAME;

private:
	CLevel_BeatSaber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_BeatSaber() = default;

public:
	virtual HRESULT				Initialize() override;
	virtual void				Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	void						Play_GameBGM(const wstring& szFileTag, _float fVolume);
	
private :
	map<const wstring, NOTE_DATA_NAME>			m_SongList = {};
	_float										m_fTime = 10;
	_bool										m_bChangeLevel{ false };
	_bool										m_bLevelTransitioning{ false }; // 레벨 전환 중 체크
	_bool										m_isOverlay{ true };

	_wstring									m_szSongFile{};
	_float										m_fVolume{};
	_bool										m_bSongStart{ false };
	_float										m_fSongDelay{ 1.f };
	_float										m_fTimeAcc{ 0.f };

private:
	HRESULT						Ready_Lights();
	HRESULT						Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_Sky(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_UI(const _wstring& strLayerTag);

	HRESULT						Ready_Layer_BeatSpawner(const _wstring& strLayerTag);
	HRESULT						Load_SongList(const char* szDataFile);

	FMOD_RESULT	 	 			Finished_GameBGM(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2);

	//HRESULT Load_Map_Data();
	//HRESULT Load_Map_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	//HRESULT Load_Instancing_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);

	HRESULT						Load_Light_Data();
	HRESULT						Load_Dororong_Saber_Objects(const _char* szFilePath);
	HRESULT						Load_Dororong_Saber_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	NOTE_DATA_NAME*				Get_FindSongFile(const wstring& szFileTag);

private:
	CPlaySongEvent*				m_pPlaySongEvent{ nullptr };

public:
	static CLevel_BeatSaber*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void				Free() override;

};
NS_END