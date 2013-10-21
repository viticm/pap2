// ===============================================================================
//						     PHYSX SDK TRAINING PROGRAMS
//			                     COMMON LESSON CODE
//
//						   Written by Bob Schade, 12-15-05
// ===============================================================================

#ifndef COMMONCODE_H
#define COMMONCODE_H

#define NOMINMAX
#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#elif LINUX
#include <windows_replacement.h>
#include <GL/gl.h>
#include <GL/glut.h>
#elif __APPLE__
#include <windows_replacement.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif
#include <stdio.h>

#ifdef LINUX
#include <sys/time.h>
#endif

#include "NxPhysics.h"
#include "DrawShapes.h"
#include "UserData.h"

#include "Stream.h"
#include "NxCooking.h"

#include "DebugRenderer.h"
#include "UserAllocator.h"
#include "ErrorStream.h"

#include "ExitCallback.h"

bool IsSelectable(NxActor* actor);
void SelectNextActor();
void SelectNextShapeOnActor(NxActor* actor);

void ProcessCameraKeys();
void SetupCamera();

void RenderActors(bool shadows);
void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color);
void DrawForceAtShape(NxActor* actor, NxShape* shape, NxVec3& forceVec, const NxVec3& color);

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode);
void ProcessForceKeys();

NxVec3 ApplyForceToActorAtShape(NxActor* actor, NxShape* shape, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode, bool actorSelectMode);
void ProcessForceAtShapeKeys();

NxVec3 ApplyVelocityToActor(NxActor* actor, const NxVec3& velDir, const NxReal velStrength, bool velMode);
NxVec3 MoveActor(NxActor* actor, const NxVec3& moveDir, const NxReal moveStrength, bool moveMode);
void ProcessMoveKeys();

void ProcessInputs();

void RenderCallback();
void ReshapeCallback(int width, int height);
void IdleCallback();
void KeyboardCallback(unsigned char key, int x, int y);
void KeyboardUpCallback(unsigned char key, int x, int y);
void SpecialKeys(unsigned char key, int x, int y);
void SpecialCallback(int key, int x, int y);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);
void ExitCallback();
void InitGlut(int argc, char** argv, char* lessonTitle);

NxActor* CreateGroundPlane();
NxActor* CreateBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density);
NxActor* CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density);
NxActor* CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density);
NxActor* CreatePyramid(const NxVec3& pos, const NxVec3& boxDim, const NxReal density);

NxActor** CreateStack(const NxVec3& pos, const NxVec3& stackDim, const NxVec3& boxDim, NxReal density);

void InitNx();
void ReleaseNx();
void ResetNx();

NxReal UpdateTime();
void StartPhysics();
void GetPhysicsResults();

#endif  // COMMONCODE_H



