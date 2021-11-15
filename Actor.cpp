#include "Actor.h"
#include "StudentWorld.h"
#include <math.h>
#include <thread>
#include <future>

using namespace std;
// Actor Class ////////////////////////////////////////////////////////////////////////////////////////

Actor::Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth,
    StudentWorld* swActor) : GraphObject(imageID, startX, startY, dir, size, depth) {

    GraphObject::setVisible(true);
    _state = true;
    _swActor = swActor;
}

//checks if checkX, checkY is within a specified radius to x,y 
bool Actor::checkRadius(int x, int y, int checkX, int checkY, int radius) {
     if (checkX < 0 || checkX > VIEW_WIDTH || checkY < 0 || checkY > VIEW_WIDTH)
        return false;
    double distance = sqrt((checkX - x) * (checkX - x) + (checkY - y) * (checkY - y));
    if (distance <= radius)
        return true;
    return false;

}

//accessor and mutator for the state of the Actor
void Actor::setState(bool state) {
    _state = state;//return state 
}

bool Actor::getState() {
    return _state;
}

StudentWorld* Actor::getWorld() {
    return _swActor;
}
Actor::~Actor() {

}

//Person Class////////////////////////////////////////
Person::Person(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth,
    StudentWorld* swActor, int hitPoints) : Actor(imageID, startX, startY, dir, size, depth, swActor) {
    _hitPoints = hitPoints;
}

int Person::getHitPoints() {
    return _hitPoints;
}

bool Person::isAnnoyable() {
    return true;
}

bool Person::canBlock() {
    return false;
}

void Person::decHitPoints(int hitPoints) {
    _hitPoints -= hitPoints;
}

void Person::bribe() {
    return;
}

Person::~Person() {

}

// IceMan Class ///////////////////////////////////////////////////////////////////////////////////////

IceMan::IceMan(StudentWorld* swIceMan) : Person(IID_PLAYER, 30, 60, right, 1.0, 0, swIceMan, 10) {
    //initial state of the ice man. 
    _water = 5;
    _sonar = 1;
    _goldNuggets = 0;

}

void IceMan::doSomething() {
    if (!this->getState())
        return;
    int x = this->getX();
    int y = this->getY();
    StudentWorld* world = this->getWorld();

    //Remove ice if ice man's location overlaps with ice objects
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (world->isIcePresent(x + i, y + j)) {
                world->destroyIce(x + i, y + j);
                world->playSound(SOUND_DIG);
            }
        }
    }

    int key;
    if (getWorld()->getKey(key) == true) { // Accessing GameWorld's getKey fn to see if a key was hit by user.
        switch (key) {
        case KEY_PRESS_LEFT:
            if (getDirection() != left)
                setDirection(left);
            else if (getX() > 0 && !(getWorld()->isBlockableActorWithin(x - 1, y, 3)))
                moveTo(getX() - 1, getY());
            else if (!(getX() > 0))
                moveTo(getX(), getY());
            break;

        case KEY_PRESS_RIGHT:
            if (getDirection() != right)
                setDirection(right);
            else if (getX() < (VIEW_WIDTH - 4) && !(getWorld()->isBlockableActorWithin(x + 1, y, 3)))
                moveTo(getX() + 1, getY());
            else if (!(getX() < 0))
                moveTo(getX(), getY());
            break;

        case KEY_PRESS_UP:
            if (getDirection() != up)
                setDirection(up);
            else if (getY() < (VIEW_HEIGHT - 4) && !(getWorld()->isBlockableActorWithin(x, y + 1, 3)))
                moveTo(getX(), getY() + 1);
            else if (!(getY() < (VIEW_HEIGHT - 4)))
                moveTo(getX(), getY());
            break;

        case KEY_PRESS_DOWN:
            if (getDirection() != down)
                setDirection(down);
            else if (getY() > 0 && !(getWorld()->isBlockableActorWithin(x, y - 1, 3)))
                moveTo(getX(), getY() - 1);
            else if (!(getY() > 0))
                moveTo(getX(), getY());
            break;


        case KEY_PRESS_SPACE:
            // add a squirt in front of player.
            //add checks for end of map just because
            if (getWater() > 0) {
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                getWorld()->getIceMan()->decWater();
                switch (getWorld()->getIceMan()->getDirection()) {
                case up:
                    if (!getWorld()->isIcePresent(x, y + 4) && !(getWorld()->isBlockableActorWithin(x, y + 4, 3))) {
                        getWorld()->makeSquirt(x, y + 4);
                    }
                    break;
                case down:
                    if (!getWorld()->isIcePresent(x, y - 4) && !(getWorld()->isBlockableActorWithin(x, y - 4, 3))) {
                        getWorld()->makeSquirt(x, y - 4);
                    }
                    break;
                case left:
                    if (!getWorld()->isIcePresent(x - 4, y) && !(getWorld()->isBlockableActorWithin(x - 4, y, 3))) {
                        getWorld()->makeSquirt(x - 4, y);
                    }
                    break;
                case right:
                    if (!getWorld()->isIcePresent(x + 4, y) && !(getWorld()->isBlockableActorWithin(x + 4, y, 3))) {
                        getWorld()->makeSquirt(x + 4, y);
                    }
                    break;
                }
                break;
            }

        case KEY_PRESS_TAB:
            if (getGoldNuggets() > 0) {
                getWorld()->makeDroppedGoldNugget();
                getWorld()->getIceMan()->decGoldNuggets();
            }
            break;

        case KEY_PRESS_ESCAPE:
            annoy(100);
            break;
        case 'z':
            if (getSonar() > 0) {
                decSonar();
                getWorld()->makeGoodiesVisible(x, y, 12);
            }
            break;
        case 'Z':
            if (getSonar() > 0) {
                decSonar();
                getWorld()->makeGoodiesVisible(x, y, 12);
            }
            break;
        }


    }
}

