// ===============================================================================
//						    NOVODEX SDK TRAINING PROGRAMS
//							   OBJECT DRAWING ROUTINES
//
//						   Written by Bob Schade, 1-15-05
// ===============================================================================
#include "stdafx.h"
#include "NxPhysics.h"
#include "DrawObjects.h"

#define NORMAL_HACK 1

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

//extern NxActor* gSelectedActor;

static int dispListPlane;
static int dispListBox;
static int dispListSphere;
static int dispListCylinder;

void InitShapeDisplayLists()
{
	// PLANE DISPLAY LIST
	glLoadIdentity();
	dispListPlane = glGenLists(1);
	glNewList(dispListPlane, GL_COMPILE);

	glBegin(GL_QUADS);
	glNormal3f(0,1,0);
	glVertex3f(-1,0,-1);
	glVertex3f(-1,0,1);
	glVertex3f(1,0,1);
	glVertex3f(1,0,-1);

	glEnd();
	glEndList();

	// BOX DISPLAY LIST
	glLoadIdentity();
	dispListBox = glGenLists(1);//get a unique display list ID.
	glNewList(dispListBox, GL_COMPILE);      

	glBegin(GL_QUADS);
	glNormal3f(0,0,1);
	glVertex3f(-1,-1,1);
	glVertex3f(1,-1,1);
	glVertex3f(1,1,1);
	glVertex3f(-1,1,1);

	glNormal3f(0,0,-1);
	glVertex3f(-1,1,-1);
	glVertex3f(1,1,-1);
	glVertex3f(1,-1,-1);
	glVertex3f(-1,-1,-1);

	glNormal3f(0,1,0);
	glVertex3f(-1,1,1);
	glVertex3f(1,1,1);
	glVertex3f(1,1,-1);
	glVertex3f(-1,1,-1);

	glNormal3f(0,-1,0);
	glVertex3f(-1,-1,-1);
	glVertex3f(1,-1,-1);
	glVertex3f(1,-1,1);
	glVertex3f(-1,-1,1);

	glNormal3f(1,0,0);
	glVertex3f(1,-1,-1);
	glVertex3f(1,1,-1);
	glVertex3f(1,1,1);
	glVertex3f(1,-1,1);

	glNormal3f(-1,0,0);
	glVertex3f(-1,-1,1);
	glVertex3f(-1,1,1);
	glVertex3f(-1,1,-1);
	glVertex3f(-1,-1,-1);

	glEnd();
	glEndList();

	// SPHERE DISPLAY LIST
	dispListSphere = glGenLists(1);
	glNewList(dispListSphere, GL_COMPILE);      
	GLUquadricObj * quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH); 
	gluQuadricOrientation(quadObj,GLU_OUTSIDE);
	gluSphere (quadObj, 1.0f, 9, 7);	//unit sphere
	glEndList();
	gluDeleteQuadric(quadObj);

	// CYLINDER DISPLAY LIST
	dispListCylinder = glGenLists(1);
	glNewList(dispListCylinder, GL_COMPILE);      
	quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH); 
	gluQuadricOrientation(quadObj,GLU_OUTSIDE);
	gluCylinder  (quadObj, 1.0f, 1.0f, 1.0f, 18, 1);	//unit cylinder
	glEndList();
	gluDeleteQuadric(quadObj);
}

void ReleaseShapeDisplayLists()
{
	glDeleteLists(dispListPlane,1);
	glDeleteLists(dispListBox,1);
	glDeleteLists(dispListSphere,1);
	glDeleteLists(dispListCylinder,1);
}

void SetupGLMatrix(const NxVec3& pos, const NxMat33& orient)
{
	float glmat[16];	//4x4 column major matrix for OpenGL.
	orient.getColumnMajorStride4(&(glmat[0]));
	pos.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));
}

