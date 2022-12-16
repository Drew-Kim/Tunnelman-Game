// Seong Kim
// CS 30, Section #0171
// Project 3
// This assignment is focused on structuring and 
// creating a game arena with the correct actors
// to provide a video game for the user to play

#include "StudentWorld.h"
#include <string>

#include "GraphObject.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "Actor.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

// Arena Initializer
StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir), m_isFirstTick(true), m_tickSinceLast(0), m_protestersAlive(0), m_player(nullptr), m_barrelsLeft(0) {
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

// Arena Destructor
StudentWorld::~StudentWorld(){
	cleanUp();
}

// get the Player's Tunnelman
Tunnelman* StudentWorld::getPlayer(){
	return m_player;
}

// get the First Tick
bool StudentWorld::FirstTick() {
	return m_isFirstTick;
}

// StudentWorld Initializer
int StudentWorld::init() {
	 // Reset the Tick, Protesters, and Barrel Count
	m_isFirstTick = true;
	m_tickSinceLast = 0;
	m_protestersAlive = 0;
	m_barrelsLeft = 0;
	// Create the Tunnelman 
	m_player = new Tunnelman(this);
	// Initialize Earth Objects
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 60; y++) {
			if (x < 30 || x>33 || y < 4) { // Ensure Starting Mine Shaft Is Clear
				m_earth[x][y] = new Earth(this, x, y);
			}
		}
	}
	// Arguments for Number of Gold Nuggets, Barrels, and Boulders
	int B = min((int)getLevel() / 2 + 2, 9);
	int G = max((int)getLevel() / 2, 2);
	int L = min(2 + (int)getLevel(), 21);
	// Create the correct number each
	addBoulderorGoldorBarrel(B, 'B');
	addBoulderorGoldorBarrel(L, 'L');
	addBoulderorGoldorBarrel(G, 'G');

	return GWSTATUS_CONTINUE_GAME;
}

