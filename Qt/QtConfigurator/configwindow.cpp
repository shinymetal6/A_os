#include "configwindow.h"
#include "ui_configwindow.h"
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

ConfigWindow::ConfigWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

#ifdef Q_OS_WIN
#define PROGRAMS_PATH   "c:/Nevol_Presso_Resources/Programs"
#else
#define PROGRAMS_PATH   "/Devel/Stm32_18.1_A_os_2025.10-rc"
#endif

void ConfigWindow::create_config(void)
{
    QFile file(filename);
    if ( ! file.exists())
    {
        if (file.open(QIODevice::WriteOnly))
        {
            file.write("/* \n");
            file.write("* This program is free software: you can redistribute it and/or modify  \n");
            file.write("* it under the terms of the GNU General Public License as published by  \n");
            file.write("* the Free Software Foundation, version 3.\n");
            file.write("*\n");
            file.write("* This program is distributed in the hope that it will be useful, but \n");
            file.write("* WITHOUT ANY WARRANTY; without even the implied warranty of \n");
            file.write("* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU \n");
            file.write("* General Public License for more details.\n");
            file.write("*\n");
            file.write("* You should have received a copy of the GNU General Public License \n");
            file.write("* along with this program. If not, see <http://www.gnu.org/licenses/>.\n");
            file.write("*\n");
            file.write("* Project : A_os\n");
            file.write("*/\n");
            file.write("/*\n");
            file.write("* user_config.h\n");
            file.write("*\n");
            file.write("*  Created on: Sep 1, 2025\n");
            file.write("*      Author: fil\n");
            file.write("*/\n");
            file.write("#ifndef PROJECT_LIB_MODULES_H_\n");
            file.write("#define PROJECT_LIB_MODULES_H_\n");
            file.write("\n");
            file.write("#define		USER_PROCESSES	1\n");
            file.write("\n");
            file.write("/* Libraries */\n");
            file.write("//#define	HELIX_ENABLE	1\n");
            file.write("//#define	LORAWAN_ENABLE	1\n");
            file.write("//#define	LWIP_ENABLE		1\n");
            file.write("//#define	WIFI_ENABLE		1\n");
            file.write("\n");
            file.write("#ifdef LWIP_ENABLE\n");
            file.write("	#define	NETWORKING_ENABLED		1\n");
            file.write("	//#define	MQTT_ENABLE	1\n");
            file.write("#endif // #ifdef LWIP_ENABLE\n");
            file.write("#ifdef WIFI_ENABLE\n");
            file.write("	#define	WIFI_ESP01S		1\n");
            file.write("#endif // #ifdef WIFI_ENABLE\n");
            file.write("\n");
            file.write("/* Modules */\n");
            file.write("#define	HEX_DEC_ENABLE	1\n");
            file.write("#define	XMODEM_ENABLE	1\n");
            file.write("//#define	MODBUS_ENABLE	1\n");
            file.write("//#define	SOUND_ENABLED	1\n");
            file.write("\n");
            file.write("/* drivers */\n");
            file.write("\n");
            file.write("#define USB_DEVICE_ENABLED	1\n");
            file.write("#ifdef USB_DEVICE_ENABLED\n");
            file.write("	#define	USB_CDC				1\n");
            file.write("	//#define	USB_MIDI			1\n");
            file.write("#endif // #ifdef USB_ENABLED\n");
            file.write("\n");
            file.write("#define LD1_Pin			LED_Pin\n");
            file.write("#define LD1_GPIO_Port	LED_GPIO_Port\n");
            file.write("\n");
            file.write("#define	BOARD_NAME			\"Concentrator-2412172_00\"\n");
            file.write("#define	MACHINE_NAME		\"Membrane\"\n");
            file.write("#define	MACHINE_VERSION		\"A\"\n");
            file.write("\n");
            file.write("#endif /* PROJECT_LIB_MODULES_H_ */\n");
            qDebug() << "File created successfully!";
            file.close();
        }
    }
}

void ConfigWindow::on_SelectCreate_UserConfig_pushButton_clicked()
{
    QString filters = "H files (*.h)";
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select directory"), QCoreApplication::applicationDirPath());
    qDebug() << directory;
    filename = directory+"/user_config.h";
    create_config();
    config_filename = QFileDialog::getOpenFileName(this, tr("Open user_config.h File"), directory,filters);
/*
    qDebug() << config_filename;
    filename = directory+"/user_config.h";
    create_config();
    */
}
