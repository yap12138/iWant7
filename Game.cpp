#include "Game.h"
#include <cstdio>
#include <vector>
#include <ctime>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

int countScore;
int countScoreUi;


Game::Game(QObject *parent) :
    QObject(parent),
    map(sizeX)
{
    Game::GameScore = 0;
    Game::block[0] = MovingBlock();
    Game::block[1] = MovingBlock();
    Game::gStatus = Game::Waitting;
    memset(supportMap,-1,sizeof(supportMap));

    Game::map.reserve(sizeX);

    QDir tempDir("src/record.txt");
    QString fileName = tempDir.absolutePath();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(0, QString("Read error"), QString("can't find record.txt"));
    }
    QTextStream fin(&file);
    Game::ScoreRecord = fin.readLine().toInt();;\
    file.close();
}

Game::~Game()
{

}

int Game::getX_size(int x)
{
    return Game::map[x].size();
}

void Game::push_in_lineX(int x, Block & newblock)
{
    Game::supportMap[x][getX_size(x)] = newblock.value_;
    Game::map[x].push_back(newblock);
}

void Game::erase_in_lineX(int x, int index)
{
    Game::map[x].erase(map[x].begin()+index);
}

void Game::erase_in_lineX(int x, int begin, int end)
{
    Game::map[x].erase(map[x].begin()+begin,map[x].begin()+end);
}

void Game::clearMap_in_lineX(int x)
{
    Game::map[x].clear();
}

void Game::unionMap()
{
    //调整map
    for (int i = 0; i < sizeX; i++)                 //将辅助地图整合到map
    {
        for (uint j = 0; j < map[i].size(); j++)
        {
            map[i][j].clear();
            map[i][j].value_ = supportMap[i][j];
        }
    }

    for (int i = 0; i < sizeX; i++)                 //将map里值为-1的Block推出
    {
        for (uint j = 0; j < map[i].size(); j++)
        {
            if (map[i][j].value_ == -1)
            {
                this->erase_in_lineX(i,j);
                this->map[i][j].priority++;
                j--;
            }
        }
    }
    //调整supportMap
    memset(supportMap,-1,sizeof(supportMap));
    for (int i = 0; i < sizeX; i++)
    {
        for (uint j = 0; j < map[i].size(); j++)
        {
            supportMap[i][j] = map[i][j].value_;
        }
    }
}

bool Game::isCrossLine(int x, int y)
{
    if (x < 0 || x > 5 || y <0 || y > 7)
        return true;
    else
        return false;
}

bool Game::isMix(int x1, int y1, int x2, int y2)
{
    if (isCrossLine(x1,y1) || isCrossLine(x2,y2))
        return false;
    if (supportMap[x1][y1] == -1 || supportMap[x2][y2] == -1)
        return false;
    if (supportMap[x1][y1] + supportMap[x2][y2] != 7)
        return false;
    return true;
}

//遍历一遍可合成的方块，并且记录进mixQueue等待合成
void Game::scanMap()
{
    for (int i = 0; i < Game::sizeX; i++)
    {
        for (uint j = 0; j < map[i].size(); j++)
        {
            //分别判断上下左右四个方块，能否合成
            if ( isMix(i, j, i+1, j) )
                map[i][j].absorb++;
            if ( isMix(i, j, i, j-1) )
                map[i][j].absorb++;
            if ( isMix(i, j, i-1, j) )
                map[i][j].absorb++;
            if ( isMix(i, j, i, j+1) )
                map[i][j].absorb++;

            if (map[i][j].absorb != 0 && map[i][j].value_ != 0)   //有可以合成且不等于0则进队列
            {
                Block temp = map[i][j];   //副本进队列
                temp.x_ = i;
                temp.y_ = j;
                mixQueue.push(temp);
            }
        }
    }
}

void Game::mixBlock()
{
    int i,j;
    int absorb;
    while (!mixQueue.empty())               //获取可以合成的block
    {
        i = mixQueue.top().x_;
        j = mixQueue.top().y_;
        absorb = 0;

        if ( supportMap[i][j] != -1 )           //block如果没有被合成，清算可以与多少个block合成7
        {
            if ( isMix(i, j, i+1, j) )
            {
                supportMap[i+1][j] = -1;
                absorb++;
            }
            if ( isMix(i, j, i, j-1) )
            {
                supportMap[i][j-1] = -1;
                absorb++;
            }
            if ( isMix(i, j, i-1, j) )
            {
                supportMap[i-1][j] = -1;
                absorb++;
            }
            if ( isMix(i, j, i, j+1) )
            {
                supportMap[i][j+1] = -1;
                absorb++;
            }
            if (absorb != 0)                //可合成数如果不等于0，则合成
            {
                map[i][j].grade = map[i][j].absorb;
                supportMap[i][j] = 7;
            }
        }

        mixQueue.pop();
    }
}

void Game::iWant7()
{
    countScore = 1;
    countScoreUi = 1;
    scanMap();                          //扫描一遍地图
    while ( !(mixQueue.empty())  )      //有可以合成的进入循环
    {
        mixBlock();                     //算法部分合成
        emit mixLabelReady();           //发射 合成方块信号 让ui合成
        unionMap();                     //统一地图

        scanMap();                      //再次扫描地图
        if ( mixQueue.empty() )         //不能合成的时候，开始判断计分。
        {
            scanScore();                //扫描是否有可以得分的7
            computeScore();             //计算分数，消去已得分的7
            emit clear7Ready();         //发射 清除7信号 让ui清除
            unionMap();                 //统一地图
            scanMap();                  //再次判断能否得分消去
        }
    }
    unionMap();



    if ( isGameOver() )                 //判断是否game over
        emit gameOver();
    else
        emit finish();
}

