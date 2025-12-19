#pragma once
#include "Client_Defines.h"
#include "UIStruct.h"
#include "VIBuffer_Instance.h"

NS_BEGIN(Client)
class CUIInstanceBuffer final : public CVIBuffer_Instance
{
public:
	typedef struct tagUIInstanceDesc
	{
		_uint			iNumInstance = { 0 };
		_float2			vUVAtlasSize = { 0.f, 0.f };
		_float2			vUVAtlasOffset = { 0.f, 0.f };
		_float2			vAtlasIndex = { 0.f, 0.f };
	}UI_INSTANCE_DESC;

private:
	CUIInstanceBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIInstanceBuffer(const CUIInstanceBuffer& Prototype);
	virtual ~CUIInstanceBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype(const UI_INSTANCE_DESC* pInstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources();

	void Update_Instance(vector<VTX_INSTANCE_DESC> Instances);

private:
	VTX_INSTANCE_DESC* m_pInstanceVertices = { nullptr };
	
public:
	static		CUIInstanceBuffer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INSTANCE_DESC* pInstanceDesc);
	virtual		CComponent*			Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END