#pragma once

#include "Model.h"

#pragma region Shader
#include "ComputeShader.h"
#include "Shader.h"
#pragma endregion

#include "Texture.h"
#include "Transform.h"
#include "Navigation.h"

#pragma region VI_BUFFER
#include "VIBuffer_Cube.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Rect_Instance.h"
#include "VIBuffer_Point_Instance.h"

#pragma endregion

#pragma region Collider
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "OBBCollider.h"
#pragma endregion

#pragma region Physx
#include "RigidBody.h"
#include "CharacterController.h"
#pragma endregion