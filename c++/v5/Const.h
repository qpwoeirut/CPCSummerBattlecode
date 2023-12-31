#ifndef __CHALLENGE_UTIL_H__
#define __CHALLENGE_UTIL_H__

// Constants supporting a player in the icypc challenge game.  Feel
// free to use this and extend it for your own implementation.
//
// ICPC Challenge
// Sturgill, Baylor University

const int ROUNDS = 180;

/** Width and height of the playing field. */
const int SIZE = 31;
  
/** Number of children on each team. */
const int CHILDREN = 4;

const int UNKNOWN = -1;

/** Constants for the objects in each cell of the field */
const int GROUND_EMPTY = 0;  // Just powdered snow in this space.
const int GROUND_TREE = 1;   // A tree in this space
const int GROUND_S = 2;      // A small snowball in this space
const int GROUND_M = 3;      // A medium snowball in this space
const int GROUND_MS = 4;     // A small snowball on a medium one
const int GROUND_L = 5;      // A large snowball in this space
const int GROUND_LM = 6;     // A medium snowball on a large one.
const int GROUND_LS = 7;     // A small snowball on a large one.
const int GROUND_SMR = 8;    // A red Snowman in this space
const int GROUND_SMB = 9;    // A blue Snowman in this space
  
/** Constants for the things a child can be holding */
const int HOLD_EMPTY = 0;    // Child is holding nothing
const int HOLD_P1 = 1;       // Child is holding one unit of powdered snow
const int HOLD_P2 = 2;       // Child is holding two units of powdered snow
const int HOLD_P3 = 3;       // Child is holding three units of powdered snow
const int HOLD_S1 = 4;       // Child is holding one small snowball.
const int HOLD_S2 = 5;       // Child is holding two small snowballs.
const int HOLD_S3 = 6;       // Child is holding three small snowballs.
const int HOLD_M = 7;        // Child is holding one medium snowball.
const int HOLD_L = 8;        // Child is holding one large snowball.

/** Constant for the red player (self) color */
const int RED = 0;

/** Constant for the blue player (opponent) color */
const int BLUE = 1;

/** Height for a standing child. */
const int STANDING_HEIGHT = 9;
  
/** Height for a crouching child. */
const int CROUCHING_HEIGHT = 6;

/** Maximum Euclidean distance a child can throw. */
const int THROW_LIMIT = 24;

/** Snow capacity limit for a space. */
const int MAX_PILE = 9;

/** Snow that's too deep to move through. */
const int OBSTACLE_HEIGHT = 6;


/** Simple representation for a child in the Game. */
struct Child {
    // Location of the child.
    int x, y;

    // True if  the child is standing.
    bool standing;

    // Side the child is on.
    int color;

    // What's the child holding.
    int holding;

    // How many more turns this child is dazed.
    int dazed;

    bool targetClaimed = false;

    const int holdingSize() const {
        switch (holding) {
            case HOLD_EMPTY: return 0;  // nothing
            case HOLD_P1:    return 1;     // Child is holding one unit of powdered snow
            case HOLD_P2:    return 2;     // Child is holding two units of powdered snow
            case HOLD_P3:    return 3;     // Child is holding three units of powdered snow
            case HOLD_S1:    return 1;     // Child is holding one small snowball
            case HOLD_S2:    return 2;     // Child is holding two small snowballs
            case HOLD_S3:    return 3;     // Child is holding three small snowballs
            case HOLD_M:     return 2;      // Child is holding one medium snowball
            case HOLD_L:     return 3;      // Child is holding one large snowball
        }
        return 0;
    }
};

const int INIT = 1001001001;

#endif
