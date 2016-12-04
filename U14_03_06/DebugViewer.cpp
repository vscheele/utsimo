//DebugViewer.cpp
#include "stdafx.h"
#include "DebugViewer.h"
#include "DrawObjects.h"


void DisplayFunction(void);
void KeyboardFunction(unsigned char key, int x, int y);
void changeSize(int w, int h) ;
void drawing_renderGrid();
void drawing_renderLanes();
void drawing_renderStreets();
void drawing_renderCrossings();
void drawing_renderSpawns();
void drawing_renderCars();
void drawing_renderInfofield();
void drawing_renderPhysics();
void drawing_renderUserCars();

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];
bool gLayers[10] = { true, true, true, true, true , true, true ,true, true, true};
bool gChaseCam =false ;


void vecVertex(NxVec3 vec);
void renderBitmapString(float x, float y, float z, void *font, char *string);

NxVec3 pos(0,25,-30);
NxVec3 target(1,0,0);
NxVec3 toSky(0,1,0);

NxReal dleftright=0;
NxReal angleLR=0;
NxReal angleUD=(float)PI;
NxReal dforward=0; 

UINT64 ticksPerSecond;
float stepDuration=0;
float lastDrawingDuration=0;
float pureStepTime=0;

int lastMouseX=0;
int lastMouseY=0;
bool mouseActive=false;

Utsimo* gUtsimo;


DebugViewer::DebugViewer(Utsimo* uts){
	utsimo=uts;
	gUtsimo=uts;
}

void orientMe(float ang, float ang2) {


	target.x = sin(ang)*sin(ang2);
	target.z = -cos(ang)*sin(ang2);
	
	target.y = cos(ang2);

	glLoadIdentity();
	gluLookAt(pos.x, pos.y, pos.z, 
		      pos.x + target.x,pos.y + target.y,pos.z + target.z,
			  0.0f,1.0f,0.0f);
}