//annoy
void IceMan::annoy(int annoyValue) {
    if (getHitPoints() != 0) {
        int hitPoints = getHitPoints();
        decHitPoints(annoyValue);
    }
    if (getHitPoints() <= 0) {
        setState(false);
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

//acessor and mutator for barrels
int IceMan::getBarrels() {
    return _barrelsLeft;
}
void IceMan::setBarrels(int barrelsLeft) {
    _barrelsLeft = barrelsLeft;
}
void IceMan::reduceBarrels() {
    _barrelsLeft--;
}


//acessor and mutator for water
int IceMan::getWater() {
    return _water;
}
void IceMan::decWater() {
    _water--;
}
void IceMan::incWater(int num) {
    _water += num;
}
//acessor and mutator for sonar
int IceMan::getSonar() {
    return _sonar;
}
void IceMan::decSonar() {
    _sonar--;
}
void IceMan::incSonar() {
    _sonar++;
}
//acessor and mutator for goldNuggets
int IceMan::getGoldNuggets() {
    return _goldNuggets;
}
void IceMan::incGoldNuggets() {
    _goldNuggets++;
}

void IceMan::decGoldNuggets() {
    _goldNuggets--;
}

IceMan::~IceMan() {
    this->setVisible(false);
}

// Ice Class //////////////////////////////////////////////////////////////////////////////////////////

Ice::Ice(int startX, int startY, StudentWorld* swIce) : Actor(IID_ICE, startX, startY, right, 0.25, 3, swIce) {
}

void Ice::doSomething() {
    return;
}
void Ice::annoy(int) {
    //ice cannot be annnoyed
}

bool Ice::canBlock() {
    return false;// it can block but might not be required to return true for implimentation
}
bool Ice::isAnnoyable() {
    return false;
}

void Ice::bribe() {
    return;
}

Ice::~Ice() {
    this->setVisible(false);
}

// Protester Class ////////////////////////////////////////////////////////////////////////////////////

Protester::Protester(int image, int hitPoints, StudentWorld* swProtester, bool state)
    : Person(image, 60, 60, left, 1.0, 0, swProtester, hitPoints) {
    GraphObject::setVisible(true);
    _image = image;
    _isLeavingField = false;
    _level = getWorld()->getLevel();
    _ticksToWaitBetweenMoves = std::max(0, 3 - _level / 4);
    _ticksToWaitBetweenYells = 15;
    _numSquaresToMoveInCurrentDirection = rand() % (53) + 8; // rand()%(max-min + 1) + min;
    _ticksBetweenPerpendicularTurns = 200;
}

int Protester::getIceManX() {
    return getWorld()->getIceMan()->getX();
}

int Protester::getIceManY() {
    return getWorld()->getIceMan()->getY();
}

// Accessor and Mutator for isLeavingField
bool Protester::getIsLeavingField() {
    return _isLeavingField;
}

void Protester::setIsLeavingField(bool isLeavingField) {
    _isLeavingField = isLeavingField;
}

bool Protester::isAbleToMove() {
    if (_ticksToWaitBetweenMoves <= 0) {
        int waitPeriod = 3 - getWorld()->getLevel() / 4;
        _ticksToWaitBetweenMoves = std::max(0, waitPeriod);
        _ticksToWaitBetweenYells--;
        _ticksBetweenPerpendicularTurns--;
        return true;
    }
    else {
        _ticksToWaitBetweenMoves--;
        return false;
    }
}

bool Protester::isAbleToYell() {
    if (_ticksToWaitBetweenYells <= 0) {

        /*
        bool correctDirection = false;
        switch (getDirection()) {
        case left:
            if (getIceManX() < getX()) {
                _ticksToWaitBetweenYells = 15;
                correctDirection = true;
            }
            break;
        case right:
            if (getIceManX() > getX()) {
                _ticksToWaitBetweenYells = 15;
                correctDirection = true;
            }
            break;
        case up:
            if (getIceManY() > getY()) {
                _ticksToWaitBetweenYells = 15;
                correctDirection = true;
            }
            break;
        case down:
            if (getIceManY() < getY()) {
                _ticksToWaitBetweenYells = 15;
                correctDirection = true;
            }
            break;
        }
        return correctDirection;
        */
        _ticksToWaitBetweenYells = 15;

        return true;
    }
    else {
        return false;
    }
}

bool Protester::isIceManInClearSight() {
    int i = 0;
    if (getX() == getIceManX()) { // If in same column... 
        if (getY() == getIceManY() && getDirection() == down) { // => ice can't be blocking in this proximity.
            moveTo(getX(), getY() - 1);
            setDirection(up);
            return true;
        }
        else if (getY() == getIceManY() && getDirection() == up) { // => ice can't be blocking in this proximity.
            moveTo(getX(), getY() + 1);
            setDirection(down);
            return true;
        } 
        else if (getY() > getIceManY() + 4) { //...and protester is above iceman by at least 5 spaces => can move at least one space down...
             while (!(getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() - i))
                 && !(getWorld()->isBlockableActorWithin(getX(), getY() - i, 3))) { 
             
                 if ((getY() - i) == getIceManY()) { //  If path is clear.
                     setDirection(down);
                     moveTo(getX(), getY() - 1);
                     return true;
                 } 
                 ++i;
             }
            return false;
        }
        else if (getY() + 4 < getIceManY()) { // If protester is below iceman by at least 5 spaces => can move at least one space up...
            while (!(getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() + i))
                && !(getWorld()->isBlockableActorWithin(getX(), getY() + i, 3))) { 
               
                if (getY() + i == getIceManY()) { //  If path is clear.
                    setDirection(up);
                    moveTo(getX(), getY() + 1);
                    return true;
                }
                ++i;
            }
            return false;
        }
    } 

    i = 0;
    if (getY() == getIceManY()) { // If in same row...
        if (getX() == getIceManX() && getDirection() == left) { // => ice can't be blocking in this proximity.
            moveTo(getX() - 1, getY());
            setDirection(right);
            return true;
        }
        else if (getX() == getIceManX() && getDirection() == right) { // => ice can't be blocking in this proximity.
            moveTo(getX() + 1, getY());
            setDirection(left);
            return true;

        } 
        else if (getX() > getIceManX() + 4) { //...and protester is right of iceman by at least 5 spaces => can move at least one space left...
            while (!(getWorld()->isIceAt(getX(), getY(), getX() - i, getY() + 3))
                && !(getWorld()->isBlockableActorWithin(getX() - i, getY(), 3))) { // NEW
                ++i;
                if (getX() - i == getIceManX()) { //  If path is clear up until x coord iceman
                    setDirection(left);
                    moveTo(getX() - 1, getY());
                    return true;
                }
            }
            return false;
        }
        else if (getX() + 4 < getIceManX()) { // If protester is left of iceman by more than 4 spaces => can move at least one space right...
            while (!(getWorld()->isIceAt(getX(), getY(), getX() + i, getY() + 3))
                && !(getWorld()->isBlockableActorWithin(getX() + i, getY(), 3))) { // NEW
                ++i;
                if (getX() + i == getIceManX()) { //  If path is clear.
                    setDirection(right);
                    moveTo(getX() + 1, getY());
                    return true;
                }
            }
            return false;
        }
    }  
        return false;
} 

