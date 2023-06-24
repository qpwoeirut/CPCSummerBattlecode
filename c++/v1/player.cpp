#include "pick_move.cpp"

using namespace std;

void readField(vector<vector<int>>& height, vector<vector<int>>& ground) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char a, b;
            cin >> a >> b;
            if (a != '*') {
                height[i][j] = a - '0';
                ground[i][j] = b - 'a';
            } else {
                height[i][j] = UNKNOWN;
                ground[i][j] = UNKNOWN;
            }
        }
    }
}
Child readChild(int color) {
    Child child;
    string token;
    cin >> token;
    if (token == "*") {
        // Can't see this child
        child.x = -1;
        child.y = -1;
    } else {
        // Can see this child, read the description.
        child.x = atoi(token.c_str());
        cin >> child.y;

        // Fill in the child's side.
        child.color = color;

        // Read the child's stanse, what he's holding and how much
        // longer he's dazed.
        char ch;
        cin >> ch;
        child.standing = (ch == 'S');
        cin >> ch;
        child.holding = (ch - 'a');
        cin >> child.dazed;
    }
    return child;
}

void readChildren(vector<Child>& ourTeam, vector<Child>& theirTeam) {
    for (int i = 0; i < ourTeam.size(); i++) {
        ourTeam[i] = readChild(RED);
    }
    for (int i = 0; i < theirTeam.size(); i++) {
        theirTeam[i] = readChild(BLUE);
    }
}

int main() {
    // Current game score, for red and blue
    int score[2];

    // Current snow height in each cell.
    vector <vector<int>> height(SIZE, vector<int>(SIZE));

    // Contents of each cell.
    vector <vector<int>> ground(SIZE, vector<int>(SIZE));

    // List of current information about each child.
    vector <Child> ourTeam(CHILDREN), theirTeam(CHILDREN);

    int turnNum;
    cin >> turnNum;
    while (turnNum >= 0) {
        cin >> score[RED] >> score[BLUE];
        readField(height, ground);
        readChildren(ourTeam, theirTeam);

        for (int i = 0; i < CHILDREN; i++) {
            complex<int> pos;
            Move move = pick_move(score, height, ground, ourTeam, theirTeam, pos);
            print_move(move, pos);
        }

        cin >> turnNum;
    }
}
