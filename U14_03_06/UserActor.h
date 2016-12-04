//UserActor.h
#ifndef USERACTOR_H
#define USERACTOR_H


class UserActor{
public:
	UserActor(NxActorDesc aDesc){actorDesc=aDesc;}
	NxBox obb;
	NxActor* actor;
	NxActorDesc actorDesc;
};


#endif