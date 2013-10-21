#include "header.h"
#include "KG3DHelper.h"
#include "kg3dexportoption.h"
#include "asciiexp.h"


/****************************************************************************

  Global output [Scene info]
  
****************************************************************************/


// Dump some global animation information.
void AsciiExp::ExportGlobalInfo()
{
/*	Interval range = m_ip->GetAnimRange();

	struct tm *newtime;
	time_t aclock;

	time( &aclock );
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);	// The date string has a \n appended.
	today.remove(today.length()-1);		// Remove the \n

	// Start with a file identifier and format version
	//fprintf(pStream, "%s\t%d\n", ID_FILEID, VERSION);

	// Text token describing the above as a comment.
	fprintf(pStream, "%s \"%s  %1.2f - %s\"\n", ID_COMMENT, GetString(IDS_VERSIONSTRING), VERSION / 100.0f, today);

	fprintf(pStream, "%s {\n", ID_SCENE);
	fprintf(pStream, "\t%s \"%s\"\n", ID_FILENAME, FixupName(m_ip->GetCurFileName()));
	fprintf(pStream, "\t%s %d\n", ID_FIRSTFRAME, range.Start() / GetTicksPerFrame());
	fprintf(pStream, "\t%s %d\n", ID_LASTFRAME, range.End() / GetTicksPerFrame());
	fprintf(pStream, "\t%s %d\n", ID_FRAMESPEED, GetFrameRate());
	fprintf(pStream, "\t%s %d\n", ID_TICKSPERFRAME, GetTicksPerFrame());
	
	Texmap* env = m_ip->GetEnvironmentMap();

	Control* ambLightCont;
	Control* bgColCont;
	
	if (env) {
		// Output environment texture map
		DumpTexture(env, Class_ID(0,0), 0, 1.0f, 0);
	}
	else {
		// Output background color
		bgColCont = m_ip->GetBackGroundController();
		if (bgColCont && bgColCont->IsAnimated()) {
			// background color is animated.
			fprintf(pStream, "\t%s {\n", ID_ANIMBGCOLOR);
			
			DumpPoint3Keys(bgColCont, 0);
				
			fprintf(pStream, "\t}\n");
		}
		else {
			// Background color is not animated
			Color bgCol = m_ip->GetBackGround(GetStaticFrame(), FOREVER);
			fprintf(pStream, "\t%s %s\n", ID_STATICBGCOLOR, Format(bgCol));
		}
	}
	
	// Output ambient light
	ambLightCont = m_ip->GetAmbientController();
	if (ambLightCont && ambLightCont->IsAnimated()) {
		// Ambient light is animated.
		fprintf(pStream, "\t%s {\n", ID_ANIMAMBIENT);
		
		DumpPoint3Keys(ambLightCont, 0);
		
		fprintf(pStream, "\t}\n");
	}
	else {
		// Ambient light is not animated
		Color ambLight = m_ip->GetAmbient(GetStaticFrame(), FOREVER);
		fprintf(pStream, "\t%s %s\n", ID_STATICAMBIENT, Format(ambLight));
	}

	fprintf(pStream,"}\n");*/
}

/****************************************************************************

  GeomObject output
  
****************************************************************************/

HRESULT AsciiExp::ExportGeomObject(INode* node, int indentLevel)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    LPMESH pMesh = NULL;

	ObjectState os = node->EvalWorldState(GetStaticFrame());
    KGLOG_PROCESS_ERROR(os.obj);
	
	// Targets are actually geomobjects, but we will export them
	// from the camera and light objects, so we skip them here.
	KGLOG_PROCESS_ERROR(os.obj->ClassID() != Class_ID(TARGET_CLASS_ID, 0));

    //else if (os.obj->ClassID() == PArray_CLASS_ID)
	//{
	//	ExchangeParticleArrayData(&m_cMeshTable,node,GetStaticFrame());
	//	return;
	//}
	//else
	{
        hrRetCode = m_cMeshTable.Get1NewMesh(&pMesh);
        KG_COM_PROC_ERR_RET_ERR(hrRetCode);
	
		hrRetCode = ExportMesh(node, pMesh,GetStaticFrame(), indentLevel);
        KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
		
		if (GetIncludeMtl()) 
		{
			ExportMaterial(node,pMesh,indentLevel);
		}

		if(GetSaveAnimation())
		{
			if (GetAnimationType() == ANIMATION_VERTICES_RELATIVE)
			{
				ExportAnimMesh(node,pMesh,indentLevel);
			}
			else if (GetAnimationType() == ANIMATION_VERTICES)
			{
				ExportAnimMesh(node,pMesh,indentLevel);
			}
			else if (GetAnimationType() == ANIMATION_BONE)
			{
				ExportSkinInfo(node,pMesh);
				ExportBoneAnimation(node,pMesh);
			}
			else if (GetAnimationType() == ANIMATION_BONE_RELATIVE)
			{
				ExportSkinInfo(node,pMesh);
				ExportRelateBoneAnimation(node,pMesh);
			}
			else if ( GetAnimationType() == ANIMATION_BONE_RTS)
			{
				hrRetCode = ExportSkinInfo1(node, pMesh);
                KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
				if (hrRetCode != S_FALSE)
				{
					ExportRTSBoneAnimation(node, pMesh);
				}
			}
			else if ( GetAnimationType() == ANIMATION_BONE_KEYONLY )
			{
				ExportSkinInfo(node, pMesh);
				ExportRelateBoneAnimationKeyOnly(node, pMesh);
			}
		}
	}

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_DELETE(pMesh);
        m_cMeshTable.m_dNumMesh--;
        m_cMeshTable.m_lpMeshes[m_cMeshTable.m_dNumMesh] = NULL;
    }

    return hrResult;
}

/****************************************************************************

  Shape output
  
****************************************************************************/

void AsciiExp::ExportShapeObject(INode* node, int indentLevel)
{
/*	ExportNodeHeader(node, ID_SHAPE, indentLevel);
	ExportNodeTM(node, indentLevel);
	TimeValue t = GetStaticFrame();
	Matrix3 tm = node->GetObjTMAfterWSM(t);

	TSTR indent = GetIndent(indentLevel);
	
	ObjectState os = node->EvalWorldState(t);
	if (!os.obj || os.obj->SuperClassID()!=SHAPE_CLASS_ID) {
		fprintf(pStream,"%s}\n", indent.data());
		return;
	}
	
	ShapeObject* shape = (ShapeObject*)os.obj;
	PolyShape pShape;
	int numLines;

	// We will output ahspes as a collection of polylines.
	// Each polyline contains collection of line segments.
	shape->MakePolyShape(t, pShape);
	numLines = pShape.numLines;
	
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_SHAPE_LINECOUNT, numLines);
	
	for(int poly = 0; poly < numLines; poly++) {
		fprintf(pStream,"%s\t%s %d {\n", indent.data(), ID_SHAPE_LINE, poly);
		DumpPoly(&pShape.lines[poly], tm, indentLevel);
		fprintf(pStream, "%s\t}\n", indent.data());
	}
	
	if (GetIncludeAnim()) {
		ExportAnimKeys(node, indentLevel);
	}
	
	fprintf(pStream,"%s}\n", indent.data());*/
}

void AsciiExp::DumpPoly(PolyLine* line, Matrix3 tm, int indentLevel)
{
/*	int numVerts = line->numPts;
	
	TSTR indent = GetIndent(indentLevel);
	
	if(line->IsClosed()) {
		fprintf(pStream,"%s\t\t%s\n", indent.data(), ID_SHAPE_CLOSED);
	}
	
	fprintf(pStream,"%s\t\t%s %d\n", indent.data(), ID_SHAPE_VERTEXCOUNT, numVerts);
	
	// We differ between true and interpolated knots
	for (int i=0; i<numVerts; i++) {
		PolyPt* pt = &line->pts[i];
		if (pt->flags & POLYPT_KNOT) {
			fprintf(pStream,"%s\t\t%s\t%d\t%s\n", indent.data(), ID_SHAPE_VERTEX_KNOT, i,
				Format(tm * pt->p));
		}
		else {
			fprintf(pStream,"%s\t\t%s\t%d\t%s\n", indent.data(), ID_SHAPE_VERTEX_INTERP,
				i, Format(tm * pt->p));
		}
		
	}*/
}

/****************************************************************************

  Light output
  
****************************************************************************/

void AsciiExp::ExportLightObject(INode* node, int indentLevel)
{
/*	TimeValue t = GetStaticFrame();
	TSTR indent = GetIndent(indentLevel);

	ExportNodeHeader(node, ID_LIGHT, indentLevel);
	
	ObjectState os = node->EvalWorldState(t);
	if (!os.obj) {
		fprintf(pStream, "%s}\n", indent.data());
		return;
	}
	
	GenLight* light = (GenLight*)os.obj;
	struct LightState ls;
	Interval valid = FOREVER;
	Interval animRange = m_ip->GetAnimRange();

	light->EvalLightState(t, valid, &ls);

	// This is part os the lightState, but it doesn't
	// make sense to output as an animated setting so
	// we dump it outside of ExportLightSettings()

	fprintf(pStream, "%s\t%s ", indent.data(), ID_LIGHT_TYPE);
	switch(ls.type) {
	case OMNI_LIGHT:  fprintf(pStream, "%s\n", ID_LIGHT_TYPE_OMNI); break;
	case TSPOT_LIGHT: fprintf(pStream, "%s\n", ID_LIGHT_TYPE_TARG);  break;
	case DIR_LIGHT:   fprintf(pStream, "%s\n", ID_LIGHT_TYPE_DIR); break;
	case FSPOT_LIGHT: fprintf(pStream, "%s\n", ID_LIGHT_TYPE_FREE); break;
	}

	ExportNodeTM(node, indentLevel);
	// If we have a target object, export Node TM for the target too.
	INode* target = node->GetTarget();
	if (target) {
		ExportNodeTM(target, indentLevel);
	}

	int shadowMethod = light->GetShadowMethod();
	fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_LIGHT_SHADOWS,
			shadowMethod == LIGHTSHADOW_NONE ? ID_LIGHT_SHAD_OFF :
			shadowMethod == LIGHTSHADOW_MAPPED ? ID_LIGHT_SHAD_MAP :
			ID_LIGHT_SHAD_RAY);

	
	fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_LIGHT_USELIGHT, Format(light->GetUseLight()));
	
	fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_LIGHT_SPOTSHAPE, 
		light->GetSpotShape() == RECT_LIGHT ? ID_LIGHT_SHAPE_RECT : ID_LIGHT_SHAPE_CIRC);

	fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_LIGHT_USEGLOBAL, Format(light->GetUseGlobal()));
	fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_LIGHT_ABSMAPBIAS, Format(light->GetAbsMapBias()));
	fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_LIGHT_OVERSHOOT, Format(light->GetOvershoot()));

	ExclList* el = light->GetExclList();  // DS 8/31/00 . switched to NodeIDTab from NameTab
	if (el->Count()) {
		fprintf(pStream,"%s\t%s {\n", indent.data(), ID_LIGHT_EXCLUSIONLIST);
		fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_NUMEXCLUDED, Format(el->Count()));
		fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_EXCLINCLUDE, Format(el->TestFlag(NT_INCLUDE)));
		fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_EXCL_AFFECT_ILLUM, Format(el->TestFlag(NT_AFFECT_ILLUM)));
		fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_EXCL_AFFECT_SHAD, Format(el->TestFlag(NT_AFFECT_SHADOWCAST)));
		for (int nameid = 0; nameid < el->Count(); nameid++) {
			INode *n = (*el)[nameid];	// DS 8/31/00
			if (n)
				fprintf(pStream,"%s\t\t%s \"%s\"\n", indent.data(), ID_LIGHT_EXCLUDED, n->GetName());
			}
		fprintf(pStream,"%s\t}\n", indent.data());
	}

	// Export light settings for frame 0
	ExportLightSettings(&ls, light, t, indentLevel);

	// Export animated light settings
	if (!valid.InInterval(animRange) && GetIncludeCamLightAnim()) {
		fprintf(pStream,"%s\t%s {\n", indent.data(), ID_LIGHT_ANIMATION);

		TimeValue t = animRange.Start();
		
		while (1) {
			valid = FOREVER; // Extend the validity interval so the camera can shrink it.
			light->EvalLightState(t, valid, &ls);

			t = valid.Start() < animRange.Start() ? animRange.Start() : valid.Start();
			
			// Export the light settings at this frame
			ExportLightSettings(&ls, light, t, indentLevel+1);
			
			if (valid.End() >= animRange.End()) {
				break;
			}
			else {
				t = (valid.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
		}

		fprintf(pStream,"%s\t}\n", indent.data());
	}

	// Export animation keys for the light node
	if (GetIncludeAnim()) {
		ExportAnimKeys(node, indentLevel);
		
		// If we have a target, export animation keys for the target too.
		if (target) {
			ExportAnimKeys(target, indentLevel);
		}
	}
	
	fprintf(pStream,"%s}\n", indent.data());*/
}

void AsciiExp::ExportLightSettings(LightState* ls, GenLight* light, TimeValue t, int indentLevel)
{
/*	TSTR indent = GetIndent(indentLevel);

	fprintf(pStream,"%s\t%s {\n", indent.data(), ID_LIGHT_SETTINGS);

	// Frame #
	fprintf(pStream, "%s\t\t%s %d\n",indent.data(), ID_TIMEVALUE, t);

	fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_COLOR, Format(ls->color));
	fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_INTENS, Format(ls->intens));
	fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_ASPECT, Format(ls->aspect));
	
	if (ls->type != OMNI_LIGHT) {
		fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_HOTSPOT, Format(ls->hotsize));
		fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_FALLOFF, Format(ls->fallsize));
	}
	if (ls->type != DIR_LIGHT && ls->useAtten) {
		fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_ATTNSTART, Format(ls->attenStart));
		fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_ATTNEND,	Format(ls->attenEnd));
	}

	fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_TDIST, Format(light->GetTDist(t, FOREVER)));
	fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_MAPBIAS, Format(light->GetMapBias(t, FOREVER)));
	fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_MAPRANGE, Format(light->GetMapRange(t, FOREVER)));
	fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_MAPSIZE, Format(light->GetMapSize(t, FOREVER)));
	fprintf(pStream,"%s\t\t%s %s\n", indent.data(), ID_LIGHT_RAYBIAS, Format(light->GetRayBias(t, FOREVER)));

	fprintf(pStream,"%s\t}\n", indent.data());*/
}


/****************************************************************************

  Camera output
  
****************************************************************************/

void AsciiExp::ExportCameraObject(INode* node, int indentLevel)
{
/*	TSTR indent = GetIndent(indentLevel);

	ExportNodeHeader(node, ID_CAMERA, indentLevel);

	INode* target = node->GetTarget();
	if (target) {
		fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_CAMERA_TYPE, ID_CAMERATYPE_TARGET);
	}
	else {
		fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_CAMERA_TYPE, ID_CAMERATYPE_FREE);
	}


	ExportNodeTM(node, indentLevel);
	// If we have a target object, export animation keys for the target too.
	if (target) {
		ExportNodeTM(target, indentLevel);
	}
	
	CameraState cs;
	TimeValue t = GetStaticFrame();
	Interval valid = FOREVER;
	// Get animation range
	Interval animRange = m_ip->GetAnimRange();
	
	ObjectState os = node->EvalWorldState(t);
	CameraObject *cam = (CameraObject *)os.obj;
	
	cam->EvalCameraState(t,valid,&cs);
	
	ExportCameraSettings(&cs, cam, t, indentLevel);

	// Export animated camera settings
	if (!valid.InInterval(animRange) && GetIncludeCamLightAnim()) {

		fprintf(pStream,"%s\t%s {\n", indent.data(), ID_CAMERA_ANIMATION);

		TimeValue t = animRange.Start();
		
		while (1) {
			valid = FOREVER; // Extend the validity interval so the camera can shrink it.
			cam->EvalCameraState(t,valid,&cs);

			t = valid.Start() < animRange.Start() ? animRange.Start() : valid.Start();
			
			// Export the camera settings at this frame
			ExportCameraSettings(&cs, cam, t, indentLevel+1);
			
			if (valid.End() >= animRange.End()) {
				break;
			}
			else {
				t = (valid.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
		}

		fprintf(pStream,"%s\t}\n", indent.data());
	}
	
	// Export animation keys for the light node
	if (GetIncludeAnim()) {
		ExportAnimKeys(node, indentLevel);
		
		// If we have a target, export animation keys for the target too.
		if (target) {
			ExportAnimKeys(target, indentLevel);
		}
	}
	
	fprintf(pStream,"%s}\n", indent.data());*/
}