bool Protester::canPerpendicularlyMove() {
    if (_ticksBetweenPerpendicularTurns > 0) {
        return false;
    }

    int randomNumber = (rand() % 2) + 1; // random num btwn 1 and 2.

    if (getDirection() == left || getDirection() == right) {
        if (getY() < VIEW_HEIGHT - 4 && getY() > 0 
            && !(getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() + 4))
            && !(getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() - 1))
            && !getWorld()->boulderPresent(getX(), getY() + 4)
            && !getWorld()->boulderPresent(getX(), getY() - 1)) { // If able to move left and right randomly select a direction
            if (randomNumber == 1) { // move up
                setDirection(up);
                moveOneSquare();
            }
            else {  // randomNumber == 2 // move down
                setDirection(down);
                moveOneSquare();
            }
            return true;
        }
        else if (getY() < VIEW_HEIGHT - 4
            && !(getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() + 4))
            && !getWorld()->boulderPresent(getX(), getY() + 4)) { // move up

            setDirection(up);
            moveOneSquare();
        }
        else if (getY() > 0 
            && !(getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() - 1))
            && !getWorld()->boulderPresent(getX(), getY() - 1)) { // move down
            setDirection(down);
            moveOneSquare();
        }
        else { // ice above and below
            return false;
        }
        return true;
    }

    else if (getDirection() == up || getDirection() == down) {
        if (getX() > 0 && getX() < VIEW_WIDTH - 4 
            && !(getWorld()->isIceAt(getX(), getY(), getX() - 1, getY() + 3))
            && !(getWorld()->isIceAt(getX(), getY(), getX() + 4, getY() + 3))
            && !getWorld()->boulderPresent(getX() - 1, getY())
            && !getWorld()->boulderPresent(getX() + 4, getY())) {
            if (randomNumber == 1) { // move left
                setDirection(left);
                moveOneSquare();
            }
            else {  // randomNumber == 2 // move right
                setDirection(right);
                moveOneSquare();
            }
            return true;
        }
        else if (getX() > 0 
            && !(getWorld()->isIceAt(getX(), getY(), getX() - 1, getY() + 3))
            && !getWorld()->boulderPresent(getX() - 1, getY())) { // move left
            setDirection(left);
            moveOneSquare();
        }
        else if (getX() < VIEW_WIDTH - 4 
            && !(getWorld()->isIceAt(getX(), getY(), getX() + 4, getY() + 3))
            && !getWorld()->boulderPresent(getX() + 4, getY())) { // move right
            setDirection(right);
            moveOneSquare();
        }
        else { // ice on both sides
            return false;
        }
        return true;
    }
    return true;
}

