#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CCinemaData final : public CComponent
{
private:
	CCinemaData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinemaData(const CCinemaData& Prototype);
	virtual ~CCinemaData() = default;

public:
	virtual HRESULT						Initialize_Prototype(const WCHAR* CinemaFilePath);
	virtual HRESULT						Initialize(void* pArg) override;

public:
	void								Insert_KeyFrame(KEYFRAME* KeyFrame, _int iIndex = -1);
	void								Remove_KeyFrame(_uint iKeyFrameIndex);

	size_t								GetNumKeyFrame();

	HRESULT								Save_FileData(const WCHAR* CinemaFilePath);
	HRESULT								Read_FileData(const WCHAR* CinemaFilePath);

	const vector<KEYFRAME*>*			GetFrameList() { return &m_pKeyFrameList; }

private:
	vector<KEYFRAME*>					m_pKeyFrameList;

public:
	static	CCinemaData*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* CinemaFilePath = L"");
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END