void AsciiExp::ExportCameraSettings(CameraState* cs, CameraObject* cam, TimeValue t, int indentLevel)
{

}


/****************************************************************************

  Helper object output
  
****************************************************************************/

void AsciiExp::ExportHelperObject(INode* node, int indentLevel)
{
}


/****************************************************************************

  Node Header
  
****************************************************************************/

// The Node Header consists of node type (geometry, helper, camera etc.)
// node name and parent node
void AsciiExp::ExportNodeHeader(INode* node, TCHAR* type, int indentLevel)
{
}


/****************************************************************************

  Node Transformation
  
****************************************************************************/

void AsciiExp::ExportNodeTM(INode* node, int indentLevel)
{
	Matrix3 pivot = node->GetNodeTM(GetStaticFrame());
	TSTR indent = GetIndent(indentLevel);
	

	// Export TM inheritance flags
	DWORD iFlags = node->GetTMController()->GetInheritanceFlags();

	// Dump the full matrix
	DumpMatrix3(&pivot, indentLevel+2);
	
}

/****************************************************************************

  Animation output
  
****************************************************************************/

// If the object is animated, then we will output the entire mesh definition
// for every specified frame. This can result in, dare I say, rather large files.
//
// Many target systems (including MAX itself!) cannot always read back this
// information. If the objects maintains the same number of verices it can be
// imported as a morph target, but if the number of vertices are animated it
// could not be read back in withou special tricks.
// Since the target system for this exporter is unknown, it is up to the
// user (or developer) to make sure that the data conforms with the target system.

void AsciiExp::ExportAnimMesh(INode* node,LPMESH pMesh, int indentLevel)
{
	DWORD dwNumFrame = 0;
	BOOL needDel = FALSE;
	DWORD NowFrame = 0;
	int vx1, vx2, vx3;
	int k = 0;
	Interval animRange, objRange;
	TimeValue StartTime;
	TimeValue Time;
	Point3*  pAnimationVerticesPosition = NULL;
	Point3*  pAnimationVerticesNormal = NULL;
	Point3*  pAnimationVerticesUV = NULL;
	Point3*  SrcAnimationVerticesPosition = NULL;
	DWORD*   SrcAnimationVerticesIndex = NULL;
	TriObject* pTriObj = NULL;
	Mesh* pCurMesh = NULL;
	Point3 Zero = Point3(0.0f, 0.0f, 0.0f);
	float fUnitScale = 1.0f;
	float fSwapTemp = 0.0f;
	DWORD OldVerticeIndex = 0;
	const static DWORD dwAniUVChannel = 2;
	DWORD dwNumFaces = 0;
	ObjectState os = node->EvalWorldState(GetStaticFrame());
	Matrix3 tmWSM;
	BOOL negScale = FALSE;
	
	if (!os.obj)
		goto Exit0;
	// Get animation range
	animRange = m_ip->GetAnimRange();
	// Get validity of the object
	objRange = os.obj->ObjectValidity(GetStaticFrame());
	if(objRange == FOREVER)
		objRange = animRange;
		
	if(objRange.Start() > animRange.End() || objRange.End() < animRange.Start())
	{
		MessageBox(m_ip->GetMAXHWnd(), "物体在导出动画范围内无效", "错误", MB_OK);
		goto Exit0;
	}
	
	//申请内存
	dwNumFrame = (animRange.End() - animRange.Start()) / GetTicksPerFrame() + 1;
	pAnimationVerticesPosition = new Point3[pMesh->m_dNumVertices * dwNumFrame];
	assert(pAnimationVerticesPosition);
	ZeroMemory(pAnimationVerticesPosition, sizeof(Point3) * pMesh->m_dNumVertices * dwNumFrame);
	StartTime = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
	Time = StartTime;
	m_bInch2CM ? fUnitScale = 2.54f : fUnitScale = 1.0f;

	if(bIncludeUVAni)
	{
		SAFE_DELETE_ARRAY(pAnimationVerticesUV);
		pAnimationVerticesUV = new Point3[pMesh->m_dNumVertices * dwNumFrame];
		assert(pAnimationVerticesUV);
		ZeroMemory(pAnimationVerticesUV, sizeof(Point3) * pMesh->m_dNumVertices * dwNumFrame);
	}

		
	for(DWORD dwFrameIndex = 0; dwFrameIndex < dwNumFrame; dwFrameIndex++)
	{
		tmWSM = node->GetObjTMAfterWSM(Time);
		negScale = TMNegParity(tmWSM);
		pTriObj = GetTriObjectFromNode(node, Time, needDel);
		if (!pTriObj) 
		{
			MessageBox(m_ip->GetMAXHWnd(), "模型无法转换为Mesh, 无法导出", "错误", MB_OK);
			goto Exit0;
		}
		pCurMesh = &pTriObj->GetMesh();
		pCurMesh->buildNormals();

		// Order of vertex. Get 'em counter clockwise if the objects is
		// negatively scaled.
		if (negScale)
		{
			vx1 = 2;
			vx2 = 1;
			vx3 = 0;
		}
		else
		{
			vx1 = 0;
			vx2 = 1;
			vx3 = 2;
		}		
		// Export the mesh definition at this frame
		
		//Export vertex position
		for(DWORD dwVertexIndex = 0; dwVertexIndex < pMesh->m_dNumVertices; dwVertexIndex++)
		{
			OldVerticeIndex = pMesh->m_lpVerticeRemap[dwVertexIndex];
			Point3 v = tmWSM * pCurMesh->verts[OldVerticeIndex];
			if(GetExchangeYZ())
			{
				fSwapTemp = v.z;
				v.z = v.y;
				v.y = fSwapTemp;
			}
			v *= fUnitScale;
			pAnimationVerticesPosition[dwVertexIndex * dwNumFrame + dwFrameIndex] = v;
		}

		//Export vertex UV animation
		if(bIncludeUVAni)
		{
#ifdef _DEBUG
			std::set<DWORD> setVertex;
			std::set<DWORD> setVertex1;
#endif

			
			dwNumFaces = pCurMesh->getNumFaces();
			for(DWORD dwFaceIndex = 0; dwFaceIndex < dwNumFaces; dwFaceIndex++)
			{
				if(pCurMesh->tvFace)
				{
					DWORD A = pCurMesh->tvFace[dwFaceIndex].t[vx1];
					DWORD B = pCurMesh->tvFace[dwFaceIndex].t[vx2];
					DWORD C = pCurMesh->tvFace[dwFaceIndex].t[vx3];

					UVVert tvA = Zero;
					UVVert tvB = Zero;
					UVVert tvC = Zero;

#ifdef _DEBUG
					setVertex.insert(A);
					setVertex.insert(B);
					setVertex.insert(C);
#endif

					if(pCurMesh->tVerts!=NULL)
					{
						tvA = pCurMesh->tVerts[A];
						tvB = pCurMesh->tVerts[B];
						tvC = pCurMesh->tVerts[C];
					}
					else 
					{
						Point3 tv[3];
						Face* f = &pCurMesh->faces[dwFaceIndex];
						make_face_uv(f, tv);

						tvA = tv[0];
						tvB = tv[1];
						tvC = tv[2];
					}
					DWORD A1 = pMesh->m_lpFacesIndex[dwFaceIndex * 3];
					DWORD B1 = pMesh->m_lpFacesIndex[dwFaceIndex * 3 + 1];
					DWORD C1 = pMesh->m_lpFacesIndex[dwFaceIndex * 3 + 2];

#ifdef _DEBUG
					setVertex1.insert(A1);
					setVertex1.insert(B1);
					setVertex1.insert(C1);
#endif

					pAnimationVerticesUV[A1 * dwNumFrame + dwFrameIndex].x = tvA.x;
					pAnimationVerticesUV[A1 * dwNumFrame + dwFrameIndex].y = 1 - tvA.y;
					pAnimationVerticesUV[A1 * dwNumFrame + dwFrameIndex].z = tvA.z;
					
					pAnimationVerticesUV[B1 * dwNumFrame + dwFrameIndex].x = tvB.x;
					pAnimationVerticesUV[B1 * dwNumFrame + dwFrameIndex].y = 1 - tvB.y;
					pAnimationVerticesUV[B1 * dwNumFrame + dwFrameIndex].z = tvB.z;

					pAnimationVerticesUV[C1 * dwNumFrame + dwFrameIndex].x = tvC.x;
					pAnimationVerticesUV[C1 * dwNumFrame + dwFrameIndex].y = 1 - tvC.y;
					pAnimationVerticesUV[C1 * dwNumFrame + dwFrameIndex].z = tvC.z;
				}
			}
			//sort(setVertex.begin(), setVertex.end());
#ifdef _DEBUG
			size_t VertexInside = setVertex.size();
			size_t VertexInside1 = setVertex1.size();
			if(VertexInside1 != pMesh->m_dNumVertices)
			{
				int nDebug = 0;
			}
			setVertex.clear();
			setVertex1.clear();
#endif
		}
		
		if(Time >= animRange.End())
		{
			break;
		}
		else 
		{
			Time += GetTicksPerFrame();
		}
		if (needDel) 
		{
			delete pTriObj;
		}
	}
	
	//把结果保存到MESH中
	pMesh->m_dNumAnimationVertices = pMesh->m_dNumVertices;
	
	pMesh->m_dNumFrames = dwNumFrame;
	pMesh->m_fFrameLength = GetTicksPerFrame();
	SrcAnimationVerticesPosition = new Point3[pMesh->m_dNumVertices * dwNumFrame];
	ZeroMemory(SrcAnimationVerticesPosition ,sizeof(Point3)*pMesh->m_dNumVertices * dwNumFrame);
	SrcAnimationVerticesIndex = new DWORD[pMesh->m_dNumVertices];
	ZeroMemory(SrcAnimationVerticesIndex ,sizeof(DWORD)*pMesh->m_dNumVertices);
	
	DWORD K = 0;
	for(DWORD dwVertexIndex = 0; dwVertexIndex < pMesh->m_dNumVertices; dwVertexIndex++)
	{
		SrcAnimationVerticesIndex[K] = dwVertexIndex;
		for(DWORD n = 0; n < dwNumFrame; n++)
		{
			Point3& v = pAnimationVerticesPosition[dwVertexIndex * dwNumFrame + n];
			SrcAnimationVerticesPosition[K * dwNumFrame + n] = v;
		}
		K++;
	}
	pMesh->m_lpAnimationVerticesPosition = (CMesh::LPVEC3) SrcAnimationVerticesPosition;
	pMesh->m_lpAnimationUV = (CMesh::LPVEC3) pAnimationVerticesUV;
	pMesh->m_lpAnimationVerticesIndex = SrcAnimationVerticesIndex;
	
	//释放内存
	delete [] pAnimationVerticesPosition;
	delete [] pAnimationVerticesNormal;
	//delete [] pAnimationVerticesUV;
 
	pMesh->m_dwAnimationType = ANIMATION_VERTICES;

Exit0:
	return;
}


// ================================================== FindPhysiqueModifier()
// Find if a given node contains a Physique Modifier
// DerivedObjectPtr requires you include "modstack.h" from the MAX SDK
Modifier* FindPhysiqueModifier (INode* nodePtr)
{
    // Get object from node. Abort if no object.
    Object* ObjectPtr = nodePtr->GetObjectRef();
  
	if ( NULL == ObjectPtr)
    {
        return NULL;
    }

    // Is derived object ?
    if (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        // Yes -> Cast.
        IDerivedObject* DerivedObjectPtr = static_cast<IDerivedObject*>(ObjectPtr);

        // Iterate over all entries of the modifier stack.
        int ModStackIndex = 0;

        while (ModStackIndex < DerivedObjectPtr->NumModifiers())
        {
            // Get current modifier.
            Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);
           					
            // Is this Physique ?
            if (ModifierPtr->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
            {
                // Yes -> Exit.
               return ModifierPtr;
            }

            // Next modifier stack entry.
            ModStackIndex++;
        }
    }

    // Not found.
    return NULL;
}

// ================================================== FindPhysiqueModifier()
// Find if a given node contains a Physique Modifier
// DerivedObjectPtr requires you include "modstack.h" from the MAX SDK
Modifier* FindSkinModifier(INode* nodePtr)
{
    // Get object from node. Abort if no object.
    Object* ObjectPtr = nodePtr->GetObjectRef();
  
	if ( NULL == ObjectPtr)
    {
        return NULL;
    }

    // Is derived object ?
    if (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        // Yes -> Cast.
        IDerivedObject* DerivedObjectPtr = static_cast<IDerivedObject*>(ObjectPtr);

        // Iterate over all entries of the modifier stack.
        int ModStackIndex = 0;

        while (ModStackIndex < DerivedObjectPtr->NumModifiers())
        {
            // Get current modifier.
            Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);
            Class_ID clsid = ModifierPtr->ClassID();
						
            // Is this Skin ?
            if (ModifierPtr->ClassID() == SKIN_CLASSID)
            {
                // Yes -> Exit.
               return ModifierPtr;
            }

            // Next modifier stack entry.
            ModStackIndex++;
        }
    }

    // Not found.
    return NULL;
}

DWORD  FindBone(INode** ppBone,INode* pObjectBone,DWORD Max)
{
	for(DWORD i =0;i<Max;i++)
	{
		if(ppBone[i]==pObjectBone)
			return i;
	}
	return -1;
}


INode* AsciiExp::FindRootBone(INode* pNode)
{
	while( pNode != m_ip->GetRootNode() )
	{
		pNode = pNode->GetParentNode();
	}
	return pNode;
}

/****************************************************************************

  Skin Mesh information output
//说明：这个函数在修改器堆栈中查找 Physique，如果成功就收集皮肤的信息，并保存到CMesh
//对象中
//这个函数要求在导出MESH成功之后使用
****************************************************************************/

