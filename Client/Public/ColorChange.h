#pragma once

#include "Client_Defines.h"
#include "ReserveDeferred.h" 

NS_BEGIN(Engine)
class CGameInstance;
class CTexture;
NS_END

/* 
#####################################
클라이언트단 후처리 예약 객체. 기본적으로 CReserveDeferred를 상속받음. 

지금 테스트 로직 넣어둔건 넘버패드 1번으로 켜고, 넘버패드 2번으로 끌 수 있어. (플레이어 내부 업데이트에서 동작 중)
만약 잘 동작한다면, 켰을떄 화면이 붉게 나와야 해.
셰이더 파일은 07. ReserveDeferred 필터 내부의 Shaderfiles 안에 넣어둔 Shader_ColorChange 셰이더 파일 사용중임.
일단 이 녀석은 테스트용 + 예제로 만든 객체임. 이거 그대로 수정해서 써도 되고, 아니면 따로 만들어도 돼. 
#####################################
*/

/*
#####################################
객체 생성 어디서 어떻게 해줘야함?
-> 지금은 내가 MainApp Ready_ClientDeferred 에서 넣어주고 있음. 그거 참고해
#####################################
*/

/*
 #####################################
그럼 요 객체들 어디서 관리해야함?
-> 클라이언트에 있는 GameManager 내부에 있는 ShaderManager 에 담는걸 전제로 함.
-> 얘가 알아서 렌더러로 전달까지 해줘.
-> 그리고 Set_Active, Set_Desc 등도 얘 통해서 해주면 돼.
-> 더 궁금하면 플레이어 Update_TestLogic 한번 참고해봐.

Active 상태는 기본적으로 False니까, 켜려면 너가 원하는 타이밍에 True로 세팅해주면 돼.
그리고 Set_Desc 이용해서 여러 셰이더에서 사용할 변수(색, 강도 등)들 값도 제어해야 하고.

상위 객체에 있는 	Set_ShaderPassIdx 함수로 출력될 셰이더 패스도 바꿔줄 수 있으니까 참고해.
#####################################
*/

/* 
#####################################
렌더는 어디서 돌아감? -> 렌더러에서 CReserveDeferred에 존재하는 Render함수 끌어다 쓰고 있어.
렌더러의 Render_BackBuffer 함수에서 실행되고 있음.
#####################################
*/

/*
#####################################
렌더타겟 저글링 따로 안해도 됨? -> ㅇㅇ 엔진단에서 미리 생성하고 적용해줌. 
대신 한 프레임에 적용될 수 있는 클라이언트단 후처리 객체는 최대 8개까지야.
이건 필요하면 늘려볼게.
#####################################
*/

/*
#####################################
더 궁금하거나 그런거 있으면 갠디해줘. 그리고 구조 맘에 안들면 바꿔도 되는데 말만 해주고
#####################################
*/

NS_BEGIN(Client)

class CColorChange final : public CReserveDeferred
{
private:
	CColorChange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CColorChange() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Bind_Resources(const _wstring& strRTTag) override;

	// void 포인터로 받아오니까 너가 필요한거 캐스팅해서 세팅해주면 돼.
	// 구조체는 GameStruct에 넣어두긴 했어
	virtual void Set_Desc(void* pArg) override;

private:
	//색 변경
	_float4 m_vColor = { 1.f, 0.f, 0.f, 0.f };
	//강도 설정
	_float  m_fIntensity = { 0 };

	CTexture* m_pGlassTexture = {};

public:
	static CColorChange* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free();
};

NS_END

