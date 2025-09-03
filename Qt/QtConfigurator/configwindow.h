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

    void on_Generate_UserConfig_pushButton_clicked();

    void on_LwIp_checkBox_toggled(bool checked);

private:
    void create_config();
    void Generate_Header();
    void Generate_Libraries();
    void Generate_Modules();
    void Generate_Drivers();
    void Generate_Footer();

    Ui::ConfigWindow *ui;
    QString filename,config_filename;
    QFile file;
};
#endif // CONFIGWINDOW_H
