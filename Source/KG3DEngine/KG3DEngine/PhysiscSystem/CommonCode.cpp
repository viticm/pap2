// ===============================================================================
//						     PHYSX SDK TRAINING PROGRAMS
//			                     COMMON LESSON CODE
//
//						   Written by Bob Schade, 12-15-05
// ===============================================================================

#include "CommonCode.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0,-9.8,0);

// User report globals
DebugRenderer     gDebugRenderer;
UserAllocator*	  gAllocator = NULL;
ErrorStream       gErrorStream;
NxRemoteDebugger* gRemoteDebugger = NULL;

// Display globals
int gMainHandle;
int mx = 0;
int my = 0;

// Camera globals
float gCameraAspectRatio = 1;
NxVec3 gCameraPos(0,5,-15);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
NxReal gCameraSpeed = 0.15;

// Force globals
NxVec3 gForceVec(0,0,0);
NxReal gForceStrength = 750;
bool bForceMode = true;

// Kinematic globals
NxReal gKinematicVelocity = 0.1;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
bool bPause = false;
bool bShadows = true;
bool bDebugWireframeMode = false;
bool bShapeSelectMode = false;

// Actor globals
NxActor* groundPlane = NULL;
NxActor* box = NULL;
NxActor* sphere = NULL;
NxActor* capsule = NULL;
NxActor* pyramid = NULL;
NxActor* cab = NULL;
NxActor* board = NULL;
NxActor* bike = NULL;

// Focus actor and shape
NxActor* gSelectedActor = NULL;
NxShape* gSelectedShape = NULL;

NxI32 gNbTouchedBodies;

bool IsSelectable(NxActor* actor)
{
   NxShape*const* shapes = gSelectedActor->getShapes();
   NxU32 nShapes = gSelectedActor->getNbShapes();
   while (nShapes--)
   {
       if (shapes[nShapes]->getFlag(NX_TRIGGER_ENABLE)) 
       {           
           return false;
       }
   }

   if (actor == groundPlane)
       return false;

   return true;
}

void SelectNextActor()
{
   NxU32 nbActors = gScene->getNbActors();
   NxActor** actors = gScene->getActors();
   for(NxU32 i = 0; i < nbActors; i++)
   {
       if (actors[i] == gSelectedActor)
       {
           NxU32 j = 1;
           gSelectedActor = actors[(i+j)%nbActors];
           while (!IsSelectable(gSelectedActor))
           {
               j++;
               gSelectedActor = actors[(i+j)%nbActors];
           }
           break;
       }
   }
}

void SelectNextShapeOnActor(NxActor* actor)
{
	NxU32 nbShapes = actor->getNbShapes();
	NxShape*const* shapes = actor->getShapes();
	for(NxU32 i = 0; i < nbShapes; i++)
	{
		if (shapes[i] == gSelectedShape)
		{
			gSelectedShape = shapes[(i+1)%nbShapes];
			break;
		}
	}
}

void ProcessCameraKeys()
{
	// Process camera keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Camera controls
			case 'w':{ gCameraPos += gCameraForward*gCameraSpeed; break; }
			case 's':{ gCameraPos -= gCameraForward*gCameraSpeed; break; }
			case 'a':{ gCameraPos -= gCameraRight*gCameraSpeed; break; }
			case 'd':{ gCameraPos += gCameraRight*gCameraSpeed; break; }
			case 'z':{ gCameraPos -= NxVec3(0,1,0)*gCameraSpeed; break; }
			case 'q':{ gCameraPos += NxVec3(0,1,0)*gCameraSpeed; break; }
		}
	}
}

void SetupCamera()
{
	// Setup camera
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, gCameraAspectRatio, 1.0f, 10000.0f);
	gluLookAt(gCameraPos.x,gCameraPos.y,gCameraPos.z,gCameraPos.x + gCameraForward.x, gCameraPos.y + gCameraForward.y, gCameraPos.z + gCameraForward.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color)
{
	// Draw only if the force is large enough
	NxReal force = forceVec.magnitude();
	if (force < 0.1f)  return;

	forceVec = 3*forceVec/force;

	NxVec3 pos = actor->getCMassGlobalPosition();
	DrawArrow(pos, pos + forceVec, color);
}

