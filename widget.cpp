#include "widget.h"
#include "ui_widget.h"
#include "ui_Setting.h"
#include <ctime>
#include <windows.h>
#include <QLabel>
#include <QBitmap>
#include <QPropertyAnimation>
#include <QKeyEvent>
#include <QPoint>
#include <QTimer>
#include <QDir>
#include <QMediaPlaylist>
#include <QMessageBox>

extern int countScoreUi;

using std::endl;

typedef pair<int, QLabel*> iLabel;

//label大小
const int b_width = 68;
const int b_height = 68;
//速度
const int per_speed = 60;

Widget::Widget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Widget),
    block_map(Widget::MAX_BLOCK_X)
{
    ui->setupUi(this);

    Widget::gameArea = new GameArea(this);
    Widget::game = new Game();
    game->moveToThread(&workThread);
    Widget::StartButton = new QToolButton(this);
    Widget::SettingButton = new QToolButton(this);
    Widget::Logo = new QLabel(this);
    Widget::settings = new Setting(this);
    settings->hide();
    ui->recordLogo->hide();
    ui->recordLabel->hide();
    Widget::blockAnima_1 = new QPropertyAnimation(this);
    Widget::blockAnima_2 = new QPropertyAnimation(this);

    Widget::block_map.reserve(Widget::MAX_BLOCK_X);

    initialization();
    initializeSound();

    this->workThread.start();
    this->musicThread.start();

    this->setFocus();
    setWindowTitle(tr("i want 7"));
}

Widget::~Widget()
{
    workThread.quit();
    workThread.wait();
    musicThread.quit();
    musicThread.wait();

    this->BGMPlayer->deleteLater();
    this->hitSound->deleteLater();
    delete ui;

}

void Widget::keyPressEvent(QKeyEvent *key)
{
    if (game->gStatus == Game::Gaming)         //游戏开始才获取键盘输入
    {
        QWidget *widget = focusWidget();
        if(widget == this)
        {
            switch (key->key()) {
            case Qt::Key_Left:
                keyLeftClicked();
                break;
            case Qt::Key_Right:
                keyRightClicked();
                break;
            case Qt::Key_Down:
                QThread::currentThread()->msleep(10);
                hitSound->play();
                keyDownClicked();
                break;
            case Qt::Key_Up:
                keyUpClicked();
                break;
            default:
                QWidget::keyPressEvent(key);
                break;
            }
        }
    }
}

//开始按钮事件
void Widget::startGame()
{
    createBlock();
    this->game->gStatus = Game::Gaming;
    setScores();
    setRecord();
    ui->recordLogo->show();
    ui->recordLabel->show();
    StartButton->hide();
    Logo->hide();
    setShadow();
}

void Widget::createBlockSlot()
{
    createBlock();
    Sleep(200);
    game->iWant7();
}

void Widget::gameLose()
{
    deleteBlockAll();
    this->game->gStatus = Game::Waitting;
    this->repaint();
    Sleep(1000);
    cleanScores();
    ui->recordLogo->hide();
    ui->recordLabel->hide();
    shadow[0]->hide();
    shadow[1]->hide();
    Widget::StartButton->show();
    Widget::Logo->show();
}

void Widget::initialization()
{
    setLogo();
    setStartBtn();
    shadow[0] = new QLabel(this);
    shadow[1] = new QLabel(this);
    setSignalSlot();
}

void Widget::initializeSound()
{
    this->BGMPlayer = new QMediaPlayer();
    this->hitSound = new QMediaPlayer();

    QDir temDir1("src/sound/BGM2.wav");
    QString absDir1 = temDir1.absolutePath();
    absDir1.replace("/", "\\");
    QMediaPlaylist *list = new QMediaPlaylist(this->BGMPlayer);
    list->addMedia(QUrl::fromLocalFile(absDir1));
    list->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    BGMPlayer->setVolume(60);
    BGMPlayer->setPlaylist(list);

    QDir temDir2("src/sound/crashBlock.mp3");
    QString absDir2 = temDir2.absolutePath();
    absDir2.replace("/", "\\");
    hitSound->setMedia(QUrl::fromLocalFile(absDir2));

    BGMPlayer->moveToThread(&musicThread);
    hitSound->moveToThread(&musicThread);
    BGMPlayer->play();

}