void moveMeFlatForward(int i) {
	NxVec3 ntarget(target.x,0,target.z);
	ntarget.normalize();
	pos.x = (NxReal)(pos.x + i*(ntarget.x)*stepDuration/10);
	pos.z = (NxReal)(pos.z + i*(ntarget.z)*stepDuration/10);
	glLoadIdentity();
	gluLookAt(pos.x, pos.y, pos.z, 
		      pos.x + target.x,pos.y + target.y,pos.z + target.z,
			  0.0f,1.0f,0.0f);
}
void moveMeFlatLeftRight(int i) {
	NxVec3 ntarget(-target.z,0,target.x);
	ntarget.normalize();
	pos.x = (NxReal)(pos.x + i*(ntarget.x)*stepDuration/10);
	pos.z = (NxReal)(pos.z + i*(ntarget.z)*stepDuration/10);
	glLoadIdentity();
	gluLookAt(pos.x, pos.y, pos.z, 
		      pos.x + target.x,pos.y + target.y,pos.z + target.z,
			  0.0f,1.0f,0.0f);
}
void DisplayFunction(void){


	//ChaseCAM!!
	if (gChaseCam /*&& (gUtsimo->utsNx->userCar.actor->getLinearVelocity().magnitude()>1)*/){
		float carSpeed=gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor->getLinearVelocity().magnitude();
		NxVec3 v=gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor->getLinearVelocity(); v.normalize();
		NxVec3 newPos(gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor->getGlobalPosition().x-v.x*20,
			gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor->getGlobalPosition().y+10,
			gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor->getGlobalPosition().z-v.z*20);

		//no fast movement
		NxVec3 diff=newPos-pos;
			
		double tmp=gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor->getGlobalOrientationQuat().getAngle();
		
		if ( diff.magnitude()<0.1*carSpeed)
			pos = newPos;
		else {
			diff.setMagnitude(0.1*carSpeed);
			pos= pos+diff;
		}

		NxVec3 t = gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor->getGlobalPosition()+NxVec3(0,5,0)-pos ; t.normalize();
		target=t;

		glLoadIdentity();
		gluLookAt(pos.x, pos.y, pos.z, 
			pos.x + target.x,pos.y + target.y,pos.z + target.z,
			0.0f,1.0f,0.0f);

	//Flying CAM
	} else if (!gChaseCam){
		if (dforward)
			moveMeFlatForward((int)dforward);
		if (dleftright)
			moveMeFlatLeftRight((int)dleftright);
		orientMe(angleLR,angleUD); 
	}


	
	//}
	//clearing the depth buffer 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	
	// save the previous settings, in this case save 
	// we're refering to the camera settings.
	glPushMatrix();
	
	glDisable(GL_LIGHTING);
	 drawing_renderGrid();

	 
	 if (gLayers[0]) drawing_renderStreets();
	 if (gLayers[1]) drawing_renderCrossings();

	 if (gLayers[2]) drawing_renderLanes();	 
	 //if (gLayers[3]) drawing_renderSpawns();
	 if (gLayers[3]) drawing_renderCars();
	
	 glEnable(GL_LIGHTING);

	 if (gLayers[4]) drawing_renderPhysics();




	 if (gLayers[5]) drawing_renderUserCars();

	 glDisable(GL_LIGHTING);
	 if (gLayers[9]) drawing_renderInfofield();
	 glEnable(GL_LIGHTING);

	NxVec3 p1(0,0,20);
	NxVec3 p2(0,0,0);
	NxVec3 p3(10,0,10);
	NxVec3 p4(20,0,0);


	// discard the modelling transformations
	// after this the matrix will have only the camera settings.
	glPopMatrix();
	
	// swapping the buffers causes the rendering above to be 
	// shown
	glutSwapBuffers();
	
	// finally increase the angle for the next frame
	//angle++;
}
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = (float)1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);
	
   glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos.x, pos.y, pos.z, 
		      pos.x + target.x,pos.y + target.y,pos.z + target.z,
			  0.0f,1.0f,0.0f);
	orientMe(angleLR,angleUD);


}
void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;

	switch (key)
	{
		case 'r': { break; }
		case '1':
			gLayers[0]=!gLayers[0]; break;
		case '2':
			gLayers[1]=!gLayers[1];break;
		case '3':
			gLayers[2]=!gLayers[2];break;
		case '4':
			gLayers[3]=!gLayers[3];break;
		case '5':
			gLayers[4]=!gLayers[4];break;
		case '6':
			gLayers[5]=!gLayers[5];break;
		case '7':
			gLayers[6]=!gLayers[6];break;
		case '8':
			gLayers[7]=!gLayers[7];break;
		case '9':
			gLayers[8]=!gLayers[8];break;
		case '0':
			gLayers[9]=!gLayers[9];break;
		case 'v':
			if (gUtsimo->utsNx->userRaycastVehicleSpawned){
				gUtsimo->utsNx->userRaycastCar.vehicle->standUp();
			}
			;break;
		case 'c':{
			if (gUtsimo->utsNx->userRaycastVehicleSpawned){
				gChaseCam=!gChaseCam; 
			}
			break;}
	default: { break; }
	}
}
void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{		
		case 27 : { exit(0); break; }
		default : { break; }
	}
}
void pressKey(int key, int x, int y) {

	switch (key) {

		case GLUT_KEY_LEFT : 
			dleftright = -1;break;
		case GLUT_KEY_RIGHT : 
			dleftright = 1;break;
		case GLUT_KEY_UP : 
			dforward = 1;break;
		case GLUT_KEY_DOWN : 
			dforward = -1;break;
		case GLUT_KEY_PAGE_UP :
			 if (gUtsimo->artificialTimeStep==-1)
				gUtsimo->artificialTimeStep=1;
			else if (gUtsimo->artificialTimeStep==1)
				gUtsimo->artificialTimeStep=5; 
			else if (gUtsimo->artificialTimeStep==5)
				gUtsimo->artificialTimeStep=25; 
			else if (gUtsimo->artificialTimeStep==25)
				gUtsimo->artificialTimeStep=125; 
			else if (gUtsimo->artificialTimeStep==125)
				gUtsimo->artificialTimeStep=300; 
			break;
		case GLUT_KEY_PAGE_DOWN :
			if (gUtsimo->artificialTimeStep==1){
				gUtsimo->reOffsetTime();
				gUtsimo->artificialTimeStep=-1; 
			}
			else if (gUtsimo->artificialTimeStep==300)
				gUtsimo->artificialTimeStep=125;
			else if (gUtsimo->artificialTimeStep==125)
				gUtsimo->artificialTimeStep=25; 
			else if (gUtsimo->artificialTimeStep==25)
				gUtsimo->artificialTimeStep=5; 
			else if (gUtsimo->artificialTimeStep==5)
				gUtsimo->artificialTimeStep=1; 

			break;
	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT : 
		case GLUT_KEY_RIGHT : 
			dleftright = 0.0f;break;
		case GLUT_KEY_UP : 
		case GLUT_KEY_DOWN : 
			dforward = 0;break;
	}
}

