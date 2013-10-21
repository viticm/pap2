// Mesh.cpp: implementation of the CMesh class.
//
//////////////////////////////////////////////////////////////////////
#include "header.h"
#include "Mesh.h"
#include <set>
#include <map>
#include <vector>
#include <direct.h>
#include <algorithm>

//#include "imtl.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

extern void printMat(const CMesh::_Matrix& mat, FILE* fp);

CMesh::CMesh()
{
	ID = 0;
	m_pNode = NULL;
	m_bSkined = FALSE;
	m_dNumFaces = 0;
	m_dNumSubsets = 0;
	m_dNumVertices = 0;
	m_lpFacesIndex = NULL;
	m_lpSubsetID = NULL;
	m_lpVerticeNormal = NULL;
	m_lpVerticePosition = NULL;
	m_lpVerticeRemap = NULL;
	m_lpVerticeTextCoord1 = NULL;
	m_lpVerticeTextCoord2 = NULL;
	m_lpVerticeTextCoord3 = NULL;
	m_lpMaterial = NULL;
	m_lpVerticeDiffuse = NULL;

	m_dNumAnimationVertices = 0;
	m_lpAnimationVerticesIndex = NULL;
	m_lpAnimationVerticesPosition = NULL;
	m_fFrameLength = 0.1f;
	m_dNumFrames = 0;

	m_dNumBone = 0;
	m_lpBoneInfo = NULL;
	m_lpVerticesSkinInfo = NULL;
	m_lpSubsetRemap = NULL;

	m_dNumAnimationBones = 0;
	m_lpAnimationBonesName = NULL;
	m_lpAnimationBonesTrans = NULL;

	m_dwAnimationType = ANIMATION_NONE;

	m_lpAnimationName = new TCHAR[30];

	m_lpAniBScaling = NULL;
	m_lpAniBRotation = NULL;
	m_lpAniBTranslation = NULL;

	m_lpRTSAniBones = NULL;
	m_lpAnimationUV = NULL;

}

CMesh::~CMesh()
{
	delete [] m_lpAnimationName;
	m_lpAnimationName = NULL;
}

