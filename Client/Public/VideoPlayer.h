#pragma once
#include "Client_Defines.h"
#include "UIStruct.h"
#include "Component.h"

//NS_BEGIN(Engine)
//struct IMFSourceReader;
//struct ID3D11Texture2D;
//struct ID3D11ShaderResourceView;
//NS_END

NS_BEGIN(Client)
class CVideoPlayer final : public CComponent
{
private:
	CVideoPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVideoPlayer(const CVideoPlayer& Prototype);
	virtual ~CVideoPlayer() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual void Update(_float fDeltaTime);
    virtual HRESULT Render(); // 보통 UI/Quad에서 SRV만 사용

    void Play();
    void Pause();
    void Stop();
    _bool isPlaying() const { return m_bPlaying; }

    HRESULT Set_Source(const _tchar* szPath);
    //ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }
    ID3D11ShaderResourceView* Get_SRV_Y() const { return m_pSRV_Y; }
    ID3D11ShaderResourceView* Get_SRV_UV() const { return m_pSRV_UV; }

    _uint Get_Width() const { return m_iWidth; }
    _uint Get_Height() const { return m_iHeight; }

private:
    HRESULT Create_SourceReader(const _tchar* szPath);
    HRESULT Create_Textures(_uint width, _uint height);
    HRESULT Read_VideoFrame();
    HRESULT SeekToStart();
    void    Release_Source();

private:
    // Media Foundation
    IMFSourceReader* m_pReader{ nullptr };

    // DX11
    ID3D11Texture2D* m_pTexY{ nullptr };
    ID3D11Texture2D* m_pTexUV{ nullptr };
    ID3D11ShaderResourceView* m_pSRV_Y{ nullptr };
    ID3D11ShaderResourceView* m_pSRV_UV{ nullptr };

    // Video Info
    _uint m_iWidth{0};
    _uint m_iHeight{0};
    _float m_fFPS{ 30.f };
    _uint m_iStride{0};
    _uint m_iStrideY = 0;
    _uint m_iStrideUV = 0;

    // Playback
    _bool m_bPlaying{ false };
    _bool m_bLoop{ false };
    _float m_fTimeAcc{ 0.f };
	
public:
	static		CVideoPlayer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*			Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END