#ifndef BLOCK_H
#define BLOCK_H

//方块
class BaseBlock                     //抽象基类
{
public:
    int x_;
    int y_;
    int value_;

    BaseBlock();
    BaseBlock(const int x, const int y, const int value);
    virtual ~BaseBlock() {}
    virtual void setX_Y(const int x, const int y) = 0;
};

class Block : public BaseBlock      //存储在地图的方块
{
public:
    int absorb;                     //能吸收多少个方块
    int priority;                   //合成处理优先级
    int grade;                      //合成为几级的7
    int haveScan;                   //是否已经扫描过,0:N,1:Y

    Block();
    Block(const int value);
    Block(const Block & copy);      //复制构造函数
    void clear();

    virtual void setX_Y(const int x, const int y);
    friend bool operator <(const Block & block1, const Block & block2);
    Block & operator =(const Block &another);

};

class MovingBlock : public BaseBlock    //移动用的方块
{
public:
    bool m_on_left_down;                //用于判断是否在左下角

    MovingBlock();
    MovingBlock(const int x, const int y, const int value, const bool on_left_down);

    static MovingBlock & OnLeftDown(MovingBlock * block);
    static MovingBlock & No_OnLeftDown(MovingBlock * block);
    virtual void setX_Y(const int x, const int y);
    friend bool operator ==(const MovingBlock & block, const MovingBlock & another);
};

#endif // BLOCK_H