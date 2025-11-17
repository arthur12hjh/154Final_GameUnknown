#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CTransform;

class ENGINE_DLL CCinemaTrack final : public CComponent
{
private:
	CCinemaTrack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinemaTrack(const CCinemaTrack& Prototype);
	virtual ~CCinemaTrack() = default;

public:
	virtual HRESULT						Initialize_Prototype(const WCHAR* CinemaFilePath);
	virtual HRESULT						Initialize(void* pArg) override;

public:
	void								SetPlayTime(_float fTime);


	void								Insert_KeyFrame(KEYFRAME* KeyFrame, _int iIndex = -1);
	void								Remove_KeyFrame(_uint iKeyFrameIndex);

	size_t								GetNumKeyFrame();
	_float								GetPlayTime();

	HRESULT								Save_FileData(const WCHAR* CinemaFilePath);
	HRESULT								Read_FileData(const WCHAR* CinemaFilePath);

	const vector<KEYFRAME*>*			GetFrameList() { return &m_pKeyFrameList; }

	void								Set_Animation(_float PlayTime);
	_bool								Play_Animation(CTransform* pTransform, _float fTimeDelta);

private:
	_uint								m_iIndex = { };
	_float2								m_fTime = {};
	_bool								m_bIsAnimation = { false };

	vector<KEYFRAME*>					m_pKeyFrameList;

public:
	static	CCinemaTrack*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* CinemaFilePath = L"");
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END