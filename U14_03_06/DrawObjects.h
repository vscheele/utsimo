// ===============================================================================
//						    NOVODEX SDK TRAINING PROGRAMS
//							   OBJECT DRAWING ROUTINES
//
//						   Written by Bob Schade, 1-15-05
// ===============================================================================

#ifndef DRAWOBJECTS_H
#define DRAWOBJECTS_H

class NxShape;
class NxActor;

void InitShapeDisplayLists();
void ReleaseShapeDisplayLists();
void SetupGLMatrix(const NxVec3& pos, const NxMat33& orient);
void DrawLine(const NxVec3& p0, const NxVec3& p1, const NxVec3& color);
void DrawCircle(NxU32 nbSegments, const NxMat34& matrix, const NxVec3& color, const NxF32 radius, const bool semicircle = false);
void DrawEllipse(NxU32 nbSegments, const NxMat34& matrix, const NxVec3& color, const NxF32 radius1, const NxF32 radius2, const bool semicircle = false);
void DrawWirePlane(NxShape *plane, const NxVec3& color);
void DrawPlane(NxShape *plane);
void DrawWireBox(NxShape *box, const NxVec3& color);
void DrawBox(NxShape *box);
void DrawWireSphere(NxShape *sphere, const NxVec3& color);
void DrawSphere(NxShape *sphere);
void DrawWireCapsule(NxShape *capsule, const NxVec3& color);
void DrawCapsule(NxShape *capsule);
void DrawWireConvex(NxShape *mesh, const NxVec3& color);
void DrawConvex(NxShape *mesh);
void DrawWireMesh(NxShape *mesh, const NxVec3& color);
void DrawMesh(NxShape *mesh);

void DrawArrow(const NxVec3& posA, const NxVec3& posB, const NxVec3& color);
void DrawContactPoint(const NxVec3& pos, const NxReal radius, const NxVec3& color);

void DrawWireShape(NxShape *shape, const NxVec3& color);
void DrawShape(NxShape *shape);
void DrawActor(NxActor *actor);
void DrawActorShadow(NxActor *actor);
void DrawActorShadow2(NxActor *actor);

#endif  // DRAWOBJECTS_H