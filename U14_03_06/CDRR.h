#ifndef CDRR_H
#define CDRR_H


struct _Vector2D {
 float x, y;
};

struct _RotRect {
 _Vector2D C;
 _Vector2D S;
 float ang;
};

#include "CDRR.h"

inline void AddVectors2D(_Vector2D * v1, _Vector2D * v2)
{ v1->x += v2->x; v1->y += v2->y; }

inline void SubVectors2D(_Vector2D * v1, _Vector2D * v2)
{ v1->x -= v2->x; v1->y -= v2->y; }

inline void RotateVector2DClockwise(_Vector2D * v, float ang)
{
 float t,
       cosa = cos(ang),
       sina = sin(ang);
 t = v->x; v->x = t*cosa + v->y*sina; v->y = -t*sina + v->y*cosa;
}

// Rotated Rectangles Collision Detection, Oren Becker, 2001
inline int RotRectsCollision(_RotRect * rr1, _RotRect * rr2)
{
 _Vector2D A, B,   // vertices of the rotated rr2
	   C,      // center of rr2
	   BL, TR; // vertices of rr2 (bottom-left, top-right)

 float ang = rr1->ang - rr2->ang, // orientation of rotated rr1
       cosa = cos(ang),           // precalculated trigonometic -
       sina = sin(ang);           // - values for repeated use

 float t, x, a;      // temporary variables for various uses
 float dx;           // deltaX for linear equations
 float ext1, ext2;   // min/max vertical values

 // move rr2 to make rr1 cannonic
 C = rr2->C;
 SubVectors2D(&C, &rr1->C);

 // rotate rr2 clockwise by rr2->ang to make rr2 axis-aligned
 RotateVector2DClockwise(&C, rr2->ang);

 // calculate vertices of (moved and axis-aligned := 'ma') rr2
 BL = TR = C;
 SubVectors2D(&BL, &rr2->S);
 AddVectors2D(&TR, &rr2->S);

 // calculate vertices of (rotated := 'r') rr1
 A.x = -rr1->S.y*sina; B.x = A.x; t = rr1->S.x*cosa; A.x += t; B.x -= t;
 A.y =  rr1->S.y*cosa; B.y = A.y; t = rr1->S.x*sina; A.y += t; B.y -= t;

 t = sina*cosa;

 // verify that A is vertical min/max, B is horizontal min/max
 if (t < 0)
 {
  t = A.x; A.x = B.x; B.x = t;
  t = A.y; A.y = B.y; B.y = t;
 }

 // verify that B is horizontal minimum (leftest-vertex)
 if (sina < 0) { B.x = -B.x; B.y = -B.y; }

 // if rr2(ma) isn't in the horizontal range of
 // colliding with rr1(r), collision is impossible
 if (B.x > TR.x || B.x > -BL.x) return 0;

 // if rr1(r) is axis-aligned, vertical min/max are easy to get
 if (t == 0) {ext1 = A.y; ext2 = -ext1; }
 // else, find vertical min/max in the range [BL.x, TR.x]
 else
 {
  x = BL.x-A.x; a = TR.x-A.x;
  ext1 = A.y;
  // if the first vertical min/max isn't in (BL.x, TR.x), then
  // find the vertical min/max on BL.x or on TR.x
  if (a*x > 0)
  {
   dx = A.x;
   if (x < 0) { dx -= B.x; ext1 -= B.y; x = a; }
   else       { dx += B.x; ext1 += B.y; }
   ext1 *= x; ext1 /= dx; ext1 += A.y;
  }
  
  x = BL.x+A.x; a = TR.x+A.x;
  ext2 = -A.y;
  // if the second vertical min/max isn't in (BL.x, TR.x), then
  // find the local vertical min/max on BL.x or on TR.x
  if (a*x > 0)
  {
   dx = -A.x;
   if (x < 0) { dx -= B.x; ext2 -= B.y; x = a; }
   else       { dx += B.x; ext2 += B.y; }
   ext2 *= x; ext2 /= dx; ext2 -= A.y;
  }
 }

 // check whether rr2(ma) is in the vertical range of colliding with rr1(r)
 // (for the horizontal range of rr2)
 return !((ext1 < BL.y && ext2 < BL.y) ||
	  (ext1 > TR.y && ext2 > TR.y));
}

/*double UtsVector2D::angleToVector(UtsVector2D* v){

 double cosine=(x*v->x+y*v->y)/(sqrt(x*x+y*y)*sqrt(v->x*v->x+v->y*v->y));
 if ((x * v->y - y * v->x) < 0)
    return -acos(cosine);
  else
    return acos(cosine);
}*/

inline float XZAngle(NxVec3 v1, NxVec3 v2){
 double srt=(sqrt(v1.x*v1.x+v1.z*v1.z)*sqrt(v2.x*v2.x+v2.z*v2.z));
 double cosine=(v1.x*v2.x+v1.z*v2.z)/srt;
 if ((v1.x * v2.z - v1.z * v2.x) < 0)
    return -acos(cosine);
  else
    return acos(cosine);
}
inline float AngleToXZ(NxVec3 v1){
	if (v1.y<0) return -(NxAngle(v1,NxVec3(v1.x,0,v1.z)));
	else return NxAngle(v1,NxVec3(v1.x,0,v1.z));
}
inline float distanceToSegment(NxVec3 p, NxVec3 s0, NxVec3 s1){
	NxVec3 v = s1 - s0;
    NxVec3 w = p - s0;

    double c1 = w.dot(v);
    if ( c1 <= 0 )
		return p.distance(s0);

    double c2 = v.dot(v);
    if ( c2 <= c1 )
        return p.distance(s1);

    double b = c1 / c2;
    NxVec3 Pb = s0 + b * v;
    return p.distance(Pb);
}

//class Car;
//class UserCar;

/*inline bool collTestCarCar(Car* c1, Car* c2){
	//both not actors, simple 2D collision test without Novodex
	if ((!c1->actor)&&(!c2->actor))
		return c1->collidesWith(c2);
	else if ((!c1->actor)||(!c2->actor)){
		if (!c1->actor)
	}
}
inline bool collTestUCarCar(UserCar u, Car* c){
	//TODO
}*/
#endif