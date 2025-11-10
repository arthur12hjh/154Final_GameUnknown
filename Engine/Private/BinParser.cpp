#include "BinParser.h"
#include "fstream"

CBinParser::CBinParser()
{
}


HRESULT CBinParser::Initialize()
{
	return S_OK;
}


HRESULT CBinParser::ReadBin(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut)
{
	ifstream fileBinaryStream;
	fileBinaryStream.open(pModelFilePath, ios_base::binary);

	binModel* pModel = new binModel;

	fileBinaryStream.read((char*)(&pModel->iNumMeshes), sizeof(pModel->iNumMeshes));
	fileBinaryStream.read((char*)(&pModel->iNumMaterials), sizeof(pModel->iNumMaterials));
	fileBinaryStream.read((char*)(&pModel->iNumAnimations), sizeof(pModel->iNumAnimations));

	char* szTemp;

	if (true)
	{
		binNode RootNode;
		// BINMODEL->BINNODE->iNumChildren
		fileBinaryStream.read((char*)(&RootNode.iNumChildren), sizeof(RootNode.iNumChildren));
		// BINMODEL->BINNODE->matTransformation
		fileBinaryStream.read((char*)(&RootNode.matTransformation), sizeof(XMFLOAT4X4));
		// BINMODEL->BINNODE->szName
		szTemp = ReadString(fileBinaryStream);
		strcpy_s(RootNode.szName, szTemp);
		delete[] szTemp;

		pModel->iRootNodeIndex = 0;
		pModel->vNodes.push_back(RootNode);

		// BINMODEL->BINNODE->vChildren
		for (size_t i = 0; i < RootNode.iNumChildren; ++i)
		{
			pModel->vNodes[pModel->iRootNodeIndex].vChildrenIndex.push_back(Read_BinNode(fileBinaryStream, pModel));
		}

		for (size_t i = 0; i < pModel->iNumMeshes; ++i)
		{
			binMesh meshTmp;
			fileBinaryStream.read((char*)(&meshTmp.iNumVertices), sizeof(meshTmp.iNumVertices));
			fileBinaryStream.read((char*)(&meshTmp.iNumFaces), sizeof(meshTmp.iNumFaces));
			fileBinaryStream.read((char*)(&meshTmp.iNumBones), sizeof(meshTmp.iNumBones));
			fileBinaryStream.read((char*)(&meshTmp.iMaterialIndex), sizeof(meshTmp.iMaterialIndex));
			meshTmp.vTextureCoords.resize(1);
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				meshTmp.vPositions.push_back(ReadFloat3(fileBinaryStream));
			}
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				meshTmp.vNormals.push_back(ReadFloat3(fileBinaryStream));
			}
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				meshTmp.vTangents.push_back(ReadFloat3(fileBinaryStream));
			}
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				meshTmp.vBinormals.push_back(ReadFloat3(fileBinaryStream));
			}
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				meshTmp.vTextureCoords[0].push_back(ReadFloat2(fileBinaryStream));
			}

			for (size_t x = 0; x < meshTmp.iNumBones; ++x)
			{
				//char					szName[MAX_PATH];	
				//unsigned int			iNumWeights;		
				//vector<unsigned int>	vWeights;			
				//XMFLOAT4X4			OffsetMatrix;		
				binBone binBoneTmp;
				szTemp = ReadString(fileBinaryStream);
				strcpy_s(binBoneTmp.szName, szTemp);
				delete[] szTemp;
				fileBinaryStream.read((char*)(&binBoneTmp.iNumWeights), sizeof(binBoneTmp.iNumWeights));

				for (size_t y = 0; y < binBoneTmp.iNumWeights; ++y)
				{
					binVertexWeight weightTmp;
					fileBinaryStream.read((char*)(&weightTmp.iVertexId), sizeof(weightTmp.iVertexId));
					fileBinaryStream.read((char*)(&weightTmp.fWeight), sizeof(weightTmp.fWeight));

					binBoneTmp.vWeights.push_back(weightTmp);
				}


				XMStoreFloat4x4(&binBoneTmp.OffsetMatrix, XMMatrixIdentity());

				fileBinaryStream.read((char*)(&binBoneTmp.OffsetMatrix), sizeof(XMFLOAT4X4));

				//pBone->pNode = m_Nodes[x];
				meshTmp.vBones.push_back(binBoneTmp);
			}

			for (size_t j = 0; j < meshTmp.iNumFaces; ++j)
			{
				binFace bFace;
				fileBinaryStream.read((char*)(&bFace.iNumIndices), sizeof(bFace.iNumIndices));
				unsigned int iTmp1;
				fileBinaryStream.read((char*)(&iTmp1), sizeof(iTmp1));
				bFace.vIndices.push_back(iTmp1);
				unsigned int iTmp2;
				fileBinaryStream.read((char*)(&iTmp2), sizeof(iTmp2));
				bFace.vIndices.push_back(iTmp2);
				unsigned int iTmp3;
				fileBinaryStream.read((char*)(&iTmp3), sizeof(iTmp3));
				bFace.vIndices.push_back(iTmp3);

				meshTmp.vFaces.push_back(bFace);
			}
			szTemp = ReadString(fileBinaryStream);
			strcpy_s(meshTmp.szName, szTemp);
			delete[] szTemp;

			pModel->vMeshes.push_back(meshTmp);
		}

		for (size_t j = 0; j < pModel->iNumMaterials; ++j)
		{
			binMaterial matTmp;
			for (size_t k = 0; k < BINMATERIAL::TEXTURETYPE::END; ++k)
			{
				unsigned int iNumSRVs;
				fileBinaryStream.read((char*)(&iNumSRVs), sizeof(iNumSRVs));
				matTmp.vNumSRVs.push_back(iNumSRVs);
				matTmp.strTexturePaths[k].clear();
				matTmp.strTexturePaths[k].reserve(iNumSRVs);
				for (size_t l = 0; l < matTmp.vNumSRVs[k]; ++l)
				{
					char* szTemp = ReadString(fileBinaryStream);
					matTmp.strTexturePaths[k].emplace_back(szTemp);
					delete[] szTemp;
				}

			}
			pModel->vMaterials.push_back(matTmp);
		}

		for (size_t i = 0; i < pModel->iNumAnimations; ++i)
		{
			binAnimation AnimTmp;

			szTemp = ReadString(fileBinaryStream);
			strcpy_s(AnimTmp.szName, szTemp);
			delete[] szTemp;
			fileBinaryStream.read((char*)(&AnimTmp.fDuration), sizeof(AnimTmp.fDuration));
			fileBinaryStream.read((char*)(&AnimTmp.fTicksPerSecond), sizeof(AnimTmp.fTicksPerSecond));
			fileBinaryStream.read((char*)(&AnimTmp.iNumChannels), sizeof(AnimTmp.iNumChannels));

			for (size_t j = 0; j < AnimTmp.iNumChannels; ++j)
			{
				binChannel channelTmp;
				szTemp = ReadString(fileBinaryStream);
				strcpy_s(channelTmp.szName, szTemp);
				delete[] szTemp;
				fileBinaryStream.read((char*)(&channelTmp.iNumScalingKeys), sizeof(channelTmp.iNumScalingKeys));
				fileBinaryStream.read((char*)(&channelTmp.iNumRotationKeys), sizeof(channelTmp.iNumRotationKeys));
				fileBinaryStream.read((char*)(&channelTmp.iNumPositionKeys), sizeof(channelTmp.iNumPositionKeys));

				for (size_t k = 0; k < channelTmp.iNumScalingKeys; ++k)
				{
					binVectorKey keyTmp;
					fileBinaryStream.read((char*)(&keyTmp.fTime), sizeof(keyTmp.fTime));
					fileBinaryStream.read((char*)(&keyTmp.vValue), sizeof(keyTmp.vValue));

					channelTmp.cScalingKeys.push_back(keyTmp);
				}
				for (size_t k = 0; k < channelTmp.iNumRotationKeys; ++k)
				{
					binVectorKey keyTmp;
					fileBinaryStream.read((char*)(&keyTmp.fTime), sizeof(keyTmp.fTime));
					fileBinaryStream.read((char*)(&keyTmp.vValue), sizeof(keyTmp.vValue));

					channelTmp.cRotationKeys.push_back(keyTmp);
				}
				for (size_t k = 0; k < channelTmp.iNumPositionKeys; ++k)
				{
					binVectorKey keyTmp;
					fileBinaryStream.read((char*)(&keyTmp.fTime), sizeof(keyTmp.fTime));
					fileBinaryStream.read((char*)(&keyTmp.vValue), sizeof(keyTmp.vValue));

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

HRESULT CBinParser::WriteBin(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut)
{
	ofstream fileBinaryStream;
	fileBinaryStream.open(pModelFilePath, ios_base::binary);

	binModel* pModel = *ppOut;


	fileBinaryStream.write((char*)(&pModel->iNumMeshes), sizeof(pModel->iNumMeshes));
	fileBinaryStream.write((char*)(&pModel->iNumMaterials), sizeof(pModel->iNumMaterials));
	fileBinaryStream.write((char*)(&pModel->iNumAnimations), sizeof(pModel->iNumAnimations));

	if (true)
	{
		// BINMODEL->BINNODE->iNumChildren
		fileBinaryStream.write((char*)(&pModel->vNodes[pModel->iRootNodeIndex].iNumChildren), sizeof(pModel->vNodes[pModel->iRootNodeIndex].iNumChildren));
		// BINMODEL->BINNODE->matTransformation
		fileBinaryStream.write((char*)(&pModel->vNodes[pModel->iRootNodeIndex].matTransformation), sizeof(XMFLOAT4X4));
		// BINMODEL->BINNODE->szName
		WriteString(fileBinaryStream, pModel->vNodes[pModel->iRootNodeIndex].szName);


		// BINMODEL->BINNODE->vChildren
		for (size_t i = 0; i < pModel->vNodes[pModel->iRootNodeIndex].iNumChildren; ++i)
		{
			Write_BinNode(fileBinaryStream, pModel, &pModel->vNodes[pModel->vNodes[pModel->iRootNodeIndex].vChildrenIndex[i]]);
		}

		for (size_t i = 0; i < pModel->iNumMeshes; ++i)
		{
			binMesh meshTmp = pModel->vMeshes[i];
			fileBinaryStream.write((char*)(&meshTmp.iNumVertices), sizeof(meshTmp.iNumVertices));
			fileBinaryStream.write((char*)(&meshTmp.iNumFaces), sizeof(meshTmp.iNumFaces));
			fileBinaryStream.write((char*)(&meshTmp.iNumBones), sizeof(meshTmp.iNumBones));
			fileBinaryStream.write((char*)(&meshTmp.iMaterialIndex), sizeof(meshTmp.iMaterialIndex));
			//meshTmp.vTextureCoords.resize(1);
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				WriteFloat3(fileBinaryStream, meshTmp.vPositions[j]);
			}
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				WriteFloat3(fileBinaryStream, meshTmp.vNormals[j]);
			}
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				WriteFloat3(fileBinaryStream, meshTmp.vTangents[j]);
			}
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				WriteFloat3(fileBinaryStream, meshTmp.vBinormals[j]);
			}
			for (_uint j = 0; j < meshTmp.iNumVertices; ++j)
			{
				WriteFloat2(fileBinaryStream, meshTmp.vTextureCoords[0][j]);
			}

			for (size_t x = 0; x < meshTmp.iNumBones; ++x)
			{
				binBone binBoneTmp = meshTmp.vBones[x];
				WriteString(fileBinaryStream, binBoneTmp.szName);
				fileBinaryStream.write((char*)(&binBoneTmp.iNumWeights), sizeof(binBoneTmp.iNumWeights));

				for (size_t y = 0; y < binBoneTmp.iNumWeights; ++y)
				{
					binVertexWeight weightTmp = binBoneTmp.vWeights[y];
					fileBinaryStream.write((char*)(&weightTmp.iVertexId), sizeof(weightTmp.iVertexId));
					fileBinaryStream.write((char*)(&weightTmp.fWeight), sizeof(weightTmp.fWeight));
				}

				fileBinaryStream.write((char*)(&binBoneTmp.OffsetMatrix), sizeof(XMFLOAT4X4));
			}


			for (size_t j = 0; j < meshTmp.iNumFaces; ++j)
			{
				fileBinaryStream.write(reinterpret_cast<char*>(&meshTmp.vFaces[j].iNumIndices), sizeof(meshTmp.vFaces[j].iNumIndices));
				fileBinaryStream.write(reinterpret_cast<char*>(&meshTmp.vFaces[j].vIndices[0]), sizeof(meshTmp.vFaces[j].vIndices[0]));
				fileBinaryStream.write(reinterpret_cast<char*>(&meshTmp.vFaces[j].vIndices[1]), sizeof(meshTmp.vFaces[j].vIndices[1]));
				fileBinaryStream.write(reinterpret_cast<char*>(&meshTmp.vFaces[j].vIndices[2]), sizeof(meshTmp.vFaces[j].vIndices[2]));
			}

			WriteString(fileBinaryStream, meshTmp.szName);
		}

		for (size_t j = 0; j < pModel->iNumMaterials; ++j)
		{
			binMaterial matTmp = pModel->vMaterials[j];
			for (size_t k = 0; k < BINMATERIAL::TEXTURETYPE::END; ++k)
			{
				fileBinaryStream.write(reinterpret_cast<char*>(&matTmp.vNumSRVs[k]), sizeof(matTmp.vNumSRVs[k]));
				for (size_t l = 0; l < matTmp.vNumSRVs[k]; ++l)
				{
					WriteString(fileBinaryStream, const_cast<char*>(matTmp.strTexturePaths[k][l].c_str()));
				}

			}
		}

		for (size_t i = 0; i < pModel->iNumAnimations; ++i)
		{
			binAnimation AnimTmp = pModel->vAnimations[i];

			WriteString(fileBinaryStream, AnimTmp.szName);
			fileBinaryStream.write((char*)(&AnimTmp.fDuration), sizeof(AnimTmp.fDuration));
			fileBinaryStream.write((char*)(&AnimTmp.fTicksPerSecond), sizeof(AnimTmp.fTicksPerSecond));
			fileBinaryStream.write((char*)(&AnimTmp.iNumChannels), sizeof(AnimTmp.iNumChannels));

			for (size_t j = 0; j < AnimTmp.iNumChannels; ++j)
			{
				binChannel channelTmp = AnimTmp.vChannels[j];
				WriteString(fileBinaryStream, channelTmp.szName);
				fileBinaryStream.write((char*)(&channelTmp.iNumScalingKeys), sizeof(channelTmp.iNumScalingKeys));
				fileBinaryStream.write((char*)(&channelTmp.iNumRotationKeys), sizeof(channelTmp.iNumRotationKeys));
				fileBinaryStream.write((char*)(&channelTmp.iNumPositionKeys), sizeof(channelTmp.iNumPositionKeys));

				for (size_t k = 0; k < channelTmp.iNumScalingKeys; ++k)
				{
					binVectorKey keyTmp = channelTmp.cScalingKeys[k];
					fileBinaryStream.write((char*)(&keyTmp.fTime), sizeof(keyTmp.fTime));
					fileBinaryStream.write((char*)(&keyTmp.vValue), sizeof(keyTmp.vValue));
				}
				for (size_t k = 0; k < channelTmp.iNumRotationKeys; ++k)
				{
					binVectorKey keyTmp = channelTmp.cRotationKeys[k];
					fileBinaryStream.write((char*)(&keyTmp.fTime), sizeof(keyTmp.fTime));
					fileBinaryStream.write((char*)(&keyTmp.vValue), sizeof(keyTmp.vValue));
				}
				for (size_t k = 0; k < channelTmp.iNumPositionKeys; ++k)
				{
					binVectorKey keyTmp = channelTmp.cPositionKeys[k];
					fileBinaryStream.write((char*)(&keyTmp.fTime), sizeof(keyTmp.fTime));
					fileBinaryStream.write((char*)(&keyTmp.vValue), sizeof(keyTmp.vValue));
				}

			}

		}


	}


	return S_OK;
}