void Widget::setSignalSlot()
{
    connect(StartButton,SIGNAL(clicked(bool)),this,SLOT(startGame()));
    connect(SettingButton,SIGNAL(clicked(bool)),this,SLOT(setting()));

    connect(settings->ui->MusicButton,SIGNAL(clicked(bool)),this,SLOT(MusicSlot()));
    connect(settings->PauseButton,SIGNAL(clicked(bool)),settings,SLOT(hideSlot()));
    connect(settings->ui->AboutButton,SIGNAL(clicked(bool)),qApp,SLOT(aboutQt()));
    connect(settings->HomeButton,SIGNAL(clicked(bool)),this,SLOT(HomeSlot()));
    connect(settings->ui->MusicButton,SIGNAL(triggered(QAction*)),this,SLOT(MusicSlot()));
    connect(settings->ReStartButton,SIGNAL(clicked(bool)),this,SLOT(ReStartSlot()));
    connect(settings->ui->HelpButton, SIGNAL(clicked(bool)), this, SLOT(aboutUs()));

    connect(this->game, SIGNAL(mixLabelReady()), this, SLOT(mixLabel()));
    connect(this->game, SIGNAL(clear7Ready()), this, SLOT(clear7Label()));
    connect(this->game, SIGNAL(gameOver()), this, SLOT(gameLose()));
    connect(this->game, SIGNAL(finish()), this, SLOT(setShadow()));

    connect(&workThread, &QThread::finished, this->game, &QObject::deleteLater);
}

void Widget::setStartBtn()
{
    StartButton->setStyleSheet("QToolButton{border:0px;}");
    StartButton->setGeometry(150,390,178,69);
    StartButton->setIconSize(QSize(178,69));
    QPixmap objPixmap(":/icon/src/pixmap/开始游戏.png");
    StartButton->setIcon(QIcon(objPixmap));
    //StartButton->setMask(objPixmap.mask());          //透明遮盖背景

    SettingButton->setStyleSheet("QToolButton{border:0px;}");
    SettingButton->setGeometry(410,50,41,41);
    SettingButton->setIconSize(QSize(41,41));
    QPixmap sbp(":/icon/src/pixmap/设置.png");
    SettingButton->setIcon(QIcon(sbp));
    SettingButton->setMask(sbp.mask());
}

void Widget::setLogo()
{
    QPixmap lgPixmap(":/demo/src/pic/IWANT7_1.png");
    Logo->setPixmap(lgPixmap);
    Logo->setGeometry(20,280,435,70);
}

void Widget::setShadow()
{
    QString name1(":/demo/src/pic/number");
    name1 = name1 + QString::number(game->block[0].value_) + QString(".png");
    QString name2(":/demo/src/pic/number");
    name2 = name2 + QString::number(game->block[1].value_) + QString(".png");

    QPixmap pixmap1(name1);
    shadow[0]->setPixmap(pixmap1);
    shadow[0]->setScaledContents(true);
    setPoint(game->block[0].x_, game->getX_size(game->block[0].x_) + game->block[0].y_, shadow[0]);
    shadow[0]->show();

    QPixmap pixmap2(name2);
    shadow[1]->setPixmap(pixmap2);
    shadow[1]->setScaledContents(true);
    setPoint(game->block[1].x_, game->getX_size(game->block[1].x_) + game->block[1].y_, shadow[1]);
    shadow[1]->show();
}

void Widget::setScores()
{
    ui->scoreLabel->setText(QString::number(game->Score()).rightJustified(3,' '));
}