void mousePressMove(int x,int y){
	if(mouseActive){
		if (!lastMouseX) lastMouseX=x;
		if (!lastMouseY) lastMouseY=y;
		angleLR+=(float)(x-lastMouseX)/100;
		angleUD+=(float)(y-lastMouseY)/100;
		if(angleUD>(float)PI) angleUD=(float)PI;
		if(angleUD<0) angleUD=0;
		lastMouseX=x;
		lastMouseY=y;
		orientMe(angleLR,angleUD);
	}

}
void mouseClick(int button, int state, int x, int y){
	if(button==GLUT_RIGHT_BUTTON){
		if (mouseActive)
			mouseActive=false;
		else{
			mouseActive=true;
			lastMouseX=x;
			lastMouseY=y;
		}

	}
	//_CrtDumpMemoryLeaks();

}
void ProcessKeys(){

	// Process keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i){
			// Force controls
			//OLD CONTROL
			/*case 'i': { gUtsimo->utsNx->userCar.motorForce = gUtsimo->utsNx->userCar.maxMotorPower; break;}
			case 'k': { gUtsimo->utsNx->userCar.motorForce = - gUtsimo->utsNx->userCar.maxMotorPower; break;}
			case 'j': {if (gUtsimo->utsNx->userCar.steeringValue > (-1.0f + gUtsimo->utsNx->userCar.steeringDelta))   
						  gUtsimo->utsNx->userCar.steeringValue -= gUtsimo->utsNx->userCar.steeringDelta;
				gUtsimo->utsNx->userCar.isSteering = true; break; 
					  }
			case 'l': {if (gUtsimo->utsNx->userCar.steeringValue < (1.0f - gUtsimo->utsNx->userCar.steeringDelta))
						  gUtsimo->utsNx->userCar.steeringValue += gUtsimo->utsNx->userCar.steeringDelta; 
				gUtsimo->utsNx->userCar.isSteering = true; break; 
					  }*/
			//NEWCONTROL 

			case 'i':{ gUtsimo->utsNx->userRaycastVehicleAcceleration=1; break;}
			case 'k':{ gUtsimo->utsNx->userRaycastVehicleAcceleration=-1; break;}
			case 'j':{ gUtsimo->utsNx->userRaycastVehicleSteering=-1; break;}
 			case 'l':{ gUtsimo->utsNx->userRaycastVehicleSteering=1; break;}

			case 'a':{pos+=NxVec3(0,1,0); break;
					 }
			case 'y':{pos+=NxVec3(0,-1,0); break;
					 }





		}
	}

}