void DrawLine(const NxVec3& p0, const NxVec3& p1, const NxVec3& color)
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(color.x, color.y, color.z);
	glVertex3fv(&p0.x);
	glVertex3fv(&p1.x);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void DrawCircle(NxU32 nbSegments, const NxMat34& matrix, const NxVec3& color, const NxF32 radius, const bool semicircle)
{
	NxF32 step = NxTwoPiF32/NxF32(nbSegments);
	NxU32 segs = nbSegments;
	if (semicircle)	
	{
		segs /= 2;
	}

	for(NxU32 i=0;i<segs;i++)
	{
		NxU32 j=i+1;
		if(j==nbSegments)	j=0;

		NxF32 angle0 = NxF32(i)*step;
		NxF32 angle1 = NxF32(j)*step;

		NxVec3 p0,p1;
		matrix.multiply(NxVec3(radius * sinf(angle0), radius * cosf(angle0), 0.0f), p0);
		matrix.multiply(NxVec3(radius * sinf(angle1), radius * cosf(angle1), 0.0f), p1);

		DrawLine(p0, p1, color);
	}
}


void DrawEllipse(NxU32 nbSegments, const NxMat34& matrix, const NxVec3& color, const NxF32 radius1, const NxF32 radius2, const bool semicircle)
{
	NxF32 step = NxTwoPiF32/NxF32(nbSegments);
	NxU32 segs = nbSegments;
	if (semicircle)	
	{
		segs /= 2;
	}

	for(NxU32 i=0;i<segs;i++)
	{
		NxU32 j=i+1;
		if(j==nbSegments)	j=0;

		NxF32 angle0 = NxF32(i)*step;
		NxF32 angle1 = NxF32(j)*step;

		NxVec3 p0,p1;
		matrix.multiply(NxVec3(radius1 * sinf(angle0), radius2 * cosf(angle0), 0.0f), p0);
		matrix.multiply(NxVec3(radius1 * sinf(angle1), radius2 * cosf(angle1), 0.0f), p1);

		DrawLine(p0, p1, color);
	}
}

void DrawWirePlane(NxShape *plane, const NxVec3& color)
{
}

void DrawPlane(NxShape *plane)
{
	NxMat34 pose =	plane->getGlobalPose();

	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(0.1f, 0.2f, 0.9f);
	pose.t.y -= 0.1f;
	SetupGLMatrix(pose.t, pose.M);
	glScaled(1024,0,1024);
	glCallList(dispListPlane);
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void DrawWireBox(NxShape* box, const NxVec3& color)
{
	NxBox obb;
	box->isBox()->getWorldOBB(obb);

	// Compute obb vertices
	NxVec3 pp[8];
	NxComputeBoxPoints(obb, pp);
		
	// Draw all lines
	const NxU32* Indices = NxGetBoxEdges();
	for(NxU32 i=0;i<12;i++)
	{
		NxU32 VRef0 = *Indices++;
		NxU32 VRef1 = *Indices++;
		DrawLine(pp[VRef0], pp[VRef1], color);
	}
}

void DrawBox(NxShape *box)
{
	NxMat34 pose = box->getGlobalPose();

	glPushMatrix();
	SetupGLMatrix(pose.t, pose.M);
	NxVec3 boxDim = box->isBox()->getDimensions();
	glScaled(boxDim.x, boxDim.y, boxDim.z);
	glCallList(dispListBox);
	glPopMatrix();
}

void DrawWireSphere(NxShape *sphere, const NxVec3& color)
{
	NxMat34 pose = sphere->getGlobalPose();

	glPushMatrix();
	NxReal r = sphere->isSphere()->getRadius();

	NxVec3 c0;	pose.M.getColumn(0, c0);
	NxVec3 c1;	pose.M.getColumn(1, c1);
	NxVec3 c2;	pose.M.getColumn(2, c2);
	DrawCircle(20, pose, color, r);

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	DrawCircle(20, pose, color, r);

	pose.M.setColumn(0, c2);
	pose.M.setColumn(1, c0);
	pose.M.setColumn(2, c1);
	DrawCircle(20, pose, color, r);

	glPopMatrix();
}

void DrawSphere(NxShape *sphere)
{
	NxMat34 pose = sphere->getGlobalPose();

	glPushMatrix();
	SetupGLMatrix(pose.t, pose.M);
	NxReal r = sphere->isSphere()->getRadius();
	glScaled(r,r,r);
	glCallList(dispListSphere);
	glPopMatrix();
}

void DrawWireCapsule(NxShape *capsule, const NxVec3& color)
{
	NxMat34 pose = capsule->getGlobalPose();

	const NxReal & r = capsule->isCapsule()->getRadius();
	const NxReal & h = capsule->isCapsule()->getHeight();

	NxSegment SG;
	pose.M.getColumn(1, SG.p1);
	SG.p1 *= 0.5*h;
	SG.p0 = -SG.p1;
	SG.p0 += pose.t;
	SG.p1 += pose.t;

	NxVec3 c0;	pose.M.getColumn(0, c0);
	NxVec3 c1;	pose.M.getColumn(1, c1);
	NxVec3 c2;	pose.M.getColumn(2, c2);
	DrawLine(SG.p0 + c0*r, SG.p1 + c0*r, color);
	DrawLine(SG.p0 - c0*r, SG.p1 - c0*r, color);
	DrawLine(SG.p0 + c2*r, SG.p1 + c2*r, color);
	DrawLine(SG.p0 - c2*r, SG.p1 - c2*r, color);

	pose.M.setColumn(0, -c1);
	pose.M.setColumn(1, -c0);
	pose.M.setColumn(2, c2);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r, true);	//halfcircle -- flipped

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, -c0);
	pose.M.setColumn(2, c2);

	pose.t = SG.p1;
	DrawCircle(20, pose, color, r, true);

	pose.M.setColumn(0, -c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r, true);//halfcircle -- good

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	pose.t = SG.p1;
	DrawCircle(20, pose, color, r, true);

	pose.M.setColumn(0, c2);
	pose.M.setColumn(1, c0);
	pose.M.setColumn(2, c1);
	pose.t = SG.p0;
	DrawCircle(20, pose, color, r);	//full circle
	pose.t = SG.p1;
	DrawCircle(20, pose, color, r);
}

