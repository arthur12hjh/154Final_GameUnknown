#pragma once

#include "Client_Defines.h"
#include "GameStruct.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CShader;
class CReserveDeferred;
NS_END

NS_BEGIN(Client)

/*
셰이더 관리, 클라단 셰이더 예약,
필요한 타이밍에 렌더링 효과 추가
*/

class CShaderManager : public CBase
{
private:
	CShaderManager();
	virtual ~CShaderManager() = default;


public:
	HRESULT		Initialize();
	HRESULT		Add_Shader(LEVEL eLevelID, const _wstring& strShaderTag, class CShader* pShader);

	class CShader* Get_Shader(LEVEL eLevelID, const _wstring& strShaderTag);

	void Update(_float fTimeDelta);
	void Clear(LEVEL eLevelID);
	HRESULT Bind_CamInfo(LEVEL eLevelID);

	HRESULT Add_ReserveDeferred(const _wstring& strReserveDeferredTag, class CReserveDeferred* pReserveDeferred = nullptr);
	void Set_Active_ReserveDeferred(const _wstring& strReserveDeferredTag, _bool bFlag);
	void Set_Desc_ReserveDeferred(const _wstring& strReserveDeferredTag, void* pArg);

	/* 레벨에 따라 셰이더 세팅을 자동으로 바꿔줌. */
	/* 파일 입출력으로 하는게 좋을거같긴한데 */
	/* 시간없어서 일단 Switch문으로 변환 */
	void Change_ShaderSetting(LEVEL eLevelID, _uint iIdx = 0);
private:
	CGameInstance* m_pGameInstance = { nullptr };
	map<_wstring, class CShader*>* m_Shaders = {};
	map<_wstring, class CReserveDeferred*> m_ReserveDeferredShaders = {};

	MOTIONBLUR_DESC* m_pMotionBlurDesc = {};
	FOG_DESC* m_pFogDesc = {};
	BLOOM_DESC* m_pBloomDesc = {};
	DOF_DESC* m_pDoFDesc = {};
	SSAO_DESC* m_pSSAODesc = {};
	VOLUMETRIC_DESC* m_pVolumetricDesc = {};
	HDR_DESC* m_pHDRDesc = {};
	LIGHT_DESC* m_pDirectionalLightDesc = { nullptr };
	VOLUMEFOG_DESC* m_pVolumeFogDesc = { nullptr };

	vector<class CTargetLight*> m_TargetLights = {};

private:
	void Load_Desert_ShaderSettings();
	void Load_Scarlet_ShaderSettings();
	void Load_TargetLights();
	void Load_Scarlet_Phase2_ShaderSettings();

	/* 홍련맵 셰이더 */
	/*
	1. 컷씬 끝나면 약한 점광원 2개 이브랑 홍련한테 붙이기
	2. 2페이즈 들어가면 화면 붉게 (광원처리?) + 점광원 범위 줄이기
	*/
public:
	static CShaderManager* Create();
	virtual void Free() override;
};

NS_END