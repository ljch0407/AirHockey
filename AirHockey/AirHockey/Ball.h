#pragma once
#include "framework.h"
#include "Player.h"

class Ball
{
private:
    Point2D m_Position;
    Accel2D m_Accel;
    bool Collide;

public:

    Ball();
    Ball(int xPos, int yPos, float xAccel, float yAccel);


    Point2D GetPos();
    Accel2D GetAccel();

    void UpdatePos_x(int Accel);
    void UpdatePos_y(int Accel);
    
    void UpdateAccel_x();
    void UpdateAccel_y();


    void ChangeAccel_x(int val);
    void ChangeAccel_y(int val);
   

    void CheckcollideCircuit();
    void CheckCollideRacket(Player* point);
    void CheckGoal(Player* p1, Player* p2);
};