#ifndef Engine_Math_h__
#define Engine_Math_h__

#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

namespace Engine
{
	
	template <typename T>
	T Clamp(const T& value, const T& min, const T& max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	template <typename T>
	T Lerp(const T& value1, const T& value2, const T& Time)
	{
		return value1 + ((value2 - value1) * Time);
	}

	static _vector SLerp(const _vector& value1, const _vector& value2, float Time)
	{
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMMATRIX m1 = XMMatrixLookToLH(XMVectorZero(), value1, vUp);
		XMMATRIX m2 = XMMatrixLookToLH(XMVectorZero(), value2, vUp);

		_vector q1 = XMQuaternionRotationMatrix(m1);
		_vector q2 = XMQuaternionRotationMatrix(m2);

		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMQuaternionSlerp(q1, q2, Time)));
	}

	// 베지어 구현해서 넣어두기
	// 시작점 0 -> n-1 까지 보간하고
	// 보간된 점들을 가지고 다시 선형보간해서
	// 정점이 1개가 나올때까지 보간한다.
	static XMVECTOR BezierCurve(unsigned int iNumPoint, const XMFLOAT3* pPoints, const float& Time)
	{
		if (0 >= iNumPoint || nullptr == pPoints)
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);

		vector<XMVECTOR> PointList;
		PointList.reserve(iNumPoint);

		for (unsigned int i = 0; i < iNumPoint; ++i)
			PointList.push_back(XMLoadFloat3(&pPoints[i]));

		for (unsigned int i = iNumPoint - 1; i > 0; --i)
		{
			for (unsigned int j = 0; j < i; ++j)
				PointList[j] = XMVectorLerp(PointList[j], PointList[j + 1], Time);
		}

		PointList[0].m128_f32[3] = 1.f;
		return PointList[0];
	}

	// 2D 공간 상에서 입니다.
	// X, Z 만 사용됨
	// 이값은 라디안으로 반환됩니다.
	static float	GetAngleVecToVecRadian2D(_vector vSrc, _vector vDest)
	{
		return XMVectorGetX(XMVector3Dot(vSrc, vDest));
	}

	// 2D 공간 상에서 입니다.
	// X, Z 만 사용됨
	// 이값은 각도로 반환됩니다.
	static float	GetAngleVecToVecDegree2D(_vector vSrc, _vector vDest)
	{
		return XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vSrc, vDest))));
	}
}
#endif // Engine_Typedef_h__