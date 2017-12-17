#ifndef GAMEAREA_H
#define GAMEAREA_H
#include <QWidget>
#include <QLabel>

class GameArea : QWidget
{
private:
    QLabel *BackGroundLabel;
public:
    GameArea(QWidget *parent = 0);

    static const int EDGE_X_LEFT = 34;
    static const int EDGE_X_RIGHT = 442;
    static const int EDGE_Y_DOWN = 720;
    static const int EDGE_Y_UP = 108;
};

#endif // GAMEAREA_H