bool Protester::isBlocked() {

    // if blocked by edge of map, ice, or boulder.
    switch (getDirection()) {
    case left:
        if (getX() <= 0 || getWorld()->isIceAt(getX(), getY(), getX() - 1, getY() + 3)           
            /*|| getWorld()->isBlockableActorWithin(getX() - 1, getY(), 3)*/ //) { // i think wrong
            || getWorld()->boulderPresent(getX() - 1, getY())) {
            return true;
        }
        break;
    case right:
        if (getX() >= (VIEW_WIDTH - 4) || getWorld()->isIceAt(getX(), getY(), getX() + 4, getY() + 3)
            || getWorld()->boulderPresent(getX() + 4, getY())) {
            return true;
        }
        break;
    case up:
        if (getY() >= (VIEW_HEIGHT - 4) || getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() + 4) 
            || getWorld()->boulderPresent(getX(), getY() + 4)) {
            return true;
        }
        break;
    case down:
        if (getY() <= 0 || getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() - 1)
            || getWorld()->boulderPresent(getX(), getY() - 1)) {
            return true;
        }
        break;
    }
    return false;
}

bool Protester::isPathBlocked() {
    return false;
}
    
void Protester::moveOneSquare() {

    /*
    if (_numSquaresToMoveInCurrentDirection <= 0) {
        _numSquaresToMoveInCurrentDirection = rand() % (53) + 8; // Reset to new random number.
        randomDirection(); // set direction to a NEW random direction.
    } */

    /*I was trying to stop the protesters from walking over ice but ended up making them go around in circles
    kinda so I just commented that part out because I don't understand why it's not working*/

    switch (getDirection()) { // Move one square
    case left:
        if (getX() > 0 && !(getWorld()->isIceAt(getX(), getY(), getX() - 1, getY() + 3))
             &&  !(getWorld()->isIceManAt(getX(), getY(), getX() - 4, getY() + 3))  // testing
             && !(getWorld()->isBlockableActorWithin(getX() - 1, getY(), 3)))
            moveTo(getX() - 1, getY());
        break; 
    case right:
        if (getX() < VIEW_WIDTH - 4 && !(getWorld()->isIceAt(getX(), getY(), getX() + 4, getY() + 3))
             && !(getWorld()->isIceManAt(getX(), getY(), getX() + 4, getY() + 3)) 
            && !(getWorld()->isBlockableActorWithin(getX() + 1, getY(), 3)))
            moveTo(getX() + 1, getY());
        break;
    case up:
        if (getY() < VIEW_HEIGHT - 4 && !(getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() + 4))
            && !(getWorld()->isIceManAt(getX(), getY(), getX() + 3, getY() + 4))
            && !(getWorld()->isBlockableActorWithin(getX(), getY() + 1, 3)))
            moveTo(getX(), getY() + 1);
        break;
    case down:
        if (getY() > 0 && !(getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() - 1))
            && !(getWorld()->isIceManAt(getX(), getY(), getX() + 3, getY() - 4))
            && !(getWorld()->isBlockableActorWithin(getX(), getY() - 1, 3)))
            moveTo(getX(), getY() - 1);
        break;
    }
    return;
}

void Protester::randomDirection() { // set direction to a NEW random direction.
    int directionNum = (rand() % 3) + 1; // random num btwn 1 and 3.

    if (getDirection() == up) {
        switch (directionNum) {
        case 1:
            if (getX() > 0)
                setDirection(left);
            break;
        case 2:
            if (getX() < VIEW_WIDTH - 4)
                setDirection(right);
            break;
        case 3:
            if (getY() > 0)
                setDirection(down);
            break;
        }
    }
    else if (getDirection() == down) {
        switch (directionNum) {
        case 1:
            if (getX() > 0)
                setDirection(left);
            break;
        case 2:
            if (getX() < VIEW_WIDTH - 4)
                setDirection(right);
            break;
        case 3:
            if (getY() < VIEW_HEIGHT - 4)
                setDirection(up);
            break;
        }
    }
    else if (getDirection() == left) {
        switch (directionNum) {
        case 1:
            if (getY() > 0)
                setDirection(down);
            break;
        case 2:
            if (getX() < VIEW_WIDTH - 4)
                setDirection(right);
            break;
        case 3:
            if (getY() < VIEW_HEIGHT - 4)
                setDirection(up);
            break;
        }
    }
    else if (getDirection() == right) { 
        switch (directionNum) {
        case 1:
            if (getY() > 0)
                setDirection(down);
            break;
        case 2:
            if (getX() > 0)
                setDirection(left);
            break;
        case 3:
            if (getY() < VIEW_HEIGHT - 4)
                setDirection(up);
            break;
        }
    }
    return;
}

