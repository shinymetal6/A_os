#ifndef QTXMODEM_H
#define QTXMODEM_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class QtXmodem; }
QT_END_NAMESPACE
#define WAIT_REPLY              1000
#define MAX_RETRY               10

class QtXmodem : public QMainWindow
{
    Q_OBJECT

public:
    QtXmodem(QWidget *parent = nullptr);
    ~QtXmodem();

private slots:
    void on_Port_comboBox_currentTextChanged(const QString &arg1);

    void on_SelectFile_pushButton_clicked();

    void on_Download_pushButton_clicked();

private:
    Ui::QtXmodem *ui;

    int serial_tx( QByteArray hex_line);
    int serial_rx( void);
    void download_binary(void);
    void create_buf_and_tx(char *data);

    QSerialPort serial;
    int serial_started;
    QString filename,bin_filename;
    int file_size;

    QByteArray blob;


};
#endif // QTXMODEM_H
