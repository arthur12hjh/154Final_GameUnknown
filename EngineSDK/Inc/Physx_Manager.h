#pragma once

#include "Base.h"

/*
뉴네오피직스매니저
*/

NS_BEGIN(Engine)

class CPhysx_Manager : public CBase
{
private:
	CPhysx_Manager();
	virtual ~CPhysx_Manager() = default;

public: 
	HRESULT Initialize();
	void	Update(_float fTimeDelta);
public:
	void TestSetting();
	void Clear();

public:
	void Add_GameObject_ToPhysx(class CGameObject* pGameObject);

	//void Add_Geometry_ToPhysx(class CGameObject* pGameObject, class CModel* pModel);

	//void  Calc_Geometry();
	//_bool Check_GeometryCollision();
	//_bool Check_GameObject_GeometryCollision(class CGameObject* pGameObject, _bool* IsCollision = nullptr);
	//_bool Check_GeometryPicking();
	//_bool Check_Ray_GeometryPicking(_float3 vRayPos, _float3 vRayDir, _float3* vResultPos, _float* fResultDist);
private:
	/* 모든 Physx 모듈을 사용하려면 필요한 인스턴스. 다렉의 Device나 현재 프레임워크의 GameInstance 같은 녀석.*/
	PxPvdTransport* m_PxTransport = { nullptr };
	PxFoundation* m_PxFoundation = { nullptr };

	/* Foundation을 생성하기 위해 필요한 녀석 */
	PxDefaultAllocator m_DefaultAllocator = {};
	PxDefaultErrorCallback m_DefaultErrorCallback = {};

	/* Scene 내 모든 녀석들에게 영향을 미치는 녀석이라고 한다. (중력 같은거?)*/
	PxPhysics* m_PxPhysx = { nullptr };

	/* 말그대로 씬의 개념. 액터들을 모아놓고 액터끼리의 상호작용을 수행한다. */
	PxScene* m_PxScene = { nullptr };

	/* Physx Visual Debugger를 사용하기 위해 필요한 녀석 */
	PxPvd* m_Pvd = { nullptr };
	/* CPU 멀티 스레딩을 사용하기 위한 녀석 */
	PxDefaultCpuDispatcher* m_PxDispatcher = { nullptr };
	PxMaterial* m_pTestMaterial = { nullptr };
	
	/* 물리 영향을 받는 Dynamic 한 녀석들, 반대로 static은 벽이나 건물 등 물리적인 영향을 받지 않을때 사용한다고 한다. */
	vector<pair<class CGameObject*, PxRigidDynamic*>> m_DynamicActors = {};
	vector<_bool> m_IsOnSlope = {};
	/*
	구워놓은 충돌용 메시들 보관용. 추후 순회해서 
	객체들과의 충돌 감지를 수행해야 한다.

	이건 나루토할때 필요해서 뽑아놨던거니까 초기화에는 필요없어.
	*/
	vector<PxTriangleMeshGeometry*> m_Geometries = {};
	vector<PxTriangleMesh*> m_TriangleMeshes = {};
public:
	static CPhysx_Manager* Create();
	virtual void Free() override;
};

NS_END