void DrawCapsule(NxShape *capsule)
{
	NxMat34 pose = capsule->getGlobalPose();

	const NxReal & r = capsule->isCapsule()->getRadius();
	const NxReal & h = capsule->isCapsule()->getHeight();

	glPushMatrix();
	SetupGLMatrix(pose.t, pose.M);

	glPushMatrix();
	glTranslated(0.0f, h*0.5f, 0.0f);
	glScaled(r,r,r);
	glCallList(dispListSphere);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0f,-h*0.5f, 0.0f);
	glScaled(r,r,r);
	glCallList(dispListSphere);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0f,h*0.5f, 0.0f);
	glScaled(r,h,r);
	glRotated(90.0f,1.0f,0.0f,0.0f);
	glCallList(dispListCylinder);
	glPopMatrix();

	glPopMatrix();
}

typedef NxVec3 Point;
typedef struct _Triangle { NxU32 p0; NxU32 p1; NxU32 p2; } Triangle;

void DrawWireConvex(NxShape *mesh, const NxVec3& color)
{
	NxMat34 pose = mesh->getGlobalPose();

	NxConvexMeshDesc meshDesc;
	mesh->isConvexMesh()->getConvexMesh().saveToDesc(meshDesc);

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));

	glPushMatrix();
	glBegin(GL_TRIANGLES);
	while(nbTriangles--)
	{
		DrawLine(points[triangles->p0], points[triangles->p1], color);
		DrawLine(points[triangles->p1], points[triangles->p2], color);
		DrawLine(points[triangles->p2], points[triangles->p0], color);
		triangles++;
	}
	glEnd();
	glPopMatrix();

	glPopMatrix();
}

void DrawConvex(NxShape *mesh)
{
	NxMat34 pose = mesh->getGlobalPose();

	NxConvexMeshDesc meshDesc;
	mesh->isConvexMesh()->getConvexMesh().saveToDesc(meshDesc);

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));

#ifdef NORMAL_HACK
	NxVec3 edge1, edge2, normal;
#endif

	glPushMatrix();
	glBegin(GL_TRIANGLES);
	while(nbTriangles--)
	{

#ifdef NORMAL_HACK
		edge1 = points[triangles->p1] - points[triangles->p0];
		edge2 = points[triangles->p2] - points[triangles->p0];
		normal = edge1.cross(edge2);
		normal.normalize();
		glNormal3f(normal.x, normal.y, normal.z);
#endif

		glVertex3fv(&points[triangles->p0].x);
		glVertex3fv(&points[triangles->p1].x);
		glVertex3fv(&points[triangles->p2].x);
		triangles++;
	}
	glEnd();
	glPopMatrix();
	
	glPopMatrix();
}

