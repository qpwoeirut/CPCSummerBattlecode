# A simple player that just tries to hit children on the opponent's
# team with snowballs.
#
# Feel free to use this as a starting point for your own player.
# Also, feel free to send in refinements or fixes to this code.  I'm
# not a python programmer.
#
# ICPC Challenge
# Sturgill, UNC Greensboro
# Mang, Henry M. Gunn High School (Gunn Competitive Programming Club)

import enum
import random
import string
import sys
from typing import Union

# Constants supporting a player in the icypc challenge game.  Feel
# free to use this and extend it for your own implementation.

# Width and height of the playing field.
SIZE: int = 31

# Number of children on each team.
CCOUNT: int = 4


class Ground(enum.IntEnum):
    # Constants for the objects in each cell of the field
    empty = 0  # Just powdered snow in this space.
    tree = 1  # A tree in this space
    small_snowball = 2  # A small snowball in this space
    medium_snowball = 3  # A medium snowball in this space
    medium_small_snowman = 4  # A small snowball on a medium one
    large_snowball = 5  # A large snowball in this space
    large_medium_snowman = 6  # A medium snowball on a large one.
    large_small_snowman = 7  # A small snowball on a large one.
    red_snowman = 8  # A red Snowman in this space
    blue_snowman = 9  # A blue Snowman in this space


class Hold(enum.IntEnum):
    # Constants for the things a child can be holding
    empty = 0  # Child is holding nothing
    powder1 = 1  # Child is holding one unit of powdered snow
    powder2 = 2  # Child is holding two units of powdered snow
    powder3 = 3  # Child is holding three units of powdered snow
    small_snowball_one = 4  # Child is holding one small snowball.
    small_snowball_two = 5  # Child is holding two small snowballs.
    small_snowball_third = 6  # Child is holding three small snowballs.
    medium_snowball = 7  # Child is holding one medium snowball.
    large_snowball = 8  # Child is holding one large snowball.


class Team(enum.IntEnum):
    # Constants for the red and blue teams.
    red = 0
    blue = 1


# Height for a standing child.
STANDING_HEIGHT: int = 9

# Height for a crouching child.
CROUCHING_HEIGHT: int = 6

# Maximum Euclidean distance a child can throw.
THROW_LIMIT: int = 24

# Snow capacity limit for a space.
MAX_PILE: int = 9

# Snow that's too deep to move through.
OBSTACLE_HEIGHT: int = 6

# Constant used to mark child locations in the map, not used in this player.
GROUND_CHILD: int = 10


# Representation of a 2D point, used for playing field locations.
class Point:
    def __init__(self, x: int, y: int):
        self.x: int = x
        self.y: int = y

    def set(self, x: int, y: int) -> None:
        self.x = x
        self.y = y


# Simple representation for a child in the game.
class Child:
    def __init__(self):
        # Location of the child.
        self.pos: Point = Point(0, 0)

        # True if the child is standing.
        self.standing: Union[int, bool] = 1

        # Side the child is on.
        self.color: Team = Team.red

        # What's the child holding.
        self.holding: Hold = Hold.empty

        # How many more turns this child is dazed.
        self.dazed: int = 0


# Simple representation for a child's action
class Move:
    # Action the child is making.
    action: str = "idle"

    # Destination of this action (or null, if it doesn't need one) */
    dest = None


# Return the value of x, clamped to the [ a, b ] range.
def clamp(x: int, a: int, b: int) -> int:
    if x < a:
        return a
    if x > b:
        return b
    return x


# Fill in move m to move the child c toward the given target location, either
# crawling or running.
def move_toward(c, target, m):
    if c.standing:
        # Run to the destination
        if c.pos.x != target.x:
            if c.pos.y != target.y:
                # Run diagonally.
                m.action = "run"
                m.dest = Point(
                    c.pos.x + clamp(target.x - c.pos.x, -1, 1),
                    c.pos.y + clamp(target.y - c.pos.y, -1, 1),
                )
            else:
                # Run left or right
                m.action = "run"
                m.dest = Point(c.pos.x + clamp(target.x - c.pos.x, -2, 2), c.pos.y)
        elif c.pos.y != target.y:
            # Run up or down.
            m.action = "run"
            m.dest = Point(c.pos.x, c.pos.y + clamp(target.y - c.pos.y, -2, 2))
    else:
        # Crawl to the destination
        if c.pos.x != target.x:
            # crawl left or right
            m.action = "crawl"
            m.dest = Point(c.pos.x + clamp(target.x - c.pos.x, -1, 1), c.pos.y)
        elif c.pos.y != target.y:
            # crawl up or down.
            m.action = "crawl"
            m.dest = Point(c.pos.x, c.pos.y + clamp(target.y - c.pos.y, -1, 1))


########################################################################################

# Source of randomness
rnd = random.Random()