HRESULT CMesh::SaveMesh(LPSTR pFileName,DWORD Option)
{
    int nRetCode = false;
	_MeshFileHead MeshFileHead;
	
	//MeshFileHead.


	DWORD BlockEnd  = 0xFFFFFFFF;  

	FILE* pFile = fopen(pFileName,"wb");
	if(pFile==NULL) return E_FAIL;


	DWORD   Pos_Start = ftell(pFile);
	MeshFileHead.MeshCount = 1;
	MeshFileHead.MeshHead.NumFaces = m_dNumFaces;
	MeshFileHead.MeshHead.NumVertices = m_dNumVertices;
	MeshFileHead.MeshHead.NumSubset = m_dNumSubsets;


	fwrite(&MeshFileHead,sizeof(_MeshFileHead),1,pFile);

	/*//计算并写入快长度
	if(!m_bSkined)
	{
		BlockLength += sizeof(DWORD);
		for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
		{
			LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
			BlockLength += sizeof(DWORD)*2+//包含点数，自骨骼数
				sizeof(MATRIX)+            //位移矩阵
				sizeof(TCHAR)*60+          //自己和父骨骼名称
				(sizeof(DWORD)+sizeof(float))*pInfo->NumVerticesRef;//顶点信息
			for(DWORD i_Child=0;i_Child<pInfo->NumChild;i_Child++)
			{
				BlockLength+=sizeof(TCHAR)*30;//
			}
		}
	}
	fwrite(&BlockLength,sizeof(DWORD),1,pFile);*/

	//fwrite(&m_dNumVertices,sizeof(DWORD),1,pFile);                  //顶点数目

	if(Option&MESH_POSITION)
	{
		MeshFileHead.MeshHead.PositionBlock = ftell(pFile) - Pos_Start;
        //FILE * fp = NULL;
        //fp = fopen("C:\Documents and Settings\chenganlai\桌面\HP001\positon.txt","w");
		fwrite( m_lpVerticePosition,sizeof(VEC3),m_dNumVertices,pFile); //顶点位置坐标
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
       // for (int i = 0 ; i < m_dNumVertices ; i ++)
       // {
       //     fprintf(fp,"%.0f %.0f %.0f \n",m_lpVerticePosition[i].x,m_lpVerticePosition[i].y,m_lpVerticePosition[i].z);
      //  }
      //  fclose(fp);
	}
	if(Option&MESH_NORMAL)
	{
		MeshFileHead.MeshHead.NormalBlock = ftell(pFile) - Pos_Start;
	
		fwrite( m_lpVerticeNormal,sizeof(VEC3),m_dNumVertices,pFile);   //顶点法向量
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}
	if(Option&MESH_DIFFUSE)
	{
		MeshFileHead.MeshHead.DiffuseBlock = ftell(pFile) - Pos_Start;
		DWORD* pdwColor = new DWORD[m_dNumVertices];
		for(DWORD s=0;s<m_dNumVertices;s++)
		{
			DWORD A = (DWORD)(m_lpVerticeDiffuse[s].a*255);
			DWORD R = (DWORD)(m_lpVerticeDiffuse[s].r*255);
			DWORD G = (DWORD)(m_lpVerticeDiffuse[s].g*255);
			DWORD B = (DWORD)(m_lpVerticeDiffuse[s].b*255);
			
			if(A>255) A = 255;
			if(R>255) R = 255;
			if(G>255) G = 255;
			if(B>255) B = 255;

			pdwColor[s] = (A<<24) + (R<<16) + (G<<8) + B;
		}

		fwrite( pdwColor,sizeof(DWORD),m_dNumVertices,pFile);   //顶点法向量
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);

		if(pdwColor)
		{
			delete [] pdwColor;
			pdwColor = NULL;
		}
	}

	if(Option&MESH_TEXTUREUVW1)
	{
		MeshFileHead.MeshHead.TextureUVW1Block = ftell(pFile) - Pos_Start;
	
		fwrite( m_lpVerticeTextCoord1,sizeof(VEC3),m_dNumVertices,pFile);   //顶点法向量
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if(Option&MESH_TEXTUREUVW2)
	{
		MeshFileHead.MeshHead.TextureUVW2Block = ftell(pFile) - Pos_Start;
	
		fwrite( m_lpVerticeTextCoord2,sizeof(VEC3),m_dNumVertices,pFile);   //顶点法向量
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if(Option&MESH_TEXTUREUVW3)
	{
		MeshFileHead.MeshHead.TextureUVW3Block = ftell(pFile) - Pos_Start;
	
		fwrite( m_lpVerticeTextCoord3,sizeof(VEC3),m_dNumVertices,pFile);   //顶点法向量
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}
	
	if(Option&MESH_FACESINDEX)
	{
		MeshFileHead.MeshHead.FacesIndexBlock = ftell(pFile) - Pos_Start;
	
		fwrite( m_lpFacesIndex,sizeof(DWORD),m_dNumFaces*3,pFile);//面索引
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}
	
	if(Option&MESH_SUBSETINDEX)
	{
		MeshFileHead.MeshHead.SubsetIndexBlock = ftell(pFile) - Pos_Start;
		//FixSubsetIndex(m_lpSubsetID, m_dNumFaces, m_dNumSubsets);

		fwrite( m_lpSubsetID,sizeof(DWORD),m_dNumFaces,pFile);//面索引
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if(m_bSkined)
	{
		//////////////////////////////////////////////////////////////////////////
		//

		//////////////////////////////////////////////////////////////////////////
		MeshFileHead.MeshHead.SkinInfoBlock = ftell(pFile) - Pos_Start;
		//骨骼信息
		fwrite(&m_dNumBone ,sizeof(DWORD),1,pFile);//
		for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
		{
			LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
			fwrite(pInfo->szName,sizeof(TCHAR),MAX_BONE_NAME_LENGTH,pFile);//
			fwrite(pInfo->szParentName,sizeof(TCHAR),MAX_BONE_NAME_LENGTH,pFile);//
			fwrite(&pInfo->NumChild,sizeof(DWORD),1,pFile);//
			
			for(DWORD i_Child=0;i_Child<pInfo->NumChild;i_Child++)
			{
				fwrite(pInfo->lpChildNames[i_Child],sizeof(TCHAR),MAX_BONE_NAME_LENGTH,pFile);//
			}
			
			fwrite(&pInfo->MatrixOffset,sizeof(MATRIX),1,pFile);//
			fwrite(&pInfo->BaseBoneMatrix,sizeof(MATRIX),1,pFile);//
			
			fwrite(&pInfo->NumVerticesRef,sizeof(DWORD),1,pFile);//
			fwrite(pInfo->VerticesIndexRef,sizeof(DWORD),
				pInfo->NumVerticesRef,pFile);//
			fwrite(pInfo->VerticesWeightRef,sizeof(float),
				pInfo->NumVerticesRef,pFile);//
		}

		DWORD NunSocket = m_listSocket.size();
		fwrite(&NunSocket,sizeof(DWORD),1,pFile);
		list<_Socket>::iterator i_Socket = m_listSocket.begin();
		while(i_Socket!=m_listSocket.end())
		{
			_Socket Socket = *i_Socket;
			TCHAR Name[30];
			TCHAR ParentName[30];
			wsprintf(Name,"%s",Socket.Name.c_str());
			wsprintf(ParentName,"%s",Socket.ParentName.c_str());

			fwrite(Name,sizeof(TCHAR),30,pFile);//
			fwrite(ParentName,sizeof(TCHAR),30,pFile);//
			fwrite(&Socket.MatrixOffset,sizeof(MATRIX),1,pFile);//
			i_Socket++;
		}

		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);

        // write flexible body information
        DWORD dwFBNum = 0;
        for (DWORD i_Bone = 0; i_Bone < m_dNumBone; i_Bone++)
        {
            LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
            
            nRetCode = strncmp(pInfo->szName, "FBR_", sizeof("FBR_") - 1);
            if (nRetCode == 0)
            {
                dwFBNum++;
            }
        }

        if (dwFBNum > 0)
        {
            MeshFileHead.MeshHead.FlexibleBodyBlock = ftell(pFile) - Pos_Start;
            DWORD NumCollisionSurface = m_listCollisionSurface.size();
            fwrite(&NumCollisionSurface, sizeof(DWORD), 1, pFile);
            i_Socket = m_listCollisionSurface.begin();
            while (i_Socket != m_listCollisionSurface.end())
            {
                _Socket Socket = *i_Socket;

                FLEXIBLE_BODY_SOCKET s;
                memset(&s, 0, sizeof(FLEXIBLE_BODY_SOCKET));
                wsprintf(s.ParentBoneName, "%s", Socket.ParentName.c_str());
                wsprintf(s.SocketName, "%s", Socket.Name.c_str());
                s.ParentBoneName[29] = '\0';
                s.SocketName[29] = '\0';
                s.Matrix = Socket.MatrixOffset;
                wsprintf(s.BindBoneName, "%s", &s.SocketName[6]);

                fwrite(&s, 1, sizeof(FLEXIBLE_BODY_SOCKET), pFile);

                i_Socket++;
            }

            fwrite(&dwFBNum, sizeof(DWORD), 1, pFile);
            while (dwFBNum)
            {
                FLEXIBLE_BODY_PARAM tmp;
                memset(&tmp, 0, sizeof(FLEXIBLE_BODY_PARAM));
                tmp.bEnableCollisionDetect = TRUE;
                tmp.fGravityInherit = 0;
                tmp.bEnable = TRUE;
                tmp.fHooke = 50;
                tmp.fResistance = 0.9f;
                tmp.vGravity = D3DXVECTOR3(0, -1000.0f, 0);
                fwrite(&tmp, sizeof(FLEXIBLE_BODY_PARAM), 1, pFile);
                dwFBNum--;
            }

            fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
        }
    }

	MeshFileHead.BlockLength = ftell(pFile) - Pos_Start;
	fseek(pFile,Pos_Start,SEEK_SET);
	fwrite(&MeshFileHead,sizeof(_MeshFileHead),1,pFile);
	fclose(pFile);
	return S_OK;
}

void CMesh::FixSubsetIndex(DWORD* pSubsetID, DWORD dwNumFaces, DWORD dwNumSubsets)
{
	std::set<DWORD> setIDs;
	std::vector<DWORD> vecIDs;
	std::map<DWORD, DWORD> mapIDs;
	int nIndex = 0;
	for(DWORD dwFaceIndex = 0; dwFaceIndex < dwNumFaces; dwFaceIndex++)
	{
		if(!setIDs.count(pSubsetID[dwFaceIndex]))
		{
			setIDs.insert(pSubsetID[dwFaceIndex]);
			vecIDs.push_back(pSubsetID[dwFaceIndex]);
		}
	}
	sort(vecIDs.begin(), vecIDs.end());
	for(size_t i = 0; i < vecIDs.size(); i++)
	{
		DWORD dwFaceID = vecIDs[i];
		mapIDs[vecIDs[i]] = i;
	}
	for(DWORD dwFaceIndex = 0; dwFaceIndex < dwNumFaces; dwFaceIndex++)
	{
		pSubsetID[dwFaceIndex] = mapIDs[pSubsetID[dwFaceIndex]];
	}
	setIDs.clear();
	vecIDs.clear();
	mapIDs.clear();
}


HRESULT CMesh::GetRelativePath(const TCHAR* strAbsPath1, const TCHAR* strAbsPath2, TCHAR* strRelativePath)
{
	HRESULT hr = E_FAIL;
	TCHAR strAbsPath11[MAX_PATH];
	TCHAR strAbsPath21[MAX_PATH];
	TCHAR strDrive1[MAX_PATH];
	TCHAR strPath1[MAX_PATH];
	TCHAR strFileName1[MAX_PATH];
	TCHAR strExt1[MAX_PATH];

	TCHAR strDrive2[MAX_PATH];
	TCHAR strPath2[MAX_PATH];
	TCHAR strFileName2[MAX_PATH];
	TCHAR strExt2[MAX_PATH];

	TCHAR* strFind = NULL;
	assert(strAbsPath1);
	assert(strAbsPath2);
	assert(strRelativePath);

	int nIndex = 0;
	int nDepthsOfDirectory = 0;
	int nDepthDirUp = 0;
	strRelativePath[0] = '\0'; 
	int nPath1Length = 0;
	int nPath2Length = 0;

	strcpy(strAbsPath11, strAbsPath1);
	strcpy(strAbsPath21, strAbsPath2);

	_strlwr(strAbsPath11);
	_strlwr(strAbsPath21);

	_splitpath(strAbsPath11, strDrive1, strPath1, strFileName1, strExt1);
	_splitpath(strAbsPath21, strDrive2, strPath2, strFileName2, strExt2);

	if(strcmp(strDrive1, strDrive2))
	{
		//If the driver's name is not the same, no way to got the relative path
		if(strDrive2[0] != '\0')//如果不是没有盘符，　那么就说明确实是在两个不同的盘里面，无法取得相对路径
		{
			strcpy(strRelativePath, strAbsPath2);
			goto Exit0;
		}
		else
		{
			//已经就是相对路径了
			strcpy(strRelativePath, strAbsPath2);
			hr = S_OK;
			goto Exit0;
		}
	}

	strFind = strstr(strPath2, strPath1);
	if(strFind)//strPath1 is in strPath2, means strPath2 is a sub directory of strPath1
	{
		strFind += strlen(strPath1);
		strcpy(strRelativePath, strFind);
		strcat(strRelativePath, strFileName2);
		strcat(strRelativePath, strExt2);
		hr = S_OK;
	}
	else
	{
		//If not check if strPath1 is a sub directory of strPath2
		strFind = strstr(strPath1, strPath2);
		if(strFind)
		{
			//strPath1 is a sub directory of strPath2
			nIndex = strlen(strPath1) - 1;
			nDepthsOfDirectory = 0;
			while(nIndex > 0 && (strPath1 + nIndex) != strFind + strlen(strPath2))
			{
				if(strPath1[nIndex] == '\\')
					nDepthsOfDirectory++;
				nIndex--;
			}
			for(int i = 0; i < nDepthsOfDirectory; i++)
			{
				strcat(strRelativePath, "..\\");
			}
			//if(*strPath2 == '\\')
			//	strcat(strRelativePath, strPath2 + 1);
			//else
			//	strcat(strRelativePath, strPath2);
			strcat(strRelativePath, strFileName2);
			strcat(strRelativePath, strExt2);
			hr = S_OK;
		}
		else//strPath1 and strPath2 has no sub directory relationship, but they are in the same disk
		{
			//But they might share some directory from the beginning path
			nPath1Length = static_cast<int>(strlen(strPath1));
			nPath2Length = static_cast<int>(strlen(strPath2));
			int nShorterPath = (nPath1Length > nPath2Length) ? nPath2Length : nPath1Length;
			int nSamePathIndex = (strPath1[0] == '\\' ? 1 : 0);
			int nSameDirIndex = 0;
			int nSameDir = 0;
			while(nSamePathIndex < nShorterPath)
			{
				if(strPath1[nSamePathIndex] != strPath2[nSamePathIndex])
					break;
				if(strPath1[nSamePathIndex] == '\\')
				{
					nSameDir++;
					nSameDirIndex = nSamePathIndex;
				}
				nSamePathIndex++;
			}
			nDepthsOfDirectory = 0;
			nIndex = strlen(strPath1) - 1;
			while(strPath1[nIndex] == '\\')
				nIndex--;
			while(nIndex >= 0)
			{
				if(strPath1[nIndex] == '\\')
				{
					nDepthsOfDirectory++;
				}
				nIndex--;
			}
			nDepthsOfDirectory -= nSameDir;
			for(int i = 0; i < nDepthsOfDirectory; i++)
			{
				strcat(strRelativePath, "..\\");
			}
			
			if(nSameDir)
			{
				if(strPath2[nSameDirIndex] == '\\')
					strcat(strRelativePath, strPath2 + nSameDirIndex + 1);
				else
					strcat(strRelativePath, strPath2 + nSameDirIndex);
			}
			else
			{
				if(strPath2[0] == '\\')
					strcat(strRelativePath, strPath2 + 1);
				else
					strcat(strRelativePath, strPath2);
			}
			strcat(strRelativePath, strFileName2);
			strcat(strRelativePath, strExt2);
			hr = S_OK;
		}
	}
Exit0:
	return hr;
}

HRESULT CMesh::GetAbsPath(const TCHAR* strFileName, TCHAR* strOutPath)
{
	HRESULT hr = E_FAIL;
	assert(strFileName);
	assert(strOutPath);

	//_splitpath(strAbsPath1, strDrive1, strPath1, strFileName1, strExt1);
	strcpy(strOutPath, strFileName);
	size_t strLen = strlen(strOutPath);
	while(strLen > 0 && strOutPath[strLen - 1] != '\\')
	{
		strLen--;	
	}
	if(strLen > 0)
	{
		strOutPath[strLen] = '\0';
		hr = S_OK;
	}
	return hr;
}

HRESULT CMesh::SaveMaterial(LPSTR pFileName)
{
	DWORD Mask = 0x4D41544C;//MATL对应的ASECC代码
	DWORD BlockEnd  = 0xFFFFFFFF;  
	DWORD BlockLength = 0;
	TCHAR strFixTextureName[256];
	LPTEXTURE pTexture = NULL;
	FILE* pFile = fopen(pFileName,"wb");
	if(pFile==NULL) return E_FAIL;
	fwrite(&Mask,sizeof(DWORD),1,pFile);
	//计算并写入快长度
	BlockLength += sizeof(DWORD)*2 ;//子部分数目，结束标志
	for(DWORD i_Sub=0;i_Sub<m_dNumSubsets;i_Sub++)
	{
		LPMATERIAL pMaterial = &m_lpMaterial[i_Sub]; 
		BlockLength += sizeof(float)*17 + sizeof(DWORD) + //四种颜色，反光强度，帖图数目
			pMaterial->NumTexture*(sizeof(float)+sizeof(TCHAR)*(20+256));//帖图信息 
	}
	fwrite(&BlockLength,sizeof(DWORD),1,pFile);

	fwrite(&m_dNumSubsets,sizeof(DWORD),1,pFile);                  //子部分数目
	for( i_Sub=0;i_Sub<m_dNumSubsets;i_Sub++)
	{
		LPMATERIAL pMaterial = &m_lpMaterial[i_Sub]; 
		pMaterial->Ambient.a = 1.0f;
		pMaterial->Ambient.r = 1.0f;
		pMaterial->Ambient.g = 1.0f;
		pMaterial->Ambient.b = 1.0f;

		pMaterial->Diffuse.a = 1.0f;
		pMaterial->Diffuse.r = 1.0f;
		pMaterial->Diffuse.g = 1.0f;
		pMaterial->Diffuse.b = 1.0f;

		pMaterial->Specular.a = 1.0f;
		pMaterial->Specular.r =  .0f;
		pMaterial->Specular.g =  .0f;
		pMaterial->Specular.b =  .0f;

		pMaterial->Emissive.a = 1.0f;
		pMaterial->Emissive.r =  .0f;
        pMaterial->Emissive.g =  .0f;
		pMaterial->Emissive.b =  .0f;
		
		fwrite(&pMaterial->Ambient,sizeof(COLOR4),1,pFile); 
        fwrite(&pMaterial->Diffuse,sizeof(COLOR4),1,pFile); 
		fwrite(&pMaterial->Specular,sizeof(COLOR4),1,pFile); 
		fwrite(&pMaterial->Emissive,sizeof(COLOR4),1,pFile); 
		fwrite(&pMaterial->Power,sizeof(float),1,pFile);
		
		DWORD NumOption = 0;
		fwrite(&pMaterial->Option,sizeof(DWORD),1,pFile);//材质选项
		fwrite(&NumOption,sizeof(DWORD),1,pFile);//选项数目

		fwrite(&pMaterial->NumTexture,sizeof(DWORD),1,pFile);//帖图数目
		for(DWORD i_Text=0;i_Text<pMaterial->NumTexture;i_Text++)
		{
			pTexture = &pMaterial->Textures[i_Text];
			fwrite(&pTexture->Amount,sizeof(float),1,pFile);
			fwrite( pTexture->TextureType,sizeof(TCHAR),20,pFile);
			GetRelativePath(pFileName, pTexture->FileName, strFixTextureName);
			fwrite(strFixTextureName,sizeof(TCHAR),256,pFile);
			fwrite(&pMaterial->NumTextureOption[i_Text],sizeof(DWORD),1,pFile);//帖图选项数目
			if(pMaterial->NumTextureOption[i_Text]>0)
			{
				fwrite(&pMaterial->TexturesOption[i_Text][0].Type,sizeof(DWORD),1,pFile);
				LPTEXTURETF pTf = (LPTEXTURETF) pMaterial->TexturesOption[i_Text][0].pData;
				fwrite(pTf,sizeof(_TextureTf),1,pFile);
			}
		}
	}	
	fwrite(&BlockEnd,sizeof(DWORD),1,pFile);

	fclose(pFile);
	return S_OK;
}

HRESULT CMesh::SaveAnimation(LPSTR pFileName,LPSTR pAnimationName)
{
	int i;
	if(m_dwAnimationType==ANIMATION_NONE)
		return S_OK;
	if(m_dNumFrames==0)
		return S_OK;
	if((m_dNumAnimationBones == 0)&&(m_dNumAnimationVertices == 0))
		return S_OK;

	DWORD Mask = 0x414E494D;//ANIM对应的ASECC代码
	DWORD BlockEnd  = 0xFFFFFFFF;  
	DWORD BlockLength = 0;
	DWORD NumAnimation = 1;
	FILE* pFile = fopen(pFileName,"wb");
	if(pFile==NULL) return E_FAIL;
	fwrite(&Mask,sizeof(DWORD),1,pFile);
	///////////////////////////////////////////////////////////////////////////////////////
	//写入块长度
	if(m_dwAnimationType==ANIMATION_BONE)
	{
		//计算并写入快长度
		BlockLength += sizeof(DWORD)*3;//动画数目，结束标志，动画类型
		BlockLength += sizeof(float) + sizeof(DWORD)*2;//时间间隔，骨骼数目，动作帧数
		BlockLength += sizeof(TCHAR)*30*m_dNumAnimationBones;//骨骼名称
		BlockLength += sizeof(MATRIX)*m_dNumAnimationBones*m_dNumFrames;//骨骼变换矩阵

		fwrite(&BlockLength,sizeof(DWORD),1,pFile);
	}
	if(m_dwAnimationType==ANIMATION_BONE_RELATIVE)
	{
		//计算并写入快长度
		BlockLength += sizeof(DWORD)*3;//动画数目，结束标志，动画类型
		BlockLength += sizeof(float) + sizeof(DWORD)*2;//时间间隔，骨骼数目，动作帧数
		BlockLength += sizeof(TCHAR)*30*m_dNumAnimationBones;//骨骼名称
		BlockLength += sizeof(MATRIX)*m_dNumAnimationBones*m_dNumFrames;//骨骼变换矩阵

		fwrite(&BlockLength,sizeof(DWORD),1,pFile);
	}
	else if(m_dwAnimationType==ANIMATION_VERTICES_RELATIVE)
	{
		//计算并写入快长度
		BlockLength += sizeof(DWORD)*4;//动画数目，结束标志，动画类型,顶点总数
		BlockLength += sizeof(float) + sizeof(DWORD)*2;//时间间隔，顶点数目，动作帧数
		BlockLength += sizeof(DWORD)*m_dNumAnimationVertices;//顶点索引
		BlockLength += sizeof(VEC3)*m_dNumAnimationVertices*m_dNumFrames;//顶点位置信息

		fwrite(&BlockLength,sizeof(DWORD),1,pFile);
	}
	else if(m_dwAnimationType==ANIMATION_VERTICES)
	{
		//计算并写入快长度
		BlockLength += sizeof(DWORD)*4;//动画数目，结束标志，动画类型,顶点总数
		BlockLength += sizeof(float) + sizeof(DWORD)*2;//时间间隔，顶点数目，动作帧数
		BlockLength += sizeof(DWORD)*m_dNumAnimationVertices;//顶点索引
		BlockLength += sizeof(VEC3)*m_dNumAnimationVertices*m_dNumFrames;//顶点位置信息
		if(m_lpAnimationUV)
			BlockLength += sizeof(VEC3) * m_dNumAnimationVertices * m_dNumFrames;
		fwrite(&BlockLength,sizeof(DWORD),1,pFile);
	}
	else if ( m_dwAnimationType == ANIMATION_BONE_RTS )
	{
		BlockLength += sizeof(DWORD)*3;//动画数目，结束标志，动画类型
		BlockLength += sizeof(float) + sizeof(DWORD)*2;//时间间隔，骨骼数目，动作帧数
		BlockLength += sizeof(TCHAR)*30*m_dNumAnimationBones;//骨骼名称
		BlockLength += sizeof(RTSBoneAnimation)*m_dNumAnimationBones*m_dNumFrames;//骨骼变换矩阵

		fwrite(&BlockLength,sizeof(DWORD),1,pFile);
	}
	else if( m_dwAnimationType == ANIMATION_BONE_KEYONLY )
	{
		//////////////////////////////////////////////////////////////////////////
		//Add here 
		//计算并写入快长度
		BlockLength += sizeof(DWORD)*3;//动画数目，结束标志，动画类型
		BlockLength += sizeof(float) + sizeof(DWORD)*2;//时间间隔，骨骼数目，动作帧数
		BlockLength += sizeof(TCHAR)*30*m_dNumAnimationBones;//骨骼名称
		//if the last bone is a static bone, the bone_animation_size = (last_bone.offset + 1) *sizeof(MATRIX), 
		//else bone_animation_size = (last_bone.offset + numFrames) * sizeof(MATRIX)
		if ( m_lpBoneInfo[m_dNumBone - 1].Static )
		{
			BlockLength += sizeof(MATRIX) * (m_lpBoneInfo[m_dNumBone - 1].Offset + 1);
		}
		else 
		{
			BlockLength += sizeof(MATRIX) * ( m_lpBoneInfo[m_dNumBone - 1].Offset + m_dNumFrames);
		}
		fwrite(&BlockLength,sizeof(DWORD),1,pFile);
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	fwrite(&NumAnimation,sizeof(DWORD),1,pFile);                      //动画数目，这里默认为１	
	fwrite(&m_dwAnimationType,sizeof(DWORD),1,pFile);
	fwrite( pAnimationName,sizeof(TCHAR),30,pFile);

	if(m_dwAnimationType==ANIMATION_BONE)
	{
		fwrite(&m_dNumAnimationBones,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
		fwrite(&m_dNumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
		fwrite(&m_fFrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

		for(DWORD i_Bone =0;i_Bone <m_dNumAnimationBones;i_Bone++)
		{
			LPSTR BoneName = m_lpAnimationBonesName[i_Bone];
			fwrite(BoneName,sizeof(TCHAR),30,pFile); 
		}		
		fwrite(m_lpAnimationBonesTrans,sizeof(MATRIX)*m_dNumAnimationBones*m_dNumFrames,1,pFile);//输出骨骼变换数目
	}	

	else if(m_dwAnimationType==ANIMATION_BONE_RELATIVE)
	{
		fwrite(&m_dNumAnimationBones,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
		fwrite(&m_dNumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
		fwrite(&m_fFrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

		for(DWORD i_Bone =0;i_Bone <m_dNumAnimationBones;i_Bone++)
		{
			LPSTR BoneName = m_lpAnimationBonesName[i_Bone];
			fwrite(BoneName,sizeof(TCHAR),30,pFile); 
		}		
		
		fwrite(m_lpAnimationBonesTrans,sizeof(MATRIX)*m_dNumAnimationBones*m_dNumFrames,1,pFile);//输出骨骼变换数目

		/*DWORD Size = m_dNumAnimationBones*m_dNumFrames;
		
		fwrite(m_lpAniBScaling,    sizeof(VEC3)*Size,1,pFile);//输出骨骼变换数目
		fwrite(m_lpAniBRotation,   sizeof(VEC4)*Size,1,pFile);//输出骨骼变换数目
		fwrite(m_lpAniBTranslation,sizeof(VEC3)*Size,1,pFile);//输出骨骼变换数目*/
	}
	else if( m_dwAnimationType == ANIMATION_BONE_RTS )
	{
		fwrite(&m_dNumAnimationBones,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
		fwrite(&m_dNumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
		fwrite(&m_fFrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

		for(DWORD i_Bone =0;i_Bone <m_dNumAnimationBones;i_Bone++)
		{
			LPSTR BoneName = m_lpAnimationBonesName[i_Bone];
			fwrite(BoneName,sizeof(TCHAR),30,pFile); 
		}
		fwrite(m_lpRTSAniBones, sizeof(RTSBoneAnimation)*m_dNumAnimationBones*m_dNumFrames, 1, pFile);
	}
	else if( m_dwAnimationType == ANIMATION_BONE_KEYONLY )
	{
		//////////////////////////////////////////////////////////////////////////
		//Add here
		fwrite(&m_dNumAnimationBones,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
		fwrite(&m_dNumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
		fwrite(&m_fFrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

		for(DWORD i_Bone =0;i_Bone <m_dNumBone;i_Bone++)
		{
			LPSTR BoneName = m_lpAnimationBonesName[i_Bone];
			fwrite(BoneName,sizeof(TCHAR),30,pFile); 
		}		

		DWORD* pOffsetInfo = new DWORD[m_dNumAnimationBones];
		for ( i = 0; i < m_dNumAnimationBones; i++ )
		{
			pOffsetInfo[i] = m_lpBoneInfo[i].Offset;
		}
		//写入骨骼动画信息在整个动画数组的位置信息，如果骨骼没有动，那么只有一帧数据存在
		fwrite(pOffsetInfo, sizeof(DWORD), m_dNumAnimationBones, pFile);

		DWORD dwLength;
		if ( m_lpBoneInfo[m_dNumAnimationBones - 1].Static )
		{
			dwLength = pOffsetInfo[m_dNumAnimationBones - 1] + 1;
		}
		else
		{
			dwLength = pOffsetInfo[m_dNumAnimationBones - 1] + m_dNumFrames;
		}
		fwrite(&dwLength, sizeof(DWORD), 1, pFile);

		//FILE* fp = fopen("d:\\debug_final_result.txt", "w+");
		//Animation matrix
		//for(DWORD iBone = 0; iBone < m_dNumBone; iBone++ )
		//{
		//	//fprintf(fp, "Bone:%d\n", iBone);
		//	if ( m_lpBoneInfo[iBone].Static )
		//	{
		//		//printMat(m_lpAnimationBonesTrans[m_lpBoneInfo[iBone].Offset], fp);
		//		//fprintf(fp, "========================\n");
		//	}
		//	else
		//	{
		//		for(int asdf = 0; asdf < m_dNumFrames; asdf++ )
		//		{
		//			printMat(m_lpAnimationBonesTrans[m_lpBoneInfo[iBone].Offset + asdf], fp);
		//			fprintf(fp, "========================\n");
		//		}
		//	}

		//	//if ( m_lpBoneInfo[iBone].Static )
		//	//	fwrite(&m_lpAnimationBonesTrans[m_lpBoneInfo[iBone].Offset], sizeof(MATRIX), 1, pFile);
		//	//else 
		//	//	fwrite(&m_lpAnimationBonesTrans[m_lpBoneInfo[iBone].Offset], sizeof(MATRIX), m_dNumFrames, pFile);
		//}
		fwrite(m_lpAnimationBonesTrans, sizeof(MATRIX), dwLength, pFile);
		//fclose(fp);
		delete[] pOffsetInfo;
	}
	else if(m_dwAnimationType==ANIMATION_VERTICES_RELATIVE)
	{
		fwrite(&m_dNumVertices,sizeof(DWORD),1,pFile);                //顶点总数，用来检测是否和网格相匹配 
		/////////////////////////////////////////////////////////////////////////////////////
		fwrite(&m_dNumAnimationVertices,sizeof(DWORD),1,pFile);       //该动画中包含的顶点数目
		fwrite(&m_dNumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
		fwrite(&m_fFrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

		fwrite(m_lpAnimationVerticesIndex,sizeof(DWORD),m_dNumAnimationVertices,pFile); 
		fwrite(m_lpAnimationVerticesPosition,sizeof(VEC3)*m_dNumAnimationVertices*m_dNumFrames,1,pFile);//输出骨骼变换数目
	}
	else if(m_dwAnimationType==ANIMATION_VERTICES)
	{
		fwrite(&m_dNumVertices,sizeof(DWORD),1,pFile);                //顶点总数，用来检测是否和网格相匹配 
		/////////////////////////////////////////////////////////////////////////////////////
		fwrite(&m_dNumAnimationVertices,sizeof(DWORD),1,pFile);       //该动画中包含的顶点数目
		fwrite(&m_dNumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
		fwrite(&m_fFrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

		fwrite(m_lpAnimationVerticesIndex,sizeof(DWORD),m_dNumAnimationVertices,pFile); 
		fwrite(m_lpAnimationVerticesPosition,sizeof(VEC3)*m_dNumAnimationVertices*m_dNumFrames,1,pFile);//顶点位置
		if(m_lpAnimationUV)
			fwrite(m_lpAnimationUV, sizeof(VEC3) * m_dNumAnimationVertices * m_dNumFrames, 1, pFile);
	}
	fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	fclose(pFile);
	return S_OK;
}

HRESULT CMesh::CleanUp()
{
	if(m_bSkined)
	{
		if(m_lpAnimationBonesName!=NULL)
		{
			delete [] m_lpAnimationBonesName ;
			m_lpAnimationBonesName = NULL;
		}
		if(m_lpAnimationBonesTrans!=NULL)
		{
			delete [] m_lpAnimationBonesTrans;
			m_lpAnimationBonesTrans = NULL;
		}
		if(m_lpBoneInfo!=NULL)
		{
			for(DWORD i=0;i<m_dNumBone;i++)
			{
				LPBONEINFO pInfo = &m_lpBoneInfo[i];
				if(pInfo->lpChildNames!=NULL)
				{
					for (DWORD j = 0; j < pInfo->NumChild; j++)
					{
						SAFE_DELETE_ARRAY(pInfo->lpChildNames[j]);
					}
					delete [] pInfo->lpChildNames ;
					pInfo->lpChildNames = NULL;
				}
				if(pInfo->VerticesIndexRef!=NULL)
				{
					delete [] pInfo->VerticesIndexRef;
					pInfo->VerticesIndexRef = NULL;
				}
				if(pInfo->VerticesWeightRef!=NULL)
				{
					delete [] pInfo->VerticesWeightRef;
					pInfo->VerticesWeightRef = NULL;
				}
			}
			delete [] m_lpBoneInfo;
			m_lpBoneInfo = NULL;
		}
		if(m_lpVerticesSkinInfo!=NULL)
		{
			for(DWORD i=0;i<m_dNumBone;i++)
			{
				LPVERTICESSKININFO pInfo = &m_lpVerticesSkinInfo[i];
				if(pInfo->BoneName!=NULL)
				{
					delete [] pInfo->BoneName;
					pInfo->BoneName = NULL;
				}
				if(pInfo->Weight!=NULL)
				{
					delete [] pInfo->Weight;
					pInfo->Weight = NULL;
				}
			}
			delete [] m_lpVerticesSkinInfo ;
			m_lpVerticesSkinInfo = NULL;
		}
	}
	else
	{
		if(m_lpAnimationVerticesIndex !=NULL)
		{
			delete [] m_lpAnimationVerticesIndex ;
			m_lpAnimationVerticesIndex = NULL;
		}
		if(m_lpAnimationVerticesPosition !=NULL)
		{
			delete [] m_lpAnimationVerticesPosition ;
			m_lpAnimationVerticesPosition = NULL;
		}
		
		SAFE_DELETE_ARRAY(m_lpAnimationUV);
	}
	
	if(m_lpFacesIndex!=NULL)
	{
		delete [] m_lpFacesIndex ;
		m_lpFacesIndex = NULL;
	}
	
	if(m_lpMaterial!=NULL)
	{
		for(DWORD i=0;i<m_dNumSubsets;i++)
		{
			LPMATERIAL pMat = &m_lpMaterial[i];
			if(pMat->Textures!=NULL)
			{
				for(DWORD T=0;T<pMat->NumTexture;T++)
				{
					SAFE_DELETE_ARRAY(pMat->Textures[T].FileName);
					SAFE_DELETE_ARRAY(pMat->Textures[T].TextureType);

					if(pMat->NumTextureOption[T]>0)
					{
						SAFE_DELETE(pMat->TexturesOption[T][0].pData);
						SAFE_DELETE_ARRAY(pMat->TexturesOption[T]);
					}
				}
				delete [] pMat->Textures;
				pMat->Textures = NULL;
			}
		}
		delete [] m_lpMaterial ;
		m_lpMaterial = NULL;
	}
	if(m_lpSubsetID!=NULL) 
	{
		delete [] m_lpSubsetID ;
		m_lpSubsetID = NULL;
	}
	if(m_lpSubsetRemap!=NULL) 
	{
		delete [] m_lpSubsetRemap ;
		m_lpSubsetRemap = NULL;
	}
	if(m_lpVerticeNormal!=NULL) 
	{
		delete [] m_lpVerticeNormal ;
		m_lpVerticeNormal = NULL;
	}
	if(m_lpVerticeDiffuse!=NULL) 
	{
		delete [] m_lpVerticeDiffuse ;
		m_lpVerticeDiffuse = NULL;
	}
	if(m_lpVerticePosition!=NULL) 
	{
		delete [] m_lpVerticePosition ;
		m_lpVerticePosition = NULL;
	}
	if(m_lpVerticeTextCoord1!=NULL) 
	{
		delete [] m_lpVerticeTextCoord1 ;
		m_lpVerticeTextCoord1 = NULL;
	}
	if(m_lpVerticeTextCoord2!=NULL) 
	{
		delete [] m_lpVerticeTextCoord2 ;
		m_lpVerticeTextCoord2 = NULL;
	}
	if(m_lpVerticeTextCoord3!=NULL) 
	{
		delete [] m_lpVerticeTextCoord3 ;
		m_lpVerticeTextCoord3 = NULL;
	}

	if ( m_lpRTSAniBones )
	{
		delete[] m_lpRTSAniBones;
		m_lpRTSAniBones = NULL;
	}

	/*for ( size_t size = 0; size < m_BoneAnimInfo.size(); size++)
	{
		delete[] m_BoneAnimInfo[size].pKeyFrames;
	}
	m_BoneAnimInfo.clear();*/

	m_pNode = NULL;
	m_bSkined = FALSE;
	m_dNumFaces = 0;
	m_dNumSubsets = 0;
	m_dNumVertices = 0;

	m_dNumAnimationVertices = 0;
	m_fFrameLength = 0.1f;
	m_dNumFrames = 0;
	m_dNumBone = 0;
	m_dNumAnimationBones = 0;
	m_dwAnimationType = ANIMATION_NONE;

	return S_OK;
}

HRESULT CMesh::CheckUpMaterial()
{
	DWORD j = 0;
	DWORD * pMaterialRemap = new DWORD[m_dNumSubsets];
	ZeroMemory(pMaterialRemap,sizeof(DWORD)*m_dNumSubsets);

	LPMATERIAL pTempMat = new MATERIAL[m_dNumSubsets];
	ZeroMemory(pTempMat, sizeof(MATERIAL) * m_dNumSubsets);
	
	//FixSubsetIndex(m_lpSubsetID, m_dNumFaces, m_dNumSubsets);
	DWORD dwNewNumMaterial = 0;
	
	for(DWORD i = 0; i < m_dNumSubsets; i++)
	{
		LPMATERIAL pMat = &m_lpMaterial[i];
		if (GetExportOption()->m_bOptimizeSubsetForSpace)
		{
			//如果合并相同属性的Subset
			for(j = 0; j < dwNewNumMaterial; j++)
			{
				LPMATERIAL pMatUN = &pTempMat[j];
			
				int result1 = memcmp(&pMatUN->Ambient,&pMat->Ambient, sizeof(COLOR4));
				int result2 = memcmp(&pMatUN->Diffuse,&pMat->Diffuse, sizeof(COLOR4));
				int result3 = memcmp(&pMatUN->Specular,&pMat->Specular, sizeof(COLOR4));
				int result4 = memcmp(&pMatUN->Emissive,&pMat->Emissive, sizeof(COLOR4));
				int result5 = memcmp(&pMatUN->Option,&pMat->Option, sizeof(DWORD));

				BOOL TextureCmp = FALSE;
				if(pMatUN->NumTexture != pMat->NumTexture)
				{
					TextureCmp = FALSE;
				}
				else
				{
					for(DWORD K=0;K<pMatUN->NumTexture;K++)
					{
						if((strcmp(pMatUN->Textures[K].FileName,pMat->Textures[K].FileName)==0)&&
							(pMatUN->NumTextureOption[K]==pMat->NumTextureOption[K]))
							break;
					}
					if(K>=pMatUN->NumTexture)
						TextureCmp = FALSE;
					else
						TextureCmp = TRUE;
				}
				
				if((result1==0)&&(result2==0)&&
				   (result3==0)&&(result4==0)&&
				   (result5==0)&&TextureCmp)
				{
					pMaterialRemap[i] = j;
					break;
				}
			}
		}
		else
		{
			//不合并相同属性的subset
			pMaterialRemap[i] = dwNewNumMaterial;
			j = dwNewNumMaterial;
		}
		if (j >= dwNewNumMaterial)
		{
			pMat->Option |= (MATERIAL_OPTION_LIGHTING | MATERIAL_OPTION_SPECULARENABLE | MATERIAL_OPTION_ZBUFFER_TRUE);
			pTempMat[dwNewNumMaterial].Option = pMat->Option;
			pTempMat[dwNewNumMaterial].Ambient = pMat->Ambient;
			pTempMat[dwNewNumMaterial].Diffuse = pMat->Diffuse;
			pTempMat[dwNewNumMaterial].Specular = pMat->Specular;
			pTempMat[dwNewNumMaterial].Emissive = pMat->Emissive;
			pTempMat[dwNewNumMaterial].Power = pMat->Power;
			pTempMat[dwNewNumMaterial].NumTexture = pMat->NumTexture;
			if(pMat->NumTexture)
			{
				pTempMat[dwNewNumMaterial].Textures = new TEXTURE[pMat->NumTexture];
				for(DWORD k=0;k<pMat->NumTexture;k++)
				{
					pTempMat[dwNewNumMaterial].Textures[k].Amount = pMat->Textures[k].Amount;
					pTempMat[dwNewNumMaterial].Textures[k].FileName = 
							new TCHAR[256];
					pTempMat[dwNewNumMaterial].Textures[k].TextureType = 
							new TCHAR[30];

					wsprintf(pTempMat[dwNewNumMaterial].Textures[k].FileName,"%s",
						pMat->Textures[k].FileName);
					wsprintf(pTempMat[dwNewNumMaterial].Textures[k].TextureType,"%s",
						pMat->Textures[k].TextureType);
					
					pTempMat[dwNewNumMaterial].NumTextureOption[k] = pMat->NumTextureOption[k];
					
					if(pMat->NumTextureOption[k]>0)
					{					
						pTempMat[dwNewNumMaterial].TexturesOption[k] = new CMesh::_Option[1];
						pTempMat[dwNewNumMaterial].TexturesOption[k][0].Type = OPTION_TEXTURE_TRANSFROM;
						
						CMesh::_TextureTf* pTextureTF = new CMesh::_TextureTf;
						ZeroMemory(pTextureTF,sizeof(CMesh::_TextureTf));
						CMesh::_TextureTf* pSrcTextureTF = (CMesh::_TextureTf*) 
							pMat->TexturesOption[k][0].pData;
						
						pTextureTF->Scaling.x = pSrcTextureTF->Scaling.x;
						pTextureTF->Scaling.y = pSrcTextureTF->Scaling.y;
						pTextureTF->Scaling.z = pSrcTextureTF->Scaling.z;
						
						pTempMat[dwNewNumMaterial].TexturesOption[k][0].pData = pTextureTF;
					}
				}
			}
			pMaterialRemap[i] = dwNewNumMaterial;
			dwNewNumMaterial++;
		}
	}

	DWORD  NumFaces = m_dNumFaces;
	
	for(i=0;i<NumFaces;i++)
	{
		m_lpSubsetID[i] = pMaterialRemap[m_lpSubsetID[i]];
	}

	if(m_lpMaterial!=NULL)
	{
		for(DWORD i=0;i<m_dNumSubsets;i++)
		{
			LPMATERIAL pMat = &m_lpMaterial[i];
			for(DWORD T=0;T<pMat->NumTexture;T++)
			{
				SAFE_DELETE_ARRAY(pMat->Textures[T].FileName);
				SAFE_DELETE_ARRAY(pMat->Textures[T].TextureType);
				if(pMat->NumTextureOption[T]>0)
				{
					SAFE_DELETE(pMat->TexturesOption[T][0].pData);
					SAFE_DELETE_ARRAY(pMat->TexturesOption[T]);
				}
			}
			SAFE_DELETE_ARRAY(pMat->Textures);
		}
		SAFE_DELETE_ARRAY(m_lpMaterial);
	}

	m_lpMaterial = pTempMat;
	m_dNumSubsets = dwNewNumMaterial;
	SAFE_DELETE_ARRAY(pMaterialRemap);

	CompressSubset();
	return S_OK;
}

HRESULT CMesh::FindTexture(Interface* ip, const TCHAR* strOrg, TCHAR* strFixed)
{
	HRESULT hr = E_FAIL;
	TCHAR strFileName[MAX_PATH];
	TCHAR strFileExt[MAX_PATH];
	TCHAR strMaxPath[MAX_PATH];
	TCHAR strMaxDriver[MAX_PATH];

	TSTR strMaxFileName = ip->GetCurFilePath();
	_splitpath(strMaxFileName, strMaxDriver, strMaxPath, NULL, NULL);
	_splitpath(strOrg, NULL, NULL, strFileName, strFileExt);

	sprintf(strFixed, "%s%stexture\\%s%s", strMaxDriver, strMaxPath, strFileName, strFileExt);
	FILE* fpFile = fopen(strFixed, "r");
	if (!fpFile)
	{
		fpFile = fopen(strOrg, "r");
		if (fpFile)
		{
			strcpy(strFixed, strOrg);
		}
		else
		{
			return E_FAIL;
		}
	}
	hr = S_OK;
	return hr;
}

HRESULT CMesh::MoveTextureFile(Interface* ip, LPCSTR strTexturePath, LPCSTR strAbsMeshPath)
{
	HRESULT hr = E_FAIL;
	TCHAR strDrive[MAX_PATH];
	TCHAR strDir[MAX_PATH]; 
	TCHAR strFileName[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	TCHAR strTgaFileName[MAX_PATH];
	TCHAR strAbsTgaFileName[MAX_PATH];
	CPSD2TGA psd2tga;
	assert(strTexturePath);
	mkdir(strTexturePath);
	for(DWORD i = 0; i < m_dNumSubsets; i++)
	{
		LPMATERIAL pMaterial = &m_lpMaterial[i]; 
		for(DWORD j = 0; j < pMaterial->NumTexture; j++)
		{
			
			TCHAR strFixedTexture[MAX_PATH];
			hr = FindTexture(ip, pMaterial->Textures[j].FileName, strFixedTexture);
			if (FAILED(hr))
				pMaterial->Textures[j].FileName[0] = '\0';
			else
				strcpy(pMaterial->Textures[j].FileName, strFixedTexture);
			
			_splitpath(pMaterial->Textures[j].FileName, strDrive, strDir, strFileName, strExt);
			sprintf(strTgaFileName, "%s%s.tga", strTexturePath, strFileName);
			GetAbsPathFromRelativePath(strAbsMeshPath, strTgaFileName, strAbsTgaFileName);
			if(_strcmpi(strExt,".psd")==0)
			{
				//如果是使用得PSD文件,需要把它转换成TGA
				psd2tga.PSD2TGA(pMaterial->Textures[j].FileName, strAbsTgaFileName);
			}
			else
			{
				CopyFile(pMaterial->Textures[j].FileName, strAbsTgaFileName, FALSE);
			}
			wsprintf(pMaterial->Textures[j].FileName, "%s", strTgaFileName);
		}
	}
	return hr;
}


HRESULT CMesh::MoveTextureFile1(Interface* ip, LPCSTR strDestTexturePath)
{
	HRESULT hr = E_FAIL;
	TCHAR strDrive[MAX_PATH];
	TCHAR strDir[MAX_PATH]; 
	TCHAR strFileName[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	TCHAR strTgaFileName[MAX_PATH];
	CPSD2TGA psd2tga;
	assert(strDestTexturePath);
	mkdir(strDestTexturePath);
	for(DWORD i = 0; i < m_dNumSubsets; i++)
	{
		LPMATERIAL pMaterial = &m_lpMaterial[i]; 
		for(DWORD j = 0; j < pMaterial->NumTexture; j++)
		{

			TCHAR strFixedTexture[MAX_PATH];
			hr = FindTexture(ip, pMaterial->Textures[j].FileName, strFixedTexture);
			if (FAILED(hr))
				pMaterial->Textures[j].FileName[0] = '\0';
			else
				strcpy(pMaterial->Textures[j].FileName, strFixedTexture);

			_splitpath(pMaterial->Textures[j].FileName, strDrive, strDir, strFileName, strExt);
			sprintf(strTgaFileName, "%s%s.tga", strDestTexturePath, strFileName);
			if(_strcmpi(strExt,".psd")==0)
			{
				//如果是使用得PSD文件,需要把它转换成TGA
				psd2tga.PSD2TGA(pMaterial->Textures[j].FileName, strTgaFileName);
			}
			else
			{
				CopyFile(pMaterial->Textures[j].FileName, strTgaFileName, FALSE);
			}
			wsprintf(pMaterial->Textures[j].FileName, "%s", strTgaFileName);
		}
	}
	return hr;
}

HRESULT CMesh::GetAbsPathFromRelativePath(const TCHAR* strAbsPath, const TCHAR* strRelativePath, TCHAR* strOutPath)
{
	HRESULT hr = E_FAIL;
	assert(strAbsPath);
	assert(strRelativePath);
	assert(strOutPath);

	const TCHAR* strDir = strRelativePath;
	const TCHAR* strFinalRelativePath = strDir;
	int nDirUp = 0;
	int nIndex = strlen(strAbsPath) - 1;
	do
	{	
		strDir = strstr(strDir, "..\\");
		if(strDir)
		{
			nDirUp++;
			strDir += 3;
			strFinalRelativePath = strDir;
		}
	}while(strDir);

	//除掉路径最后的那些"\"
	while(strAbsPath[nIndex] == '\\')
		nIndex--;
	while(nIndex > 0 && nDirUp)
	{
		if(strAbsPath[nIndex] == '\\') 
		{
			nDirUp--;			
		}
		nIndex--;
	}
	if(nDirUp == 0)
	{	
		strcpy(strOutPath, strAbsPath);
		strOutPath[nIndex + 1] = '\0';
		if(*strFinalRelativePath != '\\')
		{
			strcat(strOutPath, "\\");
		}
		strcat(strOutPath, strFinalRelativePath);

		hr = S_OK;
	}
	return hr;
}


void CMesh::CompressSubset()
{
	BOOL* pSubsetUsed = new BOOL[m_dNumSubsets];
	DWORD dwNumSubset = m_dNumSubsets;
	KG_PROCESS_ERROR(pSubsetUsed);

	memset(pSubsetUsed, 0, sizeof(BOOL) * m_dNumSubsets);

	for (int i = 0; i < m_dNumFaces; i++)
	{
		if (m_lpSubsetID[i] >= m_dNumSubsets)
		{
			//KGLogPrintf(KGLOG_ERR, "Face %d Subset ID invalidate.", i);
			continue;
		}
		pSubsetUsed[m_lpSubsetID[i]] = TRUE;
	}

	
	for (int i = 0; i < dwNumSubset; i++)
	{
		if (!pSubsetUsed[i])
		{
			for (int j = 0; j < m_dNumFaces; j++)
			{
				if (m_lpSubsetID[j] >= i)
				{
					m_lpSubsetID[j]--;
				}
			}
			for (int j = i; j < m_dNumSubsets - 1; j++)
			{
				memcpy(&m_lpMaterial[j], &m_lpMaterial[j + 1], sizeof(MATERIAL));
			}
			m_dNumSubsets--;
		}
	}

Exit0:
	SAFE_DELETE(pSubsetUsed);
}