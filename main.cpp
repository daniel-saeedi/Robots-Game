#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <random>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

constexpr char PLAYER = '@';
constexpr char SPACE = '.';
constexpr char GARBAGE = '*';
constexpr char ROBOT = '+';
constexpr char TELEPORT = 't';
const string PLAYER_WINS = "Player Wins!";
const string ROBOTS_WIN = "Robots Win!";

typedef vector<char> WordRow;
typedef vector<WordRow> WordTable;

struct Point
{
    int x;
    int y;
    Point(int x=0, int y=0) 
        : x(x), y(y) {}
    // assignment operator modifies object, therefore non-const
    Point& operator=(const Point& a)
    {
        x=a.x;
        y=a.y;
        return *this;
    }
    // addop. doesn't modify object. therefore const.
    Point operator+(const Point& a) const
    {
        return Point(a.x+x, a.y+y);
    }
    // equality comparison. doesn't modify object. therefore const.
    bool operator==(const Point& a) const
    {
        return (x == a.x && y == a.y);
    }
    void print()
    {
        cout << "x:" << x << " y:" << y << endl;
    }
};

double distance(Point p1,Point p2)
{
    double x_diff = p1.x + (-1*p2.x);
    double y_diff = p1.y + (-1*p2.y);
    return sqrt(x_diff*x_diff + y_diff*y_diff);
}

int random_num(int min,int max)
{
    int finalNum = rand()%(max-min+1)+min;
    return finalNum;
}

class ReadData
{
public:
    void run();
    int get_dim(){return dim;}
    string get_moves(){return operations;}
    WordTable get_map(){return map;}
private:
    int dim;
    string operations;
    WordTable map;
    WordRow read_word_row(int table_size);
    WordTable read_word_table(int table_size);
};

void ReadData::run()
{
    cin >> dim;
    map = read_word_table(dim);
    cin >> operations;
}

WordRow ReadData::read_word_row(int table_size) {
    WordRow word_row;
    for(int i = 0; i < table_size; i++) {
        char character;
        cin >> character;
        word_row.push_back(character);
    }
    return word_row;
}

WordTable ReadData::read_word_table(int table_size) {
    WordTable word_table;
    for(int i = 0; i < table_size; i++) {
        WordRow word_row = read_word_row(table_size);
        word_table.push_back(word_row);
    }
    return word_table;
}

class Game
{
public:
    Game(int _dim,string _moves,WordTable _map) : dim(_dim),operations(_moves),map(_map)
    {
        moves_delta['h'] = Point(0,-1);
        moves_delta['y'] = Point(-1,-1);
        moves_delta['k'] = Point(-1,0);
        moves_delta['u'] = Point(-1,+1);
        moves_delta['l'] = Point(0,+1);
        moves_delta['n'] = Point(+1,+1);
        moves_delta['j'] = Point(+1,0);
        moves_delta['b'] = Point(+1,-1);
        moves_delta['t'] = Point(0,0);
        result = _map;
        Point player_pos = find_player_pos();
        is_finished = false;
    }
    void run();
private:
    bool is_finished;
    int dim;
    string operations;
    WordTable map;
    WordTable result;
    std::map<char, Point> moves_delta;
    Point player_pos;
    void move(Point delta);
    void move_robots();
    void move_robot(Point current_point);
    void move_player();
    void teleport();
    bool is_invalid(Point point);
    bool is_move_allowed(Point pos);
    Point find_player_pos();
    Point find_minimum_distance_point(Point robot_pos);
    vector<Point> find_robots();
    void check_game_status();
    Point generate_random_cell();
    void print_result();
    void print();
};

void Game::run()
{
    player_pos = find_player_pos();
    for(int i = 0;i < operations.length();i++)
    {
        check_game_status();
        if(is_finished) break;
        char operation = operations[i];
        if (operation == TELEPORT)
            teleport();
        if(!is_invalid(player_pos))
        {
            Point delta = moves_delta[operation];
            move(delta);
        }
        print();
    }
    print_result();
}

void Game::move(Point delta)
{
    Point new_player_pos = player_pos + delta;
    if(!is_invalid(new_player_pos)) player_pos = new_player_pos;
    move_robots();
    move_player();
}