void Protester::doSomething() {
    
    if (!getIsLeavingField()) {
        if (!this->getState()) // If dead, return immediately. 
            return;

        else if (!isAbleToMove()) { // If in rest state, return immediately and decrement ticks left to wait. 
            return;
        }

        else if (getHitPoints() <= 0 && getX() == 60 && getY() == 60) { // If dead and at exit point. 
                this->getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
                setState(false); // Indicator for deletion.
                setVisible(false);
                return;
        }

        else if (checkRadius(getX(), getY(), getIceManX(), getIceManY(), 4) && isAbleToYell()) { // 4   
            // If able (w/ in radius), shout at iceMan. 
                this->getWorld()->playSound(SOUND_PROTESTER_YELL);
                getWorld()->getIceMan()->annoy(2);
                return;
        }
        
        else if (isIceManInClearSight() // If in clear sight, fn switches direction toward iceman and moves one square his way.
            && !checkRadius(getX(), getY(), getIceManX(), getIceManY(), 4)) { 
            return;
        }
     
        else if (--_numSquaresToMoveInCurrentDirection <= 0) { 
            do {
                randomDirection();
            }         
            while (isBlocked()); 

            _numSquaresToMoveInCurrentDirection = rand() % (53) + 8;

            if (!checkRadius(getX(), getY(), getIceManX(), getIceManY(), 3)) {
                moveOneSquare();
                --_numSquaresToMoveInCurrentDirection;
           }
            return;
        }
    
        else if (canPerpendicularlyMove()) { 
            _ticksBetweenPerpendicularTurns = 200;
            _numSquaresToMoveInCurrentDirection = rand() % (53) + 8;
            return;
        } 
        
        else { 
            if (!checkRadius(getX(), getY(), getIceManX(), getIceManY(), 3)) {
                moveOneSquare();
                --_numSquaresToMoveInCurrentDirection;

            }
            
            return;
        } 
    }

    else {
        // should only be calling it once. I think.
        // have to use threads
        // have to take care of ticks and also the direction
        if (!isAbleToMove())  // If in rest state, return immediately and decrement ticks left to wait. // 2
            return;
        goBack();


        //auto fut = async(&Protester::leaveOilField, this);

        //fut.get(); // or wait
    } 
    return;

}

void Protester::leaveOilField() {

    shared_ptr<Coordinate> test = nullptr;
    queue<shared_ptr<Coordinate>> check;
    Coordinate xy({ 60, 60 }, 0);
    check.push(make_shared<Coordinate>(xy)); // push exit point
    store.push(check.front());
    //cerr << "Protester coordinates: " << getX() << " " << getY() << endl;

    while (!check.empty()) { //


        test = check.front(); // next coord to check
        check.pop();
        //cerr << "Currently testing " <<test->getFirst() << " " << test->getSecond() << " " << test->getSteps() << endl;

        if (test->getFirst() == getX() && test->getSecond() == getY()) { // if current guess = loc of protester => protester foun
      //      cerr << "Found: "<< test->getFirst() << " " << test->getSecond() << " " << test->getSteps() << endl;
            _vertex = test;
            setIsLeavingField(true);
            return;
        }

        // check if ice/boulder is up, down, left, right.
        else {

            //can move up
            int x = test->getFirst();
            int y = test->getSecond();
            if (y < 60 && !getWorld()->isIceAt(x, y, x + 3, y + 4) && !(getWorld()->isBlockableActorWithin(x, y + 1, 3))) {
                if (!ifVisited(x, y + 1, store)) {
                    Coordinate p({ x, y + 1 }, test->getSteps() + 1);
                    cerr << p.getFirst() << " " << p.getSecond() << " " << p.getSteps() << endl;
                    check.push(make_shared<Coordinate>(p));
                    store.push(make_shared<Coordinate>(p));
                }
            }
            //checking if can move right
            if (x < 60 && !getWorld()->isIceAt(x, y, x + 4, y + 3) && !(getWorld()->isBlockableActorWithin(x + 1, y, 3))
                && !ifVisited(x + 1, y, store)) {
                Coordinate p({ x + 1, y }, test->getSteps() + 1);
                cerr << p.getFirst() << " " << p.getSecond() << " " << p.getSteps() << endl;
                check.push(make_shared<Coordinate>(p));
                store.push(make_shared<Coordinate>(p));

            }
            //can move left
            if (x > 0 && !getWorld()->isIceAt(x, y, x - 1, y + 3) && !(getWorld()->isBlockableActorWithin(x - 1, y, 3))
                && !ifVisited(x - 1, y, store)) {
                
                Coordinate p({ x - 1 ,y }, test->getSteps() + 1);
                cerr << p.getFirst() << " " << p.getSecond() << " " << p.getSteps() << endl;
                check.push(make_shared<Coordinate>(p));
                store.push(make_shared<Coordinate>(p));

            }
            //can move down
            if (y > 0 && !getWorld()->isIceAt(x, y, x + 3, y - 1) && !(getWorld()->isBlockableActorWithin(x, y - 1, 3))
                && !ifVisited(x, y - 1, store)) {

                Coordinate p({ x, y - 1 }, test->getSteps() + 1);
                cerr << p.getFirst() << " " << p.getSecond() << " " << p.getSteps() << endl;
                check.push(make_shared<Coordinate>(p));
                store.push(make_shared<Coordinate>(p));
            }
        }
    }
    //cerr << "End of leave oil field" << endl;
}