// move Function for the Game
int StudentWorld::move(){
	// Update the Player's stats
	updateDisplayText();
	// Iterate through the actor vector to trigger actions 
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->isAlive()) { // If actor is alive then trigger action
			(*it)->doSomething();
		}
	}
	// Check if player is not alive
	if (!m_player->isAlive()) {
		decLives(); // Decrease life and return dead
		return GWSTATUS_PLAYER_DIED;
	}
	// Check if the player finished the level
	if (m_barrelsLeft == 0) {
		return GWSTATUS_FINISHED_LEVEL;
	}
	// Trigger player to do something
	m_player->doSomething();
	// Add either WaterPool or Sonar Kit to Arena
	addGoodies();
	// Add a Protester
	addProtesters();

	// Ensure to delete the dead actors by going through the vector
	for (it = m_actors.begin(); it != m_actors.end();) {
		if (!(*it)->isAlive()) {
			delete* it;
			it = m_actors.erase(it);
		}
		else {
			it++;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

// StudentWorld cleanUp Function
void StudentWorld::cleanUp(){
	// Delete all Earth Objects
	for (int x = 0; x < 64; x++){
		for (int y = 0; y < 60; y++){
			delete m_earth[x][y];
		}
	}
	// Delete all actors inside vector
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++){
		delete* it;
	}
	// Clear vector
	m_actors.clear();
	// Delete the Tunnelman
	delete m_player;
}

// Update Player's Stats
void StudentWorld::updateDisplayText(){
	stringstream s;
	s.fill('0'); // Ensure to fill the rest of the spaces with 0 for player's score
	s << " Scr: " << setw(6) << getScore() << "  ";
	s.fill(' '); // Ensure to fill the rest of the spaces with ' ' for other stats
	s << "Lvl: " << setw(2) << getLevel() << "  ";
	s << "Lives: " << setw(1) << getLives() << "  ";
	s << "Hlth: " << setw(3) << m_player->getHP() * 10 << "%  ";
	s << "Wtr: " << setw(2) << m_player->getWtr() << "  ";
	s << "Gld: " << setw(2) << m_player->getGld() << "  ";
	s << "Sonar: " << setw(2) << m_player->getSonar() << "  ";
	s << "Oil Left: " << setw(2) << m_barrelsLeft << " ";
	string stats = s.str();
	setGameStatText(stats);

}

// dig Earth Function
bool StudentWorld::dig(int x, int y) {
	// Initialize flag for playing sound
	bool flag = false;
	for (int i = x; i < x + 4; i++) {
		for (int j = y; j < y + 4; j++){
			// If Player is overlapping a valid Earth object
			if (m_earth[i][j] != nullptr) {
				// Delete the Earth Object
				delete m_earth[i][j];
				// Set to nullptr to avoid detecting again
				m_earth[i][j] = nullptr;
				flag = true;
			}
		}
	}
	// Return boolean value to (not) play dig sound
	return flag;
}

// Helper move Function
bool StudentWorld::canMoveInDirection(int x, int y, GraphObject::Direction d)
{
	switch (d) {
		// Returns true if the Tunnelman is valid to move in that direction
		// Ensure the Tunnelman can't move out of Boundaries/Check for Earth & Boulders
	case Actor::left:
		return (x != 0 && !isThereEarth(x - 1, y) && !isThereBoulder(x - 1, y));
	case Actor::right:
		return (x != 60 && !isThereEarth(x + 1, y) && !isThereBoulder(x + 1, y));
	case Actor::up:
		return (y != 60 && !isThereEarth(x, y + 1) && !isThereBoulder(x, y + 1));
	case Actor::down:
		return (y != 0 && !isThereEarth(x, y - 1) && !isThereBoulder(x, y - 1));
	case Actor::none:
		return false;

	}

	// Returns false when the Person cant be allowed to move
	return false;
}
// Detect Earth Helper Function
bool StudentWorld::isThereEarth(int x, int y){
	for (int i = x; i < x + 4; i++) {
		for (int j = y; j < y + 4; j++) {
			// If Earth Object is found, return true
			if (m_earth[i][j] != nullptr)
				return true;
		}
	}
	// Return false, when no Earth object is found
	return false;
}

// Detect Boulder Helper Function
bool StudentWorld::isThereBoulder(int x, int y, int radius){
	vector<Actor*>::iterator it;
	// Iterator through the vector of actors to find a boulder that may be within the radius of the Tunnelman
	for (it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->getID() == TID_BOULDER && withInRadius(x, y, (*it)->getX(), (*it)->getY(), radius))
			return true;
	}
	// Return false, when no boulder is found 
	return false;
}

// Given two coordinates, find if they are within the radius of each other
bool StudentWorld::withInRadius(int x1, int y1, int x2, int y2, int radius){
	// Use the distance forumla to find if the given cordinates are within the radius
	if ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= radius * radius)
		return true;
	// Return false, when not with in Radius
	return false;
}

// Add the actor to the vector 
void StudentWorld::addActor(Actor* actor) {
	m_actors.push_back(actor);
}

// Add a Random item to the Arena
void StudentWorld::addGoodies() {
	// Chance to add a Sonar or WaterPool
	int G = (int)getLevel() + 300;
	if (int(rand() % G) + 1 == 1) {
		// 1/5 Chance to add a Sonar
		if (int(rand() % 5) + 1 == 1) {
			addActor(new Sonar(this, 0, 60));
		}
		// 4/5 Chance to add a WaterPool
		else {
			int x, y;
			do {
				// Find a random valid Spot without EarthObjects for the Waterpool
				x = rand() % 60 + 1;
				y = rand() % 60 + 1;
			} 
			while (isThereEarth(x, y));
			addActor(new WaterPool(this, x, y));
		}
	}
}

// Add the Protester to the Arena
void StudentWorld::addProtesters(){
	// Number of ticks to add Protester
	int T = max(25, 200 - (int)getLevel());
	// Number of Protesters permitted per level
	int P = fmin(15, 2 + getLevel() * 1.5);
	// Probability of a Protester being Hardcore
	int probabilityOfHardcore = min(90, (int)getLevel() * 10 + 30);
	// If it is the first tick of the game, or the next appropriate time to put in another protester
	if (m_isFirstTick || (m_tickSinceLast > T && m_protestersAlive < P)) {
		// If by chance the probability of a hardcore protester, add
		if (rand() % 100 + 1 < probabilityOfHardcore) {
			addActor(new HardcoreProtester(this));
		}
		else {
			addActor(new RegularProtester(this));
		}
		// Reset the protester add tick timer
		m_tickSinceLast = 0;
		// Increment the amount of protesters alive
		m_protestersAlive++;
		// Set FirstTick false, because first tick only happens once
		m_isFirstTick = false;
	}
	m_tickSinceLast++; // Increment tick to track when next protester should be added
}

