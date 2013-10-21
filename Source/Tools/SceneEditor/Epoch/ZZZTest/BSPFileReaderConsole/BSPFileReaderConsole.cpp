// File: BSPFileReaderConsole.cpp
// Desc:

#include"..\..\Common\Common.h"
#include"..\..\Quake3Scene\BSPFile.h"
#include"..\..\Maths\Maths.h"

#include<vector>
using std::vector;

#include<iostream.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

void main()
{
	Common::ROFile rof;
	Common::WOFile wof;
	rof.SetFilename("q3dm0.bsp");
	rof.Open();
	wof.SetFilename("BSPFileReceiver.txt");
	wof.Open();

	Quake3Scene::BSPFileHeader bsp_header;
	memset(&bsp_header, 0, sizeof(bsp_header));
	rof.ReadBuffer(&bsp_header, sizeof(bsp_header));  // Read the file header
	cout<<"File magic: "<<bsp_header.m_ID<<endl;
	cout<<"File version: "<<bsp_header.m_Version<<endl<<endl;

	/*  //
	Quake3Scene::BSPFileLump TextureLump = bsp_header.m_Lumps[Quake3Scene::BFLT_Textures];
	unsigned int NumOfTextures = TextureLump.m_Length / sizeof(Quake3Scene::BSPFileTexture);
	Quake3Scene::BSPFileTexture* pTextures = new Quake3Scene::BSPFileTexture[NumOfTextures];
	rof.SetPosition(TextureLump.m_Offset);
	rof.ReadBuffer(pTextures, TextureLump.m_Length);
	*/

	unsigned int i = 0;

	Quake3Scene::BSPFileLump VertexLump = bsp_header.m_Lumps[Quake3Scene::BFLT_Vertices];
	unsigned int NumOfVertices = VertexLump.m_Length / sizeof(Quake3Scene::BSPFileVertex);

	Quake3Scene::BSPFileLump MeshVertLump = bsp_header.m_Lumps[Quake3Scene::BFLT_MeshVerts];
	unsigned int NumOfMeshVerts = MeshVertLump.m_Length / sizeof(int);
	int* pMeshVerts = new int[NumOfMeshVerts];
	rof.SetPosition(MeshVertLump.m_Offset);
	rof.ReadBuffer(pMeshVerts, MeshVertLump.m_Length);

	int MaxValueOfMeshVert = pMeshVerts[0];
	int MinValueOfMeshVert = pMeshVerts[0];
	for(i = 0; i < NumOfMeshVerts; ++i)
	{
		if(MaxValueOfMeshVert < pMeshVerts[i])
		{
			MaxValueOfMeshVert = pMeshVerts[i];
		}
		if(MinValueOfMeshVert > pMeshVerts[i])
		{
			MinValueOfMeshVert = pMeshVerts[i];
		}
	}

	if(MaxValueOfMeshVert >= NumOfMeshVerts)
	{
		cout<<"MaxValueOfMeshVert >= NumOfMeshVerts"<<endl;
	}

	cout<<"NumOfMeshVerts: "<<NumOfMeshVerts<<"; NumOfVertices: "<<NumOfVertices<<endl;
	cout<<"MaxValueOfMeshVert: "<<MaxValueOfMeshVert<<endl;
	cout<<"MinValueOfMeshVert: "<<MinValueOfMeshVert<<endl;

	bool bContinue = true;
	for(i = MinValueOfMeshVert; i <= MaxValueOfMeshVert; ++i)  // i is the continue value
	{
		for(unsigned int j = 0; j < NumOfMeshVerts; ++j)
		{
			if(i != pMeshVerts[j])
			{
				bContinue = false;
				break;
			}
		}
	}

	if(!bContinue)
	{
		cout<<"Not continue!"<<endl;
	}
	else
	{
		cout<<"Continue!"<<endl;
	}

	delete[] pMeshVerts;
	/*
	Quake3Scene::BSPFileVertex* pVertices = new Quake3Scene::BSPFileVertex[NumOfVertices];
	rof.SetPosition(VertexLump.m_Offset);
	rof.ReadBuffer(pVertices, VertexLump.m_Length);

	cout<<"NumOfVertices: "<<NumOfVertices<<endl;

	vector<Maths::Vector3F> Vectors;
	unsigned int Count = 0;
	for(i = 0; i < NumOfVertices; ++i)
	{
		Maths::Vector3F mv;
		mv.x = pVertices[i].m_Position[0];
		mv.y = pVertices[i].m_Position[1];
		mv.z = pVertices[i].m_Position[2];

		if(Vectors.empty())
		{
			Vectors.push_back(mv);
		}
		else
		{
			bool bAlreadyIn = false;
			for(unsigned int j = 0; j < Vectors.size(); ++j)
			{
				if(mv == Vectors[j])
				{
					bAlreadyIn = true;
					break;
				}
			}

			if(!bAlreadyIn)
			{
				Vectors.push_back(mv);
			}
		}
	}

	cout<<"Different vertex num: "<<Vectors.size()<<endl;

	for(i = 0; i < NumOfVertices; ++i)
	{
		Maths::Vector3F mv;
		mv.x = pVertices[i].m_Position[0];
		mv.y = pVertices[i].m_Position[1];
		mv.z = pVertices[i].m_Position[2];

		bool bFound = false;
		for(unsigned int j = 0; j < Vectors.size(); ++j)
		{
			if(mv == Vectors[j])
			{
				bFound = true;
				break;
			}
		}

		if(!bFound)
		{
			cout<<"Not found!!"<<endl;
		}
	}
	*/

	cout<<"Done!"<<endl;



	rof.Close();
	wof.Close();

	Common::WaitInputND(0xFFFFFFFF);
}