char* CBinParser::ReadString(ifstream& fileBinaryStream)
{
	_uint iStringLength;
	fileBinaryStream.read((char*)&iStringLength, sizeof(iStringLength));

	char* str = new char[iStringLength + 1];
	fileBinaryStream.read(&str[0], iStringLength);

	str[iStringLength] = '\0';
	return str;
}

XMFLOAT3 CBinParser::ReadFloat3(ifstream& fileBinaryStream)
{
	XMFLOAT3 vTemp;
	fileBinaryStream.read((char*)&vTemp, sizeof(XMFLOAT3));
	return vTemp;
}

XMFLOAT2 CBinParser::ReadFloat2(ifstream& fileBinaryStream)
{
	XMFLOAT2 vTemp;
	fileBinaryStream.read((char*)&vTemp, sizeof(XMFLOAT2));
	return vTemp;
}

_uint CBinParser::ReadUint(ifstream& fileBinaryStream)
{
	return _uint();
}

unsigned int CBinParser::Read_BinNode(ifstream& fileBinaryStream, binModel* pModel)
{
	unsigned int iNodeIndex;
	binNode Node;
	// BINMODEL->BINNODE->iNumChildren
	fileBinaryStream.read((char*)(&Node.iNumChildren), sizeof(Node.iNumChildren));
	// BINMODEL->BINNODE->matTransformation
	fileBinaryStream.read((char*)(&Node.matTransformation), sizeof(XMFLOAT4X4));
	// BINMODEL->BINNODE->szName
	char* szTemp = ReadString(fileBinaryStream);
	strcpy_s(Node.szName, szTemp);
	delete[] szTemp;


	pModel->vNodes.push_back(Node);
	iNodeIndex = pModel->vNodes.size() - 1;

	// vChildren
	for (size_t i = 0; i < Node.iNumChildren; ++i)
	{
		pModel->vNodes[iNodeIndex].vChildrenIndex.push_back(Read_BinNode(fileBinaryStream, pModel));
	}

	return iNodeIndex;
}

