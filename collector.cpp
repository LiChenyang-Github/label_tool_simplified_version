#include "collector.h"


collector::collector(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::collector)
{
    ui->setupUi(this);

    //using self defined widget just for crossing lines
    QWidget *mainWindow = this->window();
    QLayout *layout = this->layout();
    imgWidget = new ImageWidget();
    imgWidget->isOn = false;
    QRect rect = ui->imageLabel->geometry();
    rect.setY(ui->imageLabel->y() + 13);//magic number
    rect.setHeight(ui->imageLabel->height());	//###
    imgWidget->setGeometry(rect);
    layout->addWidget(imgWidget);
    mainWindow->setLayout(layout);
    labelSuccess = true;    //##### To decide whether successfully label the image.

    //##### shortcut illustraction
    //shortcut of pre butt
    QShortcut *key_pre = new QShortcut(QKeySequence(Qt::Key_A), this);//创建一个快捷键"A"键
    connect(key_pre, SIGNAL(activated()), this, SLOT(on_lastButt_clicked()));//连接到指定槽函数

    //shortcut of next butt
    QShortcut *key_next = new QShortcut(QKeySequence(Qt::Key_D), this);//创建一个快捷键"D"键
    connect(key_next, SIGNAL(activated()), this, SLOT(on_nextButt_clicked()));//连接到指定槽函数

//    QShortcut *key_no_thumb = new QShortcut(QKeySequence(Qt::Key_S), this);//创建一个快捷键"T"键
//    connect(key_no_thumb, SIGNAL(activaed()), this, SLOT(on_autoThumbButt_clicked()));//链接到指定槽函数

    QShortcut *key_drop = new QShortcut(QKeySequence(Qt::Key_S), this);//创建一个快捷键"D"键
    connect(key_drop, SIGNAL(activated()), this, SLOT(on_dropButton_clicked()));//连接到指定槽函数
}

collector::~collector()
{
    delete ui;
}

void collector::on_videoButt_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open Video"), tr(""));
    if (path.length() == 0)
    {
        QMessageBox::information(NULL, tr("Path"), tr("Didn't Select a Correct Video File."));
        return;
    }

    //利用视频文件获取路径
    QFileInfo fileInfo(path);
    targetBaseName = fileInfo.baseName();
    targetDir = fileInfo.path();
//    qDebug() << path << std::endl << targetDir;
//    qDebug() << path << targetDir;


    //分割视频流成为帧图片
    proc.splitVideo(path, targetDir, targetBaseName);

    ui->pathLabel->setText(path);
    ui->pathLabel->setWordWrap(true);   //##### Automatically set up a new line.

    //start with existed db or new a db
    db.prepareDB(targetDir);
}

void collector::on_selectButt_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open Video"), tr(""));
    if (path.length() == 0)
    {
        QMessageBox::information(NULL, tr("Path"), tr("Didn't Select a Correct Video File."));
        return;
    }

    QFileInfo fileInfo(path);
    targetBaseName = fileInfo.baseName();//文件名（除去后缀）
    targetDir = fileInfo.path();//文件目录路径

    ui->pathLabel->setText(path);
    ui->pathLabel->setWordWrap(true);   //##### Automatically set up a new line.

    //start with existed db or new a db
    db.prepareDB(targetDir);
}



void collector::on_startButt_clicked()
{
    if (targetDir.length() == 0)
    {
        QMessageBox::information(NULL, tr("Empty Path"), tr("Pleass Open a Directory!"));
        return;
    }

    //calculate the number of files
    QDir dir(targetDir);
    if (!dir.exists())
        return;
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList filters;
    filters<<QString("*.jpg");
    dir.setNameFilters(filters);   //##### To exclude the video file, db file and .txt.
    QFileInfoList list = dir.entryInfoList();
    int file_count = list.count();
    if (file_count <= 0)
        return;

    minCount = 0;
    maxCount = file_count;

    qDebug() << file_count;

    //##### Move the following code to the constructor to avoid the short key failure when press the startButt more than one time.
//    //shortcut of pre butt
//    QShortcut *key_pre = new QShortcut(QKeySequence(Qt::Key_A), this);//创建一个快捷键"A"键
//    connect(key_pre, SIGNAL(activated()), this, SLOT(on_lastButt_clicked()));//连接到指定槽函数

//    //shortcut of next butt
//    QShortcut *key_next = new QShortcut(QKeySequence(Qt::Key_D), this);//创建一个快捷键"D"键
//    connect(key_next, SIGNAL(activated()), this, SLOT(on_nextButt_clicked()));//连接到指定槽函数

//    QShortcut *key_no_thumb = new QShortcut(QKeySequence(Qt::Key_S), this);//创建一个快捷键"T"键
//    connect(key_no_thumb, SIGNAL(activaed()), this, SLOT(on_autoThumbButt_clicked()));//链接到指定槽函数

    index = ui->lineEdit->text().toInt();
    updateWithIndex(index);
}