void AsciiExp::ExportSkinInfo(INode* node, LPMESH pMesh)
{
    HRESULT hr = S_OK;
	IPhyVertexExport *pVertexExport;
    IPhyRigidVertex* pRigidVertexExport;
    IPhyBlendedRigidVertex *pBlendedRigidVertexExport;
    INode* pBoneNode;
    Modifier* pPhyMod = NULL;
	IPhysiqueExport* pPhyExport = NULL;
    IPhyContextExport* pPhyContextExport = NULL;
	Modifier*		  pSkin = NULL;
	ISkin*			  pSkinInterface = NULL;
	ISkinContextData* pSkinContex = NULL;
    DWORD iVertex;
    DWORD cVertices;
    DWORD iVertexType;
    DWORD cTotalBones;
    DWORD iBone;
	BOOL  Skined = FALSE;

	BOOL needDel;
	TriObject* tri = GetTriObjectFromNode(node, 0, needDel);
	if (!tri) {
		return;
	}
	Mesh* mesh = &tri->GetMesh();
	//顶点数目（原汁原味的）
    DWORD NumVertex = mesh->getNumVerts();

	DWORD   NumBones = 0;
	//一般BIP包含51个骨骼，用200个怎么说也应该足够了吧
	INode** lpBoneNodes = new INode*[200];
	ZeroMemory(lpBoneNodes ,sizeof(INode*)*200);
	//记录每个骨骼影响了多少顶点
	DWORD*  lpBoneRef = new DWORD[200];
	ZeroMemory(lpBoneRef ,sizeof(DWORD)*200);
	//记录影响每个顶点的骨骼数目和和骨骼NODE，以及权重

	CMesh::LPVERTICESSKININFO lpVerSkinInfo = new CMesh::VERTICESSKININFO[NumVertex];
	ZeroMemory(lpVerSkinInfo ,sizeof(CMesh::VERTICESSKININFO)*NumVertex);
	//20个骨骼同时影响一个顶点怎么也够了
	for(DWORD i=0;i<NumVertex;i++)
	{
		lpVerSkinInfo[i].BoneName = new LPSTR[20];
		ZeroMemory(lpVerSkinInfo[i].BoneName,sizeof(LPSTR)*20);
		lpVerSkinInfo[i].Weight = new float[20];
		ZeroMemory(lpVerSkinInfo[i].Weight,sizeof(float)*20);
		lpVerSkinInfo[i].NumBoneRef = 0;
	}
	// first see if physique is present
    pPhyMod = FindPhysiqueModifier(node);
	//for debug
	//FILE * F = fopen("debug.txt","at");
    if (pPhyMod != NULL)
    {
		// Get a Physique Export interface
		pPhyExport = (IPhysiqueExport *)pPhyMod->GetInterface(I_PHYINTERFACE);
		if (pPhyExport == NULL)
		{   // not all interfaces present, so ignore
			return ;
		}
		// For a given Object's INode get a
		// ModContext Interface from the Physique Export Interface:
		pPhyContextExport = (IPhyContextExport *)pPhyExport->GetContextInterface(node);
		if(pPhyContextExport == NULL)
		{
			// not all interfaces present, so ignore
			return;
		}
		Skined = TRUE;
		// convert to rigid with blending
		pPhyContextExport->ConvertToRigid(TRUE);
		pPhyContextExport->AllowBlending(TRUE);
 		
		cVertices = pPhyContextExport->GetNumberVertices();
		
		for (iVertex = 0; iVertex < cVertices; iVertex++ )
		{
			pVertexExport = (IPhyVertexExport *)pPhyContextExport->GetVertexInterface(iVertex);    
			if (pVertexExport == NULL)
			{
				OutputDebugString("Couldn't get export interface!");
				hr = E_FAIL;
				return ;
			}
            
			// What kind of vertices are these?
			iVertexType = pVertexExport->GetVertexType();
			pPhyContextExport->ReleaseVertexInterface( pVertexExport );    
			CMesh::LPVERTICESSKININFO pInfo = &lpVerSkinInfo[iVertex];
			
			if( iVertexType == RIGID_TYPE )
			{
				pRigidVertexExport = (IPhyRigidVertex *)pPhyContextExport->GetVertexInterface(iVertex);
				if (pRigidVertexExport == NULL)
				{
					OutputDebugString("Couldn't get rigid vertex export interface!");
					hr = E_FAIL;
					return ;
				}
				// Get the vertex info!               
                pBoneNode = pRigidVertexExport->GetNode();

				//把检查到的骨骼添加到列表中
				AddBone(pBoneNode,lpBoneNodes,NumBones);

				pInfo->NumBoneRef = 1;
				pInfo->BoneName[0] = (LPSTR) pBoneNode;
				pInfo->Weight[0]  = 1.0f;

				pPhyContextExport->ReleaseVertexInterface( pRigidVertexExport);
			}
			else
			{
				//混合连接形式的顶点，所有骨骼连接的权和等于1
				assert( iVertexType == RIGID_BLENDED_TYPE );

				pBlendedRigidVertexExport = (IPhyBlendedRigidVertex *)pPhyContextExport->GetVertexInterface(iVertex);
				if (pBlendedRigidVertexExport == NULL)
				{
					OutputDebugString("Couldn't get blended rigid vertex export interface!");
					hr = E_FAIL;
					return ;
				}

				// How many nodes affect his vertex?
				cTotalBones = pBlendedRigidVertexExport->GetNumberNodes();

				for (iBone = 0; iBone < cTotalBones; iBone++ )
				{
					pBoneNode = pBlendedRigidVertexExport->GetNode(iBone);
					//把检查到的骨骼添加到列表中
					AddBone(pBoneNode,lpBoneNodes,NumBones);
					
                    float fWeight = pBlendedRigidVertexExport->GetWeight(iBone);
					
					//去掉重复的骨骼和权中为0的
					LPSTR pStr = (LPSTR) pBoneNode;
					for(DWORD KKK = 0;KKK<pInfo->NumBoneRef;KKK++)
					{
						if(pStr==pInfo->BoneName[KKK])
						{
							pInfo->Weight[KKK] += fWeight;
							break;
						}
					}
					if((KKK>=pInfo->NumBoneRef)&&(fWeight>0))
					{
						pInfo->BoneName[pInfo->NumBoneRef] = pStr;
						pInfo->Weight[pInfo->NumBoneRef]  = fWeight;
						pInfo->NumBoneRef++;
					}
				}
				// actualNum... accounts for duplicated weights to same transform node
				// that are combined automatically above
				pPhyContextExport->ReleaseVertexInterface( pBlendedRigidVertexExport);
			}
        }		
	}
	//////////////////////////////////////////////////////////////////////////
	//skin export
	// first see if skin is present
	pSkin = FindSkinModifier(node);
	//for debug
	
	if (pSkin != NULL)
	{
		// Get a Physique Export interface
		pSkinInterface = (ISkin*)pSkin->GetInterface(I_SKIN);
		if (pSkinInterface == NULL)
		{   // not all interfaces present, so ignore
			return ;
		}
		// For a given Object's INode get a
		// ModContext Interface from the Physique Export Interface:
		
		pSkinContex = (ISkinContextData *)pSkinInterface->GetContextInterface(node);
		if(pSkinContex == NULL)
		{
			// not all interfaces present, so ignore
			return;
		}
		Skined = TRUE;
		cVertices = pSkinContex->GetNumPoints();
		
		for (iVertex = 0; iVertex < cVertices; iVertex++ )
		{
			CMesh::LPVERTICESSKININFO pInfo = &lpVerSkinInfo[iVertex];
			
			int nBone = pSkinContex->GetNumAssignedBones(iVertex);
			for(iBone=0; iBone<nBone; iBone++)
			{
				int BoneIndex    = pSkinContex->GetAssignedBone(iVertex,iBone);
				pBoneNode  = pSkinInterface->GetBone(BoneIndex);
				float BoneWeight = pSkinContex->GetBoneWeight(iVertex,iBone);
				
				//把检查到的骨骼添加到列表中
				AddBone(pBoneNode, lpBoneNodes, NumBones);
				
				//去掉重复的骨骼和权中为0的
				LPSTR pStr = (LPSTR) pBoneNode;
				for(DWORD KKK = 0;KKK<pInfo->NumBoneRef;KKK++)
				{
					if(pStr == pInfo->BoneName[KKK])
					{
						pInfo->Weight[KKK] += BoneWeight;
						break;
					}
				}
				if((KKK>=pInfo->NumBoneRef)&&(BoneWeight>0))
				{
					pInfo->BoneName[pInfo->NumBoneRef] = pStr;
					pInfo->Weight[pInfo->NumBoneRef]  = BoneWeight;
					pInfo->NumBoneRef++;
				}
			}
		}
	}
 
	if(Skined)
	{
		//if (GetAnimationType() == ANIMATION_BONE_RELATIVE || GetAnimationType() == ANIMATION_BONE_RTS || GetAnimationType() == ANIMATION_BONE_KEYONLY)
		//BOOL bForearm = FALSE;
		FindAllBones(pSkinInterface, lpBoneNodes, NumBones);
		AdjustSkeleton(lpBoneNodes, pSkinInterface, !m_bForearm);
		//Here is some fix for the forearm modification
		//As the there might be a need of break the forearm into two part, foretwist and foretwist1, 
		//and the latter is the child node of the former. In this case, there would not be a forearm in 
		//the skeleton, so we have to link the hand to the foretwist1 manually.
		
		
		//现在我们已经得到了每个顶点（原）的顶点信息，现在我们把他们转换到新的顶点（整理过位置和纹理坐标后的）
		//上，还需要计算出每个面涉及的骨骼的最大数目和每个骨骼涉及的顶点数目。
		
		pMesh->m_dNumBone = NumBones;
		pMesh->m_lpVerticesSkinInfo = new CMesh::VERTICESSKININFO[pMesh->m_dNumVertices];
		ZeroMemory(pMesh->m_lpVerticesSkinInfo,sizeof(CMesh::VERTICESSKININFO)*pMesh->m_dNumVertices);
		//清除lpBoneRef 中每个骨骼被引用的次数
		for(DWORD i=0;i<NumBones;i++)
		{
			lpBoneRef[i]=0;
		}
		//先转换每个顶点的骨骼信息
		for(DWORD i_Ver=0;i_Ver<pMesh->m_dNumVertices;i_Ver++)
		{
			DWORD OldIndex = pMesh->m_lpVerticeRemap[i_Ver];
			CMesh::LPVERTICESSKININFO pVInfoNew = &pMesh->m_lpVerticesSkinInfo[i_Ver];
			CMesh::LPVERTICESSKININFO pVInfoOld = &lpVerSkinInfo[OldIndex];
			pVInfoNew->NumBoneRef = pVInfoOld->NumBoneRef;
			
			pVInfoNew->BoneName = new LPSTR[pVInfoNew->NumBoneRef];
			ZeroMemory(pVInfoNew->BoneName, sizeof(LPSTR) * pVInfoNew->NumBoneRef);
			pVInfoNew->Weight = new float[pVInfoNew->NumBoneRef];
			ZeroMemory(pVInfoNew->Weight, sizeof(float) * pVInfoNew->NumBoneRef);
			
			for(DWORD i_Bone = 0;i_Bone<pVInfoNew->NumBoneRef;i_Bone++)
			{
				//这里我们继续 在 BoneName 中存储骨骼的指针，暂时不把他转换为字符串
				INode* pBoneNode = (INode*) pVInfoOld->BoneName[i_Bone];
				pVInfoNew->BoneName[i_Bone] = pVInfoOld->BoneName[i_Bone];
				pVInfoNew->Weight  [i_Bone] = pVInfoOld->Weight  [i_Bone];
				
				DWORD BoneIndex = FindBone(lpBoneNodes,pBoneNode,NumBones);
				if ( BoneIndex == -1 )
				{	
					TSTR strBoneName = pBoneNode->NodeName();
					TCHAR info[255];
					sprintf(info, "未被导出的骨骼%s绑定了顶点", strBoneName);
					MessageBox(m_ip->GetMAXHWnd(), info, "错误", MB_OK);
					continue;
				}
				lpBoneRef[BoneIndex]++;
			}
		}
		
		//转换各个骨骼的信息
		pMesh->m_lpBoneInfo = new CMesh::BONEINFO[NumBones];
		ZeroMemory(pMesh->m_lpBoneInfo,sizeof(CMesh::BONEINFO)*NumBones);
		
		DWORD* pNumChildBoneFix = new DWORD[NumBones];
		ZeroMemory(pNumChildBoneFix, sizeof(DWORD) * NumBones);
		for(DWORD i_Bone = 0;i_Bone<NumBones;i_Bone++)
		{
			//为了方便下一步导出动画的数据，同上面一样，我们用LPSTR来存储 INode*
			//暂时不转化为最终的字符串
			INode* pBoneNode = lpBoneNodes[i_Bone];
			INode* pParentNode = pBoneNode->GetParentNode();
			INode* pFixedParentNode = NULL;
			
			TSTR  strTemp = pParentNode->NodeName();
			CHAR strParentNodeName[255];
			strcpy(strParentNodeName, strTemp);
			
			if ( CheckNodeSuffix(pParentNode, "Forearm") && !m_bForearm )
			{
				ChangeSuffix(strParentNodeName, "Forearm", "ForeTwist1");
				pFixedParentNode = FindBoneByName(strParentNodeName, lpBoneNodes, NumBones);
				if ( pFixedParentNode )
					pParentNode = pFixedParentNode;
			}
			else 
			{
				if ( m_relink.count(pBoneNode) )//This bone has been relinked
				{
					pParentNode = m_relink[pBoneNode];
#ifdef _DEBUG					
					TSTR ParentName = pParentNode->NodeName();
					int nDebug = 0;
#endif
				}
			}
			CMesh::LPBONEINFO pMeshBone = &pMesh->m_lpBoneInfo[i_Bone];
			
			Matrix3 tm;
			//Note: Bone mode use WSM, but relative mode and RTS mode use node TM 
			if ( GetAnimationType() == ANIMATION_BONE )
			{
				tm = pBoneNode->GetObjTMAfterWSM(GetStaticFrame());
			}
			else if ( GetAnimationType() == ANIMATION_BONE_RELATIVE || GetAnimationType() == ANIMATION_BONE_RTS )
			{
				tm = pBoneNode->GetNodeTM(GetStaticFrame());
			}

			if(m_bInch2CM)
			{
				Matrix3 Scale;
				Scale.SetScale(Point3(2.54f,2.54f,2.54f));
				tm *= Scale;
			}
			if(GetExchangeYZ())
			{
				tm = ExchangeMatrixYZ(&tm);
			}
			if (pParentNode != m_ip->GetRootNode())
			{
				Matrix3 ParentMat;
				if ( GetAnimationType() == ANIMATION_BONE )
				{
					ParentMat = pParentNode->GetObjTMAfterWSM(GetStaticFrame());
				}
				else if ( GetAnimationType() == ANIMATION_BONE_RELATIVE || GetAnimationType() == ANIMATION_BONE_RTS )
				{
					ParentMat = pParentNode->GetNodeTM(GetStaticFrame());
				}
				if (GetExchangeYZ())
				{
					ParentMat = ExchangeMatrixYZ(&ParentMat);
				}
				ParentMat.Invert();
				Matrix3 BaseMat = tm * ParentMat;
				Matrix3ToMatrix4(&pMeshBone->BaseBoneMatrix,&BaseMat);
			}
			else
			{
				Matrix3 BaseMat;
				BaseMat.IdentityMatrix();
				Matrix3ToMatrix4(&pMeshBone->BaseBoneMatrix, &BaseMat);
			}                  
			
			tm.Invert();
			Matrix3ToMatrix4(&pMeshBone->MatrixOffset,&tm);
        	pMeshBone->NumChild = pBoneNode->NumberOfChildren();

			//////////////////////////////////////////////////////////////////////////
			//Starting to fix the num of child bone
			if ( !m_bForearm &&  CheckNodeSuffix(pBoneNode, "UpperArm") )
			{
				//when forearm does not exist and the current bone is upperarm, do not modify anything
				pMeshBone->NumChild--;
				pMeshBone->NumChild = (pMeshBone->NumChild > 0) ? pMeshBone->NumChild : 0;
			}
			else 
			{
				DWORD dwChildIndex;
				DWORD dwChildToDel = 0;
				for( dwChildIndex = 0; dwChildIndex < pMeshBone->NumChild; dwChildIndex++ )
				{
					INode* pChildNode = pBoneNode->GetChildNode(dwChildIndex);
					if ( !IsBone(pChildNode, lpBoneNodes, m_Bones.size()))
						dwChildToDel++;//If child bone is bound to ISkin, then decrease the num of child of the parent bone
				}
				pMeshBone->NumChild -= dwChildToDel;
			}
			
			int nHandAdded = 0;
			INode* pHand = NULL;
			if ( !m_bForearm )
			{
				if ( CheckNodeSuffix(pBoneNode, "ForeTwist1") )
				{
					TSTR strTemp = pBoneNode->NodeName();
					TCHAR strNodeName[255];
					strcpy(strNodeName, strTemp);
					ChangeSuffix(strNodeName, "ForeTwist1", "Hand");
					pHand = FindBoneByName(strNodeName, lpBoneNodes, NumBones);
					if ( pHand )
					{
						//pMeshBone->NumChild++;
						nHandAdded = 1;
					}

				}
			}

			int nRelinked = 0;
			if ( m_NumrelinkChild.count(pBoneNode) )//If some bone is relinked to it, add num of child
			{
				nRelinked = static_cast<int>(m_NumrelinkChild[pBoneNode]->size());
			}
			
			pMeshBone->lpChildNames = new LPSTR[pMeshBone->NumChild + nHandAdded + nRelinked];
			
			DWORD dwChildOffset = 0;
			for(DWORD C=0;C<pMeshBone->NumChild;C++)
			{
				if (dwChildOffset >= pBoneNode->NumberOfChildren() )
				{
					TSTR strBoneName = pBoneNode->NodeName();
					break;
				}
				INode* pChildNode = pBoneNode->GetChildNode(dwChildOffset);
				TSTR strChildName = pChildNode->NodeName();
				dwChildOffset++;
				while ( !IsBone(pChildNode, lpBoneNodes, m_Bones.size()) && dwChildOffset < pBoneNode->NumberOfChildren() )
				{
					pChildNode = pBoneNode->GetChildNode(dwChildOffset);
					strChildName = pChildNode->NodeName();
					dwChildOffset++;
				}
				pMeshBone->lpChildNames[C] = (LPSTR)pChildNode;
			}
			
			if ( nHandAdded )
			{
				pMeshBone->lpChildNames[pMeshBone->NumChild] = (LPSTR)pHand;
				pMeshBone->NumChild++;
			}

			int nRelinkedIndex;
			for ( nRelinkedIndex = 0; nRelinkedIndex < nRelinked; nRelinkedIndex++ )
			{
				pMeshBone->lpChildNames[pMeshBone->NumChild + nRelinkedIndex] = (LPSTR)((*m_NumrelinkChild[pBoneNode])[nRelinkedIndex]);
			}
			pMeshBone->NumChild += nRelinked;
			//Starting to fix the num of child bone end.
			//////////////////////////////////////////////////////////////////////////
			

			DWORD NumRefVertices = lpBoneRef[i_Bone];
			//这里只为骨骼上顶点的信息分配内存，我们再使用一次循环来实现这个功能
			pMeshBone->VerticesIndexRef = new DWORD [NumRefVertices];
			ZeroMemory(pMeshBone->VerticesIndexRef,
				sizeof(DWORD)*NumRefVertices);
			pMeshBone->VerticesWeightRef = new float [NumRefVertices];
			ZeroMemory(pMeshBone->VerticesWeightRef,
				sizeof(float)*NumRefVertices);
			pMeshBone->NumVerticesRef = 0;
		}  
		
		//填充骨骼中关于涉及到所有的顶点的信息
		for(i_Ver=0; i_Ver < pMesh->m_dNumVertices; i_Ver++)
		{
			CMesh::LPVERTICESSKININFO pVInfoNew = &pMesh->m_lpVerticesSkinInfo[i_Ver];
			for(DWORD i_Bone = 0;i_Bone < pVInfoNew->NumBoneRef; i_Bone++)
			{
				//这里我们继续 在 BoneName 中存储骨骼的指针，暂时不把他转换为字符串
				INode* pBoneNode = (INode*) pVInfoNew->BoneName[i_Bone];
				DWORD BoneIndex = FindBone(lpBoneNodes,pBoneNode,NumBones);
				if ( BoneIndex == -1 || BoneIndex > NumBones )
				{
					TSTR strBoneName = pBoneNode->NodeName();
					TCHAR info[255];
					sprintf(info, "未被导出的骨骼%s绑定了顶点", strBoneName);
					MessageBox(m_ip->GetMAXHWnd(), info, "Error", MB_OK);
					continue;
				}
				CMesh::LPBONEINFO pMeshBone = &pMesh->m_lpBoneInfo[BoneIndex];
				pMeshBone->VerticesIndexRef[pMeshBone->NumVerticesRef] = i_Ver;
				pMeshBone->VerticesWeightRef[pMeshBone->NumVerticesRef] = pVInfoNew->Weight[i_Bone];
				pMeshBone->NumVerticesRef++;
			}
		}
		
		//Done,release the buffer]
		SAFE_DELETE_ARRAY(pNumChildBoneFix);
		SAFE_DELETE_ARRAY(lpBoneRef);
		SAFE_DELETE_ARRAY(lpBoneNodes);
		
		for( i=0;i<NumVertex;i++)
		{  
			CMesh::LPVERTICESSKININFO pVerInfo = &lpVerSkinInfo[i];
			SAFE_DELETE_ARRAY(pVerInfo->BoneName);
			SAFE_DELETE_ARRAY(pVerInfo->Weight);
		}
		
		SAFE_DELETE_ARRAY(lpVerSkinInfo);
		if (needDel) {
			delete tri;
		}
		pMesh->m_bSkined = TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	//for each bone, find child-node to see it is a socket
	for(DWORD K =0;K<pMesh->m_dNumBone;K++)
	{
		INode* pBone = pMesh->m_lpBoneInfo[K].pThis;
		//fprintf(pNote,"Bone  %d : %s \n",K,pBone->GetName());

		int N_Child = pBone->NumberOfChildren();
		for(int i=0;i<N_Child;i++)
		{
			INode* pChild = pBone->GetChildNode(i);

			if(pChild)
			{
				ObjectState os = pChild->EvalWorldState(0); 

				// The obj member of ObjectState is the actual object we will export.
				if (os.obj) 
				{
					// We look at the super class ID to determine the type of the object.
					if (HELPER_CLASS_ID == os.obj->SuperClassID())
					{
						TCHAR SocketName[256];
						wsprintf(SocketName,"%s",pChild->GetName());

						char flex[] = "S_";
                        char*  result;
						result = strstr(SocketName, flex);
                        char surface[] = "FCS_";
                        char* resultSurface = strstr(SocketName, surface);
						if(result || resultSurface)
						{
							CMesh::_Socket Socket;
							Socket.Node = pChild;
							Socket.ParentNode = pBone;
							Socket.Name = SocketName;
							Socket.ParentName = pBone->GetName();

							AffineParts apWSMLocal, apNodeLocal, apWSMChild;
							Matrix3 WSMParent = pBone->GetObjTMAfterWSM(GetStaticFrame());
							Matrix3 WSMChild  = pChild->GetObjTMAfterWSM(GetStaticFrame());

							Matrix3 NodeParent = pBone->GetNodeTM(GetStaticFrame());
							Matrix3 NodeChild  = pChild->GetNodeTM(GetStaticFrame());

							Matrix3 WSMLocal, NodeLocal, ObjLocal, WSMLocal1, NodeLocal1;
							WSMLocal = WSMChild * Inverse(WSMParent);
							NodeLocal = NodeChild * Inverse(NodeParent);
							////To eliminate scale
							decomp_affine(NodeLocal, &apNodeLocal);
							decomp_affine(WSMLocal, &apWSMLocal);
							Matrix3 srtm, rtm, ptm, stm, ftm;
							
							ptm.IdentityMatrix();
							if ( GetAnimationType() == ANIMATION_BONE_RTS )
							{
								ptm.SetTrans(apNodeLocal.t);
								apNodeLocal.q.MakeMatrix(rtm);
								apNodeLocal.u.MakeMatrix(srtm);
								if ( GetNormalizeSocket() )
									apNodeLocal.k = Point3( 1.0f, 1.0f, 1.0f );
								if ( m_bInch2CM )
									stm = ScaleMatrix( apNodeLocal.k / 2.54f );
								else
									stm = ScaleMatrix( apNodeLocal.k );
								ftm = ScaleMatrix( Point3( apNodeLocal.f, apNodeLocal.f, apNodeLocal.f ) );
								NodeLocal = Inverse( srtm ) * stm * srtm * rtm * ftm * ptm;
								Matrix3ToMatrix4(&Socket.MatrixOffset,&NodeLocal);
							}
							else 	
							{
								ptm.SetTrans(apWSMLocal.t); 
								apWSMLocal.q.MakeMatrix(rtm);
								apWSMLocal.u.MakeMatrix(srtm);
							
								if ( GetNormalizeSocket() )
									apWSMLocal.k = apWSMLocal.k / apNodeLocal.k;
								if ( m_bInch2CM )
								{
									stm = ScaleMatrix( apWSMLocal.k / 2.54f );
								}
								else
								{
									stm = ScaleMatrix(apWSMLocal.k );
								}

								ftm = ScaleMatrix(Point3(apWSMLocal.f ,apWSMLocal.f,apWSMLocal.f)); 
								WSMLocal = Inverse(srtm) * stm * srtm * rtm * ftm * ptm;
								Matrix3ToMatrix4(&Socket.MatrixOffset,&WSMLocal);
							}

							//Add 5-15-2005 this the the actual cause of socket orient problem
							float temp[4];
							temp[0] = Socket.MatrixOffset._31;
							temp[1] = Socket.MatrixOffset._32;
							temp[2] = Socket.MatrixOffset._33;
							temp[3] = Socket.MatrixOffset._34;

							Socket.MatrixOffset._31 = Socket.MatrixOffset._21;
							Socket.MatrixOffset._32 = Socket.MatrixOffset._22;
							Socket.MatrixOffset._33 = Socket.MatrixOffset._23;
							Socket.MatrixOffset._34 = Socket.MatrixOffset._24;

							Socket.MatrixOffset._21 = temp[0];
							Socket.MatrixOffset._22 = temp[1];
							Socket.MatrixOffset._23 = temp[2];
							Socket.MatrixOffset._24 = temp[3];
						    if (resultSurface)
							{
								pMesh->m_listCollisionSurface.remove(Socket); 
								pMesh->m_listCollisionSurface.push_back(Socket);
							}
							else
							{
								pMesh->m_listSocket.remove(Socket); 
								pMesh->m_listSocket.push_back(Socket);
							}
						}
					}
				}
			}
		}
	}
	return;
}


HRESULT AsciiExp::ExportSkinInfo1_Skin(
    INode *pNode, 
    KG_VERTICESSKININFO_VECTOR  *pVerticesSkinInfoVector,
    KG_BONE_CONTAINER_TABLE     *pBoneContainerTable
)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = S_FALSE;
    Modifier            *pSkin          = NULL;
    ISkin               *pSkinInterface = NULL;
    ISkinContextData    *pSkinContext   = NULL;
    int i = 0;
    int j = 0;
    int k = 0;
    int nBoneNum = 0;
    int nVecticesNum = 0;
    float fTotalWeightPerVertex = 0.0f;

    KGLOG_PROCESS_ERROR(pNode);
    KGLOG_PROCESS_ERROR(pVerticesSkinInfoVector);
    KGLOG_PROCESS_ERROR(pBoneContainerTable);

    //////////////////////////////////////////////////////////////////////////
    //skin export
    // first see if skin is present
    pSkin = FindSkinModifier(pNode);
    KG_PROCESS_SUCCESS(!pSkin);

    // Get a Physique Export interface 
    pSkinInterface = (ISkin *)pSkin->GetInterface(I_SKIN);
    KGLOG_PROCESS_ERROR(pSkinInterface);


    // For a given Object's INode get a
    // ModContext Interface from the Physique Export Interface:
    pSkinContext = (ISkinContextData *)pSkinInterface->GetContextInterface(pNode);
    KGLOG_PROCESS_ERROR(pSkinContext);

    nBoneNum = pSkinInterface->GetNumBones();
    for (i = 0; i < nBoneNum; i++)
    {
        INode *pBoneNode = pSkinInterface->GetBone(i);
        
        hrRetCode = KG_AddBone(pBoneContainerTable, pBoneNode);
        KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
    }

    nVecticesNum = pSkinContext->GetNumPoints();

    for (i = 0; i < nVecticesNum; i++)
    {
        CMesh::VERTICESSKININFO VerticesSkinInfo;
        memset(&VerticesSkinInfo, 0, sizeof(VerticesSkinInfo));
        
        fTotalWeightPerVertex = 0.0f;

        int nEffectedBoneNum = pSkinContext->GetNumAssignedBones(i);
        for(j = 0; j < nEffectedBoneNum; j++)
        {
            int    nBoneIndex = pSkinContext->GetAssignedBone(i, j);
            INode *pBoneNode  = pSkinInterface->GetBone(nBoneIndex);
            KGLOG_PROCESS_ERROR(pBoneNode);

            float fBoneWeight = pSkinContext->GetBoneWeight(i, j);
            if (fBoneWeight <= 0)
                continue;

            for (k = 0; k < VerticesSkinInfo.NumBoneRef; k++)
            {
                if (VerticesSkinInfo.Bone[k].pBoneNode == pBoneNode)
                {
                    VerticesSkinInfo.Bone[k].fWeight += fBoneWeight;
                    fTotalWeightPerVertex += fBoneWeight;
                    break;
                }
            }

            if (k < VerticesSkinInfo.NumBoneRef)
                continue;

            if (VerticesSkinInfo.NumBoneRef >= VERTICESSKININFOITEM_MAX)
            {
                char szOutput[512];
                char szBoneName[80];
                sprintf(szOutput, "Model %s, Vertex [%d] is effect by Bone more than %d, ", 
                    pNode->NodeName(), i,  VERTICESSKININFOITEM_MAX
                );
                for (int m = 0; m < VerticesSkinInfo.NumBoneRef; m++)
                {
                    sprintf(szBoneName, "Bone [%s], ", 
                        VerticesSkinInfo.Bone[m].pBoneNode->GetName()
                    );
                    strcat(szOutput, szBoneName);
                }
                sprintf(szBoneName, "Bone [%s], ", pBoneNode->GetName());
                strcat(szOutput, szBoneName);

                m_ExportError.AddError(szOutput);
                KGLOG_PROCESS_ERROR(false);
            }

            VerticesSkinInfo.Bone[VerticesSkinInfo.NumBoneRef].pBoneNode = pBoneNode;
            //if (nBoneIndex == 3)
            //{
                VerticesSkinInfo.Bone[VerticesSkinInfo.NumBoneRef].fWeight   = fBoneWeight;
            //}
            VerticesSkinInfo.NumBoneRef++;
            fTotalWeightPerVertex += fBoneWeight;
        }                     

        if (abs(fTotalWeightPerVertex - 1.0f) >= 0.01)
        {
            char szOutput[512];
            sprintf(szOutput, "Model %s, Vertex [%d] is Weight is %f not equal 1.0f, ", 
                pNode->NodeName(), i,  fTotalWeightPerVertex
            );
            for (int m = 0; m < VerticesSkinInfo.NumBoneRef; m++)
            {
                char szBoneName[80];
                sprintf(
                    szBoneName, "Bone [%s] Weight = %f, ", 
                    VerticesSkinInfo.Bone[m].pBoneNode->GetName(),
                    VerticesSkinInfo.Bone[m].fWeight
                );
                strcat(szOutput, szBoneName);
            }
            m_ExportError.AddError(szOutput);
            KGLOG_PROCESS_ERROR(false);
        }

        pVerticesSkinInfoVector->push_back(VerticesSkinInfo);
    }
	hrRetCode = S_OK;
Exit1:
    hrResult = hrRetCode;
Exit0:
    if (pSkinInterface)
    {
        pSkin->ReleaseInterface(I_SKIN, pSkinInterface);
        pSkinInterface = NULL;
    }

    return hrResult;

}

