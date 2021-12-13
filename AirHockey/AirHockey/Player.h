#pragma once
#include "framework.h"

class Player
{
private:
    Point2D m_Position;
    Accel2D m_Accel;
    int m_Goal;
    int m_score;
public:
    Player();

    Player(int xPos, int yPos, float xAccel, float yAccel);
    Point2D GetPos();

    Accel2D GetAccel();

    void UpdatePos_x(int pos);
    void UpdatePos_y(int pos);
    void UpdateAccel_x(int pos);
    void UpdateAccel_y(int pos);
    void Goal(int score);
    int GetScore(void);
};