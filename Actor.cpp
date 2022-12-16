// Seong Kim
// CS 30, Section #0171
// Project 3
// This assignment is focused on structuring and 
// creating a game arena with the correct actors
// to provide a video game for the user to play
#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

#include "GraphObject.h"
#include <algorithm>

using namespace std;

//******* StudentWorld *******//

// Get the StudentWorld
StudentWorld* Actor::getWorld() {
	return m_world;
}

//******* Actor *******//

// Initialize for Actor
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, 
	unsigned int depth): GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), m_alive(true){
	setVisible(true);
}
// Desctructor
Actor::~Actor(){
	setVisible(false);
}
// move Function for Actor to move within boundaries
void Actor::moveTo(int x, int y) {
	// Ensure to move within the boundaries of the window
	if (x < 0) {
		x = 0;
	}
	if (x > 60) {
		x = 60;
	}
	if (y < 0) {
		y = 0;
	}
	if (y > 60) {
		y = 60;
	}
	GraphObject::moveTo(x, y);
}
// set Function for Actor's status to dead
void Actor::die() {
	m_alive = false;
}
// get Function for Actor's active status
bool Actor::isAlive() {
	return m_alive;
}

//******* Earth *******//

// Initialize for Earth
Earth::Earth(StudentWorld* world, int startX, int startY)
	:Actor(world, TID_EARTH, startX, startY, right, 0.25, 3) {}

// Destructor for Earth
Earth::~Earth() {}

// Earth does nothing
void Earth::doSomething() {}

//******* Person *******//

// Initialize Person
Person::Person(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp)
	:Actor(world, imageID, startX, startY, dir, 1.0, 0), m_hP(hp) {}

// get Health Points
int Person::getHP() {
	return m_hP;
}

// Decrement Health Points
void Person::decHP(int points){
	m_hP -= points;
}

//******* Tunnelman *******//

// Initialize for Tunnelman
Tunnelman::Tunnelman(StudentWorld* world)
	:Person(world, TID_PLAYER, 30, 60, right, 10), m_wtr(5), m_sonar(1), m_gld(0) {}

// doSomething Function for Tunnelman
void Tunnelman::doSomething(){
	if (!isAlive()) { // Check if alive, it not do nothing.
		return;
	}
	int input;
	if (getWorld()->getKey(input) == true) { // When player inputs a key
		switch (input)
		{
		case KEY_PRESS_ESCAPE: // If player press escape key, kill Tunnelman.
			die();
			break;
		// If player presses movement key, and if possible to move one unit & facing direction already, then move player
		case KEY_PRESS_LEFT: moveInDirection(left); break;
		case KEY_PRESS_RIGHT:moveInDirection(right); break;
		case KEY_PRESS_UP:   moveInDirection(up); break;
		case KEY_PRESS_DOWN: moveInDirection(down); break;
		// Player Shoots Water
		case KEY_PRESS_SPACE:
			if (m_wtr > 0) {
				m_wtr--;
				shoot();
			}
			break;
		// Player uses Sonar Kit
		case 'z':
		case 'Z':
			if (m_sonar > 0){
				m_sonar--;
				getWorld()->detectNearActors(getX(), getY(), 12);
				getWorld()->playSound(SOUND_SONAR);
			}
			break;
		// Player drops Gold Nugget for Bribes
		case KEY_PRESS_TAB:
			if (m_gld > 0) {
				getWorld()->addActor(new Gold(getWorld(), getX(), getY(), true, true));
				m_gld--;
			}
			break;
		}
	}
	if (getWorld()->dig(getX(), getY())) { // If there is earth where the Tunnel went, dig dirt.
		getWorld()->playSound(SOUND_DIG);
	}
}