void collector::on_endButt_clicked()
{
    db.convertDBtoTxt(targetDir);
    QMessageBox::information(NULL, tr("Finish!"), tr("Collection Finished! Please Check the Label txt File!"));
}

void collector::on_lastButt_clicked()
{
    if (proc.clickedTimes == 5)
    {
        int ttx = 0; int tty = 0;
        int tjx = 0; int tjy = 0;

        proc.addCircleInImage(ttx, tty);
        proc.addCircleInImage(tjx, tjy);
        proc.hasThumb = false;

        recordDataIntoDb();
    }
    else if (proc.clickedTimes == 7){
        recordDataIntoDb();
    }
    else if (proc.clickedTimes > 0){    //##### Add a QMessageBox when the click times is larger than 0 but not equal to 5 or 7.
        QMessageBox::information(NULL, tr(""), tr("Invalid click times! The coordinates won't be saved!"));
        labelSuccess = false;    //##### To keep at the current image.
    }

    //move to previous index
    if(labelSuccess)
    {
        index--;
    }
    else
    {
        labelSuccess = true;
    }

    if (index < minCount)
    {
        QMessageBox::information(NULL, tr(""), tr("Arrived Minimal Index!"));
        index = minCount;
        return;
    }

    updateWithIndex(index);
}

void collector::on_nextButt_clicked()
{
    if (proc.clickedTimes == 5)
    {
        int ttx = 0; int tty = 0;
        int tjx = 0; int tjy = 0;

        proc.addCircleInImage(ttx, tty);
        proc.addCircleInImage(tjx, tjy);
        proc.hasThumb = false;

        recordDataIntoDb();
    }
    else if (proc.clickedTimes == 7){
        recordDataIntoDb();
    }
    else if (proc.clickedTimes > 0){    //##### Add a QMessageBox when the click times is larger than 0 but not equal to 5 or 7.
        QMessageBox::information(NULL, tr(""), tr("Invalid click times! The coordinates won't be saved!"));
        labelSuccess = false;   //##### To keep at the current image.
    }

    //move to next index
    if(labelSuccess)
    {
        index++;
    }
    else
    {
        labelSuccess = true;
    }

    if (index >= maxCount)
    {
        QMessageBox::information(NULL, tr(""), tr("Arrive Maximal Index!"));
        return;
    }
    updateWithIndex(index);
}

//##### Add a drop image button, to draw the point around the left top.
void collector::on_dropButton_clicked()
{
    proc.addCircleInImage(0, 0);
    proc.addCircleInImage(2, 2);
    proc.addCircleInImage(1, 1);
    proc.addCircleInImage(1, 1);
    proc.addCircleInImage(1, 1);

    on_nextButt_clicked();
}


////*****************
//// 鼠标事件
////*****************
void collector::mousePressEvent(QMouseEvent *e)
{
    //get coordinate inside label
    int posx = e->x();
    int posy = e->y();

    posx -= (ui->imageLabel->x());
    posy -= (ui->imageLabel->y() + ui->mainToolBar->height());	//###后面“+ ui.mainToolBar->height()”原因在于e->x()和e->y()是以整个窗口的左上角为基准的，而ui.imageLabel->x()和ui.imageLabel->y()是以窗口除去上面的工具栏的其余部分的左上角为基准的！所以在y方向上会相差一个工具栏的高度，即ui.mainToolBar->height()！

    //return if out
    if (posx>FRAME_WIDTH || posy > FRAME_HEIGHT || posx < 0 || posy < 0)
        return;

    //circle it out
    proc.addCircleInImage(posx, posy);

    if (proc.clickedTimes>=2)	//###
        imgWidget->isOn = false;

    displayMat(proc.showDstImage());
    displayData();
}

//##### Add wheel event.
void collector::wheelEvent(QWheelEvent* event){
    if(event->delta()>0){//up
        on_lastButt_clicked();
    }else{//down
        on_nextButt_clicked();
    }
}


//*****************
// 基于序号index更新视图
//*****************
void collector::updateWithIndex(int currentIndex)
{
    //get current file
    currentImageName = targetBaseName + "_color_" + QString::number(currentIndex) + ".jpg";
    QString imageInPath = targetDir + "/" + currentImageName;
    proc.readImage(imageInPath.toStdString().data());

    imgWidget->isOn = true;

    vector<float> v = db.execSelect(index,currentImageName);
    if (v.size() == 0)//查询db返回空
        displayMat(proc.showDstImage());
    if (v.size() == COLLECT_PARAM_COUNT)//查询db返回已有数据
        displayMat(proc.showRecordedImage(v));

    displayData();
}