// Decrement the amount of Barrels left
void StudentWorld::decBarrel(){
	m_barrelsLeft--;
}
// Decrement the amount of Protesters on the Arena
void StudentWorld::decProtester() {
	m_protestersAlive--;
}

// Detect if any actors are within radius
bool StudentWorld::ActorsWithInRadius(int x, int y, int radius) {
	vector<Actor*>::iterator it;
	it = m_actors.begin();
	while (it != m_actors.end()) {
		if (withInRadius(x, y, (*it)->getX(), (*it)->getY(), radius))
			return true;
		it++;
	}
	return false;
}

// Add a Boulder, Gold Nugget, or a Barrel of Oil
void StudentWorld::addBoulderorGoldorBarrel(int num, char actor) {
	// Initalize variables for coordinates to place
	int x, y;
	for (int i = 0; i < num; i++) {
		do {
			x = rand() % 60 + 1;
			if (actor == 'B') {
				y = rand() % 36 + 1 + 20;
			}
			else {
				y = rand() % 56 + 1;
			}
		} 
		while (ActorsWithInRadius(x, y, 6) || (x > 26 && x < 34 && y>0));
		// Case to detect which Object to place
		switch (actor) {
		// Place a Boulder
		case 'B':
			addActor(new Boulder(this, x, y));
			break;
		// Place a Gold Nugget
		case 'G':
			addActor(new Gold(this, x, y, false, false));
			break;
		case 'L':
		// Place a Barrel of Oil 
			addActor(new Barrel(this, x, y));
			m_barrelsLeft++;
			break;
		}
	}
}

// Detect if the Player and the Actor passed is within radius range
bool StudentWorld::isPlayerInRadius(Actor* actor, int radius){
	return withInRadius(actor->getX(), actor->getY(), m_player->getX(), m_player->getY(), radius);
}

// Detect if the Protester is within radius of the Tunnelman
Protester* StudentWorld::protesterInRadius(Actor* actor, int radius) {
	// Iternate through the vector searching for protesters and hardcore protesters
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER) {
			// If within range return the Protester dynamic_cast
			if (withInRadius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY(), radius)) {
				return dynamic_cast<Protester*> (*it);
			}
		}
	}
	// Else, if none found return nullptr
	return nullptr;
}


// Detect Near Gold Nuggets and Barrels of Oil for visibility
void StudentWorld::detectNearActors(int x, int y, int radius)
{
	int a, b;
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->getID() == TID_BARREL || (*it)->getID() == TID_GOLD)
		{
			a = (*it)->getX(); b = (*it)->getY();
			if ((x - a) * (x - a) + (y - b) * (y - b) <= radius * radius)
				(*it)->setVisible(true);
		}
	}
}


// Detect Earth Objects to trigger boulder
bool StudentWorld::isAboveEarth(int x, int y){
	for (int i = x; i < x + 4; i++){
		// Check if any earth objects
		if (m_earth[i][y] != nullptr) {
			return true;
		}
	}
	return false;
}