void DrawForceAtShape(NxActor* actor, NxShape* shape, NxVec3& forceVec, const NxVec3& color)
{
	// Draw only if the force is large enough
	NxReal force = forceVec.magnitude();
	if (force < 0.1f)  return;

	forceVec = 3*forceVec/force;

	NxVec3 pos;	
	if (bShapeSelectMode && bForceMode)
		pos = shape->getGlobalPosition();
	else
		pos = actor->getCMassGlobalPosition();
	DrawArrow(pos, pos + forceVec, color);
}

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode)
{
	NxVec3 forceVec = forceStrength*forceDir;

	if (forceMode)
		actor->addForce(forceVec);
	else 
		actor->addTorque(forceVec);

	return forceVec;
}

void ProcessForceKeys()
{
    if (!gSelectedActor)  return;

	// Process force keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Force controls
			case 'i': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,1),gForceStrength,bForceMode); break; }
			case 'k': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,-1),gForceStrength,bForceMode); break; }
			case 'j': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(1,0,0),gForceStrength,bForceMode); break; }
			case 'l': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(-1,0,0),gForceStrength,bForceMode); break; }
			case 'u': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,1,0),gForceStrength,bForceMode); break; }
			case 'm': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,-1,0),gForceStrength,bForceMode); break; }

		    // Return focus actor to (0,5,0)
		    case 't': { gSelectedActor->setGlobalPosition(NxVec3(0,5,0)); break; }
		}
	}
}

NxVec3 ApplyForceToActorAtShape(NxActor* actor, NxShape* shape, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode, bool shapeSelectMode)
{
	NxVec3 forceVec = forceStrength*forceDir;

	if (forceMode) 
	{
		if (shapeSelectMode) 
		{
			actor->addForceAtLocalPos(forceVec, shape->getLocalPosition());
		} 
		else 
		{
			actor->addForce(forceVec);
		}
	} 
	else 
	{ 
		actor->addTorque(forceVec);
	}

	return forceVec;
}

void ProcessForceAtShapeKeys()
{
	// Process force keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Force at shape controls
			case 'i': {gForceVec = ApplyForceToActorAtShape(gSelectedActor,gSelectedShape,NxVec3(0,0,1),gForceStrength,bForceMode,bShapeSelectMode); break; }
			case 'k': {gForceVec = ApplyForceToActorAtShape(gSelectedActor,gSelectedShape,NxVec3(0,0,-1),gForceStrength,bForceMode,bShapeSelectMode); break; }
			case 'j': {gForceVec = ApplyForceToActorAtShape(gSelectedActor,gSelectedShape,NxVec3(1,0,0),gForceStrength,bForceMode,bShapeSelectMode); break; }
			case 'l': {gForceVec = ApplyForceToActorAtShape(gSelectedActor,gSelectedShape,NxVec3(-1,0,0),gForceStrength,bForceMode,bShapeSelectMode); break; }
			case 'u': {gForceVec = ApplyForceToActorAtShape(gSelectedActor,gSelectedShape,NxVec3(0,1,0),gForceStrength,bForceMode,bShapeSelectMode); break; }
			case 'm': {gForceVec = ApplyForceToActorAtShape(gSelectedActor,gSelectedShape,NxVec3(0,-1,0),gForceStrength,bForceMode,bShapeSelectMode); break; }

		    // Return focus actor to (0,5,0)
		    case 't': { gSelectedActor->setGlobalPosition(NxVec3(0,5,0)); break; }
		}
	}
}

NxVec3 ApplyVelocityToActor(NxActor* actor, const NxVec3& velDir, const NxReal velStrength, bool velMode)
{
	NxVec3 velVec = velStrength*velDir;

	if (velMode) 
	{
		actor->moveGlobalPosition(actor->getGlobalPosition() + 0.0001*velStrength*velDir);
	} 
	else 
	{
		NxMat33 orient = actor->getGlobalOrientation();
        NxMat33 m;

		m.id();

		if (velDir == NxVec3(1,0,0))
           m.rotX(0.01);
		else if (velDir == NxVec3(-1,0,0))
           m.rotX(-0.01);
		else if (velDir == NxVec3(0,1,0))
           m.rotY(0.01);
		else if (velDir == NxVec3(0,-1,0))
           m.rotY(-0.01);
		else if (velDir == NxVec3(0,0,1))
           m.rotZ(0.01);
		else if (velDir == NxVec3(0,0,-1))
           m.rotZ(-0.01);

		orient = m * orient;

		actor->moveGlobalOrientation(orient);
	}

	return velVec;
}

