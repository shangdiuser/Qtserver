#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Qtserver.h"

class Qtserver : public QMainWindow
{
    Q_OBJECT

public:
    Qtserver(QWidget *parent = nullptr);
    ~Qtserver();

private:
    Ui::QtserverClass ui;
};