// Add to Tunnelman's Inventory
void Tunnelman::add(int id){
	// Increment Gold Nugget
	if (id == TID_GOLD) {
		m_gld++;
	}
	// Increment Water by 5
	if (id == TID_WATER_POOL) {
		m_wtr += 5;
	}
	// Increment Sonar Kit by 2
	if (id == TID_SONAR) {
		m_sonar += 2;
	}
}

// Tunnelman Shoot
void Tunnelman::shoot() {
	switch (getDirection()){
	// Shoots Water where the Tunnelman is facing
	case left:
		// Detect for Earth Objects and Boulder for valid placement
		if (!getWorld()->isThereEarth(getX() - 4, getY()) && !getWorld()->isThereBoulder(getX() - 4, getY()))
			getWorld()->addActor(new Squirt(getWorld(), getX() - 4, getY(), getDirection()));
		break;
	case right:
		// Detect for Earth Objects and Boulder for valid placement
		if (!getWorld()->isThereEarth(getX() + 4, getY()) && !getWorld()->isThereBoulder(getX() + 4, getY()))
			getWorld()->addActor(new Squirt(getWorld(), getX() + 4, getY(), getDirection()));
		break;
	case down:
		// Detect for Earth Objects and Boulder for valid placement
		if (!getWorld()->isThereEarth(getX(), getY() - 4) && !getWorld()->isThereBoulder(getX(), getY() - 4))
			getWorld()->addActor(new Squirt(getWorld(), getX(), getY() - 4, getDirection()));
		break;
	case up:
		// Detect for Earth Objects and Boulder for valid placement
		if (!getWorld()->isThereEarth(getX(), getY() + 4) && !getWorld()->isThereBoulder(getX(), getY() + 4))
			getWorld()->addActor(new Squirt(getWorld(), getX(), getY() + 4, getDirection()));
		break;
	case none:return;
	}
	// Play Shoot Sound
	getWorld()->playSound(SOUND_PLAYER_SQUIRT);
}

