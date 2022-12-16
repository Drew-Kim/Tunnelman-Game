// Seong Kim
// CS 30, Section #0171
// Project 3
// This assignment is focused on structuring and 
// creating a game arena with the correct actors
// to provide a video game for the user to play
#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#include "GameConstants.h"
#include "StudentWorld.h"

class StudentWorld; // Declare to create the Game World

//****** Class for Actor derived from GraphObject ******//
class Actor : public GraphObject { // Abstract Base Class
public:
	// Intializer
	Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, 
		unsigned int depth);
	// Destuctor
	virtual ~Actor();
	// Get StudentWorld world
	StudentWorld* getWorld();
	// Actor move
	void moveTo(int x, int y);
	// Get Actor's status
	bool isAlive();
	// Set Actor's status
	void die();
	// Pure Virtual Function
	virtual void doSomething() = 0;
	// Virtual Annoyed Function 
	virtual void isAnnoyed(int hP) {}
private:
	StudentWorld* m_world;
	bool m_alive; // Determine if alive
};

//****** Class for Earth derived from Actor ******//
class Earth :public Actor {
public:
	Earth(StudentWorld* world, int startX, int startY); // Derived from actor class
	~Earth();
	virtual void doSomething();
private:
};

//****** Class for Person derived from Actor ******//
class Person :public Actor {
public:
	// Person constructor for Tunnelman and Protesters
	Person(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp);
	// Get the Health Points
	int getHP();
	// Decrease Health Points
	void decHP(int points);
	// Pure Virutal Functions
	virtual void moveInDirection(Direction direction) = 0;
	virtual void isAnnoyed(int hp) = 0;
private:
	int m_hP; // Health Points
};

//****** Class for TunnelMan derived from Person ******//
class Tunnelman :public Person {
public:
	// Initializer
	Tunnelman(StudentWorld* world);
	// Action 
	virtual void doSomething();
	// Add to Inventory
	void add(int id);
	// Shoot
	void shoot();
	// get Function Water
	int getWtr();
	// get Function Sonar
	int getSonar();
	// get Function Gold Nugget
	int getGld();
	// Virtual Function Annoyed
	virtual void isAnnoyed(int hP);
	// Vituall Function Move
	virtual void moveInDirection(Direction direction);
private:
	// Water 
	int m_wtr;
	// Sonar Kit
	int m_sonar;
	// Gold Nuggets
	int m_gld;
};

//****** Class for Boulder derived from Actor ******//

class Boulder : public Actor {
public:
	// Initialize Boulder
	Boulder(StudentWorld* world, int startX, int startY);
	// doSomething Function
	virtual void doSomething();
	// When falling on Person
	void annoyMan();
private:
	// Stable State
	bool isStable;
	// Falling State
	bool isFalling;
	// Tick tracking for when to falling
	int m_ticks;

};

//****** Class for Squirt derived from Actor ******//

class Squirt : public Actor {
public:
	// Initialize Squirt
	Squirt(StudentWorld* world, int startX, int startY, Direction dir);
	// doSomething virtual Function
	virtual void doSomething();
	// Annoy Protester
	bool annoyProtesters();
private:
	int m_travel; // Travel Length
};

//****** Class for Pickupable derived from Actor ******//

class Pickupable :public Actor { // Abstract Base Class
public:
	// Initialize Pickupable Objects
	Pickupable(StudentWorld* world, int imageID, int startX, int startY);
	// Pure Virutal Function
	virtual void doSomething() = 0;
	// Ticks for disappearing Objects
	void disappearIn(int ticks);
private:
	int m_tick; // Track ticks for disappearing effect
};

//****** Class for Barrel derived from Pickupable ******//

class Barrel : public Pickupable {
public:
	// Initialize Barrel
	Barrel(StudentWorld* world, int startX, int startY);
	// doSomething Virutal Function
	virtual void doSomething();
};

//****** Class for Gold derived from Pickupable ******//

class Gold :public Pickupable {
public:
	// Initialize Gold
	Gold(StudentWorld* world, int startX, int startY, bool visible, bool dropped);
	// doSomething Virtual Function
	virtual void doSomething();
private:
	bool isDropped; // Dropped by Player
};

//****** Class for Goodie derived from Pickupable ******//

class Goodie : public Pickupable {
public:
	// Initialize Goodie
	Goodie(StudentWorld* world, int imageID, int startX, int startY);
	// Virutal doSomething Function
	virtual void doSomething();
};

//****** Class for Sonar derived from Goodie ******//

class Sonar :public Goodie {
public:
	// Initialize Sonar
	Sonar(StudentWorld* world, int startX, int startY);
};

//****** Class for WaterPool derived from Goodie ******//

class WaterPool : public Goodie {
public:
	// Initialize WaterPool
	WaterPool(StudentWorld* world, int startX, int startY);
};

//****** Class for Protester derived from Person ******//

class Protester : public Person {
public:
	// Initialize Protester
	Protester(StudentWorld* world, int imageID, int hP);
	// doSomething Virtual Function
	virtual void doSomething();
	// Bribed Virtual Function
	virtual void getBribed();
	// Protester move 
	void moveInDirection(Direction direction);
	// Protester Random move
	void randomNumToMove();
	// Annoyed Virtual Function
	virtual void isAnnoyed(int hP);
	// Protester Stunned 
	void getStunned();
	// Detect if Protester is facing Player
	bool isFacingPlayer();
	// Helper Function
	Direction directionToPlayer();
	// Determines if Straight Path exists
	bool straightPathToPlayer(Direction direction);
	// Helper Function Random Directions
	Direction randomDirection();
	// Detect if at Intersection
	bool isAtIntersection();
	// Determines direction at Intersection
	void pickViableDirectionToTurn();

private:
	// When Protester is set to leave
	bool m_leave;
	// Protester number to move
	int m_numToMove;
	// Tick since Last move
	int m_tickSinceLastTurn;
	// Tick since Last yell
	int m_tickNoYell;
	// Tick Rest
	int m_tickRest;
};

//****** Class for RegularProtester derived from Protester ******//

class RegularProtester : public Protester {
public:
	// Initialize RegularProtester
	RegularProtester(StudentWorld* world);
};

//****** Class for HardcoreProtester derived from Protester ******//

class HardcoreProtester : public Protester {
public:
	// Initialize HardcoreProtester
	HardcoreProtester(StudentWorld* world);
};



#endif // ACTOR_H_
