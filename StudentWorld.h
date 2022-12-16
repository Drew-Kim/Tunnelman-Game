// Seong Kim
// CS 30, Section #0171
// Project 3
// This assignment is focused on structuring and 
// creating a game arena with the correct actors
// to provide a video game for the user to play
#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <queue>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

// Declare Class Objects
class GraphObject;
class Earth;
class Tunnelman;
class Actor;
class Protester;

class StudentWorld : public GameWorld
{
public:
	// Constructor
	StudentWorld(std::string assetDir);
	// Destructor
	~StudentWorld();
	// Get function or Tunnelman
	Tunnelman* getPlayer();

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	// Update the Player's stats
	void updateDisplayText();

	// dig Function for Earth Objects
	bool dig(int x, int y);

	// Helper function to Move a direction
	bool canMoveInDirection(int x, int y, GraphObject::Direction direction);
	// Helper function to detect for Earth objects to place boulders or move
	bool isThereEarth(int x, int y);
	// Helper function to detect for Boulder collision
	bool isThereBoulder(int x, int y, int radius = 3);
	// Helper function to detect Protesters radius and Boulder's radius
	bool withInRadius(int x1, int y1, int x2, int y2, int radius);

	// Add Actor to vector
	void addActor(Actor* actor);
	// Add WaterPool or Sonar Kit
	void addGoodies();
	// Add Protester or Hardcore Protester
	void addProtesters();

	// Decrement Barrels left
	void decBarrel();
	// Decrement Protesters alive
	void decProtester();

	// Add a Boulder, Gold Nugget, or Barrel of Oil
	void addBoulderorGoldorBarrel(int num, char actor);

	// Helper Function to detect if actor is within range of Tunnelman
	bool ActorsWithInRadius(int x, int y, int radius);
	// Helper Function to detect if Player is within range of an Actor
	bool isPlayerInRadius(Actor* actor, int radius);
	// Helper Function to determine if Protester is within Range for detections
	Protester* protesterInRadius(Actor* actor, int radius);
	// Detect Gold Nuggets, Barrels of Oil for Visibility
	void detectNearActors(int x, int y, int radius);
	// Detect to tigger falling boulder
	bool isAboveEarth(int x, int y);
	// Tiggers Protester's movement to exit
	void moveToExit(Protester* pr);
	// Triggers Protester to move towards Player on detection
	GraphObject::Direction senseSignalFromPlayer(Protester* pr, int M);

	// Get First Tick
	bool FirstTick();
private:
	// To Detect Game's first tick
	bool m_isFirstTick;
	// Tick since last Protester was added
	int m_tickSinceLast;
	// Number of Protesters alive
	int m_protestersAlive;
	// Number of Barrels left
	int m_barrelsLeft;
	// Earth Objects
	Earth* m_earth[64][64];
	// Tunnelman
	Tunnelman* m_player;
	// Vector of Actors
	std::vector<Actor*> m_actors;
	// Grid for pathfinding
	int m_maze[64][64];
	struct Grid {
		int x;
		int y;
		Grid(int a, int b) : x(a), y(b) {}
	};
};
#endif // STUDENTWORLD_H_