Matrix3 g_mat3Inch2CMScale(
    Point3(2.54f,   0.0f,   0.0f),
    Point3(0.0f,    2.54f,  0.0f),
    Point3(0.0f,    0.0f,   2.54f),
    Point3(0.0f,    0.0f,   0.0f)
);

INode* AsciiExp::ValidateParentNode(INode* pNode,
									const KG_BONE_CONTAINER_TABLE& crBoneContainerTable)
{
	INode* pParentNode = NULL;
	KG_PROCESS_ERROR(pNode);
	KG_PROCESS_ERROR(pNode != m_ip->GetRootNode());
	pParentNode = pNode->GetParentNode();
	while (!KG_FindBoneByName(crBoneContainerTable, pParentNode->NodeName()) &&
		pParentNode != m_ip->GetRootNode()) 
	{
		pParentNode = pParentNode->GetParentNode();
	};
Exit0:
	return pParentNode;
}


HRESULT AsciiExp::ExportSkinInfo1_ProcessBoneBaseInfo(
    const KG_BONE_CONTAINER_TABLE &crBoneContainerTable,
    KG_3D_EXPORT_BONE_VECTOR *pExportBoneVector
)
{
    HRESULT hrResult = E_FAIL;
    int i = 0;
    KG_BONE_CONTAINER_TABLE::const_iterator it;
    KG_3D_EXPORT_BONE Bone;

    KGLOG_PROCESS_ERROR(pExportBoneVector);


    pExportBoneVector->clear();

    Bone.clear();   // equal memset
    pExportBoneVector->resize(crBoneContainerTable.size(), Bone);

    for (it = crBoneContainerTable.begin(); it != crBoneContainerTable.end(); ++it)
    {
        Bone.clear();   // equal memset

        Bone.pThis      = it->first;
        Bone.pParent    = Bone.pThis->GetParentNode();

        int nChildrenNum = it->first->NumberOfChildren();
        for (i = 0; i < nChildrenNum; i++)
        {
            INode *pChild = Bone.pThis->GetChildNode(i);
            KG_BONE_CONTAINER_TABLE::const_iterator itTemp = crBoneContainerTable.find(pChild);
            if (itTemp == crBoneContainerTable.end())
                continue;   // if not found then the child node is not skined

            Bone.Children.push_back(pChild);
        }

		Matrix3 mat3Matrix3;
        Matrix3 mat3Base(TRUE); //IdentityMatrix
       // ASSERT(mat3Base.IsIdentity());

        mat3Matrix3 = Bone.pThis->GetNodeTM(GetExportOption()->m_nStaticFrame);

		if (GetExportOption()->m_Type == ENUM_EXPORTTYPE_NORMAL)
		{
			Bone.pParent = ValidateParentNode(Bone.pThis, crBoneContainerTable);	
		}
		
		if (Bone.pParent != m_ip->GetRootNode())
        {
            Matrix3 mat3Parent;
            mat3Parent = Bone.pParent->GetNodeTM(GetExportOption()->m_nStaticFrame);

            mat3Parent.Invert();
            mat3Base = mat3Matrix3 * mat3Parent;
        }             

		if (GetExportOption()->m_bExchangeYZ)
            GetHelper().ChangeMatrixYZ(&mat3Base);
        Matrix3ToMatrix4(&Bone.BaseBoneMatrix, &mat3Base);

        if (GetExportOption()->m_bInch2CM)
            mat3Matrix3 *= g_mat3Inch2CMScale;
        if (GetExportOption()->m_bExchangeYZ)
            GetHelper().ChangeMatrixYZ(&mat3Matrix3);

        mat3Matrix3.Invert();
        Matrix3ToMatrix4(&Bone.MatrixOffset, &mat3Matrix3);

        (*pExportBoneVector)[it->second] = Bone;
    }

    //Write Log

                                          
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT ExportSkinInfo1_TransVertexWeightToBone_WriteLog(
    LPMESH pMesh,
    const KG_VERTICESSKININFO_VECTOR  &crVerticesSkinInfoVector,
    const KG_3D_EXPORT_BONE_VECTOR    &crExportBoneVector
)
{
    HRESULT hrResult = E_FAIL;
    int i = 0;
    int j = 0;
    float **ppWeight = NULL;
    int nBoneNum   = crExportBoneVector.size();
    int nOrignVertexNum = crVerticesSkinInfoVector.size();
    int nEffectVertexNum = 0;
    CStr strOutput;
    CStr strTemp;

    KGLOG_PROCESS_ERROR(pMesh);
    KGLOG_PROCESS_ERROR(pMesh->m_lpVerticeRemap);

    ppWeight = new float *[nBoneNum];
    KGLOG_PROCESS_ERROR(ppWeight);
    memset(ppWeight, 0, sizeof(float *) * nBoneNum);

    for (i = 0; i < nBoneNum; i++)
    {
        ppWeight[i] = new float[nOrignVertexNum];
        KGLOG_PROCESS_ERROR(ppWeight[i]);
        memset(ppWeight[i], 0, sizeof(float) * nOrignVertexNum);

        nEffectVertexNum = crExportBoneVector[i].Vertices.size();
        for (j = 0; j < nEffectVertexNum; j++)
        {
            if (crExportBoneVector[i].Vertices[j].fWeight == 0)
                continue;


            int nIndex = crExportBoneVector[i].Vertices[j].nIndex;
            KGLOG_PROCESS_ERROR(nIndex < pMesh->m_dNumVertices);

            int nOrignIndex = pMesh->m_lpVerticeRemap[nIndex];
            KGLOG_PROCESS_ERROR(nOrignIndex < nOrignVertexNum);

            if (ppWeight[i][nOrignIndex] == 0)
            {
                ppWeight[i][nOrignIndex] = crExportBoneVector[i].Vertices[j].fWeight;
                continue;
            }

            //KG_ASSERT_EXIT(ppWeight[i][nOrignIndex] == crExportBoneVector[i].Vertices[j].fWeight);
        }
    }

    strOutput.printf("Num\t");
    for (i = 0; i < nBoneNum; i++)
    {
        strTemp.printf("%s\t,", crExportBoneVector[i].pThis->GetName());
        strOutput += strTemp;
    }
    //KGLogPrintf(KGLOG_DEBUG, "%s", (char *)strOutput);

    for (i = 0; i < nOrignVertexNum; i++)
    {
        strOutput.printf("%d\t", i);
        for (j = 0; j < nBoneNum; j++)
        {
            if (ppWeight[j][i] != 0)
                strTemp.printf("%.3f\t", ppWeight[j][i]);
            else
                strTemp.printf("-\t");

            strOutput += strTemp;
        }
        //KGLogPrintf(KGLOG_DEBUG, "%s", (char *)strOutput);
    }


    hrResult = S_OK;
Exit0:
    if (ppWeight)
    {
        for (i = 0; i < nBoneNum; i++)
        {
            KG_DELETE_ARRAY(ppWeight[i]);
        }
    }

    KG_DELETE_ARRAY(ppWeight);

    return hrResult;
}

HRESULT AsciiExp::ExportSkinInfo1_TransVertexWeightToBone(
    LPMESH pMesh,
    const KG_VERTICESSKININFO_VECTOR  &crVerticesSkinInfoVector,
    const KG_BONE_CONTAINER_TABLE     &crBoneContainerTable,
    KG_3D_EXPORT_BONE_VECTOR          *pExportBoneVector
)
{
    HRESULT hrResult = E_FAIL;
    int i = 0;
    int j = 0;

    KGLOG_PROCESS_ERROR(pMesh);
    KGLOG_PROCESS_ERROR(pExportBoneVector);
    KGLOG_PROCESS_ERROR(pMesh->m_lpVerticeRemap);

    for (i = 0; i < pMesh->m_dNumVertices; i++)
    {
        int nOrignIndex = pMesh->m_lpVerticeRemap[i];

        KGLOG_PROCESS_ERROR(nOrignIndex < crVerticesSkinInfoVector.size());

        const CMesh::VERTICESSKININFO &VerticesSkinInfo = crVerticesSkinInfoVector[nOrignIndex];
        for (j = 0; j < VerticesSkinInfo.NumBoneRef; j++)
        {
            KG_BONE_CONTAINER_TABLE::const_iterator it;

            it = crBoneContainerTable.find(VerticesSkinInfo.Bone[j].pBoneNode);
            if (it == crBoneContainerTable.end())
            {
                {
                    char szOutput[256];
                    sprintf(
                        szOutput, "Bone [%s] not found in pExportBoneVector", 
                        VerticesSkinInfo.Bone[j].pBoneNode->GetName()
                    );
                    m_ExportError.AddError(szOutput);
                }


                KGLOG_PROCESS_ERROR(false);
            }
            KGLOG_PROCESS_ERROR(it->second < pExportBoneVector->size());

            // if found
            KG_3D_EXPORT_BONE &ExportBone = (*pExportBoneVector)[it->second];

            KG_3D_EXPORT_BONE_VERTEX ExportBoneVertex;
            memset(&ExportBoneVertex, 0, sizeof(ExportBoneVertex));
            ExportBoneVertex.nIndex  = i;
            ExportBoneVertex.fWeight = VerticesSkinInfo.Bone[j].fWeight;

            ExportBone.Vertices.push_back(ExportBoneVertex);
        }
    }

    ExportSkinInfo1_TransVertexWeightToBone_WriteLog(
        pMesh,
        crVerticesSkinInfoVector,
       *pExportBoneVector
    );

    hrResult = S_OK;
Exit0:
    return hrResult;
}

static int CheckSocketNameValidate(const list<CMesh::SOCKET> &lstSocket, const char cszSocketName[])
{
    int nRetCode = false;
    list<CMesh::SOCKET>::const_iterator it;

    for (it = lstSocket.begin(); it != lstSocket.end(); ++it)
    {
        nRetCode = strcmp(it->Name.c_str(), cszSocketName);
        if (nRetCode == 0)
		{
			TCHAR strErrorInfo[MAX_PATH << 1];
			sprintf_s(strErrorInfo,
				MAX_PATH << 1,
				"插槽%s重复", 
				cszSocketName);
			MessageBox(NULL, strErrorInfo, "错误", MB_OK);	
			return false;
		}
    }
    return true;
}

HRESULT AsciiExp::ExportSkinInfo1_ExportSocketToMesh(
    LPMESH pMesh,
    const KG_3D_EXPORT_BONE_VECTOR &crExportBoneVector
)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode     = false;
    KG_3D_EXPORT_BONE_VECTOR::const_iterator it;
    int  i = 0;
    int nSocketFlag = false;
    int nFlexFlag   = false;

    for (it = crExportBoneVector.begin(); it != crExportBoneVector.end(); ++it)
    {
        INode *pBone = it->pThis;
        int nChildrenNum = pBone->NumberOfChildren();

        for (i = 0; i < nChildrenNum; i++)
        {
            INode *pChild = pBone->GetChildNode(i);
            KGLOG_PROCESS_ERROR(pChild);

            ObjectState osState = pChild->EvalWorldState(GetExportOption()->m_nStaticFrame);
            if (!osState.obj)
                continue;

            if (HELPER_CLASS_ID != osState.obj->SuperClassID())
                continue;

            char szSocketName[256];
            strncpy(szSocketName, pChild->GetName(), sizeof(szSocketName));
            szSocketName[sizeof(szSocketName) - 1] = '\0';

            nSocketFlag = false;
            nRetCode = strncmp(szSocketName, "S_", sizeof("S_") - 1);
            if (nRetCode == 0)
                nSocketFlag = true;
            
            nFlexFlag   = false;
            nRetCode = strncmp(szSocketName, "FCS_", sizeof("FCS_") - 1);
            if (nRetCode == 0)
                nFlexFlag = true;

            if (nSocketFlag)
            {
                nRetCode = CheckSocketNameValidate(pMesh->m_listSocket, szSocketName);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
            else if (nFlexFlag)
            {
                nRetCode = CheckSocketNameValidate(pMesh->m_listCollisionSurface, szSocketName);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                continue;
            }


            CMesh::SOCKET Socket;
            Socket.Node         = pChild;
            Socket.Name         = szSocketName;
            Socket.ParentNode   = pBone;
            Socket.ParentName   = pBone->GetName();

            AffineParts apNodeLocal;

            Matrix3 mat3NodeParent = pBone->GetNodeTM(GetExportOption()->m_nStaticFrame);
            Matrix3 mat3NodeChild  = pChild->GetNodeTM(GetExportOption()->m_nStaticFrame);

            Matrix3 mat3NodeLocal;

            mat3NodeLocal = mat3NodeChild * Inverse(mat3NodeParent);
            if (GetExportOption()->m_bExchangeYZ)
            {
                GetHelper().ChangeMatrixYZ(&mat3NodeLocal);
            }

            decomp_affine(mat3NodeLocal, &apNodeLocal);
            
            Matrix3 mat3_apNodeLocal_t;
            mat3_apNodeLocal_t.IdentityMatrix();
            mat3_apNodeLocal_t.SetTrans(apNodeLocal.t);

            Matrix3 mat3_apNodeLocal_q;
            apNodeLocal.q.MakeMatrix(mat3_apNodeLocal_q);

            Matrix3 mat3_apNodeLocal_u;
            apNodeLocal.u.MakeMatrix(mat3_apNodeLocal_u);

            Matrix3 mat3_apNodeLocal_k;
            if (GetNormalizeSocket())
                apNodeLocal.k = Point3( 1.0f, 1.0f, 1.0f );

            if (m_bInch2CM)
                mat3_apNodeLocal_k = ScaleMatrix(apNodeLocal.k / 2.54f);
            else
                mat3_apNodeLocal_k = ScaleMatrix(apNodeLocal.k);

            Matrix3 mat3_apNodeLocal_f;
            mat3_apNodeLocal_f = ScaleMatrix(Point3(apNodeLocal.f, apNodeLocal.f, apNodeLocal.f));

            mat3NodeLocal = Inverse(mat3_apNodeLocal_u) * 
                            mat3_apNodeLocal_k * 
                            mat3_apNodeLocal_u * 
                            mat3_apNodeLocal_q * 
                            mat3_apNodeLocal_f * 
                            mat3_apNodeLocal_t;

            Matrix3ToMatrix4(&Socket.MatrixOffset, &mat3NodeLocal);


            if (nSocketFlag)
            {
                pMesh->m_listSocket.push_back(Socket);
            }
            else if (nFlexFlag)
            {
                pMesh->m_listCollisionSurface.push_back(Socket);
            }
            else
            {
                KGLOG_PROCESS_ERROR(false && "error in process socket");
            }
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT AsciiExp::ExportSkinInfo1_SetDestMesh(
    LPMESH pMesh,
    const KG_3D_EXPORT_BONE_VECTOR &crExportBoneVector
)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    int i = 0;
    CMesh::BONEINFO *pBoneInfo = NULL;
    KG_3D_EXPORT_BONE_VECTOR::const_iterator it;

    KGLOG_PROCESS_ERROR(pMesh);

    pMesh->m_dNumBone = crExportBoneVector.size();
    pMesh->m_bSkined  = TRUE;
    pMesh->m_lpVerticesSkinInfo = NULL;

    pMesh->m_lpBoneInfo = new CMesh::BONEINFO[pMesh->m_dNumBone];
    KGLOG_PROCESS_ERROR(pMesh->m_lpBoneInfo);
    memset(pMesh->m_lpBoneInfo, 0, sizeof(CMesh::BONEINFO) * pMesh->m_dNumBone);

    pBoneInfo = pMesh->m_lpBoneInfo;
    for (
        it = crExportBoneVector.begin(),    pBoneInfo = pMesh->m_lpBoneInfo; 
        it != crExportBoneVector.end(); 
        ++it,                               pBoneInfo++
    )
    {
        pBoneInfo->pThis = it->pThis;
        char *pszName = NULL;
        pszName = it->pThis->GetName();
        KGLOG_PROCESS_ERROR(pszName);
        nRetCode = strlen(pszName);
        if (nRetCode >= MAX_BONE_NAME_LENGTH)
        {
            CStr strOutput;
            strOutput.printf("骨骼[%s]名字过长", pszName);

            MessageBox(NULL, strOutput, "错误", MB_OK);
			m_ExportError.AddError(strOutput);
            KGLOG_PROCESS_ERROR(false);
        }

        strncpy(pBoneInfo->szName, pszName, MAX_BONE_NAME_LENGTH);
        pBoneInfo->szName[MAX_BONE_NAME_LENGTH - 1] = '\0';

        pBoneInfo->pParent = it->pParent;
        pszName = it->pParent->GetName();
        KGLOG_PROCESS_ERROR(pszName);
        nRetCode = strlen(pszName);
        if (nRetCode >= MAX_BONE_NAME_LENGTH)
        {
            CStr strOutput;
			strOutput.printf("骨骼[%s]的父骨骼[%s]名字过长", pBoneInfo->szName, pszName);
			MessageBox(NULL, strOutput, "错误", MB_OK);
            m_ExportError.AddError(strOutput);
            KGLOG_PROCESS_ERROR(false);
        }

        strncpy(pBoneInfo->szParentName, pszName, MAX_BONE_NAME_LENGTH);
        pBoneInfo->szParentName[MAX_BONE_NAME_LENGTH - 1] = '\0';


        pBoneInfo->NumVerticesRef = it->Vertices.size();
        if (pBoneInfo->NumVerticesRef > 0)
        {
            pBoneInfo->VerticesIndexRef = new DWORD[pBoneInfo->NumVerticesRef];
            KGLOG_PROCESS_ERROR(pBoneInfo->VerticesIndexRef);
            memset(pBoneInfo->VerticesIndexRef, 0, sizeof(DWORD) * pBoneInfo->NumVerticesRef);

            pBoneInfo->VerticesWeightRef = new float[pBoneInfo->NumVerticesRef];
            KGLOG_PROCESS_ERROR(pBoneInfo->VerticesWeightRef);
            memset(pBoneInfo->VerticesWeightRef, 0, sizeof(float) * pBoneInfo->NumVerticesRef);

            for (i = 0; i < pBoneInfo->NumVerticesRef; i++)
            {
                pBoneInfo->VerticesIndexRef[i]  = it->Vertices[i].nIndex;
                pBoneInfo->VerticesWeightRef[i] = it->Vertices[i].fWeight;
            }
        }
        

        pBoneInfo->MatrixOffset   = it->MatrixOffset;
        pBoneInfo->BaseBoneMatrix = it->BaseBoneMatrix;


        pBoneInfo->NumChild = it->Children.size();
        if (pBoneInfo->NumChild > 0)
        {
            pBoneInfo->lpChildNames = new LPSTR[pBoneInfo->NumChild];
            KGLOG_PROCESS_ERROR(pBoneInfo->lpChildNames);
            memset(pBoneInfo->lpChildNames, 0, sizeof(LPSTR) * pBoneInfo->NumChild);

            for (i = 0; i < pBoneInfo->NumChild; i++)
            {
                pszName = it->Children[i]->GetName();
                KGLOG_PROCESS_ERROR(pszName);
                nRetCode = strlen(pszName);
                if (nRetCode >= MAX_BONE_NAME_LENGTH)
                {
                    CStr strOutput;
                    strOutput.printf("骨骼[%s]子骨骼[%s]名字过长", pBoneInfo->szName, pszName);
					MessageBox(NULL, strOutput, "错误", MB_OK);
                    m_ExportError.AddError(strOutput);
                    KGLOG_PROCESS_ERROR(nRetCode < MAX_BONE_NAME_LENGTH);
                }

                pBoneInfo->lpChildNames[i] = new TCHAR[MAX_BONE_NAME_LENGTH];
                KGLOG_PROCESS_ERROR(pBoneInfo->lpChildNames[i]);
                
                strncpy(pBoneInfo->lpChildNames[i], pszName, MAX_BONE_NAME_LENGTH);
                pBoneInfo->lpChildNames[i][MAX_BONE_NAME_LENGTH - 1] = '\0';
            }
        }
    }


    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pMesh)
        {
            if (pMesh->m_lpBoneInfo)
            {
                for (int i = 0; i < pMesh->m_dNumBone; i++)
                {
                    if (pMesh->m_lpBoneInfo[i].lpChildNames)
                    {
                        for (int j = 0; j < pMesh->m_lpBoneInfo[i].NumChild; j++)
                        {
                            KG_DELETE_ARRAY(pMesh->m_lpBoneInfo[i].lpChildNames[j]);
                        }
                        KG_DELETE_ARRAY(pMesh->m_lpBoneInfo[i].lpChildNames);
                    }
                }
                KG_DELETE_ARRAY(pMesh->m_lpBoneInfo[i].VerticesWeightRef);
                KG_DELETE_ARRAY(pMesh->m_lpBoneInfo[i].VerticesIndexRef);
            }

            KG_DELETE_ARRAY(pMesh->m_lpBoneInfo);
        }
    }
    return hrResult;
}

HRESULT AsciiExp::ExportSkinInfo1(INode* node, LPMESH pMesh)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

	HRESULT hr = S_OK;
    KG_BONE_CONTAINER_TABLE     BoneContainerTable;
    KG_VERTICESSKININFO_VECTOR  VerticesSkinInfoVector;
    KG_3D_EXPORT_BONE_VECTOR    ExportBoneVector;

    hrRetCode = ExportSkinInfo1_Skin(
        node, &VerticesSkinInfoVector, &BoneContainerTable
    );
	hrResult = hrRetCode;
	KG_PROCESS_ERROR(hrRetCode != S_FALSE);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

    hrRetCode = ExportSkinInfo1_ProcessBoneBaseInfo(
        BoneContainerTable,
        &ExportBoneVector
    );
    KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

    hrRetCode = ExportSkinInfo1_TransVertexWeightToBone(
        pMesh, 
        VerticesSkinInfoVector, 
        BoneContainerTable, 
        &ExportBoneVector
    );
    KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

    hrRetCode = ExportSkinInfo1_SetDestMesh(
        pMesh,
        ExportBoneVector
    );
    KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

    hrRetCode = ExportSkinInfo1_ExportSocketToMesh(
        pMesh,
        ExportBoneVector
    );
    KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);


    hrResult = S_OK;