void DrawWireMesh(NxShape *mesh, const NxVec3& color)
{
	NxMat34 pose = mesh->getGlobalPose();

	NxTriangleMeshDesc meshDesc;
	mesh->isTriangleMesh()->getTriangleMesh().saveToDesc(meshDesc);

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));

	glPushMatrix();
	glBegin(GL_TRIANGLES);
	while(nbTriangles--)
	{
		DrawLine(points[triangles->p0], points[triangles->p1], color);
		DrawLine(points[triangles->p1], points[triangles->p2], color);
		DrawLine(points[triangles->p2], points[triangles->p0], color);
		triangles++;
	}
	glEnd();
	glPopMatrix();

	glPopMatrix();
}

void DrawMesh(NxShape *mesh)
{
	NxMat34 pose = mesh->getGlobalPose();

	NxTriangleMeshDesc meshDesc;
	mesh->isTriangleMesh()->getTriangleMesh().saveToDesc(meshDesc);

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));

	//clear the elements we don't need:
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;

	glMultMatrixf(&(glmat[0]));

	if (meshDesc.heightFieldVerticalAxis != NX_NOT_HEIGHTFIELD) 
	{
	    glDisable(GL_LIGHT0);
	    glEnable(GL_LIGHT1);
	}

#ifdef NORMAL_HACK
	NxVec3 edge1, edge2, normal;
#endif

	glPushMatrix();
	glBegin(GL_TRIANGLES);
	while(nbTriangles--)
	{

#ifdef NORMAL_HACK
		edge1 = points[triangles->p1] - points[triangles->p0];
		edge2 = points[triangles->p2] - points[triangles->p0];
		normal = edge1.cross(edge2);
		normal.normalize();
		glNormal3f(normal.x, normal.y, normal.z);
#endif

		glVertex3fv(&points[triangles->p0].x);
		glVertex3fv(&points[triangles->p1].x);
		glVertex3fv(&points[triangles->p2].x);
		triangles++;
	}
	glEnd();
	glPopMatrix();

	if (meshDesc.heightFieldVerticalAxis != NX_NOT_HEIGHTFIELD) 
	{
	    glDisable(GL_LIGHT1);
	    glEnable(GL_LIGHT0);
	}
	
	glPopMatrix();
}

