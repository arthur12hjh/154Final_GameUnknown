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

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL_TYPE::NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;

 	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	// BINMODEL->iNumMaterials
	pModel->iNumMaterials = m_pAIScene->mNumMaterials;
	// BINMODEL->iNumMeshes
	pModel->iNumMeshes = m_pAIScene->mNumMeshes;
	// BINMODEL->iNumAnimations
	pModel->iNumAnimations = m_pAIScene->mNumAnimations;

	if (true)
	{
		binNode RootNode;
		// BINMODEL->BINNODE->iNumChildren
		RootNode.iNumChildren = m_pAIScene->mRootNode->mNumChildren;
		// BINMODEL->BINNODE->matTransformation
		RootNode.matTransformation = XMFLOAT4X4(m_pAIScene->mRootNode->mTransformation.a1, m_pAIScene->mRootNode->mTransformation.a2, m_pAIScene->mRootNode->mTransformation.a3, m_pAIScene->mRootNode->mTransformation.a4,
			m_pAIScene->mRootNode->mTransformation.b1, m_pAIScene->mRootNode->mTransformation.b2, m_pAIScene->mRootNode->mTransformation.b3, m_pAIScene->mRootNode->mTransformation.b4,
			m_pAIScene->mRootNode->mTransformation.c1, m_pAIScene->mRootNode->mTransformation.c2, m_pAIScene->mRootNode->mTransformation.c3, m_pAIScene->mRootNode->mTransformation.c4,
			m_pAIScene->mRootNode->mTransformation.d1, m_pAIScene->mRootNode->mTransformation.d2, m_pAIScene->mRootNode->mTransformation.d3, m_pAIScene->mRootNode->mTransformation.d4);
		// BINMODEL->BINNODE->szName
		strcpy_s(RootNode.szName, m_pAIScene->mRootNode->mName.data);

		pModel->iRootNodeIndex = 0;
		pModel->vNodes.push_back(RootNode);

		// BINMODEL->BINNODE->vChildren
		for (size_t i = 0; i < RootNode.iNumChildren; ++i)
		{
			pModel->vNodes[pModel->iRootNodeIndex].vChildrenIndex.push_back(Get_BinNodeIndex(m_pAIScene->mRootNode->mChildren[i], pModel));
		}


		for (size_t i = 0; i < pModel->iNumMeshes; ++i)
		{
			binMesh meshTmp;
			meshTmp.iNumVertices = m_pAIScene->mMeshes[i]->mNumVertices;
			meshTmp.iNumFaces = m_pAIScene->mMeshes[i]->mNumFaces;
			meshTmp.iNumBones = m_pAIScene->mMeshes[i]->mNumBones;
			meshTmp.iMaterialIndex = m_pAIScene->mMeshes[i]->mMaterialIndex;
			meshTmp.vTextureCoords.resize(1);
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				meshTmp.vPositions.push_back(_float3(m_pAIScene->mMeshes[i]->mVertices[j].x, m_pAIScene->mMeshes[i]->mVertices[j].y, m_pAIScene->mMeshes[i]->mVertices[j].z));
				meshTmp.vNormals.push_back(_float3(m_pAIScene->mMeshes[i]->mNormals[j].x, m_pAIScene->mMeshes[i]->mNormals[j].y, m_pAIScene->mMeshes[i]->mNormals[j].z));
				meshTmp.vTangents.push_back(_float3(m_pAIScene->mMeshes[i]->mTangents[j].x, m_pAIScene->mMeshes[i]->mTangents[j].y, m_pAIScene->mMeshes[i]->mTangents[j].z));
				meshTmp.vBinormals.push_back(_float3(m_pAIScene->mMeshes[i]->mBitangents[j].x, m_pAIScene->mMeshes[i]->mBitangents[j].y, m_pAIScene->mMeshes[i]->mBitangents[j].z));
				meshTmp.vTextureCoords[0].push_back(_float2(m_pAIScene->mMeshes[i]->mTextureCoords[0][j].x, m_pAIScene->mMeshes[i]->mTextureCoords[0][j].y));
			}

			//vBones

			for (size_t x = 0; x < m_pAIScene->mMeshes[i]->mNumBones; ++x)
			{
				binBone binBoneTmp;
				strcpy_s(binBoneTmp.szName, m_pAIScene->mMeshes[i]->mBones[x]->mName.data);

				binBoneTmp.iNumWeights = m_pAIScene->mMeshes[i]->mBones[x]->mNumWeights;

				for (size_t y = 0; y < binBoneTmp.iNumWeights; ++y)
				{
					binVertexWeight weightTmp;
					weightTmp.iVertexId = m_pAIScene->mMeshes[i]->mBones[x]->mWeights[y].mVertexId;
					weightTmp.fWeight = m_pAIScene->mMeshes[i]->mBones[x]->mWeights[y].mWeight;

					binBoneTmp.vWeights.push_back(weightTmp);
				}


				XMStoreFloat4x4(&binBoneTmp.OffsetMatrix, XMMatrixIdentity());
				memcpy(&binBoneTmp.OffsetMatrix, &m_pAIScene->mMeshes[i]->mBones[x]->mOffsetMatrix, sizeof(_float4x4));

				//pBone->pNode = m_Nodes[x];
				meshTmp.vBones.push_back(binBoneTmp);

			}

			_uint iNumIndices = {};

			for (size_t j = 0; j < m_pAIScene->mMeshes[i]->mNumFaces; ++j)
			{
				binFace bFace;
				bFace.vIndices.push_back(m_pAIScene->mMeshes[i]->mFaces[j].mIndices[0]);
				++iNumIndices;
				bFace.vIndices.push_back(m_pAIScene->mMeshes[i]->mFaces[j].mIndices[1]);
				++iNumIndices;
				bFace.vIndices.push_back(m_pAIScene->mMeshes[i]->mFaces[j].mIndices[2]);
				++iNumIndices;
				bFace.iNumIndices = iNumIndices;
				meshTmp.vFaces.push_back(bFace);
			}

			strcpy_s(meshTmp.szName, m_pAIScene->mMeshes[i]->mName.data);

			pModel->vMeshes.push_back(meshTmp);
		}

		for (size_t i = 0; i < pModel->iNumMaterials; ++i)
		{
			binMaterial matTmp;
			for (size_t j = 0; j < BINMATERIAL::TEXTURETYPE::END; ++j)
			{
				matTmp.vNumSRVs.push_back(m_pAIScene->mMaterials[i]->GetTextureCount(static_cast<aiTextureType>(j)));
				matTmp.strTexturePaths[j].clear();
				matTmp.strTexturePaths[j].reserve(matTmp.vNumSRVs[j]);
				for (size_t k = 0; k < matTmp.vNumSRVs[j]; ++k)
				{
					aiString strTexturePath;
					m_pAIScene->mMaterials[i]->GetTexture(static_cast<aiTextureType>(j), k, &strTexturePath);

					matTmp.strTexturePaths[j].emplace_back(strTexturePath.data);
				}

			}

			strcpy_s(matTmp.szName, m_pAIScene->mMaterials[i]->GetName().C_Str());
			pModel->vMaterials.push_back(matTmp);
		}

		for (size_t i = 0; i < pModel->iNumAnimations; ++i)
		{
			binAnimation AnimTmp;
			const char* szAnimName = strchr(m_pAIScene->mAnimations[i]->mName.data, '|');
			if (nullptr != szAnimName && strcmp(szAnimName, ""))
				strcpy_s(AnimTmp.szName, MAX_PATH, strchr(m_pAIScene->mAnimations[i]->mName.data, '|') + 1);
			else
				strcpy_s(AnimTmp.szName, m_pAIScene->mAnimations[i]->mName.data);

			AnimTmp.fDuration = m_pAIScene->mAnimations[i]->mDuration;
			AnimTmp.fTicksPerSecond = m_pAIScene->mAnimations[i]->mTicksPerSecond;
			AnimTmp.iNumChannels = m_pAIScene->mAnimations[i]->mNumChannels;

			for (size_t j = 0; j < AnimTmp.iNumChannels; ++j)
			{
				binChannel channelTmp;
				strcpy_s(channelTmp.szName, m_pAIScene->mAnimations[i]->mChannels[j]->mNodeName.data);
				channelTmp.iNumScalingKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys;
				channelTmp.iNumRotationKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys;
				channelTmp.iNumPositionKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys;
				for (size_t k = 0; k < channelTmp.iNumScalingKeys; ++k)
				{
					binVectorKey keyTmp;
					keyTmp.fTime = m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime;
					keyTmp.vValue.x = m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.x;
					keyTmp.vValue.y = m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.y;
					keyTmp.vValue.z = m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.z;
					keyTmp.vValue.w = 0.f;

					channelTmp.cScalingKeys.push_back(keyTmp);
				}
				for (size_t k = 0; k < channelTmp.iNumRotationKeys; ++k)
				{
					binVectorKey keyTmp;
					keyTmp.fTime = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime;
					keyTmp.vValue.x = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.x;
					keyTmp.vValue.y = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.y;
					keyTmp.vValue.z = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.z;
					keyTmp.vValue.w = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.w;

					channelTmp.cRotationKeys.push_back(keyTmp);
				}
				for (size_t k = 0; k < channelTmp.iNumPositionKeys; ++k)
				{
					binVectorKey keyTmp;
					keyTmp.fTime = m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime;
					keyTmp.vValue.x = m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.x;
					keyTmp.vValue.y = m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.y;
					keyTmp.vValue.z = m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.z;
					keyTmp.vValue.w = 0.f;

					channelTmp.cPositionKeys.push_back(keyTmp);
				}

				AnimTmp.vChannels.push_back(channelTmp);
			}

			pModel->vAnimations.push_back(AnimTmp);
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

	m_Importer.FreeScene();
}
