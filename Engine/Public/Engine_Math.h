#pragma once

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

	// 베지어 구현해서 넣어두기

	// 스플라인 곡선

}