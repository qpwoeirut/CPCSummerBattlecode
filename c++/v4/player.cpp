#include "pick_move.cpp"

using namespace std;

void readField(vector<vector<Position>>& field) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char a, b;
            cin >> a >> b;
            if (a != '*') {
                field[i][j].height = a - '0';
                field[i][j].ground = b - 'a';
            } else {
                field[i][j].height = UNKNOWN;
                field[i][j].ground = UNKNOWN;
            }
            field[i][j].childTeam = -1;
            field[i][j].pickupTaken = field[i][j].movementTaken = false;
        }
    }
}
Child readChild(const int color) {
    Child child;
    string token;
    cin >> token;
    if (token == "*") {
        // Can't see this child
        child.x = UNKNOWN;
        child.y = UNKNOWN;
    } else {
        // Can see this child, read the description.
        child.x = atoi(token.c_str());
        cin >> child.y;

        // Fill in the child's side.
        child.color = color;

        // Read the child's stance, what he's holding and how much
        // longer he's dazed.
        char ch;
        cin >> ch;
        child.standing = (ch == 'S');
        cin >> ch;
        child.holding = (ch - 'a');
        cin >> child.dazed;
    }
    child.targetClaimed = false;
    return child;
}

void readChildren(vector<vector<Position>>& field, vector<Child>& ourTeam, vector<Child>& theirTeam, vector<pair<int, complex<int>>>& theirLastPosition) {
    for (int i = 0; i < ourTeam.size(); i++) {
        ourTeam[i] = readChild(RED);
        if (ourTeam[i].x != UNKNOWN && ourTeam[i].y != UNKNOWN) {
            field[ourTeam[i].x][ourTeam[i].y].childTeam = RED;
        }
    }
    for (int i = 0; i < theirTeam.size(); i++) {
        theirTeam[i] = readChild(BLUE);
        if (theirTeam[i].x != UNKNOWN && theirTeam[i].y != UNKNOWN) {
            field[theirTeam[i].x][theirTeam[i].y].childTeam = BLUE;
            theirLastPosition[i] = make_pair(0, complex<int>(theirTeam[i].x, theirTeam[i].y));
        } else {
            theirLastPosition[i].first++;
        }
    }
}

int main() {
    // Current game score, for red and blue
    int score[2];

    // Contents of each cell.
    vector <vector<Position>> field(SIZE, vector<Position>(SIZE));

    // List of current information about each child.
    vector <Child> ourTeam(CHILDREN), theirTeam(CHILDREN);

    vector<pair<int, complex<int>>> theirLastPosition(CHILDREN);
    for (int i = 0; i < CHILDREN; i++) {
        theirLastPosition[i] = make_pair(ROUNDS, complex<int>(UNKNOWN, UNKNOWN));
    }

    int turnNum;
    cin >> turnNum;
    while (turnNum >= 0) {
//        cerr << "Turn: " << turnNum << endl;
        cin >> score[RED] >> score[BLUE];
        readField(field);
        readChildren(field, ourTeam, theirTeam, theirLastPosition);

        for (int i = 0; i < CHILDREN; i++) {
//            cerr << "Calculating move for child " << i << endl;
            complex<int> pos(0, 0);
            Move move = pick_move(turnNum, score, field, ourTeam, theirTeam, theirLastPosition, i, pos);
            print_move(move, pos);
//            cerr << "Move for child " << i << ": " << move_to_str(move) << ' ' << pos.real() << ' ' << pos.imag() << endl;
        }

        cin >> turnNum;
    }
}
