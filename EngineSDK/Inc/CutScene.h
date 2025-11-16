#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCinemaData;

class CCutScene final : public CBase
{
private:
	CCutScene();
	virtual ~CCutScene() = default;

public:
	HRESULT						ADD_CutSceneData(const WCHAR* szTag, CCinemaData* pCinemaComponent);

	const CCinemaData*								GetCutSceneData(const WCHAR* szTag);
	const unordered_map<_wstring, CCinemaData*>*	GetAllSceneData();

	HRESULT											Export(const WCHAR* szFilePath);
	HRESULT											Import(const WCHAR* szFilePath);

private:
	unordered_map<_wstring, CCinemaData*>	m_SceneDatas;

public:
	static CCutScene*					Create();
	virtual void						Free() override;
};
NS_END