bool Protester::ifVisited(int x, int y, stack<shared_ptr<Coordinate>> index) {
    //cerr << "In ifvisited" << endl;
    while (!index.empty()) {
        if (index.top()->getFirst() == x && index.top()->getSecond() == y) {
     //       cerr << "Visited" << endl;
            return true;
        }

        index.pop();
    }
   // cerr << "Not visited" << endl;
    return false;
}

void Protester::goBack() {
    // cerr << "In goBack() " << endl;
     //cerr << " protester coordinate: " << _vertex->getFirst() << " " << _vertex->getSecond() << " " << _vertex->getSteps()<<endl;
    if (_vertex->getFirst() == 60 && _vertex->getSecond() == 60) {
        setState(false);
        return;
    }
    //cerr << " observing coordinate: " << store.top()->getFirst() << " " << store.top()->getSecond() << " " << store.top()->getSteps() << endl;

    while (!store.empty()) {

        if (store.top()->getSteps() == (_vertex->getSteps() - 1)) {

            //up
            if (store.top()->getFirst() == _vertex->getFirst() && store.top()->getSecond() == _vertex->getSecond() + 1) {
                setDirection(up);
                moveTo(store.top()->getFirst(), store.top()->getSecond());
                //cerr << "Moving up" << endl;
                _vertex = store.top();
                store.pop();
                return;
            }
            //down
            else if (store.top()->getFirst() == _vertex->getFirst() && store.top()->getSecond() == _vertex->getSecond() - 1) {
                setDirection(down);
                moveTo(store.top()->getFirst(), store.top()->getSecond());
                //cerr << "Moving down" << endl;
                _vertex = store.top();
                store.pop();
                return;
            }
            //right
            else if (store.top()->getFirst() == _vertex->getFirst() + 1 && store.top()->getSecond() == _vertex->getSecond()) {
                setDirection(right);
                moveTo(store.top()->getFirst(), store.top()->getSecond());
                //cerr << "Moving right" << endl;
                _vertex = store.top();
                store.pop();
                return;
            }
            //left
            else if (store.top()->getFirst() == _vertex->getFirst() - 1 && store.top()->getSecond() == _vertex->getSecond()) {
                setDirection(left);
                moveTo(store.top()->getFirst(), store.top()->getSecond());
                //cerr << "Moving left" << endl;
                _vertex = store.top();
                store.pop();
                return;
            }
            else {
                store.pop();

            }     
        }
        else {
            store.pop();

        }
    }
}

void Protester::annoy(int value) {
    if (getIsLeavingField())
        return;
    if (getHitPoints() > 0) {
        decHitPoints(value);
        if (getHitPoints() > 0) {
            getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
            int N = 100 - (getWorld()->getLevel() * 10);
            setTicksToWaitBetweenMoves(std::max(50, N));  //stunn the protester
            
        }
    }
    if (getHitPoints() <= 0) {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        //setIsLeavingField(true);
        annoyScoreInc(value);
        setTicksToWaitBetweenMoves(0);
        leaveOilField();
    }

}
void Protester::annoyScoreInc(int value){ }
void Protester::bribe() { return; }

void Protester::setTicksToWaitBetweenMoves(int ticksToWaitBetweenMoves) {
    _ticksToWaitBetweenMoves = ticksToWaitBetweenMoves;
}

Protester::~Protester() {}


// Regular Protester Class ////////////////////////////////////////////////////////////////////////////

RegularProtester::RegularProtester(StudentWorld* swRegProtester)
    : Protester(IID_PROTESTER, 5, swRegProtester, true) {
}


void RegularProtester::annoyScoreInc(int value){
    if (value == 100)
            getWorld()->increaseScore(500);
        else
            getWorld()->increaseScore(100);
}
void RegularProtester::bribe() {
    getWorld()->increaseScore(25);
    leaveOilField();
}
RegularProtester::~RegularProtester() { }

