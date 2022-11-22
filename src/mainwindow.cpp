#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsItem>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <QThread>
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , fileMenu(nullptr)
    , viewMenu(nullptr)
    , currentImage(nullptr)
{
    initUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    this->resize(800, 600);
    // setup menubar
    fileMenu = menuBar()->addMenu("&File");
    viewMenu = menuBar()->addMenu("&View");

    // setup toolbar
    fileToolBar = addToolBar("File");
    viewToolBar = addToolBar("View");


    // main area for image display
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    setCentralWidget(imageView);

    // setup status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("Image Information will be here!");

    createActions();
}

void MainWindow::createActions()
{

    // create actions, add them to menus
    openAction = new QAction("&Open", this);
    fileMenu->addAction(openAction);
    saveAsAction = new QAction("&Save as", this);
    fileMenu->addAction(saveAsAction);
    exitAction = new QAction("E&xit", this);
    fileMenu->addAction(exitAction);

    zoomInAction = new QAction("Zoom in", this);
    viewMenu->addAction(zoomInAction);
    zoomOutAction = new QAction("Zoom Out", this);
    viewMenu->addAction(zoomOutAction);
    prevAction = new QAction("&Previous Image", this);
    viewMenu->addAction(prevAction);
    nextAction = new QAction("&Next Image", this);
    viewMenu->addAction(nextAction);

    // add actions to toolbars
    fileToolBar->addAction(openAction);
    viewToolBar->addAction(zoomInAction);
    viewToolBar->addAction(zoomOutAction);
    viewToolBar->addAction(prevAction);
    viewToolBar->addAction(nextAction);

    // connect the signals and slots
    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(zoomInAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(prevAction, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(nextAction, SIGNAL(triggered(bool)), this, SLOT(nextImage()));

    setupShortcuts();
}

void MainWindow::openImage()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg *avi *.mpg)"));
    QStringList filePaths;
    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        showImage(filePaths.at(0));
    }
}

// void MainWindow::showImage(QString path)
// {
//     imageScene->clear();
//     imageView->resetTransform(); // resetMatrix();
//     QPixmap image(path);
//     currentImage = imageScene->addPixmap(image);
//     imageScene->update();
//     imageView->setSceneRect(image.rect());
//     QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
//         .arg(image.height()).arg(QFile(path).size());
//     mainStatusLabel->setText(status);
//     currentImagePath = path;
// }

void MainWindow::showImage(QString path)
{
    imageScene->clear();
    imageView->resetTransform(); // resetMatrix();

    // cv::VideoCapture capture("../sec07_mot_mpg/motor.avi");
    cv::VideoCapture capture(path.toStdString());

    if (!capture.isOpened())
    {
        // error in opening the video input
        std::cerr << "Unable to open file!" << std::endl;
    }

    uint delayTime = static_cast<uint>(1000 / 5);

    cv::Mat frame1, prvs;
    capture >> frame1;
    cv::cvtColor(frame1, prvs, cv::COLOR_BGR2GRAY);

    while(true)
    {
        cv::Mat frame2, next, original;
        capture >> frame2;
        if (frame2.empty())
        {
            break;
        }

        cv::cvtColor(frame2, next, cv::COLOR_BGR2GRAY);
        cv::Mat flow(prvs.size(), CV_32FC2);
        cv::calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

        frame2.copyTo(original);

        // By y += 5, x += 5 you can specify the grid 
        for (int y = 0; y < original.rows; y += 5)
        {
            for (int x = 0; x < original.cols; x += 5)
            {
                // get the flow from y, x position * 10 for better visibility
                const cv::Point2f flowatxy = flow.at<cv::Point2f>(y, x) * 5;
                // draw line at flow direction
                cv::line(original, cv::Point(x, y), cv::Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));
                // draw initial point
                cv::circle(original, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
            }
        }

        QPixmap image = QPixmap::fromImage(QImage( original.data, original.cols, original.rows, QImage::Format_BGR888));

        currentImage = imageScene->addPixmap(image);
        qApp->processEvents();
        imageScene->update();
        imageView->setSceneRect(image.rect());
        QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
            .arg(image.height()).arg(QFile(path).size());
        mainStatusLabel->setText(status);

        prvs = next;
        QThread::msleep(delayTime);
    }
    currentImagePath = path;
}

void MainWindow::zoomIn()
{
    imageView->scale(1.2, 1.2);
}

void MainWindow::zoomOut()
{
    imageView->scale(1/1.2, 1/1.2);
}

void MainWindow::prevImage()
{
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if(idx > 0) {
        showImage(dir.absoluteFilePath(fileNames.at(idx - 1)));
    } else {
        QMessageBox::information(this, "Information", "Current image is the first one.");
    }
}

void MainWindow::nextImage()
{
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if(idx < fileNames.size() - 1) {
        showImage(dir.absoluteFilePath(fileNames.at(idx + 1)));
    } else {
        QMessageBox::information(this, "Information", "Current image is the last one.");
    }
}

void MainWindow::saveAs()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Nothing to save.");
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Image As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0))) {
            currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }
}


void MainWindow::setupShortcuts()
{
    QList<QKeySequence> shortcuts;
    shortcuts << Qt::Key_Plus << Qt::Key_Equal;
    zoomInAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Minus << Qt::Key_Underscore;
    zoomOutAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Up << Qt::Key_Left;
    prevAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Down << Qt::Key_Right;
    nextAction->setShortcuts(shortcuts);
}