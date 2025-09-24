#include "qtxmodem.h"
#include "ui_qtxmodem.h"
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

QtXmodem::QtXmodem(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtXmodem)
{
    ui->setupUi(this);
    ui->Download_pushButton->setEnabled(false);
}

QtXmodem::~QtXmodem()
{
    delete ui;
}

int QtXmodem::serial_tx( QByteArray hex_line)
{
QPixmap redled (":/ledred.png");
QPixmap greenled(":/ledgreen.png");
QByteArray reply;

    if ( serial_started == 0 )
        return 1;
    serial.write(hex_line);
    return 1;
}

int QtXmodem::serial_rx( void )
{
QByteArray reply;

    if(serial.waitForReadyRead(serial_timeout))
    {
        reply = serial.readAll();
        const char *data = reply.data();
        //qDebug() << QString::number(data[0], 8);

        return data[0];
    }
    qDebug()<< "RX timeout";
    return 0x41;
}


void QtXmodem::on_Port_comboBox_currentTextChanged(const QString &arg1)
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

    serial.close();
    qDebug()<< arg1;
    if ( arg1 == "Invalid")
    {
        ui->statusbar->showMessage("Serial port closed");
        ui->Download_pushButton->setEnabled(false);
    }
    serial_started = 0;
    serial.setPortName(arg1);
    if(serial.open(QIODevice::ReadWrite))
    {
        if(!serial.setBaudRate(QSerialPort::Baud115200))
        {
            ui->Comm_label->setPixmap(redled);
            //qDebug()<< arg1 << " : " << serial.errorString();
            ui->statusbar->showMessage(arg1+" : "+serial.errorString());
            ui->Download_pushButton->setEnabled(false);
        }
        else
        {
            ui->Comm_label->setPixmap(greenled);
            serial_started = 1;
            //qDebug()<< "Serial port opened";
            ui->statusbar->showMessage(arg1+" : Serial port opened");
            serial.setReadBufferSize (1024);
        }
    }
    else
    {
        ui->Comm_label->setPixmap(redled);
        //qDebug()<< arg1 << " : " << serial.errorString();
        ui->statusbar->showMessage(arg1+" : "+serial.errorString());
    }
}

void QtXmodem:: create_buf_and_tx(char    *data)
{
#define SLEEP_HERE   10
    QByteArray ba4(QByteArray::fromRawData(data, 132));
    serial_tx(ba4);
}

void QtXmodem::on_SelectFile_pushButton_clicked()
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");
    QString filters = "BIN/WAV/HEX files (*.bin , *.wav, *.hex)";

    filename = QFileDialog::getOpenFileName(this, tr("Open bin/wav/hex File"), "/Devel/Stm32_16.1_A_os_2024.10-rc/Membrane-2412171-00-WSensor_03/Debug",filters);

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
        qDebug()<<"File not found";
    else
    {
        const QFileInfo info(filename);
        const QString ffname(info.fileName());
        bin_filename = ffname;
        ui->label_BINFILE->setText(ffname);
        file_size = file.size();
        blob = file.readAll();
        file.close();
        ui->SelectedFile_label->setPixmap(greenled);
        ui->Download_pushButton->setEnabled(true);
    }
}

void QtXmodem::on_Download_pushButton_clicked()
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");
    char    data[132];
    int i,rx_data;
    int s_unit;
    int index=0;
    int block_number;
    int csum;
    int retry_number=0;
    QString danger = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );border-bottom-right-radius: 5px;border-bottom-left-radius: 5px;border: .px solid black;}";
    QString safe= "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #78d,stop: 0.4999 #46a,stop: 0.5 #45a,stop: 1 #238 );border-bottom-right-radius: 7px;border-bottom-left-radius: 7px;border: 1px solid black;}";
    QString done= "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #00FF00,stop: 0.4999 #00FF00,stop: 0.5 #00FF00,stop: 1 #238 );border-bottom-right-radius: 7px;border-bottom-left-radius: 7px;border: 1px solid black;}";

    ui->statusbar->showMessage("Downloading "+bin_filename);
    block_number = 1;
    ui->Flashing_label->setPixmap(redled);
    s_unit = file_size/100;
    ui->download_progressBar->setStyleSheet(safe);
    ui->download_progressBar->setValue(0);

    serial_timeout = WAIT_POLL;
    index=0;
    while ( (rx_data = serial_rx()) != 0x15 );
    qDebug()<<"Start command received, downloading";
    QThread::msleep(5);

    while ( index < file_size)
    {
        data[0] = 0x01;
        data[1] = block_number;
        data[2] = 255 - block_number;
        block_number++;
        if ( block_number == 0 )
            block_number = 1;
        csum = 0;
        for(i=0;i<128;i++,index++)
        {
            if ( index < file_size )
                data[i+3] = blob[index];
            else
                data[i+3] = 0;
            csum += data[i+3];
        }
        data[131] = csum;

        serial.flush();
        create_buf_and_tx(data);
        while ( (rx_data = serial_rx()) != 0x06 )
        {
            ui->statusbar->showMessage("Retry");
            qDebug()<<"Retry on Ack, block_number "<< block_number<<" data "<<rx_data;
            serial.flush();
            create_buf_and_tx(data);
            retry_number++;
        }
        serial_timeout = WAIT_REPLY;

        if ((( index/s_unit) & 1 ) == 0)
            ui->Flashing_label->setPixmap(greenled);
        else
            ui->Flashing_label->setPixmap(redled);
        ui->download_progressBar->setStyleSheet(safe);
        ui->download_progressBar->setValue(index/s_unit);

    }
    QString retry_number_str;
    retry_number_str.setNum(retry_number);
    if (retry_number == 1 )
        ui->statusbar->showMessage(bin_filename+" downloaded, "+retry_number_str+" retry");
    else
        ui->statusbar->showMessage(bin_filename+" downloaded, "+retry_number_str+" retries");

    ui->download_progressBar->setStyleSheet(done);
    ui->download_progressBar->setValue(100);
    ui->Flashing_label->setPixmap(greenled);

    data[0] = 0x04;
    QByteArray ba1(QByteArray::fromRawData(data, 1));
    QThread::msleep(20);
    serial_tx(ba1);
    QThread::msleep(20);
    serial_tx(ba1);
    QThread::msleep(20);
    serial_tx(ba1);
    rx_data = serial_rx();
    qDebug()<<"Finished, received "<<rx_data;


}


void QtXmodem::on_SetTargetRX_pushButton_clicked()
{
    char    command[128];

    sprintf(command,"<h %d %s %s >",file_size,filename.toLatin1(),"0");
    QByteArray ba1(QByteArray::fromRawData(command, strlen(command)));
    serial_tx(ba1);

}
