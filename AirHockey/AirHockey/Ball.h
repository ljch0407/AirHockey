#pragma once
#include "framework.h"
#include "Player.h"

class Ball
{
private:
    Point2D m_Position;
    Accel2D m_Accel;
    bool Collide = false;

public:

    Ball();
    Ball(int xPos, int yPos, float xAccel, float yAccel);


    Point2D GetPos();
    Accel2D GetAccel();

    void UpdatePos_x(float posx);
    void UpdatePos_y(float posy);
    
    void UpdateAccel_x();
    void UpdateAccel_y();


    void ChangeAccel_x(int val);
    void ChangeAccel_y(int val);
   

    void CheckcollideCircuit();
    bool CheckCollideRacket(Player* point);

};