bool Game::isGameOver()
{
    for (int i = 0; i < sizeX; i++)
    {
        if (supportMap[i][7] != -1)                     //在每一列第8个如果还有方块则游戏结束
        {
            if(this->GameScore > this->ScoreRecord)     //当前分数大于最高分，则写入文档
            {
                QDir tempDir("src/record.txt");
                QString fileName = tempDir.absolutePath();
                QFile file(fileName);
                if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QMessageBox::warning(0, QString("Read error"), QString("can't find record.txt"));
                }
                QTextStream fout(&file);
                fout<<Game::GameScore;
                file.close();

                this->ScoreRecord = this->GameScore;
                this->GameScore = 0;                    //重置分数
            }

            return true;
        }
    }
    return false;
}

int Game::isScore(int x, int y)                         //判断是否可以得分
{
    int score = 1;
    if ( !isCrossLine(x, y) )
    {
        if (supportMap[x][y] == -1 || supportMap[x][y] != 7)
            return 0;
        if ( map[x][y].haveScan == 0 )                  //没被扫描过的推进临时数组，递归判断连在一起的7是否大于等于3个
        {
            Block temp = map[x][y];
            temp.x_ = x;
            temp.y_ = y;
            tempList.push_back(temp);
            map[x][y].haveScan = 1;
            return score + isScore(x-1, y) + isScore(x+1, y) + isScore(x, y-1) + isScore(x, y+1);
        }
        else
            return 0;
    }

    else
        return 0;
}

void Game::scanScore()
{
    for (int i = 0; i < sizeX; i++)
    {
        for (uint j = 0; j < map[i].size(); j++)
        {
            if ( supportMap[i][j] != 7 )
                continue;
            if ( isScore(i, j) >= 3)
            {
                for (uint k = 0; k < tempList.size(); k++)
                    scoreList.push_back(tempList.at(k));
                tempList.clear();
            }
            else
                tempList.clear();
        }
    }
}

void Game::computeScore()
{
    for (uint i = 0; i < scoreList.size(); i++)     //清算分数，清理地图已合成的7
    {
        this->GameScore += (countScore++);
        supportMap[scoreList[i].x_][scoreList[i].y_] = -1;
        map[scoreList[i].x_][scoreList[i].y_].clear();
    }
    scoreList.clear();
}

int Game::Score()
{
    return Game::GameScore;
}

int Game::Record()
{
    return Game::ScoreRecord;
}

void Game::ClearScore()
{
    Game::GameScore = 0;
}

void Game::numberRate(int &num)         //简略根据当前得分，更改0出现几率，间接修改难度
{
    if (this->GameScore <= 50)
    {
        if (num>=0 && num <=4)
            num = 0;
        else if (num <= 20)
            num = 1;
        else if (num <= 36)
            num = 2;
        else if (num <= 52)
            num = 3;
        else if (num <= 68)
            num = 4;
        else if (num <= 84)
            num = 5;
        else
            num = 6;
    }
    else if (this->GameScore <= 200)
    {
        if (num>=0 && num <=10)
            num = 0;
        else if (num <= 25)
            num = 1;
        else if (num <= 40)
            num = 2;
        else if (num <= 55)
            num = 3;
        else if (num <= 70)
            num = 4;
        else if (num <= 85)
            num = 5;
        else
            num = 6;
    }
    else
    {
        if (num>=0 && num <=16)
            num = 0;
        else if (num <= 30)
            num = 1;
        else if (num <= 44)
            num = 2;
        else if (num <= 59)
            num = 3;
        else if (num <= 73)
            num = 4;
        else if (num <= 86)
            num = 5;
        else
            num = 6;
    }
}

void Game::RandomBlock()
{
    std::srand(std::time(NULL));
    int x0 = std::rand()%100;
    int x1 = std::rand()%100;
    numberRate(x0);
    numberRate(x1);
    while (x1+x0 == 7)
    {
        x1 = std::rand()%100;
        numberRate(x1);
    }

    Game::block[0].x_ = 2;
    Game::block[0].y_ = 0;
    Game::block[0].value_ = x0;
    Game::block[0].m_on_left_down = true;

    Game::block[1].x_ = 3;
    Game::block[1].y_ = 0;
    Game::block[1].value_ = x1;
    Game::block[1].m_on_left_down = false;
}

void Game::RotateBlock()
{
    MovingBlock & temp1 = MovingBlock::OnLeftDown(this->block);
    MovingBlock & temp2 = MovingBlock::No_OnLeftDown(this->block);

    if (temp1.x_ == temp2.x_)
    {
        if (temp1.x_ == 5)
            temp1.x_--;
        temp2.x_ = temp1.x_ + 1;
        temp2.y_ = 0;
    }
    else
    {
        temp1.y_ = 1;
        temp1.m_on_left_down = false;

        temp2.x_ = temp1.x_;
        temp2.m_on_left_down = true;
    }
}