Exit0:
	return hrResult;
}
/****************************************************************************

  Mesh output
  
****************************************************************************/

HRESULT AsciiExp::ExportMesh(INode* node,LPMESH pMesh,TimeValue t, int indentLevel)
{	
	HRESULT hrResult  = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	
	BOOL needDel = FALSE;
	Mesh *pTempMesh = NULL;
	int nNumFaces = 0;
	int nNumVertics = 0;
	ObjectState os = node->EvalWorldState(t);
	Matrix3 tm;
	TSTR strNodeName;
	TCHAR strOutputInfo[MAX_PATH * 2];
	TriObject* pTriObject = NULL;

	strOutputInfo[0] = '\0';
	if (!os.obj || os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID) 
		goto Exit0; 
		
	tm = node->GetObjTMAfterWSM(t);
	//////////////////////////////////////////////////////////////////////////
	//Add 4-14-2005 Huangshan
	//Purpose: To retrieve EditNormals modifier data
	theHold.Begin();
	m_ip->SelectNode(node);
	theHold.Accept("");
	//Add 4-14-2005 Huangshan end
	//////////////////////////////////////////////////////////////////////////
	
	pTriObject = GetTriObjectFromNode(node, t, needDel);
	KGLOG_PROCESS_ERROR(pTriObject);
			
	pTempMesh = &pTriObject->GetMesh();
	KGLOG_PROCESS_ERROR(pTempMesh);
		
	pTempMesh->buildNormals();

	nNumFaces = pTempMesh->getNumFaces();
	nNumVertics = pTempMesh->getNumVerts();
	
	if(nNumFaces == 0 || nNumVertics == 0)
	{
		strNodeName = node->NodeName();
		sprintf(strOutputInfo, "物件%s没有顶点或者面，MAX有可能要死了", strNodeName);
		MessageBox(m_ip->GetMAXHWnd(), strOutputInfo, "错误", MB_OK);
		goto Exit0;
	}

	hrRetCode = ExchangeMeshData(pMesh,node,pTempMesh,NULL,t,tm);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	
	hrResult = S_OK;
Exit0:

	if (needDel) 
	{
	    KG_DELETE(pTriObject);
		needDel = false;
    }
	return hrResult;
}

