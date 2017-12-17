#include "GameArea.h"
#include <QPixmap>


GameArea::GameArea(QWidget *parent)
    : QWidget(parent)
{
    GameArea::BackGroundLabel = new QLabel(this);
    GameArea::BackGroundLabel->setGeometry(20,100,435,630);
    QPixmap bPixmap(":/demo/src/pic/gameArea_1.png");
    GameArea::BackGroundLabel->setPixmap(bPixmap);
}
