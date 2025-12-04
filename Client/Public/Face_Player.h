#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBone;
class CTexture;
NS_END

NS_BEGIN(Client)

class CFace_Player final : public CPlayer_Parts
{
public:
	enum class FACE_MATERIAL {
		//이 4개는 그냥 둬야함
		DEFAULT, SHADOW, RIMLIGHT, MOTIONBLUR,

		MA_MouthInner_Inst, M_MikeEyeBlend_Inst, M_lacrimal_fluid,
		MI_EVE_Head_V02, MI_EyeRefractive1, MI_EVE_Eyeshadow_Occlusion,
		NewMaterial, MI_EyeBrow1, EyeLight_Inst,
		MI_Teeth, MA_TeethOcculusion_Inst1,
	};
	typedef struct tagFace_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
		void*			pBodyPtr = { nullptr };
	} FACE_PLAYER_DESC;

private:
	CFace_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFace_Player(const CFace_Player& Prototype);
	virtual ~CFace_Player() = default;

public:
	HRESULT Mapping_Shader_Material(_uint iIdx);

	virtual void			Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)override {};
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	CModel* m_pBodyModelCom = { nullptr };
	// 아래 둘은 테스트용 텍스쳐입니다 혹시 치우고 싶으시면 말씀좀
	CTexture* m_pSpecDetailTextureCom = { nullptr };
	CTexture* m_pSSSAOCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Bind_BoneToPartBody(void* pArg);

public:
	static CFace_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END