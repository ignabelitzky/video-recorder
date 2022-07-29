#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    capturer = nullptr;
    data_lock = new QMutex();
    populateSavedList();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    this->resize(1000, 800);
    // setup menubar
    fileMenu = menuBar()->addMenu("&File");
    helpMenu = menuBar()->addMenu("&Help");

    QGridLayout *main_layout = new QGridLayout();
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    main_layout->addWidget(imageView, 0, 0, 12, 1);

    QGridLayout *tools_layout = new QGridLayout();
    main_layout->addLayout(tools_layout, 12, 0, 1, 1);

    monitorCheckBox = new QCheckBox(this);
    monitorCheckBox->setText("Monitor On/Off");
    tools_layout->addWidget(monitorCheckBox, 0, 0);

    recordButton = new QPushButton(this);
    recordButton->setText("Record");
    tools_layout->addWidget(recordButton, 0, 1, Qt::AlignHCenter);
    tools_layout->addWidget(new QLabel(this), 0, 2);

    connect(recordButton, SIGNAL(clicked(bool)), this, SLOT(recordingStartStop()));
    connect(monitorCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateMonitorStatus(int)));

    // list of saved videos
    saved_list = new QListView(this);
    main_layout->addWidget(saved_list, 13, 0, 4, 1);

    QWidget *widget = new QWidget();
    widget->setLayout(main_layout);
    setCentralWidget(widget);

    // setup status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("Video Recorder is ready");

    createActions();

    // list of saved videos
    saved_list = new QListView(this);
    saved_list->setViewMode(QListView::IconMode);
    saved_list->setResizeMode(QListView::Adjust);
    saved_list->setSpacing(5);
    saved_list->setWrapping(false);
    list_model = new QStandardItemModel(this);
    saved_list->setModel(list_model);
    main_layout->addWidget(saved_list, 13, 0, 4, 1);
}

void MainWindow::createActions()
{
    cameraInfoAction = new QAction("Camera &Info", this);
    openCameraAction = new QAction("Open Camera", this);
    calcFPSAction = new QAction("&Calculate FPS", this);
    exitAction = new QAction("E&xit");
    aboutAction = new QAction("&About", this);

    fileMenu->addAction(cameraInfoAction);
    fileMenu->addAction(openCameraAction);
    fileMenu->addAction(calcFPSAction);
    fileMenu->addAction(exitAction);

    helpMenu->addAction(aboutAction);

    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(cameraInfoAction, SIGNAL(triggered(bool)), this, SLOT(showCameraInfo()));
    connect(openCameraAction, SIGNAL(triggered(bool)), this, SLOT(openCamera()));
    connect(calcFPSAction, SIGNAL(triggered(bool)), this, SLOT(calculateFPS()));
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(showAbout()));
}

void MainWindow::populateSavedList()
{
    QDir dir(Utilities::getDataPath());
    QStringList nameFilters;
    nameFilters << "*.jpg";
    QFileInfoList files = dir.entryInfoList(nameFilters,
                                            QDir::NoDotAndDotDot | QDir::Files,
                                            QDir::Name);
    foreach(QFileInfo cover, files) {
        QString name = cover.baseName();
        QStandardItem *item = new QStandardItem();
        list_model->appendRow(item);
        QModelIndex index = list_model->indexFromItem(item);
        list_model->setData(index, QPixmap(cover.absoluteFilePath()).scaledToHeight(145),
                            Qt::DecorationRole);
        list_model->setData(index, name, Qt::DisplayRole);
    }
}

void MainWindow::showCameraInfo()
{
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    QString info = QString("Available Cameras: \n");

    foreach(const QCameraDevice &cameraInfo, cameras) {
        info += " - " + cameraInfo.id() + ": ";
        info += cameraInfo.description() + "\n";
    }
    QMessageBox::information(this, "Cameras", info);
}

void MainWindow::openCamera()
{
    if(capturer != nullptr) {
        // if a thread is already runngin, stop it
        capturer->setRunning(false);
        disconnect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
        connect(capturer, &CaptureThread::finished, capturer, &CaptureThread::deleteLater);
        disconnect(capturer, &CaptureThread::fpsChanged, this, &MainWindow::updateFPS);
        disconnect(capturer, &CaptureThread::videoSaved, this, &MainWindow::appendSavedVideo);
    }
    int camID = 0;
    capturer = new CaptureThread(camID, data_lock);
    connect(capturer, &CaptureThread::fpsChanged, this, &MainWindow::updateFPS);
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::videoSaved, this, &MainWindow::appendSavedVideo);
    capturer->start();
    mainStatusLabel->setText(QString("Capturing camera %1").arg(camID));
    monitorCheckBox->setCheckState(Qt::Unchecked);
    recordButton->setText("Record");
    recordButton->setEnabled(true);
}

void MainWindow::updateFrame(cv::Mat *mat)
{
    data_lock->lock();
    currentFrame = *mat;
    data_lock->unlock();

    QImage frame(currentFrame.data, currentFrame.cols, currentFrame.rows, currentFrame.step, QImage::Format_RGB888);
    QPixmap image = QPixmap::fromImage(frame);

    imageScene->clear();
    imageView->resetTransform();
    imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());
}

void MainWindow::calculateFPS()
{
    if(capturer != nullptr) {
        capturer->startCalcFPS();
    }
}

void MainWindow::updateFPS(float fps)
{
    mainStatusLabel->setText(QString("FPS of current camera is %1").arg(fps));
}

void MainWindow::recordingStartStop()
{
    QString text = recordButton->text();
    if(text == "Record" && capturer != nullptr) {
        capturer->setVideoSavingStatus(CaptureThread::STARTING);
        recordButton->setText("Stop Recording");
        monitorCheckBox->setEnabled(false);
    } else if(text == "Stop Recording" && capturer != nullptr) {
        capturer->setVideoSavingStatus(CaptureThread::STOPPING);
        recordButton->setText("Record");
        monitorCheckBox->setEnabled(true);
    }
}

void MainWindow::appendSavedVideo(QString name)
{
    QString cover = Utilities::getSavedVideoPath(name, "jpg");
    QStandardItem *item = new QStandardItem();
    list_model->appendRow(item);
    QModelIndex index = list_model->indexFromItem(item);
    list_model->setData(index, QPixmap(cover).scaledToHeight(145),
                        Qt::DecorationRole);
    list_model->setData(index, name, Qt::DisplayRole);
    saved_list->scrollTo(index);
}

void MainWindow::updateMonitorStatus(int status)
{
    if(capturer == nullptr) {
        return;
    }
    if(status) {
        capturer->setMotionDetectingStatus(true);
        recordButton->setEnabled(false);
    } else {
        capturer->setMotionDetectingStatus(false);
        recordButton->setEnabled(true);
    }
}

void MainWindow::showAbout()
{
    QString text = "Desktop application developed by\n";
    text += "Ignacio Belitzky\n\n";
    text += "License: GNU General Public License v3";
    QMessageBox::information(this, "About", text);
}