void Game::teleport()
{
    Point current_player_pos = find_player_pos();
    Point dest = generate_random_cell();
    player_pos = dest;
    result[current_player_pos.x][current_player_pos.y] = SPACE;
    if(result[dest.x][dest.y] == SPACE) result[dest.x][dest.y] = PLAYER;
    else
    {
        player_pos = Point(-1,-1);
        is_finished = true;
    }
}

Point Game::generate_random_cell()
{
    Point cell;
    int random = random_num(0,dim*dim - 1);
    cell.x = random/dim;
    cell.y = random%dim;
    return cell;
}

void Game::move_player()
{
    Point current_player_pos = find_player_pos();
    if(!is_invalid(current_player_pos))
    {
        Point new_player_pos = player_pos;
        if(result[new_player_pos.x][new_player_pos.y] == ROBOT || result[new_player_pos.x][new_player_pos.y] == GARBAGE)
        {
            result[current_player_pos.x][current_player_pos.y] = SPACE;
        }
        else
        {
            result[current_player_pos.x][current_player_pos.y] = SPACE;
            result[new_player_pos.x][new_player_pos.y] = PLAYER;
        }
    }
}

void Game::move_robots()
{
    vector<Point> robots = find_robots();
    for(int i = 0; i < robots.size();i++)
    {
        move_robot(robots[i]);
    }
}
void Game::move_robot(Point current_point)
{
    if(result[current_point.x][current_point.y] != ROBOT) return;
    Point min_dis_pos = find_minimum_distance_point(current_point);
    char dest = result[min_dis_pos.x][min_dis_pos.y];
    
    if(dest == GARBAGE || dest == ROBOT)
    {
        result[current_point.x][current_point.y] = SPACE;
        result[min_dis_pos.x][min_dis_pos.y] = GARBAGE;
        map[min_dis_pos.x][min_dis_pos.y] = GARBAGE;
    }
    else
    {
        result[current_point.x][current_point.y] = SPACE;
        result[min_dis_pos.x][min_dis_pos.y] = ROBOT;
    }
}

void Game::check_game_status()
{
    Point player_pos = find_player_pos();
    vector<Point> robots = find_robots();
    if(robots.size() == 0 || is_invalid(player_pos)) 
        is_finished = true;
}

void Game::print_result()
{
    Point player_pos = find_player_pos();
    vector<Point> robots = find_robots();
    if(robots.size() == 0) 
    {
        cout << PLAYER_WINS << endl;   
    }
    else if(is_invalid(player_pos))
    {
        cout << ROBOTS_WIN << endl;
    }
}


Point Game::find_minimum_distance_point(Point robot_pos)
{
    Point min_dis_pos = robot_pos;
    for (auto const& move : moves_delta)
    {
        Point delta = move.second;
        Point new_pos = robot_pos + delta;
        if(is_invalid(new_pos)) continue;
        double prior_distance = distance(min_dis_pos,player_pos);
        double new_distance = distance(new_pos,player_pos);
        if(new_distance < prior_distance) min_dis_pos = new_pos;
    }
    return min_dis_pos;
}

Point Game::find_player_pos()
{
    Point INVALID_POS = Point(-1,-1);
    Point position = INVALID_POS;
    for(int i = 0;i < dim;i++)
    {
        for(int j = 0;j < dim;j++)
        {
            if(result[i][j] == PLAYER)
            {
                position = Point(i,j);
                break;
            }
        }
    }
    return position;
}

vector<Point> Game::find_robots()
{
    vector<Point> robots;
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if(result[i][j] == ROBOT)
                robots.push_back(Point(i,j));
        }
    }
    return robots;
}
bool Game::is_invalid(Point point)
{
    int x = point.x;
    int y = point.y;
    return (x < 0) || (y < 0) || (x >= dim) || (y >= dim);
}

bool Game::is_move_allowed(Point pos)
{
    if(result[pos.x][pos.y] == GARBAGE) return false;
    return true;
}

void Game::print()
{
    for(int i = 0;i < dim;i++)
    {
        for(int j = 0;j < dim;j++)
            cout << result[i][j];
        cout << endl;
    }
    cout << endl;
}

int main(int argc, char const *argv[])
{
    int seed = atoi(argv[1]);
    srand(seed);
    ReadData cmd_handler;
    cmd_handler.run();
    Game robots(cmd_handler.get_dim(),cmd_handler.get_moves(),cmd_handler.get_map());
    robots.run();
    return 0;
}