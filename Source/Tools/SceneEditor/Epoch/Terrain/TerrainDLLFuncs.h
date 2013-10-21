// File: TerrainDLLFuncs.h
// Desc:

#ifndef TERRAINDLLFUNCS_H
#define TERRAINDLLFUNCS_H

#include"TerrainTree.h"

DLLAPI Terrain::TerrainTree* g_GetTerrainTree();
DLLAPI void g_SetTerrainTree(Terrain::TerrainTree* pTT);

#endif