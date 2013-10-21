////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBBox.h
//  Version     : 1.0
//  Creator     : 
//  Create Date : 2008-8-14 10:41:24
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DBBOX_H_
#define _INCLUDE_KG3DBBOX_H_

////////////////////////////////////////////////////////////////////////////////
class BBox
{
public:
	D3DXVECTOR3    A;
	D3DXVECTOR3    B;
	BOOL           bInit;

	BBox();
	BBox(D3DXVECTOR3 PosA,D3DXVECTOR3 PosB);

	void operator = (const BBox& Box);
	BOOL operator == (const BBox& Box);
	BBox& operator+= (const BBox& oth);

	//FRECT ToRect();

	D3DXVECTOR3 GetCenter() const;
	float GetLength()const;
	float GetWidth()const;
	float GetHeight()const;

	void Clear();

	float GetDiameter()const;//得到直径，原来的GetRadius，明明得到的是直径

	void AddPosition(D3DXVECTOR3 Pos);
	void AddPosition(D3DXVECTOR3* Pos,int nNum);
	void AddPosition(D3DXVECTOR3 PosA,D3DXVECTOR3 PosB,D3DXMATRIX * pMatrix);

	void AddY(FLOAT Y);//光在Y方向上撑开Box
	void AddY(const BBox& Box);

	void Extract(D3DXVECTOR3* pPositions)const;

	void GetPlane(D3DXPLANE* pPlane);

    void Transform(const D3DXMATRIX& mat);
	void TransformCoord(D3DXVECTOR3* pPositions, const D3DXMATRIX * pMatrix)const;//展开8点并用Matrix变换

	void Project(D3DXVECTOR3* pPositions, D3DVIEWPORT9 *pViewport, D3DXMATRIX *pProjection,
				D3DXMATRIX *pView, D3DXMATRIX *pWorld);

	bool IsIntersectRect(D3DXVECTOR2& vA,D3DXVECTOR2& vC);
	bool IsIntersectLine2D(float a, float b, float c, D3DXVECTOR2& v1, D3DXVECTOR2& v2);
	bool IsIntersectLine2D(float A,float B,float C);
	bool IsTwoBoxIntersect(const BBox &aabb);

	void GetXZRect(D3DXVECTOR2& vA,
		D3DXVECTOR2& vC);

	bool IsPointInside(const D3DXVECTOR3& vec3Point);

	bool IsLineSectionIntersect(const D3DXVECTOR3& vec3Start,
		const D3DXVECTOR3& vec3End);

	bool IsFaceIntersect(const D3DXVECTOR3& vA, 
		const D3DXVECTOR3& vB,	const D3DXVECTOR3& vC);

	HRESULT ComputeOrthoCameraWithLight(D3DXVECTOR3& vCameraPos,D3DXVECTOR3& vCameraFront,
		float& fWidth,float& fHeight,float& fFar ,
		const D3DXVECTOR3& vLightDirction);

	BOOL IsPointInside2D(float fx,float fz);

	BOOL IsSorted();
	BOOL IsPointInsideSorted(const D3DXVECTOR3& vPoint);

	BOOL IsRayIntersect3D(D3DXVECTOR3& Inter,const D3DXVECTOR3& rStartPos,const D3DXVECTOR3& rDir,D3DXMATRIX* pMatrix = NULL,float* pfOutHitDis = NULL);//旧的代码，以后不需要用了，改用RayIntersection吧

	BOOL RayIntersection(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, FLOAT* pfDistance, const D3DXMATRIX* pMatrix = NULL);//如果有Matrix，那么相当于Box变换到那个Matrix，然后再计算。如果发射点在Box内部，那么还是会得到交点的。如果不需要这种情况，那么只要再对vSrc判断IsPointInside就行了。
	//BOOL RayIntersection2D(const D3DXVECTOR2& vSrc2D, const D3DXVECTOR2& vNormalizedDir2D, FLOAT* pfDistance);
private:
	BOOL RayIntersectionPrivate(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, FLOAT* pfDistance, BOOL bCheckZ);//如果不检查Z的话，就变成2D相交
};



typedef BBox* LPAABBOX;
typedef BBox AABBOX;

#endif //_INCLUDE_KG3DBBOX_H_
