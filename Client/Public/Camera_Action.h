#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CCamera_Action final : public CCamera
{
public:
	enum class CAMERA_SOURCE_TYPE
	{
		BONE,
		WORLD,
		LOOK_PIVOT,
	};

	typedef struct CameraSourceDesc
	{
		CAMERA_SOURCE_TYPE		eType;
	}CAMERA_SOURCE_DESC;

	typedef struct tagCamera_Action : public CCamera::CAMERA_DESC
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
		_float		fMouseSensor;
	}CAMERA_PLAYER_DESC;

private:
	CCamera_Action(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Action(const CCamera_Action& Prototype);
	virtual ~CCamera_Action() = default;

public:
	virtual		HRESULT			Initialize_Prototype() override;
	virtual		HRESULT			Initialize(void* pArg) override;
	virtual		void			Priority_Update(_float fTimeDelta) override;
	virtual		void			Update(_float fTimeDelta) override;
	virtual		void			Late_Update(_float fTimeDelta) override;
	virtual		HRESULT			Render() override;

	void		Initialize_CameraAnimationData(_uint iCameraAnimationIndex = -1);

private:
	class CGameManager*					m_pGameManager = { nullptr };
	class CTransform*					m_pPlayerTransform = { nullptr };
	class CCameraBone_Player*			m_pCameraBone = { nullptr };

	class CModel*						m_pBoneModelCom = { nullptr };
	const _float4x4*					m_pBoneCombinedMatrix = { nullptr };
	const _float4x4*					m_pSocketMatrix = { nullptr };


	const CAMERA_ANIMATION_DATA*		m_pCameraAnimationData = {};
	CameraSourceDesc					m_CameraSource = {};
	
	_float4x4							m_CombinedWorldMatrix = {};

	_float3								m_vCameraPivot;
	vector<CAMERA_TRACK_DESC>			m_vFOVTracks;
	vector<CAMERA_TRACK_DESC>			m_vPivotTracks;
	vector<CAMERA_TRACK_DESC>			m_vPositionTracks;
	vector<CAMERA_TRACK_DESC>			m_vRotationTracks;

	_float								m_fCurrentAnimationTime = 0.f;
	_float								m_fAnimationPlayTime = -1.f;

private:
	_float3								Update_CameraTrack(const vector<CAMERA_TRACK_DESC>& vTrackList, float fCurrentTime, const _float3& vBaseValue);

	_matrix								Calculate_CombinedMatrix();



public:
	static CCamera_Action* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END