NxVec3 MoveActor(NxActor* actor, const NxVec3& moveDir, const NxReal moveStrength, bool moveMode)
{
	if (actor->readBodyFlag(NX_BF_KINEMATIC))
		return ApplyVelocityToActor(actor, moveDir, moveStrength, moveMode);
	else
		return ApplyForceToActor(actor, moveDir, moveStrength, moveMode);
}

void ProcessMoveKeys()
{
	// Process force keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Move controls
			case 'i': {gForceVec = MoveActor(gSelectedActor,NxVec3(0,0,1),gForceStrength,bForceMode); break; }
			case 'k': {gForceVec = MoveActor(gSelectedActor,NxVec3(0,0,-1),gForceStrength,bForceMode); break; }
			case 'j': {gForceVec = MoveActor(gSelectedActor,NxVec3(1,0,0),gForceStrength,bForceMode); break; }
			case 'l': {gForceVec = MoveActor(gSelectedActor,NxVec3(-1,0,0),gForceStrength,bForceMode); break; }
			case 'u': {gForceVec = MoveActor(gSelectedActor,NxVec3(0,1,0),gForceStrength,bForceMode); break; }
			case 'm': {gForceVec = MoveActor(gSelectedActor,NxVec3(0,-1,0),gForceStrength,bForceMode); break; }

		    // Return focus actor to (0,5,0)
		    case 't': { gSelectedActor->setGlobalPosition(NxVec3(0,5,0)); break; }
		}
	}
}

void ReshapeCallback(int width, int height)
{
    glViewport(0, 0, width, height);
    gCameraAspectRatio = float(width)/float(height);
}

void IdleCallback()
{
    glutPostRedisplay();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;

	switch (key)
	{
		case 'r': {	if (!bShapeSelectMode) {
						SelectNextActor();
					} else {
						SelectNextShapeOnActor(gSelectedActor);
					} break; }
		default: { break; }
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
		case 'p': { bPause = !bPause; UpdateTime(); break; }
		case 'x': { bShadows = !bShadows; break; }
		case 'b': { bDebugWireframeMode = !bDebugWireframeMode; break; }		
		case 'f': { bForceMode = !bForceMode; break; }
		case 27 : { exit(0); break; }
		default : { break; }
	}

	SpecialKeys(key, x, y);
}

void SpecialCallback(int key, int x, int y)
{
	switch (key)
    {
		// Reset PhysX
		case GLUT_KEY_F10: ResetNx(); return; 
	}
}

void MouseCallback(int button, int state, int x, int y)
{
    mx = x;
    my = y;
}

void MotionCallback(int x, int y)
{
    int dx = mx - x;
    int dy = my - y;
    
    gCameraForward.normalize();
    gCameraRight.cross(gCameraForward,NxVec3(0,1,0));

    NxQuat qx(NxPiF32 * dx * 20 / 180.0f, NxVec3(0,1,0));
    qx.rotate(gCameraForward);
    NxQuat qy(NxPiF32 * dy * 20 / 180.0f, gCameraRight);
    qy.rotate(gCameraForward);

    mx = x;
    my = y;
}

void ExitCallback()
{
	ReleaseNx();
}

void InitGlut(int argc, char** argv, char* lessonTitle)
{
    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    gMainHandle = glutCreateWindow(lessonTitle);
    glutSetWindow(gMainHandle);
    glutDisplayFunc(RenderCallback);
    glutReshapeFunc(ReshapeCallback);
    glutIdleFunc(IdleCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutKeyboardUpFunc(KeyboardUpCallback);
	glutSpecialFunc(SpecialCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
#ifndef LINUX
	glutExitFunc(ExitCallback);
#endif

    // Setup default render states
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);

    // Setup lighting
    glEnable(GL_LIGHTING);
    float AmbientColor[]    = { 0.0f, 0.1f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 0.2f, 0.2f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]   = { 0.5f, 0.5f, 0.5f, 0.0f };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[]        = { 100.0f, 100.0f, -400.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);

	InitShapeDisplayLists();
}

NxActor* CreateGroundPlane()
{
    // Create a plane with default descriptor
    NxPlaneShapeDesc planeDesc;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&planeDesc);
    return gScene->createActor(actorDesc);
}

NxActor* CreateBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&boxDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

