#include "Ball.h"

Ball::Ball()
{
        m_Position.Position_x = 0;
        m_Position.Position_y = 0;

        m_Accel.Accel_x = 0.0;
        m_Accel.Accel_y = 0.0;
        Collide = false;
}

Ball::Ball(int xPos, int yPos, float xAccel, float yAccel)
{
    m_Position.Position_x = xPos;
    m_Position.Position_y = yPos;

    m_Accel.Accel_x = xAccel;
    m_Accel.Accel_y = yAccel;
}

Point2D Ball::GetPos()
{
    return m_Position;
}

Accel2D Ball::GetAccel()
{
    return m_Accel;
}

void Ball::UpdatePos_x(float posx)
{
    m_Position.Position_x = posx;
}

void Ball::UpdatePos_y(float posy)
{
    m_Position.Position_y = posy;
}

void Ball::UpdateAccel_x()
{
    if (m_Accel.Accel_x > 0)
        m_Accel.Accel_x -= 0.1;
    else if (m_Accel.Accel_x < 0)
        m_Accel.Accel_x += 0.1;
    else
        m_Accel.Accel_x = 0;
}
void Ball::UpdateAccel_y()
{
    if (m_Accel.Accel_y > 0)
        m_Accel.Accel_y -= 0.1;
    else if (m_Accel.Accel_y < 0)
        m_Accel.Accel_y += 0.1;
    else
        m_Accel.Accel_y = 0;
}


void Ball::ChangeAccel_x(int val)
{

    m_Accel.Accel_x = val;

    if (abs((int)m_Accel.Accel_x) >= 20)
    {
        if (m_Accel.Accel_x < 0)
        {
            m_Accel.Accel_x = -20;
        }
        else if (m_Accel.Accel_x > 0)
            m_Accel.Accel_x = 20;
    }
}

void Ball::ChangeAccel_y(int val)
{
    m_Accel.Accel_y = val;

    if (abs((int)m_Accel.Accel_y) >= 20)
    {
        if (m_Accel.Accel_y < 0)
        {
            m_Accel.Accel_y = -20;
        }
        else if (m_Accel.Accel_y > 0)
            m_Accel.Accel_y = 20;
    }
}

void Ball::CheckcollideCircuit()
{
    if (m_Position.Position_x + Player_R >= 400)
    {

        ChangeAccel_x(-(m_Accel.Accel_x));
    }
    else if (m_Position.Position_x - Player_R <= 0)
    {
        ChangeAccel_x(abs((int)(m_Accel.Accel_x)));
    }

    if (m_Position.Position_y + Player_R >= 800)
    {
        ChangeAccel_y(-(m_Accel.Accel_y));
    }
    else if (m_Position.Position_y - Player_R <= 0)
    {
        ChangeAccel_y(abs((int)(m_Accel.Accel_y)));
    }
}

bool Ball::CheckCollideRacket(Player* point)
{
    if (m_Position.Position_x > point->GetPos().Position_x - 30 && m_Position.Position_x < point->GetPos().Position_x + 30)
    {
        if (m_Position.Position_x > point->GetPos().Position_x)
        {
            if (m_Position.Position_y + 30 > point->GetPos().Position_y && m_Position.Position_y - 30 < point->GetPos().Position_y)
            {
                if (m_Position.Position_y > point->GetPos().Position_y)
                {
                    Collide = true;
                    return Collide;
                }

                if (m_Position.Position_y < point->GetPos().Position_y)
                {
                    Collide = true;
                    return Collide;
                }

            }
        }
        if (m_Position.Position_x < point->GetPos().Position_x)
        {
            if (m_Position.Position_y + 30 > point->GetPos().Position_y && m_Position.Position_y - 30 < point->GetPos().Position_y)
            {
                if (m_Position.Position_y > point->GetPos().Position_y)
                {
                    Collide = true;
                    return Collide;
                }
                 if (m_Position.Position_y < point->GetPos().Position_y)
                {
                    Collide = true;
                    return Collide;
                }
            }
        }
    }

    Collide = false;
    return Collide;
}

