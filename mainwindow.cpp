#include "mainwindow.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include "MyVector.h"
#include "StudentManagement.h"
#include "SystemManagement.h"
#include "TeacherManagement.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    qDebug() << "current path: " << QDir::currentPath();
    qDebug() << "application path: " << QCoreApplication::applicationDirPath();
    this->setMaximumSize(this->size());
    this->setMinimumSize(this->size());
    data = new data_manager::GeneralDataManager();
    data->loadShit();
}

MainWindow::~MainWindow() {
    data->exportShit();
    delete ui;
}

void MainWindow::on_system_management_button_clicked() {
    auto window = (new SystemManagement(data));
    window->setWindowModality(Qt::ApplicationModal);
    window->show();
}

void MainWindow::on_pushButton_clicked() {}

void MainWindow::on_studentManageButton_clicked() {
    auto window = new StudentManagement(data, nullptr);
    window->setWindowModality(Qt::ApplicationModal);
    window->show();
}

void MainWindow::on_teacherManageButton_clicked() {
    auto window = new TeacherManagement(data, nullptr);
    window->setWindowModality(Qt::ApplicationModal);
    window->show();
}

void MainWindow::on_studentManageButton_2_clicked() {
    auto window = new StudentManagement(data, nullptr);
    window->setWindowModality(Qt::ApplicationModal);
    window->show();
}
