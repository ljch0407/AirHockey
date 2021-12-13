#include "Player.h"

Player::Player()
{
    m_Position.Position_x = 0;
    m_Position.Position_y = 0;

    m_Accel.Accel_x = 0.0;
    m_Accel.Accel_y = 0.0;
    m_Goal = 0;
    m_score = 0;
}

Player::Player(int xPos, int yPos, float xAccel, float yAccel)
{
    m_Position.Position_x = xPos;
    m_Position.Position_y = yPos;

    m_Accel.Accel_x = xAccel;
    m_Accel.Accel_y = yAccel;
    m_Goal = 0;
    m_score = 0;
}
Point2D Player::GetPos()
{
    return m_Position;
}

Accel2D Player::GetAccel()
{
    return m_Accel;
}

void Player::UpdatePos_x(int pos)
{
    m_Position.Position_x = pos;
}
void Player::UpdatePos_y(int pos)
{
    m_Position.Position_y = pos;
}

void Player::UpdateAccel_x(int pos)
{
    m_Accel.Accel_x = m_Position.Position_x - pos;
}

void Player::UpdateAccel_y(int pos)
{
    m_Accel.Accel_y = m_Position.Position_y - pos;
}
void Player::Goal(int score)
{
    m_Goal = score;
}

int Player::GetScore(void)
{
    return m_score;
}