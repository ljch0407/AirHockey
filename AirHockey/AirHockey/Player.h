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

    Player(float xPos, float yPos, float xAccel, float yAccel);
    Point2D GetPos();

    Accel2D GetAccel();

    void UpdatePos_x(float pos);
    void UpdatePos_y(float pos);
    void updateAccel_x(float pos);
    void updateAccel_y(float pos);
    void Goal(int score);
    int GetScore(void);
};