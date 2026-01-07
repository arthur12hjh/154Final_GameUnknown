#include "FbxParser.h"

CFbxParser::CFbxParser()
{
}

HRESULT CFbxParser::Initialize()
{
	return S_OK;
}

HRESULT CFbxParser::ReadFbx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut)
{
	//unsigned int				iNumMeshes;
	//unsigned int				iNumMaterials;
	//binNode* pRootNode; /**/
	//vector<binMesh>				vMeshes;
	//vector<binMaterial>			vMaterials;
	binModel* pModel = new binModel;
	_uint iFlag = {};

	Assimp::Importer ImporterBuffer;

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL_TYPE::NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;

 	auto pAIScene = ImporterBuffer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == pAIScene)
		return E_FAIL;
	
	// BINMODEL->iNumMaterials
	pModel->iNumMaterials = pAIScene->mNumMaterials;
	// BINMODEL->iNumMeshes
	pModel->iNumMeshes = pAIScene->mNumMeshes;
	// BINMODEL->iNumAnimations
	pModel->iNumAnimations = pAIScene->mNumAnimations;

	if (true)
	{
		binNode RootNode;
		// BINMODEL->BINNODE->iNumChildren
		RootNode.iNumChildren = pAIScene->mRootNode->mNumChildren;
		// BINMODEL->BINNODE->matTransformation
		RootNode.matTransformation = XMFLOAT4X4(pAIScene->mRootNode->mTransformation.a1, pAIScene->mRootNode->mTransformation.a2, pAIScene->mRootNode->mTransformation.a3, pAIScene->mRootNode->mTransformation.a4,
			pAIScene->mRootNode->mTransformation.b1, pAIScene->mRootNode->mTransformation.b2, pAIScene->mRootNode->mTransformation.b3, pAIScene->mRootNode->mTransformation.b4,
			pAIScene->mRootNode->mTransformation.c1, pAIScene->mRootNode->mTransformation.c2, pAIScene->mRootNode->mTransformation.c3, pAIScene->mRootNode->mTransformation.c4,
			pAIScene->mRootNode->mTransformation.d1, pAIScene->mRootNode->mTransformation.d2, pAIScene->mRootNode->mTransformation.d3, pAIScene->mRootNode->mTransformation.d4);
		// BINMODEL->BINNODE->szName
		strcpy_s(RootNode.szName, pAIScene->mRootNode->mName.data);

		pModel->iRootNodeIndex = 0;
		pModel->vNodes.push_back(RootNode);

		// BINMODEL->BINNODE->vChildrendg
		for (size_t i = 0; i < RootNode.iNumChildren; ++i)
		{
			pModel->vNodes[pModel->iRootNodeIndex].vChildrenIndex.push_back(Get_BinNodeIndex(pAIScene->mRootNode->mChildren[i], pModel));
		}


		for (size_t i = 0; i < pModel->iNumMeshes; ++i)
		{
			binMesh meshTmp;
			meshTmp.iNumVertices = pAIScene->mMeshes[i]->mNumVertices;
			meshTmp.iNumFaces = pAIScene->mMeshes[i]->mNumFaces;
			meshTmp.iNumBones = pAIScene->mMeshes[i]->mNumBones;
			meshTmp.iMaterialIndex = pAIScene->mMeshes[i]->mMaterialIndex;
			meshTmp.vTextureCoords.resize(1);
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				meshTmp.vPositions.push_back(_float3(pAIScene->mMeshes[i]->mVertices[j].x, pAIScene->mMeshes[i]->mVertices[j].y, pAIScene->mMeshes[i]->mVertices[j].z));
				meshTmp.vNormals.push_back(_float3(pAIScene->mMeshes[i]->mNormals[j].x, pAIScene->mMeshes[i]->mNormals[j].y, pAIScene->mMeshes[i]->mNormals[j].z));
				meshTmp.vTangents.push_back(_float3(pAIScene->mMeshes[i]->mTangents[j].x, pAIScene->mMeshes[i]->mTangents[j].y, pAIScene->mMeshes[i]->mTangents[j].z));
				meshTmp.vBinormals.push_back(_float3(pAIScene->mMeshes[i]->mBitangents[j].x, pAIScene->mMeshes[i]->mBitangents[j].y, pAIScene->mMeshes[i]->mBitangents[j].z));
				meshTmp.vTextureCoords[0].push_back(_float2(pAIScene->mMeshes[i]->mTextureCoords[0][j].x, pAIScene->mMeshes[i]->mTextureCoords[0][j].y));
			}

			//vBones

			for (size_t x = 0; x < pAIScene->mMeshes[i]->mNumBones; ++x)
			{
				binBone binBoneTmp;
				strcpy_s(binBoneTmp.szName, pAIScene->mMeshes[i]->mBones[x]->mName.data);

				binBoneTmp.iNumWeights = pAIScene->mMeshes[i]->mBones[x]->mNumWeights;

				for (size_t y = 0; y < binBoneTmp.iNumWeights; ++y)
				{
					binVertexWeight weightTmp;
					weightTmp.iVertexId = pAIScene->mMeshes[i]->mBones[x]->mWeights[y].mVertexId;
					weightTmp.fWeight = pAIScene->mMeshes[i]->mBones[x]->mWeights[y].mWeight;

					binBoneTmp.vWeights.push_back(weightTmp);
				}


				XMStoreFloat4x4(&binBoneTmp.OffsetMatrix, XMMatrixIdentity());
				memcpy(&binBoneTmp.OffsetMatrix, &pAIScene->mMeshes[i]->mBones[x]->mOffsetMatrix, sizeof(_float4x4));

				//pBone->pNode = m_Nodes[x];
				meshTmp.vBones.push_back(binBoneTmp);

			}

			_uint iNumIndices = {};

			for (size_t j = 0; j < pAIScene->mMeshes[i]->mNumFaces; ++j)
			{
				binFace bFace;
				bFace.vIndices.push_back(pAIScene->mMeshes[i]->mFaces[j].mIndices[0]);
				++iNumIndices;
				bFace.vIndices.push_back(pAIScene->mMeshes[i]->mFaces[j].mIndices[1]);
				++iNumIndices;
				bFace.vIndices.push_back(pAIScene->mMeshes[i]->mFaces[j].mIndices[2]);
				++iNumIndices;
				bFace.iNumIndices = iNumIndices;
				meshTmp.vFaces.push_back(bFace);
			}

			// 페이셜을 위한 Mesh :: AnimMesh
			if (eType == MODEL_TYPE::FACIAL)
			{
				meshTmp.iNumAnimMeshes = pAIScene->mMeshes[i]->mNumAnimMeshes;
				for (size_t j = 0; j < pAIScene->mMeshes[i]->mNumAnimMeshes; ++j)
				{
					binAnimMesh bAnimMesh;
					bAnimMesh.iNumVertices = pAIScene->mMeshes[i]->mAnimMeshes[j]->mNumVertices;

					bAnimMesh.vDeltaPositions.reserve(bAnimMesh.iNumVertices);
					bAnimMesh.vDeltaNormals.reserve(bAnimMesh.iNumVertices);

					for (size_t k = 0; k < pAIScene->mMeshes[i]->mAnimMeshes[j]->mNumVertices; ++k)
					{
						aiVector3D vBasePosition = pAIScene->mMeshes[i]->mVertices[k];
						aiVector3D vBaseNormal = pAIScene->mMeshes[i]->mNormals[k];
						
						aiVector3D vShapePosition = pAIScene->mMeshes[i]->mAnimMeshes[j]->mVertices[k];
						aiVector3D vShapeNormal = pAIScene->mMeshes[i]->mAnimMeshes[j]->mNormals[k];

						bAnimMesh.vDeltaPositions.push_back(_float3(vShapePosition.x - vBasePosition.x, vShapePosition.y - vBasePosition.y, vShapePosition.z - vBasePosition.z));
						bAnimMesh.vDeltaNormals.push_back(_float3(vShapeNormal.x - vBaseNormal.x, vShapeNormal.y - vBaseNormal.y, vShapeNormal.z - vBaseNormal.z));
					}

					strcpy_s(bAnimMesh.szName, pAIScene->mMeshes[i]->mAnimMeshes[j]->mName.data);

					meshTmp.vAnimMesh.push_back(bAnimMesh);
				}
			}


			strcpy_s(meshTmp.szName, pAIScene->mMeshes[i]->mName.data);

			pModel->vMeshes.push_back(meshTmp);
		}

		for (size_t i = 0; i < pModel->iNumMaterials; ++i)
		{
			binMaterial matTmp;
			for (size_t j = 0; j < BINMATERIAL::TEXTURETYPE::END; ++j)
			{
				matTmp.vNumSRVs.push_back(pAIScene->mMaterials[i]->GetTextureCount(static_cast<aiTextureType>(j)));
				matTmp.strTexturePaths[j].clear();
				matTmp.strTexturePaths[j].reserve(matTmp.vNumSRVs[j]);
				for (size_t k = 0; k < matTmp.vNumSRVs[j]; ++k)
				{
					aiString strTexturePath;
					pAIScene->mMaterials[i]->GetTexture(static_cast<aiTextureType>(j), k, &strTexturePath);

					matTmp.strTexturePaths[j].emplace_back(strTexturePath.data);
				}

			}

			strcpy_s(matTmp.szName, pAIScene->mMaterials[i]->GetName().C_Str());
			pModel->vMaterials.push_back(matTmp);
		}

		{
			for (size_t i = 0; i < pModel->iNumAnimations; ++i)
			{
				binAnimation AnimTmp;
				const char* szAnimName = strchr(pAIScene->mAnimations[i]->mName.data, '|');
				if (nullptr != szAnimName && strcmp(szAnimName, ""))
					strcpy_s(AnimTmp.szName, MAX_PATH, strchr(pAIScene->mAnimations[i]->mName.data, '|') + 1);
				else
					strcpy_s(AnimTmp.szName, pAIScene->mAnimations[i]->mName.data);

				AnimTmp.fDuration = pAIScene->mAnimations[i]->mDuration;
				AnimTmp.fTicksPerSecond = pAIScene->mAnimations[i]->mTicksPerSecond;
				AnimTmp.iNumChannels = pAIScene->mAnimations[i]->mNumChannels;

				for (size_t j = 0; j < AnimTmp.iNumChannels; ++j)
				{
					binChannel channelTmp;
					strcpy_s(channelTmp.szName, pAIScene->mAnimations[i]->mChannels[j]->mNodeName.data);
					channelTmp.iNumScalingKeys = pAIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys;
					channelTmp.iNumRotationKeys = pAIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys;
					channelTmp.iNumPositionKeys = pAIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys;
					for (size_t k = 0; k < channelTmp.iNumScalingKeys; ++k)
					{
						binVectorKey keyTmp;
						keyTmp.fTime = pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime;
						keyTmp.vValue.x = pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.x;
						keyTmp.vValue.y = pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.y;
						keyTmp.vValue.z = pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.z;
						keyTmp.vValue.w = 0.f;

						channelTmp.cScalingKeys.push_back(keyTmp);
					}
					for (size_t k = 0; k < channelTmp.iNumRotationKeys; ++k)
					{
						binVectorKey keyTmp;
						keyTmp.fTime = pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime;
						keyTmp.vValue.x = pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.x;
						keyTmp.vValue.y = pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.y;
						keyTmp.vValue.z = pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.z;
						keyTmp.vValue.w = pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.w;

						channelTmp.cRotationKeys.push_back(keyTmp);
					}
					for (size_t k = 0; k < channelTmp.iNumPositionKeys; ++k)
					{
						binVectorKey keyTmp;
						keyTmp.fTime = pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime;
						keyTmp.vValue.x = pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.x;
						keyTmp.vValue.y = pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.y;
						keyTmp.vValue.z = pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.z;
						keyTmp.vValue.w = 0.f;

						channelTmp.cPositionKeys.push_back(keyTmp);
					}

					AnimTmp.vChannels.push_back(channelTmp);
				}

				pModel->vAnimations.push_back(AnimTmp);
			}
		}
		

		if (eType == MODEL_TYPE::FACIAL)
		{
			// 페이셜을 위한 Model :: MorphAnimation
			for (size_t i = 0; i < pModel->iNumAnimations; ++i)
			{
				binMorphAnimation MorphAnimTmp;

				const char* szAnimName = strchr(pAIScene->mAnimations[i]->mName.data, '|');
				if (nullptr != szAnimName && strcmp(szAnimName, ""))
					strcpy_s(MorphAnimTmp.szName, MAX_PATH, strchr(pAIScene->mAnimations[i]->mName.data, '|') + 1);
				else
					strcpy_s(MorphAnimTmp.szName, pAIScene->mAnimations[i]->mName.data);

				MorphAnimTmp.fDuration = pAIScene->mAnimations[i]->mDuration;
				MorphAnimTmp.fTicksPerSecond = pAIScene->mAnimations[i]->mTicksPerSecond;
				MorphAnimTmp.iNumMorphChannels = pAIScene->mAnimations[i]->mNumMorphMeshChannels;

				for (size_t j = 0; j < MorphAnimTmp.iNumMorphChannels; ++j)
				{
					binMeshMorphChannel MorphChannelTmp;
					
					strcpy_s(MorphChannelTmp.szName, pAIScene->mAnimations[i]->mMorphMeshChannels[j]->mName.data);
					MorphChannelTmp.iNumKeys = pAIScene->mAnimations[i]->mMorphMeshChannels[j]->mNumKeys;
					for (size_t k = 0; k < MorphChannelTmp.iNumKeys; ++k)
					{
						binMeshMorphKey keyTmp;
						keyTmp.fTime = pAIScene->mAnimations[i]->mMorphMeshChannels[j]->mKeys[k].mTime;
						keyTmp.iNumValuesAndWeights = pAIScene->mAnimations[i]->mMorphMeshChannels[j]->mKeys[k].mNumValuesAndWeights;
						for (size_t l = 0; l < keyTmp.iNumValuesAndWeights; ++l)
						{
							keyTmp.vValues.push_back(pAIScene->mAnimations[i]->mMorphMeshChannels[j]->mKeys[k].mValues[l]);
							keyTmp.vWeights.push_back(pAIScene->mAnimations[i]->mMorphMeshChannels[j]->mKeys[k].mWeights[l]);
						}
						
						MorphChannelTmp.vKeys.push_back(keyTmp);
					}

					MorphAnimTmp.vMorphChannels.push_back(MorphChannelTmp);
				}

				pModel->vMorphAnimations.push_back(MorphAnimTmp);
			}
		}
		

	}

	*ppOut = pModel;
	return S_OK;
}