void Widget::setRecord()
{
    ui->recordLabel->setText(QString::number(game->Record()).rightJustified(3,' '));
}

void Widget::cleanScores()
{
    ui->scoreLabel->setText("");
}

void Widget::createBlock()
{
    Widget::block[0] = new QLabel(this);
    Widget::block[1] = new QLabel(this);

    game->RandomBlock();

    QString name1(":/prefix1/src/images666/number");
    name1 = name1 + QString::number(game->block[0].value_) + QString(".png");
    QString name2(":/prefix1/src/images666/number");
    name2 = name2 + QString::number(game->block[1].value_) + QString(".png");

    QPixmap pixmap1(name1);
    Widget::block[0]->setPixmap(pixmap1);
    Widget::block[0]->setScaledContents(true);        //让图片布满label
    //Widget::block[0]->setMask(pixmap1.mask());      //透明遮盖
    Widget::block[0]->setGeometry(170,176,b_width,b_height);
    Widget::block[0]->show();

    QPixmap pixmap2(name2);
    Widget::block[1]->setPixmap(pixmap2);
    Widget::block[1]->setScaledContents(true);        //让图片布满label
    //Widget::block[1]->setMask(pixmap2.mask());
    Widget::block[1]->setGeometry(170+b_width,176,b_width,b_height);
    Widget::block[1]->show();
}

QLabel* Widget::getBlock(int x, int y)
{
    return this->block_map[x][y].second;
}

void Widget::pushBlock_in_lineX(int x, int value, QLabel *block)
{
    iLabel temp(value,block);
    this->block_map[x].push_back(temp);
}

void Widget::deleteBlockAll()
{
    for(int i=0; i<6; i++)
    {
        for (int j=0; j<game->getX_size(i); j++)
        {
            QLabel* deleteLabel = getBlock(i,j);
            delete deleteLabel;
        }
        block_map[i].clear();
        game->clearMap_in_lineX(i);
    }
    memset(game->supportMap,-1,sizeof(game->supportMap));
    game->unionMap();
    delete this->block[0];
    delete this->block[1];

}

void Widget::dropPoint(int x, int y, QLabel *label)
{
    //如果在本来的点上，则不移动这块label
    bool ok = label->pos().y() == GameArea::EDGE_Y_DOWN-(y+1)*b_height;
    if (ok)
    {
        return ;
    }
    int dis = label->pos().y();

    while (label->pos().y() < GameArea::EDGE_Y_DOWN-(y+1)*b_height)
    {
        label->setGeometry(GameArea::EDGE_X_LEFT+x*b_width, dis, b_width, b_height);

        dis += 17;
        this->repaint();
        //QThread::currentThread()->usleep(10);
        Sleep(10);
    }
}

void Widget::setPoint(int x, int y, QLabel *label)
{
    int dis = GameArea::EDGE_Y_DOWN-(y+1)*b_height ;
    label->setGeometry(GameArea::EDGE_X_LEFT+x*b_width, dis, b_width, b_height);
}


void Widget::mixLabel_1()
{
    int count;
    for (int i = 0; i < MAX_BLOCK_X; i++)
    {
        count = 0;
        for (uint j = 0; j < block_map[i].size(); j++)
        {
            if (game->supportMap[i][j] != block_map[i][j].first)
            {
                if (game->supportMap[i][j] == -1)
                {
                    QLabel* deleteLabel = getBlock(i,j);
                    deleteLabel->hide();
                    this->repaint();
                    Sleep(10);
                    delete deleteLabel;
                }

                if (game->supportMap[i][j] == 7)
                {
                    QString num7(":/prefix1/src/images666/number7-");
                    num7 = num7 + QString::number(game->map[i][j].grade) + QString(".png");

                    QLabel* temp = getBlock(i,j);
                    QPixmap pixmap(num7);
                    temp->setPixmap(pixmap);
                    temp->setScaledContents(true);
                    this->repaint();
                    Sleep(10);
                }
                block_map[i][j].first = game->supportMap[i][j];
            }
            if (game->supportMap[i][j] != -1)
                count++;
        }
    }
}

