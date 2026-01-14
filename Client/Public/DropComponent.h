#pragma once
#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Client)
class CDropComponent final : public CComponent
{
public :
	typedef struct DropComponentDesc
	{
		_float				fDropRange = { };
		_float				fForce = { 25.f	};
		//아이템 드롭 영역을 부모 객체의 look 방향으로부터 90도이상 차이나지 않게
		//잡아주는 bool 변수
		_bool				isDropRangeHemiSphere = { false };
	}DROP_COMPONENT_DESC;

	typedef struct Drop_Result_Desc
	{
		_uint			iDropItemID = {};
		_int			iAmountVal = {};
	}DROP_RESULT_DESC;

private:
	CDropComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDropComponent() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	// 떨어져야하는 아이템의 숫자를 관리
	// 1개는 디폴트로 나옵니다.
	// 떨어지는 아이템의 개수만큼 아이템이 연산되어서 나온다.
	void							ItemDrop(_uint iDropItemCount = 1);
	void							DropRewardItem();

	HRESULT							ADD_DropItem(const pair<_uint, _float>& ItemData, _float fMinAmount, _float fMaxAmount);

private:
	static	const WCHAR*			m_szProtoTypeName; 
	static	const WCHAR*			m_szLayerName;

	// 모든 아이템에대한 토탈 가중치
	_float							m_fTotalWeight = {};
	_float							m_fDropRange = {};
	_float							m_fDropForce = {};
	_uint							m_iNumItemCount = {};

	// 아이템 및 아이템 확률
	vector<_float2>					m_AmountItemList = {};
	vector<pair<_uint, _float>>		m_DoprItemList = {};

	// 나중에 UI로 표현할거면 빼서 이거 구조체 받아서 처리하면 됩니다.
	list<DROP_RESULT_DESC>			m_DropResultList = {};
	_bool							m_isDropRangeHemiSphere = { false };
private :
	// 아이템 카운트 및 골드 습득량 등
	void							CalculationItemDrop();
	void							CreateObjectToLayer();

public:
	static		CDropComponent*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*			Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END