#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CCinemaComponent final : public CComponent
{
private:
	CCinemaComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinemaComponent(const CCinemaComponent& Prototype);
	virtual ~CCinemaComponent() = default;

public:
	virtual HRESULT							Initialize_Prototype(const char* CinemaFilePath);
	virtual HRESULT							Initialize(void* pArg) override;

public:
	void									Insert_KeyFrame(KEYFRAME KeyFrame, _int iIndex = -1);
	void									Remove_KeyFrame(_uint iKeyFrameIndex);

	size_t									GetNumKeyFrame();
	HRESULT									Save_FileData(const char* CinemaFilePath);

private:
	vector<KEYFRAME>*						m_pKeyFrameList = { nullptr };

private :
	HRESULT									Read_FileData(const char* CinemaFilePath);

public:
	static	CCinemaComponent*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* CinemaFilePath = "");
	virtual CComponent*						Clone(void* pArg) override;
	virtual void							Free() override;

};
NS_END