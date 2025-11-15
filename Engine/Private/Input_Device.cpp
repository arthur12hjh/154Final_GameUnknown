
#include "Input_Device.h"

CInput_Device::CInput_Device()
{
	ZeroMemory(m_CurKeyState, sizeof(m_CurKeyState));
	ZeroMemory(m_PreKeyState, sizeof(m_PreKeyState));
}

HRESULT CInput_Device::Initialize(HINSTANCE Instance, HWND hWnd)
{
	//DINPUT 생성
	if (FAILED(DirectInput8Create(Instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, nullptr)))
		return E_FAIL;

	//키보드 장치 생성
	if (FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	// 장치별 반환 데이터 설정
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);

	// DISCL_NONEXCLUSIVE : 다른 프로그램과 공유 안함 설정
	// DISCL_FOREGROUND : 캡쳐같은 입력을 위한 키워드라는데?
	// DISCL_NOWINKEY : 윈도우 키 사용안함
	DWORD KeyLevel = DISCL_NONEXCLUSIVE |
		DISCL_FOREGROUND |
		DISCL_NOWINKEY;

	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, KeyLevel)))
		return E_FAIL;

	//마우스 장치 생성
	if (FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 장치별 반환 데이터 설정
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	DWORD MouseLevel = DISCL_NONEXCLUSIVE |
		DISCL_FOREGROUND;

	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, MouseLevel)))
		return E_FAIL;

	return S_OK;
}

void CInput_Device::UpdateKeyFrame()
{
	if (false == m_bIsInputFoucs)
		return;

	//매프레임 키보드와 마우스에 대한 입력 포커스를 받아와야만 Input Data가 갱신된다.
	while (m_pKeyboard->Acquire() == DIERR_INPUTLOST);
	while (m_pMouse->Acquire() == DIERR_INPUTLOST);

	//이전 입력과 현재입력을 동기화 시키위함
	memcpy(m_PreKeyState, m_CurKeyState, sizeof(m_CurKeyState));
	memcpy(m_PreMouseState, m_DIMouseSate.rgbButtons, sizeof(m_DIMouseSate.rgbButtons));

	////폴링 방식을 통해서 윈도우 메시지에 접근해 키보드 입력과 마우스 입력을 가져온다.
	m_pKeyboard->GetDeviceState(sizeof(m_CurKeyState), &m_CurKeyState);
	m_pMouse->GetDeviceState(sizeof(m_DIMouseSate), &m_DIMouseSate);
}

_bool CInput_Device::KeyDown(KEY_INPUT eType, _uint KeyState)
{
	if (false == m_bIsInputFoucs)
		return false;

	if (KEY_INPUT::KEYBOARD == eType)
		return  (m_CurKeyState[KeyState] & 0x80) && !(m_PreKeyState[KeyState] & 0x80);
	else
		return  (m_DIMouseSate.rgbButtons[KeyState] & 0x80) && !(m_PreMouseState[KeyState] & 0x80);
}

_bool CInput_Device::KeyPressed(KEY_INPUT eType, _uint KeyState)
{
	if (false == m_bIsInputFoucs)
		return false;

	if (KEY_INPUT::KEYBOARD == eType)
		return  (m_CurKeyState[KeyState] & 0x80) && (m_PreKeyState[KeyState] & 0x80);
	else
		return  (m_DIMouseSate.rgbButtons[KeyState] & 0x80) && (m_PreMouseState[KeyState] & 0x80);
}

_bool CInput_Device::KeyUp(KEY_INPUT eType, _uint KeyState)
{
	if (false == m_bIsInputFoucs)
		return false;

	if (KEY_INPUT::KEYBOARD == eType)
		return !(m_CurKeyState[KeyState] & 0x80) && (m_PreKeyState[KeyState] & 0x80);
	else
		return  !(m_DIMouseSate.rgbButtons[KeyState] & 0x80) && (m_PreMouseState[KeyState] & 0x80);
}

LONG CInput_Device::GetMouseAxis(_uint iAxis)
{
	if (false == m_bIsInputFoucs)
		return false;

	if (0 == iAxis)
		return m_DIMouseSate.lX;
	else if (1 == iAxis)
		return m_DIMouseSate.lY;
	else
		return m_DIMouseSate.lZ;
}

void CInput_Device::SetInputFoucs(_bool bFlag)
{
	m_bIsInputFoucs = bFlag;
}

CInput_Device* CInput_Device::Create(HINSTANCE Instance, HWND hWnd)
{
	CInput_Device* pInputManager = new CInput_Device();
	if (FAILED(pInputManager->Initialize(Instance, hWnd)))
	{
		Safe_Release(pInputManager);
		MSG_BOX("CREATE FAIL : INPUT MANAGER");
	}
	return pInputManager;
}

void CInput_Device::Free()
{
	if (m_pMouse)
	{
		//DINPUT의 Device는 메모리 해제전에 Unacquire함수를
		//호출해야한다고 한다.
		m_pMouse->Unacquire();
	}

	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
	}

	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pDirectInput);
}