void CBinParser::WriteString(ofstream& fileBinaryStream, char* pStr)
{
	_uint iStringLength = strlen(pStr);
	fileBinaryStream.write(reinterpret_cast<const char*>(&iStringLength), sizeof(iStringLength));

	fileBinaryStream.write(pStr, iStringLength);
}

void CBinParser::WriteFloat3(ofstream& fileBinaryStream, XMFLOAT3 vTmp)
{
	fileBinaryStream.write((char*)&vTmp, sizeof(XMFLOAT3));
}

void CBinParser::WriteFloat2(ofstream& fileBinaryStream, XMFLOAT2 vTmp)
{
	fileBinaryStream.write((char*)&vTmp, sizeof(XMFLOAT2));
}

void CBinParser::WriteUint(ofstream& fileBinaryStream, _uint iTmp)
{
}

void CBinParser::Write_BinNode(ofstream& fileBinaryStream, binModel* pModel, binNode* pNode)
{
	// BINMODEL->BINNODE->iNumChildren
	fileBinaryStream.write((char*)(&pNode->iNumChildren), sizeof(pNode->iNumChildren));
	// BINMODEL->BINNODE->matTransformation
	fileBinaryStream.write((char*)(&pNode->matTransformation), sizeof(XMFLOAT4X4));
	// BINMODEL->BINNODE->szName
	WriteString(fileBinaryStream, pNode->szName);

	// vChildren
	for (size_t i = 0; i < pNode->iNumChildren; ++i)
	{
		Write_BinNode(fileBinaryStream, pModel, &pModel->vNodes[pNode->vChildrenIndex[i]]);
	}
}

CBinParser* CBinParser::Create()
{
	CBinParser* pInstance = new CBinParser();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(" Failed to Create : CBinParser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBinParser::Free()
{
	__super::Free();

}
