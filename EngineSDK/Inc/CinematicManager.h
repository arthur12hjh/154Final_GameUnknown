#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCutScene;
class CCinemaData;

class CCinematicManager final : public CBase
{
private:
	CCinematicManager();
	virtual ~CCinematicManager() = default;

public:
	HRESULT											ADD_CutSceneData(const WCHAR* szSceneTag, const WCHAR* szTag, CCinemaData* pData);

	const CCinemaData*								GetCutSceneData(const WCHAR* szSceneTag, const WCHAR* szTag);
	const unordered_map<_wstring, CCinemaData*>*	GetSceneAllDatas(const WCHAR* szSceneTag);
	const unordered_map<_wstring, CCutScene*>*		GetAllScenes();

	HRESULT											SaveCutSceneData(const WCHAR* szFilePath);
	HRESULT											LoadCutSceneData(const WCHAR* szFilePath);

private:
	unordered_map<_wstring, CCutScene*>				m_SceneDatas;

public:
	static CCinematicManager*		Create();
	virtual void					Free() override;
};
NS_END