void DebugViewer::init(){
	int windowID;

	// initialize glut
	int dummy1=0;
	char** dummy2=NULL;
	glutInit(&dummy1, dummy2);


  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );

  // define the size
  glutInitWindowSize(800,600);

  // the position where the window will appear
  glutInitWindowPosition(100,100);

  windowID = glutCreateWindow("UTSiMo DebugViewer");
		
  glutDisplayFunc(DisplayFunction);
	glutIgnoreKeyRepeat(1);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);
   glutReshapeFunc(changeSize);
   glutIdleFunc(DisplayFunction);

   glutMotionFunc(mousePressMove); 
   glutMouseFunc(mouseClick);

   //Physics Display lists
   InitShapeDisplayLists();

   //////////DEPTH BUFFER

   	glClearDepth(1.0f);							// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);							// The Type Of Depth Test To Do


	glEnable(GL_CULL_FACE);

    // Setup lighting
    glEnable(GL_LIGHTING);
    float AmbientColor[]    = { 0, 0, 0, 1.0f };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 1.0f, 1.0f, 1.0f, 1.0f };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]   = { 1.0f, 1.0f, 1.0f, 1.0f };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[]        = { 0.0f, 1.0f, 0.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);

   glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
   
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glEnable ( GL_COLOR_MATERIAL ) ;
   //glLineWidth(2.0);


   glutPostRedisplay();
   glutMainLoopEvent();
   //this->lastStepTime=(float)clock();
   
   if(!QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond)) cout<< "ticks fehler";

   UINT64 tmp;
	QueryPerformanceCounter((LARGE_INTEGER* ) &tmp);
	this->lastStepTime=(float)tmp;
 
}
void DebugViewer::step(){
	
     UINT64 tmp;
	 QueryPerformanceCounter((LARGE_INTEGER* ) &tmp);

	 this->stepLength=1000*(tmp-this->lastStepTime)/(float)(__int64)ticksPerSecond;
	 stepDuration=stepLength;

	 this->lastStepTime=(float)tmp;
	 
	 // Flush car inputs
	 gUtsimo->utsNx->userCar.motorForce = 0;

	 // If we're not steering, straighten out the wheel
	 if (!gUtsimo->utsNx->userCar.isSteering) gUtsimo->utsNx->userCar.steeringValue = 0;
	 gUtsimo->utsNx->userCar.isSteering = false;
	
	 ProcessKeys();

	 glutPostRedisplay();
	 glutMainLoopEvent();


																				UINT64 tmp2;
																				QueryPerformanceCounter((LARGE_INTEGER* ) &tmp2);
																				lastDrawingDuration=(tmp2-tmp)/(float)(__int64)ticksPerSecond*1000;
}