# Current game score for self (red) and opponent (blue).
score = [0, 0]

# Current snow height in each cell.
height = []

# Contents of each cell.
ground = []

# Allocate the whole field.  Is there a better way to do this?
for i in range(SIZE):
    height.append([0] * SIZE)
    ground.append([0] * SIZE)

# List of children on the field, half for each team.
children_list = []

# Random destination for each player.
run_target = []

# How long the child has left to run toward its destination.
run_timer = []

for i in range(2 * CCOUNT):
    children_list.append(Child())
    run_target.append(Point(0, 0))
    run_timer.append(0)

turn_num = int(sys.stdin.readline())
while turn_num >= 0:
    # read the scores of the two sides.
    tokens = sys.stdin.readline().split()
    score[Team.red] = tokens[0]
    score[Team.blue] = tokens[1]

    # Parse the current map.
    for i in range(SIZE):
        tokens = sys.stdin.readline().split()
        for j in range(SIZE):
            # Can we see this cell?
            if tokens[j][0] == "*":
                height[i][j] = -1
                ground[i][j] = -1
            else:
                height[i][j] = string.digits.find(tokens[j][0])
                ground[i][j] = string.ascii_lowercase.find(tokens[j][1])

    # Read the states of all the children.
    for i in range(CCOUNT * 2):
        c = children_list[i]

        # Can we see this child?
        tokens = sys.stdin.readline().split()
        if tokens[0] == "*":
            c.pos.x = -1
            c.pos.y = -1
        else:
            # Record the child's location.
            c.pos.x = int(tokens[0])
            c.pos.y = int(tokens[1])

            # Compute child color based on it's index.
            if i < CCOUNT:
                c.color = Team.red
            else:
                c.color = Team.blue

            # Read the stance, what the child is holding and how much
            # longer he's dazed.
            c.standing = tokens[2] == "S"

            c.holding = string.ascii_lowercase.find(tokens[3])

            c.dazed = int(tokens[4])

    # Mark all the children in the map, so they are easy to
    # look up.
    for i in range(2 * CCOUNT):
        c = children_list[i]
        if c.pos.x >= 0:
            ground[c.pos.x][c.pos.y] = GROUND_CHILD

    # Decide what each child should do
    for i in range(CCOUNT):
        c = children_list[i]
        m = Move()

        if c.dazed == 0:
            # See if the child needs a new destination.
            while run_timer[i] <= 0 or run_target[i] == c.pos:
                run_target[i].set(rnd.randint(0, SIZE - 1), rnd.randint(0, SIZE - 1))
                run_timer[i] = rnd.uniform(1, 14)

        # Try to acquire a snowball if we need one.
        if c.holding != Hold.small_snowball_one:
            # Crush into a snowball, if we have snow.
            if c.holding == Hold.powder1:
                m.action = "crush"
            else:
                # We don't have snow, see if there is some nearby.
                sx = -1
                sy = -1
                for ox in range(c.pos.x - 1, c.pos.x + 2):
                    for oy in range(c.pos.y - 1, c.pos.y + 2):
                        # Is there snow to pick up?
                        if (
                                0 <= ox < SIZE
                                and 0 <= oy < SIZE
                                and (ox != c.pos.x or oy != c.pos.y)
                                and ground[ox][oy] == Ground.empty
                                and height[ox][oy] > 0
                        ):
                            sx = ox
                            sy = oy

                # If there is snow, try to get it.
                if sx >= 0:
                    if c.standing:
                        m.action = "crouch"
                    else:
                        m.action = "pickup"
                        m.dest = Point(sx, sy)

        else:
            # Stand up if the child is armed.
            if not c.standing:
                m.action = "stand"
            else:
                # Try to find a victim.
                victim_found = 0
                j = CCOUNT
                while j < CCOUNT * 2 and not victim_found:
                    if children_list[j].pos.x >= 0:
                        # We know where this child is, see if it's not too far away.
                        dx = children_list[j].pos.x - c.pos.x
                        dy = children_list[j].pos.y - c.pos.y
                        dsq = dx * dx + dy * dy
                        if dsq < 8 * 8:
                            victim_found = 1
                            m.action = "throw"
                            # throw past the victim, so we will probably hit them
                            # before the snowball falls into the snow.
                            m.dest = Point(c.pos.x + dx * 2, c.pos.y + dy * 2)
                    j += 1

            # Try to run toward the destination.
            if m.action == "idle":
                move_toward(c, run_target[i], m)
                run_timer[i] -= 1

        # Write out the child's move
        if m.dest is None:
            sys.stdout.write("%s\n" % m.action)
        else:
            sys.stdout.write("%s %d %d\n" % (m.action, m.dest.x, m.dest.y))

    sys.stdout.flush()
    turn_num = int(sys.stdin.readline())