NxActor* CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0,radius,0);
	actorDesc.shapes.pushBack(&sphereDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

NxActor* CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a capsule
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = NxVec3(0,radius+0.5*height,0);
	actorDesc.shapes.pushBack(&capsuleDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

NxActor* CreatePyramid(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
    NxActorDesc actorDesc;
    NxBodyDesc bodyDesc;

    // Pyramid
    NxVec3 verts[8] = { NxVec3(boxDim.x,-boxDim.y,-boxDim.z), NxVec3(-boxDim.x,-boxDim.y,-boxDim.z), NxVec3(-boxDim.x,-boxDim.y,boxDim.z), NxVec3(boxDim.x,-boxDim.y,boxDim.z), 
                        NxVec3(boxDim.x*0.5,boxDim.y,-boxDim.z*0.5), NxVec3(-boxDim.x*0.5,boxDim.y,-boxDim.z*0.5), NxVec3(-boxDim.x*0.5,boxDim.y,boxDim.z*0.5), NxVec3(boxDim.x*0.5,boxDim.y,boxDim.z*0.5) };

    // Create descriptor for convex mesh
    NxConvexMeshDesc convexDesc;
    convexDesc.numVertices			= 8;
    convexDesc.pointStrideBytes		= sizeof(NxVec3);
    convexDesc.points				= verts;
    convexDesc.flags				= NX_CF_COMPUTE_CONVEX | NX_CF_USE_LEGACY_COOKER;

	// The actor has one shape, a convex mesh
    NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.localPose.t		= NxVec3(0,boxDim.y,0);
 
	NxInitCooking();
    if (0)
    {
        // Cooking from file
#ifndef LINUX
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#else
		printf("Linux does not behave well with UserStreams, use Memorybuffers instead\n");
#endif
    }
    else
    {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
    }

    if (convexShapeDesc.meshData)
    {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&convexShapeDesc);
	    if (density)
	    {
		    actorDesc.body = &bodyDesc;
		    actorDesc.density = density;
	    }
	    else
	    {
		    actorDesc.body = NULL;
	    }
        actorDesc.globalPose.t  = pos;
		NxActor* actor = gScene->createActor(actorDesc);

		return actor;
//      gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);
    }

    return NULL;
}

NxActor** CreateStack(const NxVec3& pos, const NxVec3& stackDim, const NxVec3& boxDim, NxReal density)
{
	NxActor** stack = new NxActor*[(int)(8*stackDim.x*stackDim.y*stackDim.z)]; 

	NxVec3 offset = NxVec3(boxDim.x,0,boxDim.z) + pos;

	NxI32 count = 0;
	for (NxI32 i = -stackDim.x; i < stackDim.x; i++)
	{
		for (NxI32 j = 0; j < stackDim.y; j++)
		{
			for (NxI32 k = -stackDim.z; k < stackDim.z; k++)
			{
				NxVec3 boxPos = NxVec3(i*boxDim.x*2,j*boxDim.y*2,k*boxDim.z*2);
				stack[count++] = CreateBox(boxPos + offset, boxDim, density);
			}
		}
	}

	return stack;
}

void ReleaseNx()
{
    if (gScene)
	{
		GetPhysicsResults();  // Make sure to fetchResults() before shutting down
		gPhysicsSDK->releaseScene(*gScene);
	}
	if (gPhysicsSDK)  gPhysicsSDK->release();
    NX_DELETE_SINGLE(gAllocator);
}

void ResetNx()
{
	ReleaseNx();
	InitNx();
}

NxReal UpdateTime()
{
    NxReal deltaTime;
#ifndef LINUX
    static __int64 gTime,gLastTime;
    __int64 freq;
    QueryPerformanceCounter((LARGE_INTEGER *)&gTime);  // Get current count
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq); // Get processor freq
    deltaTime = (double)(gTime - gLastTime)/(double)freq;
    gLastTime = gTime;
#else
	struct timeval tv;
	static struct timeval lasttv = { 0 , 0 };
	if (lasttv.tv_usec == 0 && lasttv.tv_sec == 0)
		gettimeofday(&lasttv, NULL);
	gettimeofday(&tv, NULL);
	deltaTime = (tv.tv_usec - lasttv.tv_usec)/1000000.f
		+ (tv.tv_sec - lasttv.tv_sec);
	lasttv = tv;
#endif
    return deltaTime;
}

void StartPhysics()
{
	// Update the time step
	NxReal deltaTime = UpdateTime();

	// Start collision and dynamics for delta time since the last frame
    gScene->simulate(deltaTime);
	gScene->flushStream();
}

void GetPhysicsResults()
{
	// Get results from gScene->simulate(deltaTime)
	while (!gScene->fetchResults(NX_RIGID_BODY_FINISHED, false));
}



