#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QFileDialog>
#include <QCoreApplication>
#include <QTextStream>
#include <QThread>
QT_BEGIN_NAMESPACE
namespace Ui { class ConfigWindow; }
QT_END_NAMESPACE

class ConfigWindow : public QMainWindow
{
    Q_OBJECT

public:
    ConfigWindow(QWidget *parent = nullptr);
    ~ConfigWindow();

private slots:

    void on_SelectCreate_UserConfig_pushButton_clicked();

private:
    void create_config();

    Ui::ConfigWindow *ui;
    QString filename,config_filename;
    QFile file;
};
#endif // CONFIGWINDOW_H
