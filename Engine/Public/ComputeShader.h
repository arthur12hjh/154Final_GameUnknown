#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CComputeShader : public CComponent
{
public :
	enum class BUFFER_TYPE { INPUT, OUTPUT, CONSTATNT, END };

private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComputeShader(const CComputeShader& Prototype);
	virtual ~CComputeShader() = default;

public:
	virtual HRESULT							Initialize_Prototype(const WCHAR* szShaderFilePath, const char* szStartFunctionName, _uint iNumData);
	virtual HRESULT							Initialize(void* pArg) override;
	virtual HRESULT							Render() override;

	// x * y * z 곱한거만큼 셰이더의 그룹이 돌아간다.
	// 컴퓨트 셰이더의 그룹은 HLSL 에서 [numthreads(256, 1, 1)]의 개수를 말한다.
	// 즉 [numthreads(256, 1, 1)] * 그룹(x * y * z) 만큼의 스레드가 돌아감
	void									Update_Shader(_float3 iGroupCount);

	void									SetInputData(_uint iIndex, void* pData);
	void									SetConstantData(_uint iIndex, void* pData);

	_uint									GetNumData() { return m_iNumData; }
	void									GetBufferResource(BUFFER_TYPE eBufferType, _uint iBufferIndex, ID3D11Buffer* pOutBuffer);

public:
	HRESULT									ADD_Buffer(BUFFER_TYPE eBufferType, ID3D11Buffer* pBuffer, _uint iNumData = 0);

	// Append 속성이 부여된 BUFFER만 가능함
	HRESULT									ADD_AppendOutBuffer(ID3D11Buffer* pBuffer);

	HRESULT									Update_BufferResource(BUFFER_TYPE eBufferType, _uint iBufferIndex, void* pData);
	HRESULT									Update_BufferResource(BUFFER_TYPE eBufferType, _uint iBufferIndex, ID3D11Buffer* pBuffer);

	// 동시에 사용가능한 리소스 버퍼 8개 입니다.
	HRESULT									Bind_ConstBuffer(_uint iCBNum, _uint* iCBIndex);
	HRESULT									Bind_ConstBuffer_Slot(_uint iSlot, ID3D11Buffer* pCB);
	// 동시에 사용가능한 리소스 버퍼 8개 입니다.
	HRESULT									Bind_InputBuffer(_uint iBufferNum, _uint* iInputIndex);

	// 동시에 사용가능한 리소스 버퍼 8개 입니다.
	HRESULT									Bind_OutputBuffer(_uint iBufferNum, _uint* iOutputIndex);

	HRESULT									Bind_UAV(ID3D11UnorderedAccessView** pUAV);
	HRESULT									Unbind_UAV();

	HRESULT									Bind_SRV(ID3D11ShaderResourceView** pSRV);
	HRESULT									Unbind_SRV();

	HRESULT									Bind_Sampler(UINT iSlot, ID3D11SamplerState* pSamp);

	// 이펙트 툴에서 다시 재생하기위한 리셋
	HRESULT									Reset();
private:
	ID3D11ComputeShader*					m_pComputeShaderCom = nullptr;
	vector<ID3D11Buffer*>					m_pCBuffer = {};

	_uint									m_iNumData = {};

	// 입력에 관한 버퍼
	vector<ID3D11Buffer*>					m_pInputBuffer = {};
	vector<ID3D11ShaderResourceView*>		m_pSRVs = {};

	// 출력에 관한 버퍼
	vector<ID3D11Buffer*>					m_pOutputBuffer = {};
	vector<ID3D11UnorderedAccessView*>		m_pUAVs = {};

private:
	HRESULT									LoadShader(const WCHAR* szShaderFilePath, const char* szStartFunName);


public:
	// szShaderFilePath : File 경로
	// StartFunction : HLSL 진입 함수
	static	CComputeShader*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* szShaderFilePath, const char* szStartFunctionName, _uint iNumData);
	virtual CComponent*						Clone(void* pArg) override;
	virtual void							Free() override;

};
NS_END