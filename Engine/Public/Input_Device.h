#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

NS_BEGIN(Engine)

class  CInput_Device : public CBase
{
private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	HRESULT		Initialize(HINSTANCE Instance, HWND hWnd);

	void		UpdateKeyFrame();

	// 마우스 입력 0 : 왼쪽 , 1 : 오른쪽 , 2 가운데 버튼
	// 키보드 입력은 DIK_키이름
	_bool		KeyDown(KEY_INPUT eType, _uint KeyState);
	_bool		KeyPressed(KEY_INPUT eType, _uint KeyState);
	_bool		KeyUp(KEY_INPUT eType, _uint KeyState);

	// 마우스 이동방향 0 : X 축    1  : Y 축    2 : Z축
	LONG		GetMouseAxis(_uint iAxis);

	void		SetInputFoucs(KEY_INPUT eType, _bool bFlag);
	_bool		GetInputFoucs(KEY_INPUT eType);

private:
	LPDIRECTINPUT8					m_pDirectInput = nullptr;
	LPDIRECTINPUTDEVICE8			m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8			m_pMouse = nullptr;

	_bool							m_bIsInputFoucs[2] = { false, false };

	//키보드 입력 상태 배열
	BYTE							m_CurKeyState[MAX_KEY] = {};
	BYTE							m_PreKeyState[MAX_KEY] = {};

	//마우스 입력 상태 배열
	DIMOUSESTATE					m_DIMouseSate = {};
	BYTE							m_PreMouseState[MAX_MOUSEKEY] = {};

public:
	static		CInput_Device*		Create(HINSTANCE Instance, HWND hWnd);
	virtual		void				Free() override;

};

NS_END
#endif // InputDev_h__


