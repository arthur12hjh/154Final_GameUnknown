#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCinemaTrack;

class CCutScene final : public CBase
{
private:
	CCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCutScene() = default;

public:
	HRESULT									Initailize(const WCHAR* szFilePath);
	HRESULT									ADD_CutSceneData(const WCHAR* szTag, CCinemaTrack* pCinemaComponent);

	const CCinemaTrack*								GetCutSceneData(const WCHAR* szTag);
	const unordered_map<_wstring, CCinemaTrack*>*	GetAllSceneData();

	HRESULT											Export(const WCHAR* szFilePath);
	HRESULT											Import(const WCHAR* szFilePath);

private:
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };

	unordered_map<_wstring, CCinemaTrack*>	m_SceneDatas;

public:
	static CCutScene*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* szFilePath = L"");
	virtual void						Free() override;
};
NS_END