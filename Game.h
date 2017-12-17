#ifndef GAME_H
#define GAME_H
#include <vector>
#include <queue>
#include <QLabel>
#include <QObject>
#include "Block.h"

using std::vector;
using std::priority_queue;

//游戏主体

class Game : public QObject
{
    Q_OBJECT

signals:
    mixLabelReady();
    clear7Ready();
    gameOver();
    finish();

public:
    static const int sizeX = 6, sizeY = 9;          //地图的大小
    enum Status{
        Waitting=0,Gaming=1
    };

    Status gStatus;                                 //游戏状态
    MovingBlock block[2];                           //用来生成新方块
    int supportMap[sizeX][sizeY];                   //辅助地图
    vector<vector<Block> > map;                     //游戏地图

    explicit Game(QObject *parent = 0);
    ~Game();

    //地图操作
    int getX_size(int x);
    void push_in_lineX(int x, Block & newblock);
    void erase_in_lineX(int x, int index);
    void erase_in_lineX(int x, int begin, int end);
    void clearMap_in_lineX(int x);
    void unionMap();

    void iWant7();                                  //下降一次的循环

    int Score();
    int Record();
    void ClearScore();

    void RandomBlock();                             //产生随机数字
    void RotateBlock();                             //旋转方块

private:
    int GameScore;          //记录游戏分数
    int ScoreRecord;        //获取文件记录的最高分
    priority_queue<Block> mixQueue;                 //合成队列
    vector<Block> scoreList;
    vector<Block> tempList;

    bool isCrossLine(int x, int y);                 //判断越界
    bool isMix(int x1, int y1, int x2, int y2);     //判断合成
    void scanMap();                                 //扫描地图
    void mixBlock();                                //合成方块

    bool isGameOver();                              //判断是否游戏结束
    int isScore(int x, int y);                      //递归判断连接的7超过3个？
    void scanScore();                               //扫描可否得分
    void computeScore();                            //清算得分

    void numberRate(int &num);
};

#endif // GAME_H