// Tunnelman Annoyed
void Tunnelman::isAnnoyed(int hP){
	// Decrease Health Points
	decHP(hP);
	// Detect if Tunnelman should be dead
	if (getHP() <= 0) {
		die();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}

// Tunnelman move in direction
void Tunnelman::moveInDirection(Direction direction)
{
	switch (direction) {
	case left:
		if (getDirection() == left) {
			// Ensure not to go through Boulders
			if (!getWorld()->isThereBoulder(getX() - 1, getY()))
				moveTo(getX() - 1, getY());
		}
		else setDirection(left);
		break;
	case right:
		if (getDirection() == right) {
			// Ensure not to go through Boulders
			if (!getWorld()->isThereBoulder(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
		}
		else setDirection(right);
		break;
	case up:
		if (getDirection() == up) {
			// Ensure not to go through Boulders
			if (!getWorld()->isThereBoulder(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
		}
		else setDirection(up);
		break;
	case down:
		if (getDirection() == down) {
			// Ensure not to go through Boulders
			if (!getWorld()->isThereBoulder(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
		}
		else setDirection(down);
		break;
	case none: return;
	}
}

// get Function Water
int Tunnelman::getWtr(){
	return m_wtr;
}
// get Function Sonar Kits
int Tunnelman::getSonar(){
	return m_sonar;
}
// get Function Gold Nuggets
int Tunnelman::getGld(){
	return m_gld;
}

//******* Boulder *******//

// Initialize for Boulder
Boulder::Boulder(StudentWorld* world, int startX, int startY)
	: Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1), isStable(true), m_ticks(0){
	world->dig(startX, startY); // Dig Area for Boulder
}

// doSomething Function for Boulder
void Boulder::doSomething() {
	if (!isAlive()) { // Detect if Boulder is broken
		return;
	}
	// Determine if the Boulder should be stable again
	if (isStable) {
		if (getWorld()->isAboveEarth(getX(), getY() - 1))
			return;
		else isStable = false;
	}
	// After a certain amount of ticks, trigger falling state
	if (m_ticks == 30) {
		isFalling = true;
		getWorld()->playSound(SOUND_FALLING_ROCK);
	}
	// Increment tick
	m_ticks++;
	// Falling State, for falling Boulder to detect crash
	if (isFalling) {
		if (getWorld()->isAboveEarth(getX(), getY() - 1) || getWorld()->isThereBoulder(getX(), getY() - 4, 0) || getY() == 0) {
			die();
		}
		else {
			moveTo(getX(), getY() - 1);
		}
		annoyMan();
	}
}

// Boulder Annoy Person  
void Boulder::annoyMan(){
	// Damage Person by 100, if fallen on Person
	if (getWorld()->isPlayerInRadius(this, 3)) {
		getWorld()->getPlayer()->isAnnoyed(100);
	}
	Protester* p = getWorld()->protesterInRadius(this, 3);
	if (p != nullptr) {
		p->isAnnoyed(100);
	}
}

//******* Squirt *******//

// Initialize for Squirt
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction dir)
	:Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1), m_travel(0) {}

// doSomething Function for Tunnelman's Squirt
void Squirt::doSomething() {
	if (!isAlive()) { // Check if squirt is alive
		return;
	}
	// Annoy Protester and set dead OR Squirt reached max length
	if (annoyProtesters() || m_travel == 4) {
		die();
		return;
	}
	switch (getDirection()) {
	case left:
		// Check if blocked by Earth Object or Boulder
		if (getWorld()->isThereEarth(getX() - 1, getY()) || getWorld()->isThereBoulder(getX() - 1, getY())) {
			die(); return;
		}
		else   moveTo(getX() - 1, getY());
		break;
	case right:
		// Check if blocked by Earth Object or Boulder
		if (getWorld()->isThereEarth(getX() + 1, getY()) || getWorld()->isThereBoulder(getX() + 1, getY())) {
			die(); return;
		}
		else   moveTo(getX() + 1, getY());
		break;
	case up:
		// Check if blocked by Earth Object or Boulder
		if (getWorld()->isThereEarth(getX(), getY() + 1) || getWorld()->isThereBoulder(getX(), getY() + 1)) {
			die(); return;
		}
		else moveTo(getX(), getY() + 1);
		break;
	case down:
		// Check if blocked by Earth Object or Boulder
		if (getWorld()->isThereEarth(getX(), getY() - 1) || getWorld()->isThereBoulder(getX(), getY() - 1)) {
			die(); return;
		}
		else moveTo(getX(), getY() - 1);
		break;
	case none: return;
	}
	// Increment Squirt's Travel Length
	m_travel++;
}

// Annoy Protester
bool Squirt::annoyProtesters(){
	// Annoy Protester when in Radius
	Protester* p = getWorld()->protesterInRadius(this, 3);
	if (p != nullptr) {
		p->isAnnoyed(2);
		return true;
	}
	return false;
}

//******* Pickupable *******//

// Initialize for Pickupable
Pickupable::Pickupable(StudentWorld* world, int imageID, int startX, int startY)
	:Actor(world, imageID, startX, startY, right, 1.0, 2) {}

// disappearIn Function
void Pickupable::disappearIn(int ticks){
	if (m_tick == ticks) {
		die();
	}
	else {
		m_tick++;
	}
}

//******* Barrel *******//

//Initialize Barrel
Barrel::Barrel(StudentWorld* world, int startX, int startY)
	:Pickupable(world, TID_BARREL, startX, startY){
	setVisible(false);
}

// doSomething Function
void Barrel::doSomething() {
	if (!isAlive()) { // Check if Alive
		return;
	}
	// If not Visible but in radius, set visible
	if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
		setVisible(true);
		return;
	}
	// If picked up Barrel
	if (getWorld()->isPlayerInRadius(this, 3)) {
		die();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decBarrel();
		return;
	}
}

//******* Gold *******//

// Initialize Gold
Gold::Gold(StudentWorld* world, int startX, int startY, bool isVisible, bool dropped)
	:Pickupable(world, TID_GOLD, startX, startY), isDropped(dropped){
	setVisible(isVisible);
}

// doSomething Function
void Gold::doSomething() {
	if (!isAlive()) { // Check if Alive
		return;
	}
	// If not Visible but in radius, set visible
	if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
		setVisible(true);
		return;
	}
	// If Original Gold and in radius, pick up
	if (!isDropped && getWorld()->isPlayerInRadius(this, 3)) {
		die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->add(getID());
		getWorld()->increaseScore(10);
		return;
	}
	// If dropped Gold, either bribed or disappeared
	if (isDropped) {
		Protester* p = (getWorld()->protesterInRadius(this, 3));
		if (p != nullptr) {
			die();
			p->getBribed();
		};
		disappearIn(100);
	}
}

//******* Goodie *******//

// Initialize Goodie
Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY)
	:Pickupable(world, imageID, startX, startY) {}

// doSomething Function
void Goodie::doSomething() {
	if (!isAlive()) { // Check if Alive
		return;
	}
	// Check if can be picked Up
	if (getWorld()->isPlayerInRadius(this, 3)) {
		die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->add(getID());
		getWorld()->increaseScore(75);
		return;
	}
	// Disappear pickupable object
	disappearIn(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
}

//******* Sonar *******//

// Initialize Sonar
Sonar::Sonar(StudentWorld* world, int startX, int startY)
	:Goodie(world, TID_SONAR, startX, startY) {}

//******* WaterPool *******//

// Initialize WaterPool
WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
	:Goodie(world, TID_WATER_POOL, startX, startY) {}


//******* Protester *******//

// Initialize Protester
Protester::Protester(StudentWorld* world, int imageID, int hP)
	: Person(world, imageID, 60, 60, left, hP), m_leave(false), m_tickSinceLastTurn(200), m_tickNoYell(15){
	randomNumToMove();

	// Is this fine?
	moveTo(59, 60);

	m_tickRest = max(0, 3 - (int)getWorld()->getLevel() / 4);
}

// Random move
void Protester::randomNumToMove(){
	m_numToMove = rand() % 53 + 8;
}

// doSomething Function
void Protester::doSomething(){
	if (!isAlive()) { // Check if Alive
		return;
	}

	// Check if in Rest Tick
	if (m_tickRest > 0) {
		m_tickRest--;
		return;
	}
	// Else, set next rest tick, and increment tracker ticks
	else {
		m_tickRest = max(0, 3 - (int)getWorld()->getLevel() / 4);
		m_tickSinceLastTurn++;
		m_tickNoYell++;
	}
	// Detect if Protester is dead
	if (m_leave) {
		if (getX() == 60 && getX() == 60) {
			die();
			getWorld()->decProtester();
			return;
		}
		getWorld()->moveToExit(this);// move one square closer to exit
		return;
	}

	// Detect if in Radius to Shout
	if (getWorld()->isPlayerInRadius(this, 4) && isFacingPlayer()) {
		if (m_tickNoYell > 15) {
			getWorld()->getPlayer()->isAnnoyed(2);
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			m_tickNoYell = 0;
			return;
		}
		return;
	}
	// HARDCORE PROTESTERS: DETECTION if STATEMENT
	if (getID() == TID_HARD_CORE_PROTESTER) {
		int M = 16 + int(getWorld()->getLevel());
		Direction s = getWorld()->senseSignalFromPlayer(this, M);
		if (s != none) {
			moveInDirection(s);
			return;
		}
	}
	// Detect if Line of Sight exists to Player
	Direction d = directionToPlayer();
	if (d != none && straightPathToPlayer(d) && (!getWorld()->isPlayerInRadius(this, 4))) {
		setDirection(d);
		moveInDirection(d);
		m_numToMove = 0;
		return;
	}
	// Decrement Number to move
	m_numToMove--;
	if (m_numToMove <= 0) {
		Direction k = none;
		k = randomDirection();
		while (true) {
			k = randomDirection();
			if (getWorld()->canMoveInDirection(getX(), getY(), k)) break;
		}
		// Set Direction and new random number to move
		setDirection(k);
		randomNumToMove();
	}
	// Check if at Intersection
	else if (isAtIntersection() && m_tickSinceLastTurn > 200) {
		pickViableDirectionToTurn();
		m_tickSinceLastTurn = 0;
		randomNumToMove();
	}
	// Direction
	moveInDirection(getDirection());
	// Move when capable
	if (!getWorld()->canMoveInDirection(getX(), getY(), getDirection())) {
		m_numToMove = 0;
	}


}

// Protester move Function
void Protester::moveInDirection(Direction direction)
{
	switch (direction) {
	case left:
		if (getDirection() == left) {
			// Check if to not move out of boundary, avoid going through Earth Objects and Boulder
			if (getX() == 0 || (getWorld()->isThereEarth(getX() - 1, getY()) || getWorld()->isThereBoulder(getX() - 1, getY()))) {
				setDirection(right);
				return;
			}
			moveTo(getX() - 1, getY());
		}
		else setDirection(left);
		break;
	case right:
		if (getDirection() == right) {
			// Check if to not move out of boundary, avoid going through Earth Objects and Boulder
			if (getX() == 60 || (getWorld()->isThereEarth(getX() + 1, getY()) || getWorld()->isThereBoulder(getX() + 1, getY()))) {
				setDirection(left);
				return;
			}
			moveTo(getX() + 1, getY());
		}
		else setDirection(right);
		break;
	case up:
		if (getDirection() == up) {
			// Check if to not move out of boundary, avoid going through Earth Objects and Boulder
			if (getY() == 60 || (getWorld()->isThereEarth(getX(), getY() + 1) || getWorld()->isThereBoulder(getX(), getY() + 1))) {
				setDirection(down);
				return;
			}
			moveTo(getX(), getY() + 1);
		}
		else setDirection(up);
		break;
	case down:
		if (getDirection() == down) {
			// Check if to not move out of boundary, avoid going through Earth Objects and Boulder
			if (getY() == 0 || (getWorld()->isThereEarth(getX(), getY() - 1) || getWorld()->isThereBoulder(getX(), getY() - 1))) {
				setDirection(up);
				return;
			}
			moveTo(getX(), getY() - 1);
		}
		else setDirection(down);
		break;
	case none: return;
	}
}

// Detect Line of Sight to Player
GraphObject::Direction Protester::directionToPlayer(){
	// Player Coordinates
	int playerX = getWorld()->getPlayer()->getX();
	int playerY = getWorld()->getPlayer()->getY();
	if (getY() == playerY && getX() == playerX){
		return getDirection();
	}
	// Detect which Direction up or down is line of sight
	if (getX() == playerX) {
		if (getY() < playerY)
			return up;
		if (getY() > playerY)
			return down;
	}
	// Detect which Direction left or right is line of sight
	if (getY() == playerY) {
		if (getX() > playerX)
			return left;
		if (getY() < playerY)
			return right;
	}
	// No Line of Sight
	return none; 
}

// Straight Path to Player
bool Protester::straightPathToPlayer(Direction direction){
	// Player Coordinates
	int playerX = getWorld()->getPlayer()->getX();
	int playerY = getWorld()->getPlayer()->getX();
	switch (direction) {
	case left:
		for (int i = getX(); i >= playerX; i--) {
			// Detect for Earth Objects and Boulder for valid path
			if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY()))
				return false;
		}
		return true;
		break;
	case right:
		for (int i = getX(); i <= playerX; i++) {
			// Detect for Earth Objects and Boulder for valid path
			if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY()))
				return false;
		}
		return true;
		break;
	case up:
		for (int j = getY(); j <= playerY; j++) {
			// Detect for Earth Objects and Boulder for valid path
			if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereBoulder(getX(), j))
				return false;
		}
		return true;
		break;
	case down:
		for (int j = getY(); j >= playerY; j--) {
			// Detect for Earth Objects and Boulder for valid path
			if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereBoulder(getX(), j))
				return false;
		}
		return true;
		break;
	case none:
		return false;
	}
}