void Widget::dropLabel()
{
    int count;
    for (int i = 0; i < MAX_BLOCK_X; i++)
    {
        count = 0;
        for (uint j = 0; j < block_map[i].size(); j++)
        {
            if (game->supportMap[i][j] != -1)   //如果不是已经清除的方块则下降
            {
                QLabel* temp = getBlock(i,j);
                dropPoint(i, count, temp);
                //QThread::currentThread()->msleep(10);
                count++;
            }
        }
    }

    for (int i = 0; i < MAX_BLOCK_X; i++)
    {
        for (uint j = 0; j < block_map[i].size(); j++)
        {
            if (block_map[i][j].first == -1)
            {
                block_map[i].erase(block_map[i].begin()+j);
                j--;
            }
        }
    }
}

void Widget::mixLabel()
{
    mixLabel_1();
    Sleep(100);
    dropLabel();
}

void Widget::clear7Label_1()
{
    for (int i = 0; i < MAX_BLOCK_X; i++)
    {

        for (uint j = 0; j < block_map[i].size(); j++)
        {
            if (game->supportMap[i][j] != block_map[i][j].first)
            {
                if (game->supportMap[i][j] == -1)
                {

                    QString addscore(":/prefix1/src/images666/changedaddscores");
                    addscore = addscore + QString::number(countScoreUi) + QString(".png");
                    countScoreUi++;

                    QLabel* temp = getBlock(i,j);
                    QPixmap pixmap(addscore);
                    temp->setPixmap(pixmap);
                    temp->setScaledContents(true);
                    this->repaint();
                    Sleep(50);
                }
                block_map[i][j].first = game->supportMap[i][j];
            }

        }
    }

    Sleep(400);
    for (int i = 0; i < MAX_BLOCK_X; i++)
    {
        for (uint j = 0; j < block_map[i].size(); j++)
        {
            if (block_map[i][j].first == -1)
            {

                QLabel* deleteLabel = getBlock(i,j);
                deleteLabel->hide();
                this->repaint();
                //Sleep(100);
                delete deleteLabel;

            }
        }

    }
}


void Widget::clear7Label()
{
    clear7Label_1();

    dropLabel();

    setScores();
}


void Widget::keyLeftClicked()
{
    if(blockAnima_1->state() == QPropertyAnimation::Running)
    {
        return;
    }
    //获取左下的block
    MovingBlock & temp1 = MovingBlock::OnLeftDown(this->game->block);
    //判断是否越界
    if(temp1.x_-1 < 0)
    {
        return;
    }

    //算法block移动
    game->block[0].x_--;
    game->block[1].x_--;

    /*动画block移动*/

    blockAnima_1->setTargetObject(block[0]);
    blockAnima_1->setPropertyName("geometry");
    blockAnima_1->setDuration(per_speed+20);
    blockAnima_1->setStartValue(QRect(block[0]->pos().x(), block[0]->pos().y(), b_width, b_height));
    //判断是否能移动
    blockAnima_1->setEndValue(QRect(block[0]->pos().x()-b_width, block[0]->pos().y(), b_width, b_height));
    blockAnima_1->setEasingCurve(QEasingCurve::Linear);


    blockAnima_2->setTargetObject(block[1]);
    blockAnima_2->setPropertyName("geometry");
    blockAnima_2->setDuration(per_speed+20);
    blockAnima_2->setStartValue(QRect(block[1]->pos().x(), block[1]->pos().y(), b_width, b_height));
    //判断是否能移动
    blockAnima_2->setEndValue(QRect(block[1]->pos().x()-b_width, block[1]->pos().y(), b_width, b_height));
    blockAnima_2->setEasingCurve(QEasingCurve::Linear);

    blockAnima_1->start();
    blockAnima_2->start();

    setPoint(game->block[0].x_, game->getX_size(game->block[0].x_) + game->block[0].y_, shadow[0]);
    setPoint(game->block[1].x_, game->getX_size(game->block[1].x_) + game->block[1].y_, shadow[1]);
}

