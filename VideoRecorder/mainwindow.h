#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QCheckBox>
#include <QPushButton>
#include <QListView>
#include <QStatusBar>
#include <QLabel>
#include <QGridLayout>
#include <QPixmap>
#include <QMediaDevices>
#include <QCameraDevice>
#include <QMessageBox>
#include <QMutex>
#include "opencv2/opencv.hpp"
#include "capture_thread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction *cameraInfoAction;
    QAction *openCameraAction;
    QAction *exitAction;
    QAction *calcFPSAction;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QCheckBox *monitorCheckBox;
    QPushButton *recordButton;

    QListView *saved_list;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    cv::Mat currentFrame;

    // for capture thread
    QMutex *data_lock;
    CaptureThread *capturer;

private:
    void initUI();
    void createActions();

private slots:
    void showCameraInfo();
    void openCamera();
    void updateFrame(cv::Mat *mat);
    void calculateFPS();
    void updateFPS(float fps);
};
#endif // MAINWINDOW_H