void drawing_renderGrid(){
	float ypos=-30;
	float fineOffsetX=(float)(((int)(pos.x))/10)*10;
	float fineOffsetZ=(float)(((int)(pos.z))/10)*10;
  /* Render grid over 0..rows, 0..columns. */
  glBegin(GL_LINES);
    /* Horizontal lines. */
  glColor3f(0.2f, 0.2f, 0.2f);
    for (float i=-100; i<=100; i++) {
      glVertex3f(fineOffsetX-100,ypos, fineOffsetZ+i);
      glVertex3f(fineOffsetX+100,ypos, fineOffsetZ+i);
    }
    /* Vertical lines. */
    for (float i=-100; i<=100; i++) {
      glVertex3f(fineOffsetX+i,ypos, fineOffsetZ-100);
      glVertex3f(fineOffsetX+i,ypos, fineOffsetZ+100);
    }
	glColor3f(0.3f, 0.3f, 0.3f);

	for (float i=-100; i<=100; i+=10) {
      glVertex3f(fineOffsetX-100,ypos, fineOffsetZ+i);
      glVertex3f(fineOffsetX+100,ypos, fineOffsetZ+i);
    }
	for (int i=-100; i<=100; i+=10) {
      glVertex3f(fineOffsetX+i,ypos, fineOffsetZ-100);
      glVertex3f(fineOffsetX+i,ypos, fineOffsetZ+100);
    }
	glColor3f(0.4f, 0.4f, 0.4f);
	      glVertex3f(0,ypos, -100);
		  glVertex3f(0,ypos, 100);
		  glVertex3f(-100,ypos, 0);
		  glVertex3f(100,ypos, 0);
  glEnd();
}
void drawing_renderLanes(){



	glColor3f(0.0f, 0.0f, 0.3f);
	glBegin(GL_QUADS);
	for (int i =0;i<gUtsimo->numOfLanes;i++){
		vecVertex(gUtsimo->lanes[i].rectangle[0]);
		vecVertex(gUtsimo->lanes[i].rectangle[1]);
		vecVertex(gUtsimo->lanes[i].rectangle[2]);
		vecVertex(gUtsimo->lanes[i].rectangle[3]);
	}
	glEnd();

	glColor3f(0.2f, 0.2f, 1.0f);
	glBegin(GL_LINES);
	for (int i =0;i<gUtsimo->numOfLanes;i++){
		vecVertex(gUtsimo->lanes[i].start+NxVec3(0,0.1f,0));
		vecVertex(gUtsimo->lanes[i].end+NxVec3(0,0.1f,0));	
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int i =0;i<gUtsimo->numOfLanes;i++){
		if (gUtsimo->lanes[i].endCrossing->index==-1)
			glColor3f(1.0f, 0.0f, 0.0f);
		else
			glColor3f(0.2f, 0.2f, 1.0f);

		vecVertex(gUtsimo->lanes[i].end+NxVec3(1,0,1));
		vecVertex(gUtsimo->lanes[i].end+NxVec3(1,0,-1));
		vecVertex(gUtsimo->lanes[i].end+NxVec3(-1,0,-1));
		vecVertex(gUtsimo->lanes[i].end+NxVec3(-1,0,1));
	}
	glEnd();

	for (int i =0;i<gUtsimo->numOfLanes;i++){
			char* buffer;
			buffer= new char[10];
			itoa(gUtsimo->lanes[i].registeredCarLengthFromCorridor,buffer,10);
			glColor3f(0.9f, 0.5f, 0.5f);
			NxVec3 tmp=gUtsimo->lanes[i].end-gUtsimo->lanes[i].diffVec/2;
			renderBitmapString(tmp.x,tmp.y,tmp.z,GLUT_BITMAP_HELVETICA_10,buffer);
			delete [ ]buffer;
			glColor3f(0.8f, 0.0f, 0.0f);
	}

	for (int i =0;i<gUtsimo->numOfLanes;i++){
			char* buffer;
			buffer= new char[10];
			itoa(i,buffer,10);
			glColor3f(8.0f, 8.0f, 1.0f);
			renderBitmapString(gUtsimo->lanes[i].end.x,gUtsimo->lanes[i].end.y,gUtsimo->lanes[i].end.z,GLUT_BITMAP_HELVETICA_10,buffer);
			delete [ ]buffer;
	}


}
void drawing_renderStreets(){
	glColor3f(0.0f, 0.4f, 0.4f);
	glBegin(GL_QUADS);
	/*for (int i =0;i<gUtsimo->streets.size();i++){
		vecVertex(gUtsimo->streets[i].rectangleCrossingPoints[0]-NxVec3(0,0.5f,0));
		vecVertex(gUtsimo->streets[i].rectangleCrossingPoints[1]-NxVec3(0,0.5f,0));
		vecVertex(gUtsimo->streets[i].rectangleCrossingPoints[2]-NxVec3(0,0.5f,0));
		vecVertex(gUtsimo->streets[i].rectangleCrossingPoints[3]-NxVec3(0,0.5f,0));
	}*/
	for (int i =0;i<gUtsimo->streets.size();i++){
		vecVertex(gUtsimo->streets[i].rectangle[0]-NxVec3(0,0.5f,0));
		vecVertex(gUtsimo->streets[i].rectangle[1]-NxVec3(0,0.5f,0));
		vecVertex(gUtsimo->streets[i].rectangle[2]-NxVec3(0,0.5f,0));
		vecVertex(gUtsimo->streets[i].rectangle[3]-NxVec3(0,0.5f,0));
	}
	glEnd();
}