// Select Random Direction
GraphObject::Direction Protester::randomDirection(){
	int r;
	r = rand() % 4; // Randomizer
	switch (r) {
	case 0: return left;
	case 1: return  right;
	case 2: return up;
	case 3: return down;
	}
	return none;
}

// Detect if at Intersection
bool Protester::isAtIntersection(){
	// Cross Intersections
	if (getDirection() == up || getDirection() == down) {
		return (getWorld()->canMoveInDirection(getX(), getY(), left) || getWorld()->canMoveInDirection(getX(), getY(), right));
	}
	else
		return (getWorld()->canMoveInDirection(getX(), getY(), up) || getWorld()->canMoveInDirection(getX(), getY(), down));
}

// Pick Viable Direction for Protester
void Protester::pickViableDirectionToTurn(){
	if (getDirection() == up || getDirection() == down) {
		if (!getWorld()->canMoveInDirection(getX(), getY(), left)) setDirection(right);
		else if (!getWorld()->canMoveInDirection(getX(), getY(), right)) setDirection(left);
		else {
			switch (rand() % 2) {
			case 0: setDirection(left);
			case 1: setDirection(right);
			}
		}
	}
	else {
		if (!getWorld()->canMoveInDirection(getX(), getY(), up)) setDirection(down);
		else if (!getWorld()->canMoveInDirection(getX(), getY(), down)) setDirection(up);
		else {
			switch (rand() % 2) {
			case 0: setDirection(up);
			case 1: setDirection(down);
			}
		}
	}

}

