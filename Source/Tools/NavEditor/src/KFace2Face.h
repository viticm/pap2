#ifndef _K_FACE2FACE_H_
#define _K_FACE2FACE_H_

#include "KApplication.h"
#include "MathTools.h"

void BOP_Face2Face(KVertex* pVertex, int nVertexCount, KTriangle* pFacesA, int nFaceCountA, KTriangle* pFacesB, int nFaceCountB);
//void BOP_sew(BOP_Mesh *mesh, BOP_Faces *facesA, BOP_Faces *facesB);
//void BOP_removeOverlappedFaces(BOP_Mesh *mesh,  BOP_Faces *facesA,  BOP_Faces *facesB);

#endif  // _K_FACE2FACE_H_
