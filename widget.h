#ifndef WIDGET_H
#define WIDGET_H
#include <QDialog>
#include "GameArea.h"
#include "Game.h"
#include "Setting.h"
#include <vector>
#include <utility>
#include <QWidget>
#include <QSound>
#include <QTime>
#include <QThread>
#include <QMediaPlayer>
#include <QUrl>

class QLabel;
class QToolButton;
class QPropertyAnimation;

namespace Ui {
class Widget;
}

using std::vector;
using std::pair;

class Widget : public QDialog
{
    Q_OBJECT

public:
    static const int MAX_BLOCK_Y = 9;
    static const int MAX_BLOCK_X = 6;


    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void keyPressEvent(QKeyEvent *key);     //重写keyPressEvent获取键盘输入

private slots:
    void startGame();                       //连接开始按钮
    void createBlockSlot();
    void setShadow();
    void gameLose();

    void mixLabel_1();
    void dropLabel();
    void mixLabel();
    void clear7Label_1();
    void clear7Label();

    void setting();                         //调用设置界面
    void MusicSlot();                       //调节音乐
    void HomeSlot();                        //返回主界面
    void ReStartSlot();                     //重新开始
    void aboutUs();                         //简略的关于

private:
    Ui::Widget *ui;
    GameArea *gameArea;                     //游戏区域
    Game *game;                             //游戏主体
    Setting *settings;                      //设置界面
    QLabel *block[2];                       //新生成的2个方块组
    QLabel *shadow[2];                      //方块位置辅助显示
    vector< vector<pair<int, QLabel*> > > block_map;        //保存生成的图片
    QLabel *Logo;                           //I Want 7 Logo

    QToolButton *StartButton;               //开始按钮
    QToolButton *SettingButton;             //设置按钮
    QPropertyAnimation *blockAnima_1;       //控制block移动
    QPropertyAnimation *blockAnima_2;

    QMediaPlayer *BGMPlayer;
    QMediaPlayer *hitSound;

    //两个线程， work处理游戏算法， music处理音乐播放
    QThread workThread;
    QThread musicThread;

private:
    void initialization();      //初始化
    void initializeSound();     //初始化音乐
    void setSignalSlot();       //设置信号和槽连接
    void setStartBtn();         //设置开始按钮图标
    void setLogo();             //设置Logo图标
    void setScores();           //设置当前分数
    void setRecord();           //设置最高分数
    void cleanScores();
    void createBlock();         //生成方块label
    QLabel*  getBlock(int x, int y);        //获取（x,y）单元格的label
    void pushBlock_in_lineX(int x, int value, QLabel * block);
    void deleteBlockAll();                  //释放所有方块label内存

    void dropPoint(int x, int y, QLabel * label);
    void setPoint(int x, int y, QLabel * label);

    //按键函数
    void keyLeftClicked();
    void keyRightClicked();
    void keyUpClicked();
    void keyDownClicked();
};

#endif // WIDGET_H