void drawing_renderCrossings(){
	glColor3f(0.2f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	for (int i =0;i<gUtsimo->numOfCrossings;i++){
		vecVertex(gUtsimo->crossings[i]->rectangle[3]);
		vecVertex(gUtsimo->crossings[i]->rectangle[2]);
		vecVertex(gUtsimo->crossings[i]->rectangle[1]);
		vecVertex(gUtsimo->crossings[i]->rectangle[0]);
	}
	glEnd();
	glColor3f(0.8f, 0.0f, 0.0f);
	
	for (int i =0;i<gUtsimo->numOfCrossings;i++){
		for (unsigned h=0;h<gUtsimo->crossings[i]->corridors.size();h++){
			glBegin(GL_LINES);
			for (unsigned j=0;j</*Worldparams::INTERPOLATIONSTEPS-1*/10;j++){
				vecVertex(NxVec3(0,.2f,0)+gUtsimo->crossings[i]->corridors[h].preCalculated[(int)(Worldparams::INTERPOLATIONSTEPS/10)*j]);
				vecVertex(NxVec3(0,.2f,0)+gUtsimo->crossings[i]->corridors[h].preCalculated[(int)(Worldparams::INTERPOLATIONSTEPS/10)*(j+1)-1]);
			}
			glEnd();

			char* buffer;
			buffer= new char[10];
			itoa(gUtsimo->crossings[i]->corridors[h].rowSemaphore,buffer,10);
			glColor3f(0.9f, 0.5f, 0.5f);
			NxVec3 tmp=gUtsimo->crossings[i]->corridors[h].preCalculated[(int)(Worldparams::INTERPOLATIONSTEPS/2)];
			renderBitmapString(tmp.x,tmp.y,tmp.z,GLUT_BITMAP_HELVETICA_10,buffer);
			delete [ ]buffer;
			glColor3f(0.8f, 0.0f, 0.0f);
		}
	}
	
}
void drawing_renderSpawns(){
	glColor3f(0.0f, 0.7f, 0.5f);
	glBegin(GL_QUADS);
	for (int i =0;i<gUtsimo->singleSpawns.size();i++){
		vecVertex(gUtsimo->singleSpawns[i].rectangle[0]);
		vecVertex(gUtsimo->singleSpawns[i].rectangle[1]);
		vecVertex(gUtsimo->singleSpawns[i].rectangle[2]);
		vecVertex(gUtsimo->singleSpawns[i].rectangle[3]);
	}
	glEnd();
	glColor3f(0.0f, 0.5f, 0.7f);
	glBegin(GL_QUADS);
	for (int i =0;i<gUtsimo->rateSpawns.size();i++){
		vecVertex(gUtsimo->rateSpawns[i].rectangle[0]);
		vecVertex(gUtsimo->rateSpawns[i].rectangle[1]);
		vecVertex(gUtsimo->rateSpawns[i].rectangle[2]);
		vecVertex(gUtsimo->rateSpawns[i].rectangle[3]);
	}
	glEnd();
}
void drawing_renderCars(){
	
	
	
	for(int laneIndex=0; laneIndex < gUtsimo->numOfLanes; laneIndex++){
		Lane* lane=&gUtsimo->lanes[laneIndex];
		for (unsigned carIndex=0; carIndex < gUtsimo->lanes[laneIndex].cars.size(); carIndex++){
			Car* c =gUtsimo->lanes[laneIndex].cars[carIndex];

			NxReal carHeight=c->carType->getHeight();

			
			NxMat34 pose(c->getOrientationQuat() ,c->middle);
	 glEnable(GL_LIGHTING);
			glPushMatrix();
			SetupGLMatrix(pose.t, pose.M);
			glScaled(c->carType->getLength()/2, c->carType->getHeight()/2, c->carType->getWidth()/2);
			if (c->leader) glColor3f(0.0f, 1.0f, 0.0f);
			else glColor3f(1.0f, 0.0f, 0.0f);

			//blocked
			//if (c->currentRightOfWay()) glColor3f(1.0f, 1.0f, 0.0f);

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
			glPopMatrix();
	 glDisable(GL_LIGHTING);
			
			/*
			NxVec3 veloNorma= c->orientation; veloNorma.normalize();

			if (c->leader) glColor3f(0.0f, 1.0f, 0.0f);
			else glColor3f(1.0f, 0.0f, 0.0f);
			
			NxVec3 veloOrthoNorma= veloNorma.cross(Worldparams::SKY_VECTOR); veloOrthoNorma.normalize();
			NxVec3 p1=c->offsetPosition + veloNorma*c->carType->length*0.75 + veloOrthoNorma*c->carType->width/2+NxVec3(0,0.0f,0);
			NxVec3 p2=c->offsetPosition - veloNorma*c->carType->length*0.25 + veloOrthoNorma*c->carType->width/2+NxVec3(0,0.0f,0);
			NxVec3 p3=c->offsetPosition - veloNorma*c->carType->length*0.25 - veloOrthoNorma*c->carType->width/2+NxVec3(0,0.0f,0);
			NxVec3 p4=c->offsetPosition + veloNorma*c->carType->length*0.75 - veloOrthoNorma*c->carType->width/2+NxVec3(0,0.0f,0);

			
			glBegin(GL_QUADS);
				
				vecVertex(p1);
				vecVertex(p2);
				vecVertex(p3);
				vecVertex(p4);

				vecVertex(p1);
				vecVertex(p1+carHeight*c->currentLane->skyNorma);
				vecVertex(p2+carHeight*c->currentLane->skyNorma);
				vecVertex(p2);

				vecVertex(p2);
				vecVertex(p2+carHeight*c->currentLane->skyNorma);
				vecVertex(p3+carHeight*c->currentLane->skyNorma);
				vecVertex(p3);

				vecVertex(p3);
				vecVertex(p3+carHeight*c->currentLane->skyNorma);
				vecVertex(p4+carHeight*c->currentLane->skyNorma);
				vecVertex(p4);

				vecVertex(p4);
				vecVertex(p4+carHeight*c->currentLane->skyNorma);
				vecVertex(p1+carHeight*c->currentLane->skyNorma);
				vecVertex(p1);

				vecVertex(p4+carHeight*c->currentLane->skyNorma);
				vecVertex(p3+carHeight*c->currentLane->skyNorma);
				vecVertex(p2+carHeight*c->currentLane->skyNorma);
				vecVertex(p1+carHeight*c->currentLane->skyNorma);


			glEnd();
			*/
			
			if (c->leader){
				glColor3f(0.7f, 0.7f, 0.7f);
				glBegin(GL_LINES);
					vecVertex(c->offsetPosition+NxVec3(0,1,0));
					vecVertex(c->leader->offsetPosition+NxVec3(0,1,0));
				glEnd();
			}
			if ((c->laneState==Car::IN_PREPARATION) && (c->currentLane->cars[0]==c) && (!c->currentRightOfWay())){
				glColor3f(0.2f, 0.2f, 0.7f);
				glBegin(GL_LINES);
					vecVertex(c->offsetPosition+NxVec3(0,1,0));
					vecVertex(c->corridor->endLane->start+NxVec3(0,1,0));
				glEnd();
			}

			char* buffer;
			buffer= new char[15];
			itoa(c->ID,buffer,10);
			//gcvt(c->velocity.magnitude(),8,buffer);
			glColor3f(1.0f, 1.0f, 1.0f);
			renderBitmapString(c->position.x,c->position.y+carHeight+1,c->position.z,GLUT_BITMAP_HELVETICA_10,buffer);
			delete []buffer;

		} 
	}
	
}
void drawing_renderPhysics(){
	 // Render all the actors in the scene
   glColor3f(0.5f, 0.5f, 0.5f);

    int nbActors = gUtsimo->utsNx->scene->getNbActors();

    NxActor** actors = gUtsimo->utsNx->scene->getActors();
    while (nbActors--)
    {
		NxActor* actor = *actors++;
		if (!gUtsimo->utsNx->userRaycastVehicleSpawned
			||(actor!=gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor))
			DrawActor(actor);
    }


}

void drawing_renderUserCars(){
	if (gUtsimo->utsNx->userRaycastVehicleSpawned)
		DrawActor(gUtsimo->utsNx->userRaycastCar.vehicle->_bodyActor);
}

void drawing_renderInfofield(){
			NxVec3 fieldPos=pos+target*5;
			NxVec3 orthoNorma=target.cross(NxVec3(0,1,0));
			orthoNorma.normalize();
			NxVec3 orthoNorma2=target.cross(orthoNorma);
			orthoNorma2.normalize();
				//Kasten
				glColor4f(0.0f, 0.5f, 0.0f, 0.3f);
			
				glBegin(GL_QUADS);
					vecVertex(fieldPos+3.6f*orthoNorma-2.6f*orthoNorma2+target);
					vecVertex(fieldPos-3.6f*orthoNorma-2.6f*orthoNorma2+target);
					vecVertex(fieldPos-3.6f*orthoNorma-2.1f*orthoNorma2+target);
					vecVertex(fieldPos+3.6f*orthoNorma-2.1f*orthoNorma2+target);
				glEnd();


			glColor3f(1.0f, 1.0f, 1.0f);
			fieldPos=pos+target*5-2*(orthoNorma+orthoNorma2);
			fieldPos-=orthoNorma*0.5;
			char buffer[128];
			strcpy(buffer, "number of cars: ");
			char num[15];
			itoa(gUtsimo->numOfCurrentCars ,num,10);
			strcat(buffer,num);
			glColor3f(1.0f, 1.0f, 1.0f);
			renderBitmapString(fieldPos.x, fieldPos.y, fieldPos.z,GLUT_BITMAP_HELVETICA_12,buffer);

			fieldPos+=orthoNorma;
			char buffer2[128];
			strcpy(buffer2, "time for drawing last frame: ");
			char num2[15];
			gcvt(lastDrawingDuration,3,num2);
			strcat(buffer2,num2);
			strcat(buffer2," ms");
			glColor3f(1.0f, 1.0f, 1.0f);
			renderBitmapString(fieldPos.x, fieldPos.y, fieldPos.z,GLUT_BITMAP_HELVETICA_12,buffer2);

			fieldPos+=orthoNorma*2;
			char buffer3[128];
			strcpy(buffer3, "time for pure UTSiMo: ");
			char num3[15];
			gcvt(gUtsimo->microsecondsaverage,5,num3);
			strcat(buffer3,num3);
			strcat(buffer3," ms");
			glColor3f(1.0f, 1.0f, 1.0f);
			renderBitmapString(fieldPos.x, fieldPos.y, fieldPos.z,GLUT_BITMAP_HELVETICA_12,buffer3);

		
			fieldPos-=orthoNorma*3;
			fieldPos+=orthoNorma2*0.1;
			char buffer4[128];
			strcpy(buffer4, "timestep: ");
			char num4[15];
			if (gUtsimo->artificialTimeStep==-1)
				strcat(buffer4,"realtime");
			else {
				gcvt(gUtsimo->artificialTimeStep,5,num4);
				strcat(buffer4,num4);
				strcat(buffer4," ms");
			}
			glColor3f(1.0f, 1.0f, 1.0f);
			renderBitmapString(fieldPos.x, fieldPos.y, fieldPos.z,GLUT_BITMAP_HELVETICA_12,buffer4);

			fieldPos+=orthoNorma2*0.1;
			char buffer5[128];
			strcpy(buffer5, "simulation steps: ");
			char num5[15];
			itoa(gUtsimo->stepCounter ,num5,10);
			strcat(buffer5,num5);
			glColor3f(1.0f, 1.0f, 1.0f);
			renderBitmapString(fieldPos.x, fieldPos.y, fieldPos.z,GLUT_BITMAP_HELVETICA_12,buffer5);

			fieldPos+=orthoNorma*2;
			char buffer6[128];
			strcpy(buffer6, "Layers active: ");
			for (int i=0; i<10;i ++){
				if (gLayers[i]){
					char t[3];
					itoa(i+1 ,t,10);
					strcat(buffer6,t);
					strcat(buffer6,"  ");
					glColor3f(1.0f, 1.0f, 1.0f);
				}
			}
			renderBitmapString(fieldPos.x, fieldPos.y, fieldPos.z,GLUT_BITMAP_HELVETICA_12,buffer6);
}

void vecVertex(NxVec3 vec){
	glVertex3f((float)vec.x,(float)vec.y,(float)vec.z);
}
void renderBitmapString(
		float x, 
		float y, 
		float z, 
		void *font, 
		char *string) {  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}