Point3 AsciiExp::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;
	
	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) {
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) {
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) {
			vertexNormal = rv->rn.getNormal();
		}
		else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) {
				if (rv->ern[i].getSmGroup() & smGroup) {
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else {
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	
	return vertexNormal;
}

/****************************************************************************

  Inverse Kinematics (IK) Joint information
  
****************************************************************************/

void AsciiExp::ExportIKJoints(INode* node, int indentLevel)
{
/*	Control* cont;
	TSTR indent = GetIndent(indentLevel);

	if (node->TestAFlag(A_INODE_IK_TERMINATOR)) 
		fprintf(pStream,"%s\t%s\n", indent.data(), ID_IKTERMINATOR);

	if(node->TestAFlag(A_INODE_IK_POS_PINNED))
		fprintf(pStream,"%s\t%s\n", indent.data(), ID_IKPOS_PINNED);

	if(node->TestAFlag(A_INODE_IK_ROT_PINNED))
		fprintf(pStream,"%s\t%s\n", indent.data(), ID_IKROT_PINNED);

	// Position joint
	cont = node->GetTMController()->GetPositionController();
	if (cont) {
		JointParams* joint = (JointParams*)cont->GetProperty(PROPID_JOINTPARAMS);
		if (joint && !joint->IsDefault()) {
			// Has IK Joints!!!
			fprintf(pStream,"%s\t%s {\n", indent.data(), ID_IKJOINT);
			DumpJointParams(joint, indentLevel+1);
			fprintf(pStream,"%s\t}\n", indent.data());
		}
	}

	// Rotational joint
	cont = node->GetTMController()->GetRotationController();
	if (cont) {
		JointParams* joint = (JointParams*)cont->GetProperty(PROPID_JOINTPARAMS);
		if (joint && !joint->IsDefault()) {
			// Has IK Joints!!!
			fprintf(pStream,"%s\t%s {\n", indent.data(), ID_IKJOINT);
			DumpJointParams(joint, indentLevel+1);
			fprintf(pStream,"%s\t}\n", indent.data());
		}
	}*/
}

void AsciiExp::DumpJointParams(JointParams* joint, int indentLevel)
{
/*	TSTR indent = GetIndent(indentLevel);
	float scale = joint->scale;

	fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_IKTYPE,   joint->Type() == JNT_POS ? ID_IKTYPEPOS : ID_IKTYPEROT);
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKDOF,    joint->dofs);

	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKXACTIVE,  joint->flags & JNT_XACTIVE  ? 1 : 0);
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKYACTIVE,  joint->flags & JNT_YACTIVE  ? 1 : 0);
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKZACTIVE,  joint->flags & JNT_ZACTIVE  ? 1 : 0);

	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKXLIMITED, joint->flags & JNT_XLIMITED ? 1 : 0);
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKYLIMITED, joint->flags & JNT_YLIMITED ? 1 : 0);
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKZLIMITED, joint->flags & JNT_ZLIMITED ? 1 : 0);

	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKXEASE,    joint->flags & JNT_XEASE    ? 1 : 0);
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKYEASE,    joint->flags & JNT_YEASE    ? 1 : 0);
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKZEASE,    joint->flags & JNT_ZEASE    ? 1 : 0);

	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_IKLIMITEXACT, joint->flags & JNT_LIMITEXACT ? 1 : 0);

	for (int i=0; i<joint->dofs; i++) {
		fprintf(pStream,"%s\t%s %d %s %s %s\n", indent.data(), ID_IKJOINTINFO, i, Format(joint->min[i]), Format(joint->max[i]), Format(joint->damping[i]));
	}
*/
}

/****************************************************************************

  Material and Texture Export
  
****************************************************************************/

void AsciiExp::ExportMaterialList()
{
/*	if (!GetIncludeMtl()) {
		return;
	}

	fprintf(pStream, "%s {\n", ID_MATERIAL_LIST);

	int numMtls = mtlList.Count();
	fprintf(pStream, "\t%s %d\n", ID_MATERIAL_COUNT, numMtls);

	for (int i=0; i<numMtls; i++) {
		DumpMaterial(mtlList.GetMtl(i), i, -1, 0);
	}

	fprintf(pStream, "}\n");*/
}

void AsciiExp::ExportMaterial(INode* node,LPMESH pMesh, int indentLevel)
{
    HRESULT hrResult = E_FAIL;
    TSTR indent = GetIndent(indentLevel);

    KGLOG_PROCESS_ERROR(node);
    KGLOG_PROCESS_ERROR(pMesh);

    pMesh->m_lpMaterial = new CMesh::_Material[pMesh->m_dNumSubsets];
    KGLOG_PROCESS_ERROR(pMesh->m_lpMaterial);

    ZeroMemory(pMesh->m_lpMaterial, sizeof(CMesh::_Material) * pMesh->m_dNumSubsets);

	Mtl* mtl = node->GetMtl();
	
	// If the node does not have a material, export the wireframe color

	//如果物体还没有赋上材质，我们需要按照物体部分的数目创建一个材质的数组，
	//并且把默认的线框颜色赋给它
	if (!mtl)
	{
		DWORD c = node->GetWireColor();
		for(DWORD i = 0; i < pMesh->m_dNumSubsets; i++)
		{
			pMesh->m_lpMaterial[i].Ambient.b = ((c & 0x00ff0000)>>16)/255.0f;
			pMesh->m_lpMaterial[i].Ambient.g = ((c & 0x0000ff00) >>8)/255.0f;
			pMesh->m_lpMaterial[i].Ambient.r = (c & 0x000000ff)/255.0f;
			pMesh->m_lpMaterial[i].Ambient.a = 1.0f;
			pMesh->m_lpMaterial[i].Diffuse = pMesh->m_lpMaterial[i].Ambient;
			pMesh->m_lpMaterial[i].Specular = pMesh->m_lpMaterial[i].Ambient;
		}
		goto Exit1;
	}

	//如果该材质的子物体是0，说明信息都保存在该材质中，否则本材质中不包含信息，存储在子材质里面
	int Num = mtl->NumSubMtls();
	if(Num <= 0) 
	{
		Num = 1;
		Color color = mtl->GetAmbient();
		float Alpha = 1 - mtl->GetXParency();
		pMesh->m_lpMaterial[0].Ambient.r = color.r; 
		pMesh->m_lpMaterial[0].Ambient.g = color.g; 
		pMesh->m_lpMaterial[0].Ambient.b = color.b; 
		pMesh->m_lpMaterial[0].Ambient.a = Alpha; 
		color = mtl->GetDiffuse();
		pMesh->m_lpMaterial[0].Diffuse.r = color.r; 
		pMesh->m_lpMaterial[0].Diffuse.g = color.g; 
		pMesh->m_lpMaterial[0].Diffuse.b = color.b; 
		pMesh->m_lpMaterial[0].Diffuse.a = Alpha; 
		color = mtl->GetSpecular();
		pMesh->m_lpMaterial[0].Specular.r = color.r; 
		pMesh->m_lpMaterial[0].Specular.g = color.g; 
		pMesh->m_lpMaterial[0].Specular.b = color.b; 
		pMesh->m_lpMaterial[0].Specular.a = Alpha; 

		if(mtl->GetShinStr() >= 10)
			pMesh->m_lpMaterial[0].Power = 1 / mtl->GetShinStr();
		else
			pMesh->m_lpMaterial[0].Power = 0;
		
		DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
						   MATERIAL_OPTION_SHADE_GOURAUD|
						   MATERIAL_OPTION_SPECULARENABLE|
						   MATERIAL_OPTION_LIGHTING;

		if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
		{
			StdMat* std = (StdMat*)mtl;
			if (std->GetTwoSided())
			{
				Def_Option |= MATERIAL_OPTION_CULL_NONE;
			}
			else
			{
				Def_Option |= MATERIAL_OPTION_CULL_CW;
			}

			if (std->GetWire()) 
			{
				Def_Option |= MATERIAL_OPTION_FILL_WIREFRAME;
			}
			else
			{
				Def_Option |= MATERIAL_OPTION_FILL_SOLID;
			}
		}

		pMesh->m_lpMaterial[0].Option = Def_Option;

		//导出帖图信息
		//首先确定一下帖图的层数
		DWORD NumTexture = 0;
		for (int i=0; i < mtl->NumSubTexmaps(); i++) 
		{
			Texmap* subTex = mtl->GetSubTexmap(i);
			if(subTex) 
			{
				if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
					if (!((StdMat*)mtl)->MapEnabled(i))
						continue;
				}
				NumTexture++;
			}
		}
		//为帖图分配空间
		pMesh->m_lpMaterial[0].NumTexture = NumTexture;
        pMesh->m_lpMaterial[0].Textures   = NULL;
        if (NumTexture > 0)
        {
		    pMesh->m_lpMaterial[0].Textures = new CMesh::TEXTURE[NumTexture];
		    ZeroMemory(pMesh->m_lpMaterial[0].Textures,sizeof(CMesh::TEXTURE)*NumTexture);
        }

		NumTexture = 0;
		for (i = 0; i < mtl->NumSubTexmaps(); i++) 
		{
			Texmap* subTex = mtl->GetSubTexmap(i);
			if (subTex)
			{
				float amt = 1.0f;
				if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
					if (!((StdMat*)mtl)->MapEnabled(i))
						continue;
					amt = ((StdMat*)mtl)->GetTexmapAmt(i, 0);
				}

				pMesh->m_lpMaterial[0].Textures[NumTexture].TextureType = new TCHAR[20];
				wsprintf(pMesh->m_lpMaterial[0].Textures[NumTexture].TextureType,"%s",
					GetMapID(mtl->ClassID(), i));
				pMesh->m_lpMaterial[0].Textures[NumTexture].FileName = new TCHAR[256];
				wsprintf(pMesh->m_lpMaterial[0].Textures[NumTexture].FileName,"");
				
				Matrix3 UVtrans;
				subTex->GetUVTransform(UVtrans);
				Point3 Row = UVtrans.GetRow(0);
				float Us = Row.x ;
				Row = UVtrans.GetRow(1);
				float Vs = Row.y ;
				Row = UVtrans.GetRow(2);
				float Ws = Row.z ;

				if((Us!=1.0f)||(Vs!=1.0f)||(Ws!=1.0f))
				{
					pMesh->m_lpMaterial[0].NumTextureOption[NumTexture] = 1;
					
					pMesh->m_lpMaterial[0].TexturesOption[NumTexture] = new CMesh::_Option[1];
					pMesh->m_lpMaterial[0].TexturesOption[NumTexture][0].Type = OPTION_TEXTURE_TRANSFROM;
					CMesh::_TextureTf* pTextureTF = new CMesh::_TextureTf;
					ZeroMemory(pTextureTF,sizeof(CMesh::_TextureTf));
					pTextureTF->Scaling.x = Us;
					pTextureTF->Scaling.y = Vs;
					pTextureTF->Scaling.z = Ws;
					pMesh->m_lpMaterial[0].TexturesOption[NumTexture][0].pData = pTextureTF;
				}

				if (subTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
				{
					TSTR mapName = ((BitmapTex *)subTex)->GetMapName();
					wsprintf(pMesh->m_lpMaterial[0].Textures[NumTexture].FileName,"%s",
						mapName);
				}
				pMesh->m_lpMaterial[0].Textures[NumTexture].Amount = amt;
				NumTexture++;
			}
		}

		for( i=1; i<pMesh->m_dNumSubsets; i++)
		{
			pMesh->m_lpMaterial[i].Ambient = pMesh->m_lpMaterial[0].Ambient; 
			pMesh->m_lpMaterial[i].Diffuse = pMesh->m_lpMaterial[0].Diffuse; 
			pMesh->m_lpMaterial[i].Specular = pMesh->m_lpMaterial[0].Specular; 
			pMesh->m_lpMaterial[i].Power = pMesh->m_lpMaterial[0].Power; 
			pMesh->m_lpMaterial[i].Option = pMesh->m_lpMaterial[0].Option; 
			pMesh->m_lpMaterial[i].NumTexture = pMesh->m_lpMaterial[0].NumTexture;
            pMesh->m_lpMaterial[i].Textures   = NULL;
            if (pMesh->m_lpMaterial[0].NumTexture > 0)
            {
			    pMesh->m_lpMaterial[i].Textures = new CMesh::TEXTURE[pMesh->m_lpMaterial[i].NumTexture];
            }
		
			for(int j=0;j<pMesh->m_lpMaterial[i].NumTexture;j++)
			{
				pMesh->m_lpMaterial[i].Textures[j].Amount =
					pMesh->m_lpMaterial[0].Textures[j].Amount ;
				pMesh->m_lpMaterial[i].Textures[j].FileName = new TCHAR[256]; 
				pMesh->m_lpMaterial[i].Textures[j].TextureType = new TCHAR[20];
				
				wsprintf(pMesh->m_lpMaterial[i].Textures[j].FileName,"%s",
					pMesh->m_lpMaterial[0].Textures[j].FileName);
				wsprintf(pMesh->m_lpMaterial[i].Textures[j].TextureType,"%s",
					pMesh->m_lpMaterial[0].Textures[j].TextureType) ;
				
				pMesh->m_lpMaterial[i].NumTextureOption[j] = 
					pMesh->m_lpMaterial[0].NumTextureOption[j];

				if(pMesh->m_lpMaterial[i].NumTextureOption[j]>0)
				{
					pMesh->m_lpMaterial[i].TexturesOption[j] = new CMesh::_Option[1];
					CMesh::LPOPTION pDestOption = &pMesh->m_lpMaterial[i].TexturesOption[j][0];

					pDestOption->Type = OPTION_TEXTURE_TRANSFROM;
					CMesh::_TextureTf* pTextureTF = new CMesh::_TextureTf;
					ZeroMemory(pTextureTF,sizeof(CMesh::_TextureTf));
					CMesh::_TextureTf* pSrcTextureTF = (CMesh::_TextureTf*) 
						pMesh->m_lpMaterial[0].TexturesOption[j][0].pData;
					
					pTextureTF->Scaling.x = pSrcTextureTF->Scaling.x;
					pTextureTF->Scaling.y = pSrcTextureTF->Scaling.y;
					pTextureTF->Scaling.z = pSrcTextureTF->Scaling.z;

					pDestOption->pData = pTextureTF;
				}
			}
		}
	}
	else
	{
		DWORD MNum = mtl->NumSubMtls();
		Num = pMesh->m_dNumSubsets;
		if(Num>MNum) Num = MNum;

		for(DWORD i_Sub=0;i_Sub<Num;i_Sub++)
		{
			Mtl* pSubMaterial = mtl->GetSubMtl(i_Sub); 
			if(!pSubMaterial) continue;
			Color color = pSubMaterial->GetAmbient();
			float Alpha = 1 - pSubMaterial->GetXParency();
			pMesh->m_lpMaterial[i_Sub].Ambient.r = color.r; 
			pMesh->m_lpMaterial[i_Sub].Ambient.g = color.g; 
			pMesh->m_lpMaterial[i_Sub].Ambient.b = color.b; 
			pMesh->m_lpMaterial[i_Sub].Ambient.a = Alpha; 
			color = pSubMaterial->GetDiffuse();
			pMesh->m_lpMaterial[i_Sub].Diffuse.r = color.r; 
			pMesh->m_lpMaterial[i_Sub].Diffuse.g = color.g; 
			pMesh->m_lpMaterial[i_Sub].Diffuse.b = color.b; 
			pMesh->m_lpMaterial[i_Sub].Diffuse.a = Alpha; 
			color = pSubMaterial->GetSpecular();
			pMesh->m_lpMaterial[i_Sub].Specular.r = color.r; 
			pMesh->m_lpMaterial[i_Sub].Specular.g = color.g; 
			pMesh->m_lpMaterial[i_Sub].Specular.b = color.b; 
			pMesh->m_lpMaterial[i_Sub].Specular.a = Alpha; 
			
			if(pSubMaterial->GetShinStr()>=10)
				pMesh->m_lpMaterial[i_Sub].Power = 1/pSubMaterial->GetShinStr();
			else
				pMesh->m_lpMaterial[i_Sub].Power = 0;

			DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
				MATERIAL_OPTION_SHADE_GOURAUD|
				MATERIAL_OPTION_SPECULARENABLE|
				MATERIAL_OPTION_LIGHTING;
			if (pSubMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
			{
				StdMat* std = (StdMat*)pSubMaterial;
				if (std->GetTwoSided())
				{
					Def_Option |= MATERIAL_OPTION_CULL_NONE;
				}
				else
				{
					Def_Option |= MATERIAL_OPTION_CULL_CW;
				}
				
				if (std->GetWire()) 
				{
					Def_Option |= MATERIAL_OPTION_FILL_WIREFRAME;
				}
				else
				{
					Def_Option |= MATERIAL_OPTION_FILL_SOLID;
				}
			}
			
			pMesh->m_lpMaterial[i_Sub].Option = Def_Option;
			//导出帖图信息
			//首先确定一下帖图的层数
			DWORD NumTexture = 0;
			for (int i=0; i<pSubMaterial->NumSubTexmaps(); i++) 
			{
				Texmap* subTex = pSubMaterial->GetSubTexmap(i);
				if(subTex) 
				{
					if (pSubMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
						if (!((StdMat*)pSubMaterial)->MapEnabled(i))
							continue;
					}
					NumTexture++;
				}
			}
			//为帖图分配空间
			pMesh->m_lpMaterial[i_Sub].NumTexture = NumTexture;
			pMesh->m_lpMaterial[i_Sub].Textures = new CMesh::TEXTURE[NumTexture];
			ZeroMemory(pMesh->m_lpMaterial[i_Sub].Textures,sizeof(CMesh::TEXTURE)*NumTexture);
		
			NumTexture = 0;
			for ( i=0; i<pSubMaterial->NumSubTexmaps(); i++) 
			{
				Texmap* subTex = pSubMaterial->GetSubTexmap(i);
				if (subTex)
				{
					float amt = 1.0f;
					if (pSubMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
						if (!((StdMat*)pSubMaterial)->MapEnabled(i))
							continue;
						amt = ((StdMat*)pSubMaterial)->GetTexmapAmt(i, 0);
					}

					pMesh->m_lpMaterial[i_Sub].Textures[NumTexture].TextureType = new TCHAR[20];
					wsprintf(pMesh->m_lpMaterial[i_Sub].Textures[NumTexture].TextureType,"%s",
						GetMapID(pSubMaterial->ClassID(), i));

					pMesh->m_lpMaterial[i_Sub].Textures[NumTexture].FileName = new TCHAR[256];
					wsprintf(pMesh->m_lpMaterial[i_Sub].Textures[NumTexture].FileName,"");

					Matrix3 UVtrans;
					subTex->GetUVTransform(UVtrans);

					Point3 Row = UVtrans.GetRow(0);
					float Us = Row.x ;
					Row = UVtrans.GetRow(1);
					float Vs = Row.y ;
					Row = UVtrans.GetRow(2);
					float Ws = Row.z ;
					
					if((Us!=1.0f)||(Vs!=1.0f)||(Ws!=1.0f))
					{
						pMesh->m_lpMaterial[i_Sub].NumTextureOption[NumTexture] = 1;
						pMesh->m_lpMaterial[i_Sub].TexturesOption[NumTexture] = new CMesh::_Option[1];
						pMesh->m_lpMaterial[i_Sub].TexturesOption[NumTexture][0].Type = OPTION_TEXTURE_TRANSFROM;
						CMesh::_TextureTf* pTextureTF = new CMesh::_TextureTf;
						ZeroMemory(pTextureTF,sizeof(CMesh::_TextureTf));
						pTextureTF->Scaling.x = Us;
						pTextureTF->Scaling.y = Vs;
						pTextureTF->Scaling.z = Ws;
						pMesh->m_lpMaterial[i_Sub].TexturesOption[NumTexture][0].pData = 
							pTextureTF;
					}
					
					if (subTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
					{
						TSTR mapName = ((BitmapTex *)subTex)->GetMapName();
						wsprintf(pMesh->m_lpMaterial[i_Sub].Textures[NumTexture].FileName,"%s",
							mapName);
					}
					pMesh->m_lpMaterial[i_Sub].Textures[NumTexture].Amount = amt;
					NumTexture++;
				}	
			}
		}
	}
Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(pMesh->m_lpMaterial);
    }
}

void AsciiExp::DumpMaterial(Mtl* mtl, int mtlID, int subNo, int indentLevel)
{
/*	int i;
	TimeValue t = GetStaticFrame();
	
	if (!mtl) return;
	
	TSTR indent = GetIndent(indentLevel+1);
	
	TSTR className;
	mtl->GetClassName(className);
	
	
	if (subNo == -1) {
		// Top level material
		//fprintf(pStream,"%s%s %d {\n",indent.data(), ID_MATERIAL, mtlID);
	}
	else {
		//fprintf(pStream,"%s%s %d {\n",indent.data(), ID_SUBMATERIAL, subNo);
	}
	//fprintf(pStream,"%s\t%s \"%s\"\n",indent.data(), ID_MATNAME, FixupName(mtl->GetName()));
	//fprintf(pStream,"%s\t%s \"%s\"\n",indent.data(), ID_MATCLASS, FixupName(className));
	
	// We know the Standard material, so we can get some extra info
/*	if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
		StdMat* std = (StdMat*)mtl;

		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_AMBIENT, Format(std->GetAmbient(t)));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_DIFFUSE, Format(std->GetDiffuse(t)));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_SPECULAR, Format(std->GetSpecular(t)));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_SHINE, Format(std->GetShininess(t)));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_SHINE_STRENGTH, Format(std->GetShinStr(t)));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_TRANSPARENCY, Format(std->GetXParency(t)));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_WIRESIZE, Format(std->WireSize(t)));

		fprintf(pStream,"%s\t%s ", indent.data(), ID_SHADING);
		switch(std->GetShading()) {
		case SHADE_CONST: fprintf(pStream,"%s\n", ID_MAT_SHADE_CONST); break;
		case SHADE_PHONG: fprintf(pStream,"%s\n", ID_MAT_SHADE_PHONG); break;
		case SHADE_METAL: fprintf(pStream,"%s\n", ID_MAT_SHADE_METAL); break;
		case SHADE_BLINN: fprintf(pStream,"%s\n", ID_MAT_SHADE_BLINN); break;
		default: fprintf(pStream,"%s\n", ID_MAT_SHADE_OTHER); break;
		}
		
		fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_XP_FALLOFF, Format(std->GetOpacFalloff(t)));
		fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_SELFILLUM, Format(std->GetSelfIllum(t)));
		
		if (std->GetTwoSided()) {
			fprintf(pStream,"%s\t%s\n", indent.data(), ID_TWOSIDED);
		}
		
		if (std->GetWire()) {
			fprintf(pStream,"%s\t%s\n", indent.data(), ID_WIRE);
		}
		
		if (std->GetWireUnits()) {
			fprintf(pStream,"%s\t%s\n", indent.data(), ID_WIREUNITS);
		}
		
		fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_FALLOFF, std->GetFalloffOut() ? ID_FALLOFF_OUT : ID_FALLOFF_IN);
		
		if (std->GetFaceMap()) {
			fprintf(pStream,"%s\t%s\n", indent.data(), ID_FACEMAP);
		}
		
		if (std->GetSoften()) {
			fprintf(pStream,"%s\t%s\n", indent.data(), ID_SOFTEN);
		}
		
		fprintf(pStream,"%s\t%s ", indent.data(), ID_XP_TYPE);
		switch (std->GetTransparencyType()) {
		case TRANSP_FILTER: fprintf(pStream,"%s\n", ID_MAP_XPTYPE_FLT); break;
		case TRANSP_SUBTRACTIVE: fprintf(pStream,"%s\n", ID_MAP_XPTYPE_SUB); break;
		case TRANSP_ADDITIVE: fprintf(pStream,"%s\n", ID_MAP_XPTYPE_ADD); break;
		default: fprintf(pStream,"%s\n", ID_MAP_XPTYPE_OTH); break;
		}
	}
	else {
		// Note about material colors:
		// This is only the color used by the interactive renderer in MAX.
		// To get the color used by the scanline renderer, we need to evaluate
		// the material using the mtl->Shade() method.
		// Since the materials are procedural there is no real "diffuse" color for a MAX material
		// but we can at least take the interactive color.
		
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_AMBIENT, Format(mtl->GetAmbient()));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_DIFFUSE, Format(mtl->GetDiffuse()));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_SPECULAR, Format(mtl->GetSpecular()));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_SHINE, Format(mtl->GetShininess()));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_SHINE_STRENGTH, Format(mtl->GetShinStr()));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_TRANSPARENCY, Format(mtl->GetXParency()));
		fprintf(pStream,"%s\t%s %s\n",indent.data(), ID_WIRESIZE, Format(mtl->WireSize()));
	}

	for (i=0; i<mtl->NumSubTexmaps(); i++) {
		Texmap* subTex = mtl->GetSubTexmap(i);
		float amt = 1.0f;
		if (subTex) {
			// If it is a standard material we can see if the map is enabled.
			if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
				if (!((StdMat*)mtl)->MapEnabled(i))
					continue;
				amt = ((StdMat*)mtl)->GetTexmapAmt(i, 0);
				
			}
			DumpTexture(subTex, mtl->ClassID(), i, amt, indentLevel+1);
		}
	}
	
	if (mtl->NumSubMtls() > 0)  {
		fprintf(pStream,"%s\t%s %d\n",indent.data(), ID_NUMSUBMTLS, mtl->NumSubMtls());
		
		for (i=0; i<mtl->NumSubMtls(); i++) {
			Mtl* subMtl = mtl->GetSubMtl(i);
			if (subMtl) {
				DumpMaterial(subMtl, 0, i, indentLevel+1);
			}
		}
	}
	fprintf(pStream,"%s}\n", indent.data());*/
}


// For a standard material, this will give us the meaning of a map
// givien its submap id.
TCHAR* AsciiExp::GetMapID(Class_ID cid, int subNo)
{
	static TCHAR buf[50];
	
	if (cid == Class_ID(0,0)) {
		strcpy(buf, ID_ENVMAP);
	}
	else if (cid == Class_ID(DMTL_CLASS_ID, 0)) {
		switch (subNo) {
		case ID_AM: strcpy(buf, ID_MAP_AMBIENT); break;
		case ID_DI: strcpy(buf, ID_MAP_DIFFUSE); break;
		case ID_SP: strcpy(buf, ID_MAP_SPECULAR); break;
		case ID_SH: strcpy(buf, ID_MAP_SHINE); break;
		case ID_SS: strcpy(buf, ID_MAP_SHINESTRENGTH); break;
		case ID_SI: strcpy(buf, ID_MAP_SELFILLUM); break;
		case ID_OP: strcpy(buf, ID_MAP_OPACITY); break;
		case ID_FI: strcpy(buf, ID_MAP_FILTERCOLOR); break;
		case ID_BU: strcpy(buf, ID_MAP_BUMP); break;
		case ID_RL: strcpy(buf, ID_MAP_REFLECT); break;
		case ID_RR: strcpy(buf, ID_MAP_REFRACT); break;
		}
	}
	else {
		strcpy(buf, ID_MAP_GENERIC);
	}
	
	return buf;
}

void AsciiExp::DumpTexture(Texmap* tex, Class_ID cid, int subNo, float amt, int indentLevel)
{
	/*if (!tex) return;
	
	TSTR indent = GetIndent(indentLevel+1);
	
	TSTR className;
	tex->GetClassName(className);
	
	fprintf(pStream,"%s%s {\n", indent.data(), GetMapID(cid, subNo));
	
	fprintf(pStream,"%s\t%s \"%s\"\n", indent.data(), ID_TEXNAME, FixupName(tex->GetName()));
	fprintf(pStream,"%s\t%s \"%s\"\n", indent.data(), ID_TEXCLASS, FixupName(className));
	
	// If we include the subtexture ID, a parser could be smart enough to get
	// the class name of the parent texture/material and make it mean something.
	fprintf(pStream,"%s\t%s %d\n", indent.data(), ID_TEXSUBNO, subNo);
	
	fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_TEXAMOUNT, Format(amt));
	
	// Is this a bitmap texture?
	// We know some extra bits 'n pieces about the bitmap texture
	if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)) {
		TSTR mapName = ((BitmapTex *)tex)->GetMapName();
		fprintf(pStream,"%s\t%s \"%s\"\n", indent.data(), ID_BITMAP, FixupName(mapName));
		
		StdUVGen* uvGen = ((BitmapTex *)tex)->GetUVGen();
		if (uvGen) {
			DumpUVGen(uvGen, indentLevel+1);
		}
		
		TextureOutput* texout = ((BitmapTex*)tex)->GetTexout();
		if (texout->GetInvert()) {
			fprintf(pStream,"%s\t%s\n", indent.data(), ID_TEX_INVERT);
		}
		
		fprintf(pStream,"%s\t%s ", indent.data(), ID_BMP_FILTER);
		switch(((BitmapTex*)tex)->GetFilterType()) {
		case FILTER_PYR:  fprintf(pStream,"%s\n", ID_BMP_FILT_PYR); break;
		case FILTER_SAT: fprintf(pStream,"%s\n", ID_BMP_FILT_SAT); break;
		default: fprintf(pStream,"%s\n", ID_BMP_FILT_NONE); break;
		}
	}
	
	for (int i=0; i<tex->NumSubTexmaps(); i++) {
		DumpTexture(tex->GetSubTexmap(i), tex->ClassID(), i, 1.0f, indentLevel+1);
	}
	
	fprintf(pStream, "%s}\n", indent.data());*/
}

void AsciiExp::DumpUVGen(StdUVGen* uvGen, int indentLevel)
{
	/*int mapType = uvGen->GetCoordMapping(0);
	TimeValue t = GetStaticFrame();
	TSTR indent = GetIndent(indentLevel+1);
	
	fprintf(pStream,"%s%s ", indent.data(), ID_MAPTYPE);
	
	switch (mapType) {
	case UVMAP_EXPLICIT: fprintf(pStream,"%s\n", ID_MAPTYPE_EXP); break;
	case UVMAP_SPHERE_ENV: fprintf(pStream,"%s\n", ID_MAPTYPE_SPH); break;
	case UVMAP_CYL_ENV:  fprintf(pStream,"%s\n", ID_MAPTYPE_CYL); break;
	case UVMAP_SHRINK_ENV: fprintf(pStream,"%s\n", ID_MAPTYPE_SHR); break;
	case UVMAP_SCREEN_ENV: fprintf(pStream,"%s\n", ID_MAPTYPE_SCR); break;
	}
	
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_U_OFFSET, Format(uvGen->GetUOffs(t)));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_V_OFFSET, Format(uvGen->GetVOffs(t)));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_U_TILING, Format(uvGen->GetUScl(t)));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_V_TILING, Format(uvGen->GetVScl(t)));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_ANGLE, Format(uvGen->GetAng(t)));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_BLUR, Format(uvGen->GetBlur(t)));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_BLUR_OFFSET, Format(uvGen->GetBlurOffs(t)));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_NOISE_AMT, Format(uvGen->GetNoiseAmt(t)));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_NOISE_SIZE, Format(uvGen->GetNoiseSize(t)));
	fprintf(pStream,"%s%s %d\n", indent.data(), ID_NOISE_LEVEL, uvGen->GetNoiseLev(t));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_NOISE_PHASE, Format(uvGen->GetNoisePhs(t)));*/
}

/****************************************************************************

  Face Mapped Material functions
  
****************************************************************************/

BOOL AsciiExp::CheckForAndExportFaceMap(Mtl* mtl, Mesh* mesh, int indentLevel)
{
	/*if (!mtl || !mesh) {
		return FALSE;
	}
	
	ULONG matreq = mtl->Requirements(-1);
	
	// Are we using face mapping?
	if (!(matreq & MTLREQ_FACEMAP)) {
		return FALSE;
	}
	
	TSTR indent = GetIndent(indentLevel+1);
	
	// OK, we have a FaceMap situation here...
	
	fprintf(pStream, "%s%s {\n", indent.data(), ID_MESH_FACEMAPLIST);
	for (int i=0; i<mesh->getNumFaces(); i++) {
		Point3 tv[3];
		Face* f = &mesh->faces[i];
		make_face_uv(f, tv);
		fprintf(pStream, "%s\t%s %d {\n", indent.data(), ID_MESH_FACEMAP, i);
		fprintf(pStream, "%s\t\t%s\t%d\t%d\t%d\n", indent.data(), ID_MESH_FACEVERT, (int)tv[0].x, (int)tv[0].y, (int)tv[0].z);
		fprintf(pStream, "%s\t\t%s\t%d\t%d\t%d\n", indent.data(), ID_MESH_FACEVERT, (int)tv[1].x, (int)tv[1].y, (int)tv[1].z);
		fprintf(pStream, "%s\t\t%s\t%d\t%d\t%d\n", indent.data(), ID_MESH_FACEVERT, (int)tv[2].x, (int)tv[2].y, (int)tv[2].z);
		fprintf(pStream, "%s\t}\n", indent.data());
	}
	fprintf(pStream, "%s}\n", indent.data());*/
	
	return TRUE;
}


/****************************************************************************

  Misc Utility functions
  
****************************************************************************/

// Return an indentation string
TSTR AsciiExp::GetIndent(int indentLevel)
{
	TSTR indentString = "";
	for (int i=0; i<indentLevel; i++) {
		indentString += "\t";
	}
	
	return indentString;
}

// Determine is the node has negative scaling.
// This is used for mirrored objects for example. They have a negative scale factor
// so when calculating the normal we should take the vertices counter clockwise.
// If we don't compensate for this the objects will be 'inverted'.
BOOL AsciiExp::TMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

// Return a pointer to a TriObject given an INode or return NULL
// if the node cannot be converted to a TriObject
TriObject* AsciiExp::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{ 
		TriObject *tri = (TriObject *) obj->ConvertToType(t, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else {
		return NULL;
	}
}

// Print out a transformation matrix in different ways.
// Apart from exporting the full matrix we also decompose
// the matrix and export the components.
void AsciiExp::DumpMatrix3(Matrix3* m, int indentLevel)
{
	/*Point3 row;
	TSTR indent = GetIndent(indentLevel);
	
	// Dump the whole Matrix
	row = m->GetRow(0);
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_ROW0, Format(row));
	row = m->GetRow(1);
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_ROW1, Format(row));
	row = m->GetRow(2);
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_ROW2, Format(row));
	row = m->GetRow(3);
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_ROW3, Format(row));
	
	// Decompose the matrix and dump the contents
	AffineParts ap;
	float rotAngle;
	Point3 rotAxis;
	float scaleAxAngle;
	Point3 scaleAxis;
	
	decomp_affine(*m, &ap);

	// Quaternions are dumped as angle axis.
	AngAxisFromQ(ap.q, &rotAngle, rotAxis);
	AngAxisFromQ(ap.u, &scaleAxAngle, scaleAxis);

	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_POS, Format(ap.t));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_ROTAXIS, Format(rotAxis));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_ROTANGLE, Format(rotAngle));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_SCALE, Format(ap.k));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_SCALEAXIS, Format(scaleAxis));
	fprintf(pStream,"%s%s %s\n", indent.data(), ID_TM_SCALEAXISANG, Format(scaleAxAngle));*/
}

// From the SDK
// How to calculate UV's for face mapped materials.
static Point3 basic_tva[3] = { 
	Point3(0.0,0.0,0.0),Point3(1.0,0.0,0.0),Point3(1.0,1.0,0.0)
};
static Point3 basic_tvb[3] = { 
	Point3(1.0,1.0,0.0),Point3(0.0,1.0,0.0),Point3(0.0,0.0,0.0)
};
static int nextpt[3] = {1,2,0};
static int prevpt[3] = {2,0,1};

void AsciiExp::make_face_uv(Face *f, Point3 *tv)
{
	int na,nhid,i;
	Point3 *basetv;
	/* make the invisible edge be 2->0 */
	nhid = 2;
	if (!(f->flags&EDGE_A))  nhid=0;
	else if (!(f->flags&EDGE_B)) nhid = 1;
	else if (!(f->flags&EDGE_C)) nhid = 2;
	na = 2-nhid;
	basetv = (f->v[prevpt[nhid]]<f->v[nhid]) ? basic_tva : basic_tvb; 
	for (i=0; i<3; i++) {  
		tv[i] = basetv[na];
		na = nextpt[na];
	}
}


/****************************************************************************

  String manipulation functions
  
****************************************************************************/

#define CTL_CHARS  31
#define SINGLE_QUOTE 39

// Replace some characters we don't care for.
TCHAR* AsciiExp::FixupName(TCHAR* name)
{
	static char buffer[256];
	TCHAR* cPtr;
	
    _tcscpy(buffer, name);
    cPtr = buffer;
	
    while(*cPtr) {
		if (*cPtr == '"')
			*cPtr = SINGLE_QUOTE;
        else if (*cPtr <= CTL_CHARS)
			*cPtr = _T('_');
        cPtr++;
    }
	
	return buffer;
}

// International settings in Windows could cause a number to be written
// with a "," instead of a ".".
// To compensate for this we need to convert all , to . in order to make the
// format consistent.
void AsciiExp::CommaScan(TCHAR* buf)
{
    for(; *buf; buf++) if (*buf == ',') *buf = '.';
}

TSTR AsciiExp::Format(int value)
{
	TCHAR buf[50];
	
	sprintf(buf, _T("%d"), value);
	return buf;
}


TSTR AsciiExp::Format(float value)
{
	TCHAR buf[40];
	
	sprintf(buf, szFmtStr, value);
	CommaScan(buf);
	return TSTR(buf);
}

TSTR AsciiExp::Format(Point3 value)
{
	TCHAR buf[120];
	TCHAR fmt[120];
	
	sprintf(fmt, "%s\t%s\t%s", szFmtStr, szFmtStr, szFmtStr);
	sprintf(buf, fmt, value.x, value.y, value.z);

	CommaScan(buf);
	return buf;
}

TSTR AsciiExp::Format(Color value)
{
	TCHAR buf[120];
	TCHAR fmt[120];
	
	sprintf(fmt, "%s\t%s\t%s", szFmtStr, szFmtStr, szFmtStr);
	sprintf(buf, fmt, value.r, value.g, value.b);

	CommaScan(buf);
	return buf;
}

TSTR AsciiExp::Format(AngAxis value)
{
	TCHAR buf[160];
	TCHAR fmt[160];
	
	sprintf(fmt, "%s\t%s\t%s\t%s", szFmtStr, szFmtStr, szFmtStr, szFmtStr);
	sprintf(buf, fmt, value.axis.x, value.axis.y, value.axis.z, value.angle);

	CommaScan(buf);
	return buf;
}


TSTR AsciiExp::Format(Quat value)
{
	// A Quat is converted to an AngAxis before output.
	
	Point3 axis;
	float angle;
	AngAxisFromQ(value, &angle, axis);
	
	return Format(AngAxis(axis, angle));
}

TSTR AsciiExp::Format(ScaleValue value)
{
	TCHAR buf[280];
	
	sprintf(buf, "%s %s", Format(value.s), Format(value.q));
	CommaScan(buf);
	return buf;
}

BOOL AsciiExp::CheckNodeSuffix(INode* pNode, const TCHAR* strSuffix)
{
	if (!pNode || !strSuffix)
		return FALSE;
	TSTR strNodeName = pNode->NodeName();
	size_t SuffixLength = strlen(strSuffix);
	size_t NodeNameLenght = strlen(strNodeName);
	if ( NodeNameLenght < SuffixLength )
		return FALSE;
	if ( !strcmp(strNodeName + NodeNameLenght - SuffixLength, strSuffix))
		return TRUE;
	return FALSE;
}


BOOL AsciiExp::ChangeSuffix(TCHAR* strSource, const TCHAR* strSourceSuffix, const TCHAR* strDestSuffix)
{
	//Here is a assumption, the max length of strSource is 255.
	size_t SuffixLength = strlen(strSourceSuffix);
	size_t NodeNameLenght = strlen(strSource);
	size_t DestSuffixLength = strlen(strDestSuffix);

	if ( NodeNameLenght < SuffixLength )
		return FALSE;
	if ( !strcmp(strSource + NodeNameLenght - SuffixLength, strSourceSuffix))
	{
		//May cause overflow
		size_t TotalLength = NodeNameLenght - SuffixLength + DestSuffixLength;
		if ( TotalLength >= 255 )
		{
			TCHAR* strDestSuffixFix = new TCHAR[DestSuffixLength];
			strDestSuffixFix[254 - NodeNameLenght + SuffixLength] = '\0';//Cut off those cause overflow
			strcpy(strSource + NodeNameLenght - SuffixLength, strDestSuffixFix);
		}
		else
            strcpy(strSource + NodeNameLenght - SuffixLength, strDestSuffix);
		return TRUE;
	}
	return FALSE;
}


