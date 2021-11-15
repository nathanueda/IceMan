#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <algorithm>

#include "Actor.h"
#include <vector>

class StudentWorld : public GameWorld {
public:

    StudentWorld(std::string assetDir);
    virtual int init(); //gets called 1) game starts 2) during the start of a new level 3) loss of life
                        //when it gets called all previous objects should be deleted 
    virtual int move();
    virtual void cleanUp();
    bool isIcePresent(int x, int y);
    void destroyIce(int x, int y);
    bool isIceManAt(const int actorX, const int actorY, const int endX, const int endY);
    bool isIceAt(const int actorX, const int actorY, const int endX, const int endY);
    bool isBlockableActorNearby(Actor*, int radius);
    bool isBlockableActorWithin(int, int, int radius);
    IceMan* getIceMan();
    bool timeToCreateNewProtester(); // can probably move to private
    bool boulderPresent(int x, int y);
    bool annoyPerson(int x, int y, int annoyValue, int radius, bool annoyAll);
    bool pickGold(int x, int y, int radius);
    void makeDroppedGoldNugget();
    bool overlappingIce(int x, int y);
    void makeSquirt(int, int);
    bool checkRadiusForGoodies(int, int, int);
    void makeGoodiesVisible(int x, int y, int radius);
    virtual ~StudentWorld();

private:
    void makeGold(int);
    void makeSonar();
    void makeWaterPool();
    void makeIceMan();
    void makeOilBarrels(int num);
    void makeBoulders(int);
    void makeIceCubes();
    void makeProtesters();
    void makeGoodies();
    void makeStatString();
    std::string neatly(int, int, int, int, int, int, int, int);
    IceMan* iceMan;

    //(x, y) coordinate: column, row
    Ice* iceCube[VIEW_WIDTH][VIEW_HEIGHT - 4];//an array of ice* so that we can access the objects. 
    std::vector <Actor*> goodies;
    int _ticksSinceLastProtester;
    int _numProtesters;
};

#endif // STUDENTWORLD_H_