void Widget::keyRightClicked()
{
    if(blockAnima_1->state() == QPropertyAnimation::Running)
    {
        return;
    }
    //获取非左下的block
    MovingBlock & temp1 = MovingBlock::No_OnLeftDown(this->game->block);
    //判断是否越界
    if(temp1.x_+1 >= Game::sizeX)
    {
        return;
    }

    //算法block移动
    game->block[0].x_++;
    game->block[1].x_++;

    /*动画block移动*/

    blockAnima_1->setTargetObject(block[0]);
    blockAnima_1->setPropertyName("geometry");
    blockAnima_1->setDuration(per_speed+20);
    blockAnima_1->setStartValue(QRect(block[0]->pos().x(), block[0]->pos().y(), b_width, b_height));
    //判断是否能移动
    blockAnima_1->setEndValue(QRect(block[0]->pos().x()+b_width, block[0]->pos().y(), b_width, b_height));
    blockAnima_1->setEasingCurve(QEasingCurve::Linear);

    blockAnima_2->setTargetObject(block[1]);
    blockAnima_2->setPropertyName("geometry");
    blockAnima_2->setDuration(per_speed+20);
    blockAnima_2->setStartValue(QRect(block[1]->pos().x(), block[1]->pos().y(), b_width, b_height));
    //判断是否能移动
    blockAnima_2->setEndValue(QRect(block[1]->pos().x()+b_width, block[1]->pos().y(), b_width, b_height));
    blockAnima_2->setEasingCurve(QEasingCurve::Linear);

    blockAnima_1->start();
    blockAnima_2->start();

    setPoint(game->block[0].x_, game->getX_size(game->block[0].x_) + game->block[0].y_, shadow[0]);
    setPoint(game->block[1].x_, game->getX_size(game->block[1].x_) + game->block[1].y_, shadow[1]);
}

void Widget::keyUpClicked()
{
    this->game->RotateBlock();
    Widget::block[0]->setGeometry(this->game->block[0].x_*b_width+GameArea::EDGE_X_LEFT,GameArea::EDGE_Y_UP+b_height*(1 - this->game->block[0].y_),b_width,b_height);
    Widget::block[1]->setGeometry(this->game->block[1].x_*b_width+GameArea::EDGE_X_LEFT,GameArea::EDGE_Y_UP+b_height*(1 - this->game->block[1].y_),b_width,b_height);

    setPoint(game->block[0].x_, game->getX_size(game->block[0].x_) + game->block[0].y_, shadow[0]);
    setPoint(game->block[1].x_, game->getX_size(game->block[1].x_) + game->block[1].y_, shadow[1]);
}

