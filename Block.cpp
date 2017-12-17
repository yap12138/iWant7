#include "Block.h"

BaseBlock::BaseBlock()
{
    this->x_ = 0;
    this->y_ = 0;
    this->value_ = -1;
}

BaseBlock::BaseBlock(const int x, const int y, const int value)
{
    this->x_ = x;
    this->y_ = y;
    this->value_ = value;
}

Block::Block()
    :BaseBlock()
{
    this->absorb = 0;
    this->grade = 0;
    this->priority = 0;
    this->haveScan = 0;
}

Block::Block(const int value)
    :BaseBlock(0,0,value)
{
    this->absorb = 0;
    this->grade = 0;
    this->priority = 0;
    this->haveScan = 0;
}

Block::Block(const Block &copy)
    :BaseBlock()
{
    this->x_ = copy.x_;
    this->y_ = copy.y_;
    this->value_ = copy.value_;
    this->absorb = copy.absorb;
    this->grade = copy.grade;
    this->haveScan = copy.haveScan;
    this->priority = copy.priority;
}

void Block::clear()
{
    this->value_ = -1;
    this->absorb = 0;
    this->priority = 0;
    this->haveScan = 0;
}

void Block::setX_Y(int x, int y)
{
    this->x_ = x;
    this->y_ = y;
}

//用于合成方块出列的时候按照可吸收方块出列，若可吸收方块相等则判断优先级（下降方块优先级 > 不动方块）
bool operator <(const Block &block1, const Block &block2)
{
    if (block1.absorb == block2.absorb)
        return block1.priority < block2.priority;
    else
        return block1.absorb < block2.absorb;
}

Block &Block::operator =(const Block &another)
{
    this->x_ = another.x_;
    this->y_ = another.y_;
    this->value_ = another.value_;
    this->absorb = another.absorb;
    this->grade = another.grade;
    this->priority = another.priority;
    this->haveScan = another.haveScan;

    return *this;
}

MovingBlock::MovingBlock()
    :BaseBlock()
{
    m_on_left_down = false;
}

MovingBlock::MovingBlock(const int x, const int y, const int value, const bool on_letf_down)
    :BaseBlock(x,y,value)
{
    m_on_left_down = on_letf_down;
}

bool operator ==(const MovingBlock &block, const MovingBlock &another)
{
    return (&block)==(&another);
}

MovingBlock & MovingBlock::OnLeftDown(MovingBlock *block)
{
    return block[0].m_on_left_down?block[0]:block[1];
}

MovingBlock & MovingBlock::No_OnLeftDown(MovingBlock *block)
{
    return block[0].m_on_left_down?block[1]:block[0];
}

void MovingBlock::setX_Y(int x, int y)
{
    this->x_ = x;
    this->y_ = y;
}



