#pragma once

#include "Tool_Effect_Defines.h"
#include "Base.h"

NS_BEGIN(Tool_Effect)
class CImgBase;

class CImg_Manager final : public CBase
{
	DECLARE_SINGLETON(CImg_Manager);

private:
	CImg_Manager();
	virtual ~CImg_Manager() = default;

public:
	HRESULT					Ready_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void					Update_Manager(_float fDeletaTime);
	void					Render_Manager();

	HRESULT					ADD_UI(const _wstring& _name, CImgBase* UI);
	void					Remove_UI(const _wstring& _name, CImgBase* UI);

	CImgBase* Find_UI(const _wstring& _name);
	void					ResetData();

private:
	ImGuiIO						m_ImGuiIo = {};
	ImGuiWindowFlags			m_ImGuiWindowFlag = {};

	//복사 데이터를 저장할 맵
	unordered_map<_wstring, pair<CImgBase*, _bool>> m_UiMap;
private:
	HRESULT					ADD_ImgUserInterface();
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	virtual void			Free() override;
};
NS_END