void Widget::keyDownClicked()
{
    if(blockAnima_1->state() == QPropertyAnimation::Running || blockAnima_2->state() == QPropertyAnimation::Running)
    {
        return;
    }

    shadow[0]->hide();
    shadow[1]->hide();

    /*动画block移动*/

    MovingBlock & temp1 = MovingBlock::OnLeftDown(this->game->block);
    MovingBlock & temp2 = ((temp1==(this->game->block[0]))?this->game->block[1]:this->game->block[0]);

    Block blockToMap1 = Block(temp1.value_);
    blockToMap1.priority++;
    game->push_in_lineX(temp1.x_, blockToMap1);

    if (temp1 == this->game->block[0])              //确定左下角的block对应的方块label
    {
        blockAnima_1->setTargetObject(block[0]);
        blockAnima_2->setTargetObject(block[1]);

        //存放block图片
        Widget::pushBlock_in_lineX(game->block[0].x_, game->block[0].value_, this->block[0]);
        Widget::pushBlock_in_lineX(game->block[1].x_, game->block[1].value_, this->block[1]);
    }
    else
    {
        blockAnima_1->setTargetObject(block[1]);
        blockAnima_2->setTargetObject(block[0]);

        //存放block图片
        Widget::pushBlock_in_lineX(game->block[1].x_, game->block[1].value_, this->block[1]);
        Widget::pushBlock_in_lineX(game->block[0].x_, game->block[0].value_, this->block[0]);
    }

    blockAnima_1->setPropertyName("geometry");
    int time1 = 480-per_speed*(game->getX_size(temp1.x_));
    //获取下落位置
    int dis1 = GameArea::EDGE_Y_DOWN-(game->getX_size(temp1.x_))*b_height ;

    blockAnima_1->setDuration(time1);
    blockAnima_1->setStartValue(QRect(GameArea::EDGE_X_LEFT+temp1.x_*b_width, GameArea::EDGE_Y_UP+b_height*(1-temp1.y_), b_width, b_height));
    blockAnima_1->setEndValue(QRect(GameArea::EDGE_X_LEFT+temp1.x_*b_width, dis1, b_width, b_height));
    blockAnima_1->setEasingCurve(QEasingCurve::InQuad);

    Block blockToMap2 = Block(temp2.value_);
    blockToMap2.priority++;
    game->push_in_lineX(temp2.x_,blockToMap2);

    blockAnima_2->setPropertyName("geometry");

    int time2;
    if (temp1.x_ == temp2.x_)
    {
        time2 = time1;
    }
    else
    {
        time2 = 480-per_speed*(game->getX_size(temp2.x_));
    }
    //获取下落位置
    int dis2 = GameArea::EDGE_Y_DOWN-(game->getX_size(temp2.x_))*b_height ;

    blockAnima_2->setDuration(time2);
    blockAnima_2->setStartValue(QRect(GameArea::EDGE_X_LEFT+temp2.x_*b_width, GameArea::EDGE_Y_UP+b_height*(1-temp2.y_), b_width, b_height));
    blockAnima_2->setEndValue(QRect(GameArea::EDGE_X_LEFT+temp2.x_*b_width, dis2, b_width, b_height));
    blockAnima_2->setEasingCurve(QEasingCurve::InQuad);


    blockAnima_1->start();
    blockAnima_2->start();


    int t = time1>time2?time1:time2;

    //生成新的方块（调用QTimer的单触发定时器）

    QTimer::singleShot(t+30,this,SLOT(createBlockSlot()));
}

void Widget::setting()                  //根据游戏状态来显示设置菜单
{
    settings->Showsettings(game->gStatus);
}

void Widget::MusicSlot()
{
    if(BGMPlayer->state() == QMediaPlayer::PlayingState)
    {
        BGMPlayer->pause();
        hitSound->setVolume(0);
    }
    else
    {
        BGMPlayer->play();
        hitSound->setVolume(80);
    }
}

void Widget::HomeSlot()
{
    deleteBlockAll();
    this->game->gStatus = Game::Waitting;
    this->game->ClearScore();
    cleanScores();
    Widget::StartButton->show();
    Widget::Logo->show();
    this->settings->hide();
    this->ui->recordLabel->hide();
    this->ui->recordLogo->hide();
    this->shadow[0]->hide();
    this->shadow[1]->hide();
    this->setFocus();
}

void Widget::ReStartSlot()
{
    deleteBlockAll();
    this->game->ClearScore();
    createBlock();
    this->setScores();
    this->settings->hide();
    this->setShadow();
    this->setFocus();
}

void Widget::aboutUs()
{
    QMessageBox::about(this, tr("炒鸡乌迪小组"), tr("15级软工一班\n刘孟莹,刘婉婷,何增杰,邓晓城\n联合制作\n\n合成7然后消除它！！！"));
}