unsigned int CFbxParser::Get_BinNodeIndex(const aiNode* pAINode, binModel* pModel)
{
	unsigned int iNodeIndex;
	binNode Node;
	Node.iNumChildren = pAINode->mNumChildren;		// iNumChildren
	// matTransformation
	Node.matTransformation = XMFLOAT4X4(pAINode->mTransformation.a1, pAINode->mTransformation.a2, pAINode->mTransformation.a3, pAINode->mTransformation.a4,
		pAINode->mTransformation.b1, pAINode->mTransformation.b2, pAINode->mTransformation.b3, pAINode->mTransformation.b4,
		pAINode->mTransformation.c1, pAINode->mTransformation.c2, pAINode->mTransformation.c3, pAINode->mTransformation.c4,
		pAINode->mTransformation.d1, pAINode->mTransformation.d2, pAINode->mTransformation.d3, pAINode->mTransformation.d4);
	// szName
	strcpy_s(Node.szName, pAINode->mName.data);

	pModel->vNodes.push_back(Node);
	iNodeIndex = pModel->vNodes.size() - 1;

	// vChildren
	for (size_t i = 0; i < Node.iNumChildren; ++i)
	{
		pModel->vNodes[iNodeIndex].vChildrenIndex.push_back(Get_BinNodeIndex(pAINode->mChildren[i], pModel));
	}

	return iNodeIndex;
}

CFbxParser* CFbxParser::Create()
{
	CFbxParser* pInstance = new CFbxParser();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(" Failed to Create : CFbxParser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFbxParser::Free()
{
	__super::Free();

}