//////  Hardcore Protester//////////////////////////////////////
HardcoreProtester::HardcoreProtester(StudentWorld* swPtr) :
    Protester(IID_HARD_CORE_PROTESTER, 20, swPtr, true) {

}


void HardcoreProtester::bribe() {

    getWorld()->increaseScore(50);
    int hold = 100 - getWorld()->getLevel() * 10;
    int ticks_to_stare = std::max(50, hold); 
    setTicksToWaitBetweenMoves(ticks_to_stare);

}
void HardcoreProtester::annoyScoreInc(int value){
    if (value == 100)
            getWorld()->increaseScore(500);
        else
            getWorld()->increaseScore(250);
}

HardcoreProtester::~HardcoreProtester() { }


//////Goodie Class//////////////////////////////////////////////////////////////////////////

Goodie::Goodie(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth,
    StudentWorld* swGoodie) : Actor(imageID, startX, startY, dir, size,
        depth, swGoodie) { }

void Goodie::annoy(int) { }  //none of the goodies can be annoyed

bool Goodie::canBlock() { return false; } //Only Boulders canBlock!

bool Goodie::isAnnoyable() { return false; }  //goodies cannot be annoyed

void Goodie::bribe() { return; }  //goodies can't be bribed 

Goodie::~Goodie() { }

////////////////BOULDER CLASS///////////////////////////////////////////////////////////
Boulder::Boulder(int x, int y, StudentWorld* swBoulder) : Goodie(IID_BOULDER, x, y, down, 1.0,
    1, swBoulder) {
    _stable = true;
    _playSound = true;
}

void Boulder::doSomething() {
    static int wait = 0;

    if (!getState())
        return;

    if (_stable) {
        if (isIcePresentBelow(this->getX(), this->getY()))
            return;
        else {
            if (wait <= 30) {
                ++wait;
                return;
            }
            else {
                _stable = false;
                wait = 0;
            }
        }
    }
    else if (!_stable) {
        if (_playSound) {
            getWorld()->playSound(SOUND_FALLING_ROCK);
            _playSound = false;
        }
        //check if iceMan is within a 3 sqaure radius
        if (checkRadius(getX(), getY(), getWorld()->getIceMan()->getX(), getWorld()->getIceMan()->getY(), 3)) {
            getWorld()->getIceMan()->annoy(100);
        }
        getWorld()->annoyPerson(getX(), getY(), 100, 3, true);//make 3
        fall();
    }
}

void Boulder::fall() {
    int x = getX();
    int y = getY();
    if (y == 0) {
        setState(false);
    }
    else if (isIcePresentBelow(x, y) || getWorld()->boulderPresent(x, y - 1)
        || getWorld()->boulderPresent(x + 1, y - 1) || getWorld()->boulderPresent(x + 2, y - 1)
        || getWorld()->boulderPresent(x + 3, y - 1)) {
        setState(false);
    }
    else {
        moveTo(x, y - 1);
    }

    return;
}



//checks if ice is present in any of the four columns in the row below the x,y position
bool Boulder::isIcePresentBelow(int x, int y) {
    bool present = false;
    for (int i = 0; i < 4; i++) {
        if (getWorld()->isIcePresent(x + i, y - 1))
            present = true;
    }
    return present;
}

bool Boulder::canBlock() {
    if (!_stable)
        return false;
    else
        return true;
}

Boulder::~Boulder() { }

///////////OIL BARREL////////////////////////////////////////
OilBarrel::OilBarrel(int x, int y, StudentWorld* swGoodie) : Goodie(IID_BARREL, x, y, right, 1.0, 2, swGoodie)
{
    setVisible(false); // oil is not visible initially
}

void OilBarrel::doSomething() {
    //if dead the function returns 
    if (!getState())
        return;

    int iceManX = getWorld()->getIceMan()->getX();
    int iceManY = getWorld()->getIceMan()->getY();
    int x = getX();
    int y = getY();

    //if not visible yet check to see if iceMan is within 4 square radius. If yes make visible and return
    if (!isVisible()) {

        if (!isVisible()) {
            if (checkRadius(x, y, iceManX, iceManY, 4)) {
                setVisible(true);
                return;

            }
            else
                return;
        }
    }
    //if visible and within 3 square radius pick it up
    else if (isVisible() && checkRadius(x, y, iceManX, iceManY, 3)) {
        setState(false);
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->getIceMan()->reduceBarrels();
    }


}


OilBarrel::~OilBarrel() {
    //setVisible(false); 
}

//////SQUIRT//////////////////////////////////////////////////////////////////////
Squirt::Squirt(int x, int y, Direction dir, StudentWorld* swWorld) : Goodie(IID_WATER_SPURT
    , x, y, dir, 1.0, 1, swWorld) {
    _travelDis = 4;
}

