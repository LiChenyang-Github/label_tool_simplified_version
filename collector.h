#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <QMainWindow>
#include "ui_collector.h"

#include "common.h"

#include <QDir>
#include <QShortCut>
#include <QMouseEvent>
#include <QFileDialog>
#include <QLayout>           //###

//class to process mat
#include "processor.h"
//class to record data
#include "recorder.h"
//class to create database
#include "database.h"

#include "imagewidget.h"


namespace Ui {
class collector;
}

class collector : public QMainWindow
{
    Q_OBJECT

public:
    explicit collector(QWidget *parent = 0);
    ~collector();

private:
    Ui::collector *ui;
    ImageWidget *imgWidget;

    //core identification of image in path name
    int index;
    int minCount;
    int maxCount;

    bool labelSuccess;  //##### To decide whether successfully label the image.

    QString targetDir;
    QString targetBaseName;

    QString currentImageName;

    //mat processor
    Processor proc;
    //sqlite db
    Database db;

private slots:
    void on_videoButt_clicked();

    void on_selectButt_clicked();

    void on_startButt_clicked();

    void on_endButt_clicked();

    void on_lastButt_clicked();

    void on_nextButt_clicked();

    void on_dropButton_clicked();

protected:
    void mousePressEvent(QMouseEvent *e) override;   //##### Add override keyword to explicitly declare mouseMoveEvent() as a virtual func.
    void wheelEvent(QWheelEvent * event) override;    //##### Add wheel event for next or last button.

public:
    //auxilary function
    void updateWithIndex(int index);
    void recordDataIntoDb();

    //display funcion of ui
    void displayMat(Mat img);
    void displayData();

};

#endif // COLLECTOR_H