void collector::recordDataIntoDb()
{
    displayMat(proc.showDstImage());
    displayData();

    if (!proc.hasFinished)
        return;

    //if check ok, record; else restart
    if (proc.checkPoints())
    {
        vector<float> params;
        float tlx = proc.tlx / FRAME_WIDTH; params.push_back(tlx);
        float tly = proc.tly / FRAME_HEIGHT; params.push_back(tly);
        float brx = proc.brx / FRAME_WIDTH; params.push_back(brx);
        float bry = proc.bry / FRAME_HEIGHT; params.push_back(bry);
        float iftx = proc.iftx / FRAME_WIDTH; params.push_back(iftx);
        float ifty = proc.ifty / FRAME_HEIGHT; params.push_back(ifty);
        float ifjx = proc.ifjx / FRAME_WIDTH; params.push_back(ifjx);
        float ifjy = proc.ifjy / FRAME_HEIGHT; params.push_back(ifjy);
        float tfjx = proc.tfjx / FRAME_WIDTH; params.push_back(tfjx);
        float tfjy = proc.tfjy / FRAME_HEIGHT; params.push_back(tfjy);
        float btx = proc.btx / FRAME_WIDTH; params.push_back(btx);
        float bty = proc.bty / FRAME_HEIGHT; params.push_back(bty);
        float bjx = proc.bjx / FRAME_WIDTH; params.push_back(bjx);
        float bjy = proc.bjy / FRAME_HEIGHT; params.push_back(bjy);

        db.execInsert(index, currentImageName, params);
    }
    else
    {
        QMessageBox::information(NULL, tr(""), tr("Error In Current Frame Collection, Please Redo It!"));
        proc.clickedTimes = 0;
        proc.hasFinished = false;

        updateWithIndex(index);
        labelSuccess = false;   //#####
    }
}


//*****************
// 数据结构转化：OpenCV vs Qt
// 输出图片到QtLabel
//*****************
void collector::displayMat(Mat image)
{
    Mat rgb;
    QImage img;
    if (image.channels() == 3)
    {
        //cvt Mat BGR 2 QImage RGB
        cvtColor(image, rgb, CV_BGR2RGB);
        img = QImage((const unsigned char*)(rgb.data),
            rgb.cols, rgb.rows,
            rgb.cols*rgb.channels(),
            QImage::Format_RGB888);
    }
    else
    {
        img = QImage((const unsigned char*)(image.data),
            image.cols, image.rows,
            image.cols*image.channels(),
            QImage::Format_RGB888);
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(img));
//    qDebug() << "hahahhahaha:" << ui->imageLabel->size();
    ui->imageLabel->resize(ui->imageLabel->pixmap()->size());

//    qDebug() << "hahahhahaha:" << ui->imageLabel->pixmap()->size();
//    qDebug() << "hahahhahaha:" << ui->imageLabel->size();
}

void collector::displayData()
{
    QString imageInPath = "/" + targetBaseName + "_color_" + QString::number(index) + ".jpg";
    ui->fileNameLabel->setText(imageInPath);

    QString tlxy = "TopLeft:(" + QString::number(proc.tlx) + "," + QString::number(proc.tly) + ")";
    ui->tlLabel->setText(tlxy);

    QString brxy = "BottomRight:(" + QString::number(proc.brx) + "," + QString::number(proc.bry) + ")";
    ui->brLabel->setText(brxy);

    QString iftxy = "IndexTip:(" + QString::number(proc.iftx) + "," + QString::number(proc.ifty) + ")";
    ui->iftLabel->setText(iftxy);

    QString ifjxy = "IndexJoint:(" + QString::number(proc.ifjx) + "," + QString::number(proc.ifjy) + ")";
    ui->ifjLabel->setText(ifjxy);

//    QString tfjxy = "Palm:(" + QString::number(proc.ifjx) + "," + QString::number(proc.ifjy) + ")";
    QString tfjxy = "Palm:(" + QString::number(proc.tfjx) + "," + QString::number(proc.tfjy) + ")";
    ui->tfjLabel->setText(tfjxy);

    QString btxy = "ThumbTip:(" + QString::number(proc.btx) + "," + QString::number(proc.bty) + ")";
    ui->btLabel->setText(btxy);

    QString bjxy = "ThumbJoint:(" + QString::number(proc.bjx) + "," + QString::number(proc.bjy) + ")";
    ui->bjLabel->setText(bjxy);
}


