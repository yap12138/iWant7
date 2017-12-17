#include "Setting.h"
#include "ui_Setting.h"

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    setBaseButton();
    setControlButton();

    this->connect(ui->EscButton,SIGNAL(clicked(bool)),this,SLOT(hideSlot()));

    // 设置窗口部件背景半透明
    QPalette myPalette;
    QColor myColor(255,255,255);
    myColor.setAlphaF(0.7);
    myPalette.setBrush(backgroundRole(),myColor);
    this->setPalette(myPalette);
    this->setAutoFillBackground(true);

    this->setAttribute(Qt::WA_DeleteOnClose);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::Showsettings(Game::Status gStatus)
{
    if (gStatus == Game::Waitting)
    {
        this->PauseButton->hide();
        this->ReStartButton->hide();
        this->HomeButton->hide();
        this->ui->EscButton->show();
    }
    else
    {
        this->PauseButton->show();
        this->ReStartButton->show();
        this->HomeButton->show();
        this->ui->EscButton->hide();
    }
    this->setFocus();                //获取焦点
    this->show();
    this->raise();
}

void Setting::setBaseButton()
{
    ui->MusicButton->setStyleSheet("QToolButton{border:0px;}");
    ui->MusicButton->setIconSize(QSize(61,61));
    QPixmap mbp(":/icon/src/pixmap/音效.png");
    ui->MusicButton->setIcon(QIcon(mbp));

    ui->HelpButton->setStyleSheet("QToolButton{border:0px;}");
    ui->HelpButton->setIconSize(QSize(61,61));
    QPixmap hbp(":/icon/src/pixmap/疑问.png");
    ui->HelpButton->setIcon(QIcon(hbp));

    ui->AboutButton->setStyleSheet("QToolButton{border:0px;}");
    ui->AboutButton->setIconSize(QSize(61,61));
    QPixmap abp(":/icon/src/pixmap/关于.png");
    ui->AboutButton->setIcon(QIcon(abp));

    ui->EscButton->setStyleSheet("QToolButton{border:0px;}");
    ui->EscButton->setIconSize(QSize(41,41));
    QPixmap ebp(":/icon/src/pixmap/关闭.png");
    ui->EscButton->setIcon(QIcon(ebp));
}

void Setting::setControlButton()
{
    this->PauseButton = new QToolButton(this);
    this->PauseButton->setGeometry(120,250,61,61);
    this->ReStartButton = new QToolButton(this);
    this->ReStartButton->setGeometry(210,250,61,61);
    this->HomeButton = new QToolButton(this);
    this->HomeButton->setGeometry(300,250,61,61);

    this->PauseButton->setStyleSheet("QToolButton{border:0px;}");
    this->PauseButton->setIconSize(QSize(61,61));
    QPixmap pbp(":/icon/src/pixmap/继续游戏.png");
    this->PauseButton->setIcon(pbp);

    this->ReStartButton->setStyleSheet("QToolButton{border:0px;}");
    this->ReStartButton->setIconSize(QSize(61,61));
    QPixmap rsbp(":/icon/src/pixmap/重玩.png");
    this->ReStartButton->setIcon(rsbp);

    this->HomeButton->setStyleSheet("QToolButton{border:0px;}");
    this->HomeButton->setIconSize(QSize(61,61));
    QPixmap hbp(":/icon/src/pixmap/返回主页.png");
    this->HomeButton->setIcon(hbp);
}

void Setting::hideSlot()
{
    parentWidget()->setFocus();      //将焦点控制还给父窗口
    this->hide();
}