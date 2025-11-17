#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCutScene;
class CCinemaTrack;

class CCinematicManager final : public CBase
{
private:
	CCinematicManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCinematicManager() = default;

public:
	HRESULT											ADD_CutSceneData(const WCHAR* szSceneTag, const WCHAR* szTag, CCinemaTrack* pData);

	const CCinemaTrack*								GetCutSceneData(const WCHAR* szSceneTag, const WCHAR* szTag);
	const unordered_map<_wstring, CCinemaTrack*>*	GetSceneAllDatas(const WCHAR* szSceneTag);
	const unordered_map<_wstring, CCutScene*>*		GetAllScenes();

	HRESULT											SaveCutSceneData(const WCHAR* szFilePath);
	HRESULT											LoadCutSceneData(const WCHAR* szFilePath);

private:
	ID3D11Device*									m_pDevice = { nullptr };
	ID3D11DeviceContext*							m_pContext = { nullptr };

	unordered_map<_wstring, CCutScene*>				m_SceneDatas;

public:
	static CCinematicManager*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void					Free() override;
};
NS_END