// Triggers the movement for a protester to exit
void StudentWorld::moveToExit(Protester* pr){
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			m_maze[i][j] = 0;
		}
	}
	// Assign the protester's X and Y coordinates
	int a = pr->getX();
	int b = pr->getY();
	// Initialize a queue for maze-solving 
	queue<Grid> q;
	q.push(Grid(60, 60));
	m_maze[60][60] = 1;
	// Copy the Earth Objects
	while (!q.empty()) {
		Grid c = q.front();
		q.pop();
		int x = c.x;
		int y = c.y;

		// Detect if the Protester can move left
		if (canMoveInDirection(x, y, GraphObject::left) && m_maze[x - 1][y] == 0) {
			q.push(Grid(x - 1, y));
			m_maze[x - 1][y] = m_maze[x][y] + 1;
		}
		// Detect if the Protester can move right
		if (canMoveInDirection(x, y, GraphObject::right) && m_maze[x + 1][y] == 0) {
			q.push(Grid(x + 1, y));
			m_maze[x + 1][y] = m_maze[x][y] + 1;
		}
		// Detect if the Protester can move up 
		if (canMoveInDirection(x, y, GraphObject::up) && m_maze[x][y + 1] == 0) {
			q.push(Grid(x, y + 1));
			m_maze[x][y + 1] = m_maze[x][y] + 1;
		}
		// Detect if the Protester can move down
		if (canMoveInDirection(x, y, GraphObject::down) && m_maze[x][y - 1] == 0) {
			q.push(Grid(x, y - 1));
			m_maze[x][y - 1] = m_maze[x][y] + 1;
		}
	}
	// Performs the movement if possible towards the Tunnelman
	if (canMoveInDirection(a, b, GraphObject::left) && m_maze[a - 1][b] < m_maze[a][b])
		pr->moveInDirection(GraphObject::left);
	if (canMoveInDirection(a, b, GraphObject::right) && m_maze[a + 1][b] < m_maze[a][b])
		pr->moveInDirection(GraphObject::right);
	if (canMoveInDirection(a, b, GraphObject::up) && m_maze[a][b + 1] < m_maze[a][b])
		pr->moveInDirection(GraphObject::up);
	if (canMoveInDirection(a, b, GraphObject::down) && m_maze[a][b - 1] < m_maze[a][b])
		pr->moveInDirection(GraphObject::down);
	return;
}


GraphObject::Direction StudentWorld::senseSignalFromPlayer(Protester* pr, int M) {
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			m_maze[i][j] = 0;
		}
	}
	// Assign the protester's X and Y coordinates
	int a = pr->getX();
	int b = pr->getY();
	queue<Grid> q;
	// Push the Tunnelman on the Grid
	q.push(Grid(getPlayer()->getX(), getPlayer()->getY()));
	m_maze[getPlayer()->getX()][getPlayer()->getY()] = 1;
	while (!q.empty()) {
		Grid c = q.front();
		q.pop();
		int x = c.x;
		int y = c.y;

		// Detect if the Protester can move left
		if (canMoveInDirection(x, y, GraphObject::left) && m_maze[x - 1][y] == 0) {
			q.push(Grid(x - 1, y));
			m_maze[x - 1][y] = m_maze[x][y] + 1;
		}
		// Detect if the Protester can move right
		if (canMoveInDirection(x, y, GraphObject::right) && m_maze[x + 1][y] == 0) {
			q.push(Grid(x + 1, y));
			m_maze[x + 1][y] = m_maze[x][y] + 1;
		}
		// Detect if the Protester can move up
		if (canMoveInDirection(x, y, GraphObject::up) && m_maze[x][y + 1] == 0) {
			q.push(Grid(x, y + 1));
			m_maze[x][y + 1] = m_maze[x][y] + 1;
		}
		// Detect if the Protester can move down
		if (canMoveInDirection(x, y, GraphObject::down) && m_maze[x][y - 1] == 0) {
			q.push(Grid(x, y - 1));
			m_maze[x][y - 1] = m_maze[x][y] + 1;
		}
	}
	// Move in the direction if within range of Player
	if (m_maze[a][b] <= M + 1) {
		if (canMoveInDirection(a, b, GraphObject::left) && m_maze[a - 1][b] < m_maze[a][b])
			return GraphObject::left;
		if (canMoveInDirection(a, b, GraphObject::right) && m_maze[a + 1][b] < m_maze[a][b])
			return GraphObject::right;
		if (canMoveInDirection(a, b, GraphObject::up) && m_maze[a][b + 1] < m_maze[a][b])
			return GraphObject::up;
		if (canMoveInDirection(a, b, GraphObject::down) && m_maze[a][b - 1] < m_maze[a][b])
			return GraphObject::down;
	}
	return GraphObject::none;
}


