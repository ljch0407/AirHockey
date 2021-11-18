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

void Ball::UpdatePos_x(int Accel)
{
    m_Position.Position_x += Accel;
    if (m_Position.Position_x + Player_R > 400)
    {
        m_Position.Position_x = 370;
    }
    else if (m_Position.Position_x - Player_R < 0)
    {
        m_Position.Position_x = 30;
    }

}

void Ball::UpdatePos_y(int Accel)
{
    m_Position.Position_y += Accel;

    if (m_Position.Position_y + Player_R > 800)
    {
        m_Position.Position_y = 770;
    }
    else if (m_Position.Position_y - Player_R < 0)
    {
        m_Position.Position_y = 30;
    }
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

void Ball::CheckCollideRacket(Player* point)
{
    if (m_Position.Position_x > point->GetPos().Position_x - 30 && m_Position.Position_x < point->GetPos().Position_x + 30)
    {
        if (m_Position.Position_x > point->GetPos().Position_x)
        {
            if (m_Position.Position_y + 30 > point->GetPos().Position_y && m_Position.Position_y - 30 < point->GetPos().Position_y)
            {
                if (m_Position.Position_y > point->GetPos().Position_y)
                {
                    ChangeAccel_x(abs((int)(m_Accel.Accel_x)) + point->GetAccel().Accel_x);
                    ChangeAccel_y(abs((int)(m_Accel.Accel_y)) + point->GetAccel().Accel_y);
                }

                else if (m_Position.Position_y < point->GetPos().Position_y)
                {
                    ChangeAccel_x(abs((int)(m_Accel.Accel_x)) + point->GetAccel().Accel_x);
                    ChangeAccel_y(-(m_Accel.Accel_y) - point->GetAccel().Accel_y);
                }

            }
        }
        else if (m_Position.Position_x < point->GetPos().Position_x)
        {
            if (m_Position.Position_y + 30 > point->GetPos().Position_y && m_Position.Position_y - 30 < point->GetPos().Position_y)
            {
                if (m_Position.Position_y > point->GetPos().Position_y)
                {
                    ChangeAccel_x(-(m_Accel.Accel_x) - point->GetAccel().Accel_x);
                    ChangeAccel_y(abs((int)(m_Accel.Accel_y)) + point->GetAccel().Accel_y);
                }
                else if (m_Position.Position_y < point->GetPos().Position_y)
                {
                    ChangeAccel_x(-(m_Accel.Accel_x) - point->GetAccel().Accel_x);
                    ChangeAccel_y(-(m_Accel.Accel_y) - point->GetAccel().Accel_y);
                }
            }
        }
    }
}

void Ball::CheckGoal(Player* p1, Player* p2)
{

    if (m_Position.Position_x >= 170 && m_Position.Position_x <= 230)
    {
        if (m_Position.Position_y >= 0 && m_Position.Position_y <= 30)
        {
            m_Position.Position_x = 200;
            m_Position.Position_y = 400;
            m_Accel.Accel_x = 0;
            m_Accel.Accel_y = 0;
            p1->Goal();

        }
        else if (m_Position.Position_y >= 770 && m_Position.Position_y < 800)
        {
            m_Position.Position_x = 200;
            m_Position.Position_y = 400;
            m_Accel.Accel_x = 0;
            m_Accel.Accel_y = 0;
            p2->Goal();
        }
    }
}