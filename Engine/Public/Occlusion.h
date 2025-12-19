#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CQuery;

class COcclusion : public CBase
{
public:
	struct OCCLUSION_QUERY_DATA
	{
		map<class CGameObject*, ID3D11Query*> m_mapQueries;
	};

private:
	COcclusion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COcclusion() = default;

public:
	HRESULT							Initialize();

	//
	HRESULT Begin_Object_Query(class CGameObject* pObejct);
	HRESULT End_Obejct_Query(class CGameObject* pObject);
	HRESULT Get_Result(class CGameObject* pObject, _bool* pIsVisible);


	void							SwapFrame();

private :
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	_bool							m_bIsDataReady = false;
	//CQuery*							m_pOCclusionQuery = { nullptr };
	ID3D11Query*					m_pQuery = { nullptr };

	ID3D11Query*					m_pQueries[2] = { nullptr, nullptr };
	_uint							m_iFrameIndex = 0;

	OCCLUSION_QUERY_DATA			m_QueryData[2];

	ID3D11Query*					Create_Query(class CGameObject* pObject);
	map<class CGameObject*, ID3D11Query*> m_mapPermanentQueries;

public:
	static		COcclusion*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		void				Free() override;

};
NS_END