void DrawArrow(const NxVec3& posA, const NxVec3& posB, const NxVec3& color)
{
	NxVec3 vec = posB - posA;
	NxVec3 t0, t1, t2;
	NxNormalToTangents(vec, t1, t2);

	t0 = posB - posA;
	t0.normalize();

	NxVec3 lobe1  = posB - t0*0.15 + t1 * 0.15;
	NxVec3 lobe2  = posB - t0*0.15 - t1 * 0.15;
	NxVec3 lobe3  = posB - t0*0.15 + t2 * 0.15;
	NxVec3 lobe4  = posB - t0*0.15 - t2 * 0.15;

	glPushMatrix();
	glDisable(GL_LIGHTING);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(color.x,color.y,color.z);
	glVertex3fv(&posA.x);
	glVertex3fv(&posB.x);
	glVertex3fv(&posB.x);
	glVertex3fv(&lobe1.x);
	glVertex3fv(&posB.x);
	glVertex3fv(&lobe2.x);
	glVertex3fv(&posB.x);
	glVertex3fv(&lobe3.x);
	glVertex3fv(&posB.x);
	glVertex3fv(&lobe4.x);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void DrawContactPoint(const NxVec3& pos, const NxReal radius, const NxVec3& color)
{
	NxMat34 pose;
	pose.t = pos;

	NxVec3 c0;	pose.M.getColumn(0, c0);
	NxVec3 c1;	pose.M.getColumn(1, c1);
	NxVec3 c2;	pose.M.getColumn(2, c2);
	DrawCircle(20, pose, color, radius);

	pose.M.setColumn(0, c1);
	pose.M.setColumn(1, c2);
	pose.M.setColumn(2, c0);
	DrawCircle(20, pose, color, radius);

	pose.M.setColumn(0, c2);
	pose.M.setColumn(1, c0);
	pose.M.setColumn(2, c1);
	DrawCircle(20, pose, color, radius);
}

void DrawWireShape(NxShape *shape, const NxVec3& color)
{
    switch(shape->getType())
    {
		case NX_SHAPE_PLANE:
			DrawWirePlane(shape, color);
		break;
		case NX_SHAPE_BOX:
			DrawWireBox(shape, color);
		break;
		case NX_SHAPE_SPHERE:
			DrawWireSphere(shape, color);
		break;
		case NX_SHAPE_CAPSULE:
			DrawWireCapsule(shape, color);
		break;
		case NX_SHAPE_CONVEX:
			DrawWireConvex(shape, color);
		break;		
		case NX_SHAPE_MESH:
			DrawWireMesh(shape, color);
		break;
	}
}

void DrawShape(NxShape *shape)
{
    switch(shape->getType())
    {
		case NX_SHAPE_PLANE:
			DrawPlane(shape);
		break;
		case NX_SHAPE_BOX:
			DrawBox(shape);
		break;
		case NX_SHAPE_SPHERE:
			DrawSphere(shape);
		break;
		case NX_SHAPE_CAPSULE:
			DrawCapsule(shape);
		break;
		case NX_SHAPE_CONVEX:
			DrawConvex(shape);
		break;
		case NX_SHAPE_MESH:
			DrawMesh(shape);
		break;
	}
}

void DrawActor(NxActor *actor)
{
	NxShape *const * shapes = actor->getShapes();
	NxU32 nShapes = actor->getNbShapes();
	/*if (actor == gSelectedActor) 
	{
		while (nShapes--)
		{
			DrawWireShape(shapes[nShapes], NxVec3(1,1,1));
		}
	}*/
	nShapes = actor->getNbShapes();
	while (nShapes--)
	{
		DrawShape(shapes[nShapes]);
	}
}

void DrawActorShadow(NxActor *actor)
{
    const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

	glPushMatrix();
	glMultMatrixf(ShadowMat);

	
	glColor3f(0.05f, 0.1f, 0.15f);
	
	NxShape *const * shapes = actor->getShapes();
	NxU32 nShapes = actor->getNbShapes();
	while (nShapes--)
	{
		switch(shapes[nShapes]->getType())
		{
		    case NX_SHAPE_BOX:
			    DrawBox(shapes[nShapes]);
			break;
		    case NX_SHAPE_SPHERE:
			    DrawSphere(shapes[nShapes]);
			break;
		    case NX_SHAPE_CAPSULE:
			    DrawCapsule(shapes[nShapes]);
			break;
		    case NX_SHAPE_CONVEX:
			    DrawConvex(shapes[nShapes]);
			break;		    
			case NX_SHAPE_MESH:
			    DrawMesh(shapes[nShapes]);
			break;
		}
	}	
	
	glColor3f(1.0f, 1.0f, 1.0f);


	glPopMatrix();
}

void DrawActorShadow2(NxActor *actor)
{
    const static float ShadowMat[]={ 1,0,0,0, 1,0,-0.2,0, 0,0,1,0, 0,0,0,1 };

	glPushMatrix();
	glMultMatrixf(ShadowMat);

	glDisable(GL_LIGHTING);
	glColor3f(0.05f, 0.1f, 0.15f);
	
	NxShape *const * shapes = actor->getShapes();
	NxU32 nShapes = actor->getNbShapes();
	while (nShapes--)
	{
		switch(shapes[nShapes]->getType())
		{
		    case NX_SHAPE_BOX:
			    DrawBox(shapes[nShapes]);
			break;
		    case NX_SHAPE_SPHERE:
			    DrawSphere(shapes[nShapes]);
			break;
		    case NX_SHAPE_CAPSULE:
			    DrawCapsule(shapes[nShapes]);
			break;
		    case NX_SHAPE_CONVEX:
			    DrawConvex(shapes[nShapes]);
			break;		    
			case NX_SHAPE_MESH:
			    DrawMesh(shapes[nShapes]);
			break;
		}
	}	
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