// Protester is Annoyed
void Protester::isAnnoyed(int hP){
	if (m_leave) { // Detect if already leaving
		return;
	}
	// Decrease Health Points
	decHP(hP);
	// Play Sound
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	// Stun
	getStunned();
	// Check if dead
	if (getHP() <= 0) {
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_leave = true;
		m_tickRest = 0;
		if (hP == 100) getWorld()->increaseScore(500); // hit by boulder
		else if (getID() == TID_PROTESTER) getWorld()->increaseScore(100);
		else getWorld()->increaseScore(250);
	}
}

// Protester gets Stunned
void Protester::getStunned(){
	m_tickRest = max(50, 100 - (int)getWorld()->getLevel() * 10);
}
// Protester gets Bribed
void Protester::getBribed(){
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	// Increase score apporaiatly and proper behavior
	if (getID() == TID_PROTESTER) {
		getWorld()->increaseScore(25);
		m_leave = true;
	}
	else {
		getWorld()->increaseScore(50);
		m_tickRest = max(50, 100 - int(getWorld()->getLevel()) * 10);
	}
}

// Detect if Protester is facing Player
bool Protester::isFacingPlayer(){
	switch (getDirection()) {
	case left:
		return getWorld()->getPlayer()->getX() <= getX();
	case right:
		return getWorld()->getPlayer()->getX() >= getX();
	case up:
		return getWorld()->getPlayer()->getY() >= getY();
	case down:
		return getWorld()->getPlayer()->getY() <= getY();
	case none:
		return false;
	}
	return false;
}

//******* Regular Protester *******//

// Initialize RegularProtester
RegularProtester::RegularProtester(StudentWorld* world)
	: Protester(world, TID_PROTESTER, 5) {}

//******* HardcoreProtester *******//

// Initialize HardcoreProtester
HardcoreProtester::HardcoreProtester(StudentWorld* world)
	: Protester(world, TID_HARD_CORE_PROTESTER, 20) {}