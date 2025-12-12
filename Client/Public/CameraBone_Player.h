#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"


NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CCameraBone_Player final : public CPlayer_Parts
{
public:
	typedef struct tagCameraBone_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
		class CCharacter* pCharacter = { nullptr };
	}CAMERABONE_DESC;
private:
	CCameraBone_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraBone_Player(const CCameraBone_Player& Prototype);
	virtual ~CCameraBone_Player() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual void Set_Animation (const _char* szAnimationTag,
		_bool isLoop = true,
		_float fAnimationPlayRate = 1.f,
		_float fLerpDuration = 0.12f,
		_bool bIsRestart = FALSE,
		_float fEndTrackPosition = -1.f,
		_float fStartTrackPosition = 0.f,
		_bool isResetTrackPosition = TRUE,
		_bool isRootMotionUpdated = TRUE)override;

	void Initilaize_CameraAnimationData(const CAMERA_ANIMATION_DATA& CameraAnimationData);

private:
	CCollider* m_pColliderCom = { nullptr };

private:
	const _float4x4*					m_pSocketMatrix = { nullptr };
	class CCharacter*					m_pCharacter = { nullptr };

	_float4x4							m_ParentTransformMatrix;

	_char								m_szAnimationTag[MAX_PATH];

	CAMERA_ANIMATION_DATA				m_CameraAnimationData = {};

	vector<CAMERA_TRACK_DESC>			m_vPositionTracks;
	vector<CAMERA_TRACK_DESC>			m_vRotationTracks;

	_uint								m_iPositionTrackIndex;
	_uint								m_iRotationTrackIndex;

	_float								m_fCurrentAnimationTime = 0.f;

	_bool								m_isAnimationPlayed = FALSE;
	_bool								m_bIsStop = FALSE;
	_int								m_iAnimationIndex = 0;


private:
	_float								Get_PositionTrackRatio();
	_float								Get_RotationTrackRatio();

	_float3								Get_BeforePosition();
	_float3								Get_BeforeRotation();

private:
	HRESULT						Ready_Components();
public:
	static CCameraBone_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END