#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QToolButton>
#include "Game.h"

namespace Ui {
class Setting;
}


class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();

    void Showsettings( Game::Status gStatus = Game::Waitting);

    Ui::Setting *ui;
    QToolButton *PauseButton;
    QToolButton *ReStartButton;
    QToolButton *HomeButton;
private:
    void setBaseButton();
    void setControlButton();

private slots:
    void hideSlot();
};

#endif // SETTING_H
