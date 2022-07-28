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
#include <QStandardItemModel>
#include "opencv2/opencv.hpp"
#include "capture_thread.h"
#include "utilities.h"

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

    QStandardItemModel *list_model;

private:
    void initUI();
    void createActions();
    void populateSavedList();

private slots:
    void showCameraInfo();
    void openCamera();
    void updateFrame(cv::Mat *mat);
    void calculateFPS();
    void updateFPS(float fps);
    void recordingStartStop();
    void appendSavedVideo(QString name);
    void updateMonitorStatus(int status);
};
#endif // MAINWINDOW_H
