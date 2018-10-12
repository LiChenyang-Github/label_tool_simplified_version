#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include "common.h"

#include "qlabel.h"
#include "qevent.h"
#include "qpainter.h"

class ImageWidget :
    public QLabel
{
public:
    ImageWidget();
    ~ImageWidget();

    bool isOn;

protected:
    void mouseMoveEvent(QMouseEvent *e) override;   //##### add override keyword to explicitly declare mouseMoveEvent() as a virtual func.
    void paintEvent(QPaintEvent *e) override;   //###### the same as above

private:
    int mouseX;
    int mouseY;
};

#endif // IMAGEWIDGET_H
