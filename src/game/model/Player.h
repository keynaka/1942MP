#ifndef PLAYER_H
#define PLAYER_H
#include <string>
using namespace std;
class Player {
  private:
    string name;
    string color;
    int x;
    int y;
    bool active;
    int health;

  public:
    Player( string name, string color, int x, int y, int health = 4 );
    ~Player();
    string getName();
    string getColor();

    int getX();
    int getY();

    void setX(int x);
    void setY(int y);

    bool isActive();
    void activate();
    void deactivate();
    int getHealth();
    void takeHit();
    bool isAlive();
};
#endif