void Squirt::doSomething() {

    //if withing 3 radius of a protestor cause 2 annoyance. setstate dead
    if (getWorld()->annoyPerson(getX(), getY(), 2, 3, false)) {
        setState(false);
        return;
    }
    else if (_travelDis == 0) {
        setState(false);
        return;
    }
    //if ice or boulder infront setStaet to dead
    else if (getState()) {

        switch (getDirection()) {
        case up:
            if (getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() + 4) ||
                getWorld()->boulderPresent(getX(), getY() + 4)) {
                setState(false);
                return;
            }
            break;
        case down:
            if (getWorld()->isIceAt(getX(), getY(), getX() + 3, getY() - 1) ||
                getWorld()->boulderPresent(getX(), getY() - 1)) {
                setState(false);
                return;
            }
            break;
        case right:
            if (getWorld()->isIceAt(getX(), getY(), getX() + 4, getY() + 3) ||
                getWorld()->boulderPresent(getX() + 4, getY())) {
                setState(false);
                return;
            }
            break;
        case left:
            if (getWorld()->isIceAt(getX(), getY(), getX() - 1, getY() + 3) ||
                getWorld()->boulderPresent(getX() - 1, getY())) {
                setState(false);
                return;
            }
            break;

        }
    }

    //need to reduce travle distance
    //else move forward in dir
    if (getState()) {
        switch (getDirection()) {
        case up:
            moveTo(getX(), getY() + 1);
            decTravelDis();
            return;
        case down:
            moveTo(getX(), getY() - 1);
            decTravelDis();
            return;

        case right:
            moveTo(getX() + 1, getY());
            decTravelDis();
            return;

        case left:
            moveTo(getX() - 1, getY());
            decTravelDis();
            return;
        }
    }
}

void Squirt::decTravelDis() {
    --_travelDis;
}

Squirt::~Squirt() {
    setVisible(false);
}

/////Gold Nugget//////////////////////////////////////////////////////////////
GoldNugget::GoldNugget(int x, int y, StudentWorld* swNugget, bool isPickableProtester) : Goodie(IID_GOLD, x, y,
    right, 1.0, 2, swNugget) {
    if (isPickableProtester) {
        _pickableProtester = true;
        _permanent = false;
        setVisible(true);
    }
    else {
        _pickableProtester = false;
        _permanent = true;
        setVisible(false);
    }
}

bool GoldNugget::isPermanent() {
    return _permanent;
}


bool GoldNugget::isPickableProtester() {
    return _pickableProtester;
}

void GoldNugget::doSomething() {
    static int countTicks = 0;
    if (!getState())
        return;
    if (!isPickableProtester()) {
        if (!isVisible() && checkRadius(getX(), getY(), getWorld()->getIceMan()->getX(), getWorld()->getIceMan()->getY(), 4)) {
            setVisible(true);
            return;
        }
        else if (checkRadius(getX(), getY(), getWorld()->getIceMan()->getX(), getWorld()->getIceMan()->getY(), 3)) {
            setState(false);
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->increaseScore(10);
            getWorld()->getIceMan()->incGoldNuggets();
        }
    }
    else if (isPickableProtester()) {
        if (!isPermanent()) {
            countTicks++;
        }
        if (getWorld()->pickGold(getX(), getY(), 3)) {
            setState(false);
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);

        }
        else if (countTicks >= 100) {
            setState(false);
            countTicks = 0;
        }
    }
}

GoldNugget::~GoldNugget() {

}
///Sonar/////////////////////////////////////////////////////////////////
Sonar::Sonar(int x, int y, StudentWorld* swSonar) : Goodie(IID_SONAR, x, y, right, 1.0, 2, swSonar) {
    int num = 300 - (10 * getWorld()->getLevel());
    _ticksLeft = std::max(100, num);
}

int Sonar::getTicks() {
    return _ticksLeft;
}

void Sonar::decTicks() {
    _ticksLeft--;
}
void Sonar::doSomething() {
    if (!getState())
        return;
    if (checkRadius(getX(), getY(), getWorld()->getIceMan()->getX(), getWorld()->getIceMan()->getY(), 3)) {
        setState(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getIceMan()->incSonar();
        getWorld()->increaseScore(75);
    }
    else if (getTicks() <= 0) {
        setState(false);
    }
    else
        decTicks();
}

Sonar::~Sonar() {

}

///Water Pool///////////////////////////////////////////////////////////
WaterPool::WaterPool(int x, int y, StudentWorld* swSonar) : Goodie(IID_WATER_POOL, x, y, right, 1.0, 2, swSonar) {
    int num = 300 - (10 * getWorld()->getLevel());
    _ticksLeft = std::max(100, num);
}

int WaterPool::getTicks() {
    return _ticksLeft;
}

void WaterPool::decTicks() {
    _ticksLeft--;
}



void WaterPool::doSomething() {
    if (!getState())
        return;
    if (checkRadius(getX(), getY(), getWorld()->getIceMan()->getX(), getWorld()->getIceMan()->getY(), 3)) {
        setState(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getIceMan()->incWater(5);
        getWorld()->increaseScore(100);
    }
    else if (getTicks() <= 0) {
        setState(false);
    }
    else
        decTicks();
}


WaterPool::~WaterPool() {

}
