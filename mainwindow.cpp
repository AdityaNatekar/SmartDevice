#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QProcess>
#include<QDebug>
#include<QFile>
#include<QMessageBox>
#include <QCommonStyle>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->brightness_Value->setStyleSheet("border: 1px solid black;background-color: white;");
    ui->label_currentTime->setStyleSheet("border-top: 2px solid black;border-left: 2px solid black;border-right: 1px solid white;border-bottom: 1px solid white;");
    ui->radioButton_setip->setChecked(true);
    ui->btn_saveTime->setEnabled(false);

    //Read brightness value for startup
    QFile brightness_current("/sys/devices/soc0/backlight/backlight/backlight/brightness");
    brightness_current.open(QIODevice::ReadOnly);
    char level[1];
    brightness_current.read(level,1);
    int backlight_Flag= atoi(level);
    //qDebug() << "current brightness in int " << backlight_Flag;
    brightness_current.close();
    ui->brightness_label->setVisible(false);
    QString bf=QString::number(backlight_Flag);
    ui->brightness_label->setText(bf);

    // display brightness percentage on startup
    //70 from debug observation on startup, to avoid error
    if(backlight_Flag>=7 || backlight_Flag>=70)
    {
        ui->brightness_Value->setText("100");
        ui->Btn_Bright->setEnabled(false);
    }
    if(backlight_Flag==6 || (backlight_Flag>=60 &&  backlight_Flag<70))
    {
        ui->brightness_Value->setText("90");
    }
    if(backlight_Flag==5 || (backlight_Flag>=50 &&  backlight_Flag<60))
    {
        ui->brightness_Value->setText("80");
    }
    if(backlight_Flag==4 || (backlight_Flag>=40 &&  backlight_Flag<50))
    {
        ui->brightness_Value->setText("70");
    }
    if(backlight_Flag==3 || (backlight_Flag>=30 &&  backlight_Flag<40))
    {
        ui->brightness_Value->setText("60");
    }
    if(backlight_Flag==2 ||(backlight_Flag>=20 &&  backlight_Flag<30))
    {
        ui->brightness_Value->setText("50");
    }
    if(backlight_Flag==1 ||(backlight_Flag>=10 &&  backlight_Flag<20))
    {
        ui->brightness_Value->setText("25");
    }
    if(backlight_Flag==0 || backlight_Flag==00 )
    {
        ui->brightness_Value->setText("0");
    }

    //time
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(showTime()));
    timer->start(1000);

    //IPAddress Setting - set max to 9999 to display out of range message
    ui->lineEdit_1->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_2->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_3->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_4->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_5->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_6->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_7->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_8->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_9->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_10->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_11->setValidator(new QIntValidator(0,9999));
    ui->lineEdit_12->setValidator(new QIntValidator(0,9999));


    //IPaddress store location
    ui->lineEdit->setVisible(false);
    ui->lineEdit_dgw->setVisible(false);

    //Save IP Disable before entering value
    ui->btn_SaveIP->setEnabled(false);
    ui->SaveIP_DHCP->setVisible(false);

    //set clock
    QProcess OProcess_HwClock;
    OProcess_HwClock.start("insmod /j1drv/rtc-rx8035.ko");
    OProcess_HwClock.waitForFinished();
    OProcess_HwClock.start("hwclock -f /dev/rtc1 -s");
    OProcess_HwClock.waitForFinished();
    OProcess_HwClock.close();

    //sound setting

    //get and set SoftwareVolume LeftSpeaker
    QProcess process1SVL;
    QProcess process2SVL;

    process1SVL.setStandardOutputProcess(&process2SVL);

    process1SVL.start("amixer sget PCM");
    process2SVL.start("grep \"Left:\"");
    process2SVL.setProcessChannelMode(QProcess::ForwardedChannels);

    bool retval = false;
    QByteArray buffer;
    while ((retval = process2SVL.waitForFinished(-1)))
        buffer.append(process2SVL.readAll());

    //qDebug() << "Volume left" << buffer;
    process1SVL.close();
    process2SVL.close();

    QRegExp rx ("(\\d+)(?:\\s*)(%)");
    QStringList captured;
    int pos = 0;

    while ((pos = rx.indexIn(buffer, pos)) != -1)
    {
        captured << rx.cap(0);
        pos+=rx.matchedLength();
    }

    QString VOLleft=captured[0].remove("%");
    //qDebug() << "vol lef" << VOLleft;
    int Volume_Left=VOLleft.toInt();

    //    get and set SoftwareVolume RightSpeaker
    //    QProcess process1SVR;
    //    QProcess process2SVR;

    //    process1SVR.setStandardOutputProcess(&process2SVR);

    //    process1SVR.start("amixer sget PCM");
    //    process2SVR.start("grep \"Right:\"");
    //    process2SVR.setProcessChannelMode(QProcess::ForwardedChannels);

    //    bool retval2 = false;
    //    QByteArray buffer2;
    //    while ((retval2 = process2SVR.waitForFinished(-1)))
    //    buffer2.append(process2SVR.readAll());

    //    qDebug() << "Volume right" << buffer2;
    //    process1SVR.close();
    //    process2SVR.close();

    //    QRegExp rx2 ("(\\d+)(?:\\s*)(%)");
    //    QStringList captured2;
    //    int pos2 = 0;

    //    while ((pos = rx2.indexIn(buffer2, pos2)) != -1)
    //    {
    //    captured2 << rx2.cap(0);
    //    pos2+=rx.matchedLength();
    //    }

    //    QString VOLRight=captured2[0].remove("%");
    //    int Volume_Right=VOLRight.toInt();

    ui->stackedWidget->setCurrentIndex(0);
    ui->horizontalSlider->setValue(Volume_Left);
    ui->pushButton_VolumeMute->setIconSize(QSize(40, 40));

    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(displayValue()));
    ui->label_34->setText(QString::number(ui->horizontalSlider->value()));


    //get and set SpeakerVolume
    QProcess process1HW;
    QProcess process2HW;

    process1HW.setStandardOutputProcess(&process2HW);

    process1HW.start("amixer sget Digital");
    process2HW.start("grep \"Mono:\"");
    process2HW.setProcessChannelMode(QProcess::ForwardedChannels);

    bool retval3 = false;
    QByteArray buffer3;
    while ((retval3 = process2HW.waitForFinished(-1)))
        buffer3.append(process2HW.readAll());

    //qDebug() << "Volume Speaker" << buffer3;
    process1HW.close();
    process2HW.close();

    QRegExp rx3 ("(\\d+)(?:\\s*)(%)");
    QStringList captured3;
    int pos3 = 0;

    while ((pos3 = rx3.indexIn(buffer3, pos3)) != -1)
    {
        captured3 << rx3.cap(0);
        pos3+=rx3.matchedLength();
    }

    QString VOLHW=captured3[0].remove("%");
    int Volume_hw=VOLHW.toInt();

    ui->horizontalSlider_Speaker->setValue(Volume_hw);

    connect(ui->horizontalSlider_Speaker,SIGNAL(valueChanged(int)),this,SLOT(displayValueHW()));
    ui->label_36->setText(QString::number(ui->horizontalSlider_Speaker->value()));

    QPixmap pix(":/new/prefix1/big-speaker.png");
    ui->label_29->setPixmap(pix);

    QCommonStyle style;
    ui->Btn_testSound->setIcon(style.standardIcon(QStyle::SP_MediaPlay));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showTime()
{
    ui->timeEdit->setVisible(false);
    ui->label_currentTime->setText(QTime::currentTime().toString("hh:mm:ss"));
    ui->label_currentTime->setVisible(true);
    //ui->timeEdit->setTime(QTime::currentTime());
}


void MainWindow::on_pushButton_pressed()
{

}

void MainWindow::on_pushButton_released()
{
    QApplication::quit();
}

void MainWindow::on_Btn_Dark_pressed()
{
    //Read brightness value to feed to brightness flag
    QFile brightness_current("/sys/devices/soc0/backlight/backlight/backlight/brightness");
    brightness_current.open(QIODevice::ReadOnly);
    char level[1];
    brightness_current.read(level,1);
    int backlight_Flag= atoi(level);
    //qDebug() << "current brightness in int " << backlight_Flag;
    brightness_current.close();
    QString bf=QString::number(backlight_Flag);
    ui->brightness_label->setText(bf);
}

void MainWindow::on_Btn_Dark_released()
{
    QString bv= ui->brightness_label->text();
    int backlight_Flag=bv.toInt();
    //FROM QPROCESS
    if(backlight_Flag==7 || backlight_Flag==70)        //70 for atoi conversion issue
    {
        ui->Btn_Bright->setEnabled(true);
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight6.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag--;
        ui->brightness_Value->setText("90");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==6 || backlight_Flag==60)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight5.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        // qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag--;
        ui->brightness_Value->setText("80");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==5 || backlight_Flag==50)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight4.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        // qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag--;
        ui->brightness_Value->setText("70");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==4 || backlight_Flag==40)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight3.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag--;
        ui->brightness_Value->setText("60");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==3 || backlight_Flag==30)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight2.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag--;
        ui->brightness_Value->setText("50");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==2 || backlight_Flag==20)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight1.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag--;
        ui->brightness_Value->setText("25");
        // qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==1 || backlight_Flag==10)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight0.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag--;
        ui->brightness_Value->setText("0");
        //qDebug() <<  backlight_Flag;
        ui->Btn_Dark->setEnabled(false);
    }
}

void MainWindow::on_Btn_Bright_pressed()
{
    //Read brightness value to feed to brightness flag
    QFile brightness_current("/sys/devices/soc0/backlight/backlight/backlight/brightness");
    brightness_current.open(QIODevice::ReadOnly);
    char level[1];
    brightness_current.read(level,1);
    int backlight_Flag= atoi(level);
    //qDebug() << "current brightness in int " << backlight_Flag;
    brightness_current.close();
    QString bf=QString::number(backlight_Flag);
    ui->brightness_label->setText(bf);
}

void MainWindow::on_Btn_Bright_released()
{
    QString bv= ui->brightness_label->text();
    int backlight_Flag=bv.toInt();
    //FROM QPROCESS
    if(backlight_Flag==0 || backlight_Flag==00)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight1.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag++;
        ui->brightness_Value->setText("25");
        //qDebug() <<  backlight_Flag;
        ui->Btn_Dark->setEnabled(true);
    }
    else if(backlight_Flag==1 || backlight_Flag==10)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight2.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag++;
        ui->brightness_Value->setText("50");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==2 || backlight_Flag==20)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight3.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag++;
        ui->brightness_Value->setText("60");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==3 || backlight_Flag==30)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight4.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag++;
        ui->brightness_Value->setText("70");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==4 || backlight_Flag==40)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight5.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag++;
        ui->brightness_Value->setText("80");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==5 || backlight_Flag==50)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight6.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag++;
        ui->brightness_Value->setText("90");
        //qDebug() <<  backlight_Flag;
    }
    else if(backlight_Flag==6 || backlight_Flag==60)
    {
        QProcess OProcess_Backlight;
        QFile file(":/new/prefix1/backlight7.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        //qDebug() << data;
        QStringList Bck_Cmd;
        Bck_Cmd<< "-c" << data;
        OProcess_Backlight.start("/bin/sh",Bck_Cmd);
        OProcess_Backlight.waitForFinished();
        file.close();
        backlight_Flag++;
        ui->brightness_Value->setText("100");
        //qDebug() <<  backlight_Flag;
        ui->Btn_Bright->setEnabled(false);
    }
}

void MainWindow::on_titlebar_Close_pressed()
{

}

void MainWindow::on_titlebar_Close_released()
{
    QApplication::quit();
}



void MainWindow::on_checkBox_Buzzer_stateChanged(int arg1)
{
    if(ui->checkBox_Buzzer->isChecked())
    {
        // qDebug()<< "checkbox is checked";
    }
    else
    {
        //  qDebug()<< "checkbox is un  checked";
    }
}

void MainWindow::mousePressEvent(QMouseEvent *ev)
{
    if(ui->checkBox_Buzzer->isChecked())
    {
        // qDebug()<< "checkbox is checked from touch event";
        QProcess OProcess_Buzzer;
        QFile file(":/new/prefix1/buzzer500.sh");
        file.open(QIODevice::ReadOnly);
        QString data=file.readAll();
        file.close();
        QStringList Buzzer_Cmd;
        Buzzer_Cmd<< "-c" << data;
        OProcess_Buzzer.start("/bin/sh",Buzzer_Cmd);
        OProcess_Buzzer.waitForFinished();
        file.close();
        OProcess_Buzzer.waitForFinished();
    }
}


void MainWindow:: on_checkBox_setTime_stateChanged(int arg1)
{
    if(ui->checkBox_setTime->isChecked())
    {
        timer->stop();
        ui->btn_saveTime->setEnabled(true);
        ui->timeEdit->setVisible(true);
        ui->label_currentTime->setVisible(false);
        ui->timeEdit->setTime(QTime::currentTime());
    }else
    {
        ui->btn_saveTime->setEnabled(false);
        timer=new QTimer(this);
        connect(timer,SIGNAL(timeout()),this,SLOT(showTime()));
        timer->start(1000);
        ui->calendarWidget->showToday();
    }
}

void MainWindow::on_btn_saveTime_released()
{

}

void MainWindow::on_btn_saveTime_pressed()
{
    QMessageBox msgBox_timesave;
    msgBox_timesave.setIcon(QMessageBox::Question);
    msgBox_timesave.setWindowTitle("Save Setting");
    msgBox_timesave.setText("日付と時刻を保存しますか？");
    msgBox_timesave.setStandardButtons(QMessageBox::Yes);
    msgBox_timesave.setButtonText(QMessageBox::Yes, tr("はい"));
    msgBox_timesave.addButton(QMessageBox::No);
    msgBox_timesave.setDefaultButton(QMessageBox::No);
    msgBox_timesave.setButtonText(QMessageBox::No, tr("いいえ"));
    if(msgBox_timesave.exec()== QMessageBox::Yes)
    {
        QTime saved_Time= ui->timeEdit->time();
        QString saved_Date=ui->calendarWidget->selectedDate().toString("yyyy-MM-dd");
        QString saved_DateTime("date --set=\""+saved_Date +" "+saved_Time.toString()+"\"");

        QProcess OProcess_SaveDateTime;
        OProcess_SaveDateTime.start("insmod /j1drv/rtc-rx8035.ko");
        OProcess_SaveDateTime.waitForFinished();
        OProcess_SaveDateTime.start(saved_DateTime);
        OProcess_SaveDateTime.waitForFinished();
        OProcess_SaveDateTime.start("hwclock -f /dev/rtc1 -w");
        OProcess_SaveDateTime.waitForFinished();
    }else
    {
    }
}


void MainWindow::on_radioButton_currentip_clicked()
{
    ui->SaveIP_DHCP->setVisible(true);
    ui->btn_SaveIP->setVisible(false);

    //Display Label when fetching IPAddress from DHCP Server
    ui->label_10->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_11->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_12->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_13->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_15->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_16->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_14->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_19->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_21->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_22->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_23->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");
    ui->label_26->setStyleSheet("border: 1px solid gray;background-color:rgb(186, 189, 182)");

    //Set line feeds visibility to false
    ui->lineEdit_1->setVisible(false);
    ui->lineEdit_2->setVisible(false);
    ui->lineEdit_3->setVisible(false);
    ui->lineEdit_4->setVisible(false);
    ui->lineEdit_5->setVisible(false);
    ui->lineEdit_6->setVisible(false);
    ui->lineEdit_7->setVisible(false);
    ui->lineEdit_8->setVisible(false);
    ui->lineEdit_9->setVisible(false);
    ui->lineEdit_10->setVisible(false);
    ui->lineEdit_11->setVisible(false);
    ui->lineEdit_12->setVisible(false);

    //Set lable visibility to true
    ui->label_10->setVisible(true);
    ui->label_11->setVisible(true);
    ui->label_12->setVisible(true);
    ui->label_13->setVisible(true);
    ui->label_14->setVisible(true);
    ui->label_15->setVisible(true);
    ui->label_16->setVisible(true);
    ui->label_19->setVisible(true);
    ui->label_21->setVisible(true);
    ui->label_22->setVisible(true);
    ui->label_23->setVisible(true);
    ui->label_26->setVisible(true);

    ui->SaveIP_DHCP->setEnabled(true);
}

void MainWindow::on_radioButton_setip_clicked()
{
    ui->SaveIP_DHCP->setVisible(false);
    ui->btn_SaveIP->setVisible(true);
    //Set line feeds visibility to true
    ui->lineEdit_1->setVisible(true);
    ui->lineEdit_2->setVisible(true);
    ui->lineEdit_3->setVisible(true);
    ui->lineEdit_4->setVisible(true);
    ui->lineEdit_5->setVisible(true);
    ui->lineEdit_6->setVisible(true);
    ui->lineEdit_7->setVisible(true);
    ui->lineEdit_8->setVisible(true);
    ui->lineEdit_9->setVisible(true);
    ui->lineEdit_10->setVisible(true);
    ui->lineEdit_11->setVisible(true);
    ui->lineEdit_12->setVisible(true);

    //Set lable visibility to true
    ui->label_10->setVisible(false);
    ui->label_11->setVisible(false);
    ui->label_12->setVisible(false);
    ui->label_13->setVisible(false);
    ui->label_14->setVisible(false);
    ui->label_15->setVisible(false);
    ui->label_16->setVisible(false);
    ui->label_19->setVisible(false);
    ui->label_21->setVisible(false);
    ui->label_22->setVisible(false);
    ui->label_23->setVisible(false);
    ui->label_26->setVisible(false);
}

void MainWindow::on_btn_Touch_pressed()
{

}

//Touch Calibration
void MainWindow::on_btn_Touch_released()
{

    QProcess OProcess_Touch;
    OProcess_Touch.start("ts_calibrate");
    OProcess_Touch.waitForFinished(-1);
    //Touch calibration completition message
    QMessageBox touchmsg;
    touchmsg.setText("タッチパネルのキャリブレーションが完了しました。");
    touchmsg.exec();
    if(OProcess_Touch.NormalExit == true)
    {
        // qDebug() << "ok ";
    }
}

void MainWindow::on_lineEdit_1_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;

    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_1->setText("");
        ui->lineEdit_1->setFocus();
        ok_flag=false;
    }else{
        //move focus to next tab to enter
        ui->lineEdit_1->setText(arg1);
        if((ui->lineEdit_1->text().length())>2)
        {
            ui->lineEdit_2->setFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit2 if previous blocks are empty
        if(arg1.size()>=3 && ui->lineEdit_2->text()=="")
        {
            ui->lineEdit_2->setFocus();
        }
        //set focus to lineedit3 if previous blocks are empty
        else if(arg1.size()>=3 && ui->lineEdit_3->text()=="")
        {
            ui->lineEdit_3->setFocus();
        }
        //set focus to lineedit4 if previous blocks are empty
        else if(arg1.size()>=3 && ui->lineEdit_4->text()=="")
        {
            ui->lineEdit_4->setFocus();
        }
    }
}

void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_2->setText("");
        ui->lineEdit_2->setFocus();
        ok_flag=false;

    }else{
        ui->lineEdit_2->setText(arg1);
        if((ui->lineEdit_2->text().length())>2)
        {
            ui->lineEdit_3->setFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_1->text()=="")
        {
            ui->lineEdit_1->setFocus();
        }
        //set focus to lineedit3 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_3->text()=="")
        {
            ui->lineEdit_3->setFocus();
        }
        //set focus to lineedit4 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_4->text()=="")
        {
            ui->lineEdit_4->setFocus();
        }
    }


}

void MainWindow::on_lineEdit_3_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_3->setText("");
        ui->lineEdit_3->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_3->setText(arg1);
        if((ui->lineEdit_3->text().length())>2)
        {
            ui->lineEdit_4->setFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_1->text()=="")
        {
            ui->lineEdit_1->setFocus();
        }
        //set focus to lineedit2 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_2->text()=="")
        {
            ui->lineEdit_2->setFocus();
        }
        //set focus to lineedit4 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_3->text()=="")
        {
            ui->lineEdit_4->setFocus();
        }
    }

}

void MainWindow::on_lineEdit_4_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_4->setText("");
        ui->lineEdit_4->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_4->setText(arg1);
        if((ui->lineEdit_4->text().length())>2)
        {
            ui->lineEdit_4->clearFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_1->text()=="")
        {
            ui->lineEdit_1->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_2->text()=="")
        {
            ui->lineEdit_2->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_3->text()=="")
        {
            ui->lineEdit_3->setFocus();
        }
    }
}


void MainWindow::on_lineEdit_5_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_5->setText("");
        ui->lineEdit_5->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_5->setText(arg1);
        if((ui->lineEdit_5->text().length())>2)
        {
            ui->lineEdit_6->setFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_6->text()=="")
        {
            ui->lineEdit_6->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_7->text()=="")
        {
            ui->lineEdit_7->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_8->text()=="")
        {
            ui->lineEdit_8->setFocus();
        }
    }
}

void MainWindow::on_lineEdit_7_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_7->setText("");
        ui->lineEdit_7->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_7->setText(arg1);
        if((ui->lineEdit_7->text().length())>2)
        {
            ui->lineEdit_8->setFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_5->text()=="")
        {
            ui->lineEdit_5->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_6->text()=="")
        {
            ui->lineEdit_6->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_8->text()=="")
        {
            ui->lineEdit_8->setFocus();
        }
    }
}

void MainWindow::on_lineEdit_8_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_8->setText("");
        ui->lineEdit_8->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_8->setText(arg1);
        if((ui->lineEdit_8->text().length())>2)
        {
            ui->lineEdit_8->clearFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_5->text()=="")
        {
            ui->lineEdit_5->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_6->text()=="")
        {
            ui->lineEdit_6->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_7->text()=="")
        {
            ui->lineEdit_7->setFocus();
        }
    }
}

void MainWindow::on_lineEdit_9_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_9->setText("");
        ui->lineEdit_9->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_9->setText(arg1);
        if((ui->lineEdit_9->text().length())>2)
        {
            ui->lineEdit_10->setFocus();
        }
    }
    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_10->text()=="")
        {
            ui->lineEdit_10->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_11->text()=="")
        {
            ui->lineEdit_11->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_12->text()=="")
        {
            ui->lineEdit_12->setFocus();
        }
    }
}

void MainWindow::on_lineEdit_10_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_10->setText("");
        ui->lineEdit_10->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_10->setText(arg1);
        if((ui->lineEdit_10->text().length())>2)
        {
            ui->lineEdit_11->setFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_9->text()=="")
        {
            ui->lineEdit_9->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_11->text()=="")
        {
            ui->lineEdit_11->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_12->text()=="")
        {
            ui->lineEdit_12->setFocus();
        }
    }
}

void MainWindow::on_lineEdit_11_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_11->setText("");
        ui->lineEdit_11->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_11->setText(arg1);
        if((ui->lineEdit_11->text().length())>2)
        {
            ui->lineEdit_12->setFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_9->text()=="")
        {
            ui->lineEdit_9->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_10->text()=="")
        {
            ui->lineEdit_10->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_12->text()=="")
        {
            ui->lineEdit_12->setFocus();
        }
    }
}

void MainWindow::on_lineEdit_12_textChanged(const QString &arg1)
{

    //range validation flag
    bool ok_flag=true;
    ui->btn_SaveIP->setEnabled(true);

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_12->setText("");
        ui->lineEdit_12->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_12->setText(arg1);
        if((ui->lineEdit_12->text().length())>2)
        {
            ui->lineEdit_12->clearFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_9->text()=="")
        {
            ui->lineEdit_9->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_10->text()=="")
        {
            ui->lineEdit_10->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_11->text()=="")
        {
            ui->lineEdit_11->setFocus();
        }
    }
}

void MainWindow::on_lineEdit_6_textChanged(const QString &arg1)
{
    //range validation flag
    bool ok_flag=true;

    if((arg1.size()>=3)&& (arg1.toFloat() <0 ||arg1.toInt()>255))
    {
        QMessageBox touchmsg;
        touchmsg.setText("入力値の範囲は0〜255です");
        touchmsg.exec();
        ui->lineEdit_6->setText("");
        ui->lineEdit_6->setFocus();
        ok_flag=false;
    }else{
        ui->lineEdit_6->setText(arg1);
        if((ui->lineEdit_6->text().length())>2)
        {
            ui->lineEdit_7->setFocus();
        }
    }

    if(ok_flag==true)
    {
        //set focus to lineedit1 if previous bloacks are empty
        if(arg1.size()>=3 && ui->lineEdit_5->text()=="")
        {
            ui->lineEdit_5->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_7->text()=="")
        {
            ui->lineEdit_7->setFocus();
        }

        //set focus to lineedit1 if previous bloacks are empty
        else if(arg1.size()>=3 && ui->lineEdit_8->text()=="")
        {
            ui->lineEdit_8->setFocus();
        }
    }
}


void MainWindow::on_btn_SaveIP_pressed()
{

}

void MainWindow::on_btn_SaveIP_released()
{   //Flag to save changes if no errors
    bool OK=true;
    bool ip_empty_flag=false;
    bool mask_empty_flag=false;
    bool gateway_deleted=false;

    //input value of ipaddress
    QString ip1= ui->lineEdit_1->text();
    QString ip2= ui->lineEdit_2->text();
    QString ip3= ui->lineEdit_3->text();
    QString ip4= ui->lineEdit_4->text();

    //  qDebug() << "IP Input" << ip1+"."+ip2+"."+ip3+"."+ip4;
    QString ipaddr =ip1+"."+ip2+"."+ip3+"."+ip4;

    //input value of SUBNET MASK
    QString mask1= ui->lineEdit_5->text();
    QString mask2= ui->lineEdit_6->text();
    QString mask3= ui->lineEdit_7->text();
    QString mask4= ui->lineEdit_8->text();

    //   qDebug() << "mask Input" << mask1+"."+mask2+"."+mask3+"."+mask4;
    QString subnet_mask = mask1+"."+mask2+"."+mask3+"."+mask4;

    //input value of Default Gateway
    QString dgw1= ui->lineEdit_9->text();
    QString dgw2= ui->lineEdit_10->text();
    QString dgw3= ui->lineEdit_11->text();
    QString dgw4= ui->lineEdit_12->text();

    //  qDebug() << "Default gateway" << dgw1+"."+dgw2+"."+dgw3+"."+dgw4;
    QString default_gateway = dgw1+"."+dgw2+"."+dgw3+"."+dgw4;
    bool ipOK=true;
    bool maskOK=true;
    bool GWOK=true;

    //warning if one of the dgw tab is empty
    if(ip1 == "" || ip2 == "" || ip3 == "" ||ip4 == "")
    {
        if(ip1 == "" && ip2 == "" && ip3 == "" && ip4 == "")
        {
            OK=true;    //default g value will be retained
        }
        else{
            QMessageBox dgwEmpty_warning;
            dgwEmpty_warning.setText("入力を確認してください");
            dgwEmpty_warning.exec();
            ipOK=false;
        }
    }

    if(mask1 == "" || mask2 == "" || mask3== "" ||mask4== "")       //test  before else if
    {
        //warning if one of the ipaddress tab is empty
        if(mask1 == "" && mask2 == "" && mask3 == "" && mask4 == "")
        {
            OK=true;    //default g value will be retained
        }
        else{
            QMessageBox ipEmpty_warning;
            ipEmpty_warning.setText("入力を確認してください");
            ipEmpty_warning.exec();
            maskOK=false;
        }
    }

    if(dgw1 == "" || dgw2 == "" || dgw3 == "" ||dgw4 == "")    //test  before else if
    {
        //warning if one of the mask tab is empty
        if(dgw1 == "" && dgw2 == "" && dgw3 == "" && dgw4 == "")
        {
            OK=true;    //default g value will be retained

        }
        else{
            QMessageBox maskEmpty_warning;
            maskEmpty_warning.setText("入力を確認してください");
            maskEmpty_warning.exec();
            GWOK=false;
        }
    }

    if(OK==true && (ipOK==true && maskOK==true && GWOK==true))
    {
        QMessageBox Save_IPSettings;
        Save_IPSettings.setIcon(QMessageBox::Question);
        Save_IPSettings.setWindowTitle("Save IPSetting");
        Save_IPSettings.setText("IP設定をEEPROMに保存しますか");
        Save_IPSettings.setStandardButtons(QMessageBox::Yes);
        Save_IPSettings.setButtonText(QMessageBox::Yes, tr("はい"));
        Save_IPSettings.addButton(QMessageBox::No);
        Save_IPSettings.setDefaultButton(QMessageBox::No);
        Save_IPSettings.setButtonText(QMessageBox::No, tr("いいえ"));
        if(Save_IPSettings.exec()== QMessageBox::Yes)
        {
            QString ip_mask = "ifconfig eth0 "+ipaddr+" netmask "+subnet_mask;
            if((ip1 == "" && ip2 == "" && ip3 == "" && ip4 == "")&&(!(mask1 == "" && mask2 == "" && mask3 == "" && mask4 == "")))
            {
                //FEB1 to add previous ip if it is blank along with subnet mask

                QProcess process1;
                QProcess process2;

                process1.setStandardOutputProcess(&process2);

                process1.start("ifconfig eth0");
                process2.start("grep \"inet addr\"");
                process2.setProcessChannelMode(QProcess::ForwardedChannels);

                bool retval = false;
                QByteArray buffer;
                while ((retval = process2.waitForFinished(-1)))
                    buffer.append(process2.readAll());


               // qDebug() << "Bufferdata" << buffer;
                process1.close();
                process2.close();

      if(!(buffer==""))
       {
                QRegExp rx ("[0-9]{1,3}");
                QStringList captured;
                int pos = 0;

                while ((pos = rx.indexIn(buffer, pos)) != -1)
                {
                    captured << rx.cap(0);
                    pos+=rx.matchedLength();
                }

                //separate ip addres in xxx.xxx.xxx.xxx format
                QString last_iP=captured[0]+"."+captured[1]+"."+captured[2]+"."+captured[3];



                ip_mask="ifconfig eth0 "+last_iP+" netmask "+subnet_mask;

                //DEFAULT gATEWAY retention
                Retain_DefaultGateway();
                //  qDebug() <<ip_mask;
                ip_empty_flag=true;
      }
            }
            if((mask1 == "" && mask2 == "" && mask3 == "" && mask4 == "")&&(!(ip1 == "" && ip2 == "" && ip3 == "" && ip4 == "")))
            {
                QProcess process1;
                QProcess process2;

                process1.setStandardOutputProcess(&process2);

                process1.start("ifconfig eth0");
                process2.start("grep \"inet addr\"");
                process2.setProcessChannelMode(QProcess::ForwardedChannels);

                bool retval = false;
                QByteArray buffer;
                while ((retval = process2.waitForFinished(-1)))
                    buffer.append(process2.readAll());

                // qDebug() << "Bufferdata" << buffer;
                process1.close();
                process2.close();


     if(!(buffer==""))
        {
                QRegExp rx ("[0-9]{1,3}");
                QStringList captured;
                int pos = 0;

                while ((pos = rx.indexIn(buffer, pos)) != -1)
                {
                    captured << rx.cap(0);
                    pos+=rx.matchedLength();
                }

                //separate ip addres in xxx.xxx.xxx.xxx format
                QString last_mask=captured[8]+"."+captured[9]+"."+captured[10]+"."+captured[11];


                ip_mask="ifconfig eth0 "+ipaddr+" netmask "+last_mask;

                //DEFAULT gATEWAY retention
                Retain_DefaultGateway();

                //  ip_mask="ifconfig eth0 "+ipaddr;
                //  qDebug() <<ip_mask;
                mask_empty_flag=true;
     }
            }

            //          if(!((ip_empty_flag==true) &&  (mask_empty_flag ==true)))
            //         {
            //  qDebug() <<"ip and mask is executed";
            //Save IPaddress and subnet mask
            //         QProcess Save_IP;
            //         Save_IP.start(ip_mask);
            //         Save_IP.waitForFinished(-1);
            //         Save_IP.close();

            //retainDGW
            //         Retain_DefaultGateway();
            //          }

            //both not entered retain last value  only for ipmask  feb5

            if((mask1 == "" && mask2 == "" && mask3 == "" && mask4 == "")&&(ip1 == "" && ip2 == "" && ip3 == "" && ip4 == ""))
            {
                QProcess process1;
                QProcess process2;

                process1.setStandardOutputProcess(&process2);

                process1.start("ifconfig eth0");
                process2.start("grep \"inet addr\"");
                process2.setProcessChannelMode(QProcess::ForwardedChannels);

                bool retval = false;
                QByteArray buffer;
                while ((retval = process2.waitForFinished(-1)))
                    buffer.append(process2.readAll());

                // qDebug() << "Bufferdata" << buffer;
                process1.close();
                process2.close();


   if(!(buffer==""))
      {
                QRegExp rx ("[0-9]{1,3}");
                QStringList captured;
                int pos = 0;

                while ((pos = rx.indexIn(buffer, pos)) != -1)
                {
                    captured << rx.cap(0);
                    pos+=rx.matchedLength();
                }

                //separate ip addres in xxx.xxx.xxx.xxx format
                QString last_ipaddr=captured[0]+"."+captured[1]+"."+captured[2]+"."+captured[3];
                QString last_submask=captured[8]+"."+captured[9]+"."+captured[10]+"."+captured[11];


                ip_mask="ifconfig eth0 "+last_ipaddr+" netmask "+last_submask;
   }

            }

            //  qDebug() <<"only Default, ip and mask not executed";
            //define Defalt gateway
            //          QString DefaultGW;
            if((dgw1=="0" && dgw2=="0" && dgw3=="0" && dgw4=="0") ||(dgw1=="00" && dgw2=="00" && dgw3=="00" && dgw4=="00") || (dgw1=="000" && dgw2=="000" && dgw3=="000" && dgw4=="000"))
            {
                QMessageBox DgwDel_msg;
                DgwDel_msg.setText("デフォルトゲートウェイを削除しますか？");
                DgwDel_msg.setStandardButtons(QMessageBox::Yes);
                DgwDel_msg.setButtonText(QMessageBox::Yes, tr("はい"));
                DgwDel_msg.addButton(QMessageBox::No);
                DgwDel_msg.setDefaultButton(QMessageBox::No);
                DgwDel_msg.setButtonText(QMessageBox::No, tr("いいえ"));
                if(DgwDel_msg.exec()== QMessageBox::Yes)
                {
                    QProcess dgw_delete;
                    dgw_delete.start("ip r del default");
                    dgw_delete.waitForFinished(-1);
                    dgw_delete.close();

                    QString del="dd if=/dev/zero of=/opt/J1Tool/scripts/eeprom_DefGateway bs=1 count=80";
                    QProcess deldgw;
                    deldgw.start(del);
                    deldgw.waitForFinished(-1);
                    deldgw.close();
                    gateway_deleted=true;

                }else{}

            }

            //Save Default Gateway
            if(!(dgw1 == "" && dgw2 == "" && dgw3 == "" && dgw4 == ""))
            {
                // DefaultGW = "ip r change default via "+default_gateway;
                DefaultGW = "route add default gw "+default_gateway;

                QProcess Default_Gateway;
                Default_Gateway.start("ip r del default ");
                Default_Gateway.waitForFinished();
                Default_Gateway.start(DefaultGW);
                Default_Gateway.waitForFinished();
                QString err=Default_Gateway.readAllStandardError();
                Default_Gateway.close();
            }
            else{

                Retain_DefaultGateway();

            }

            //saved Message
            QMessageBox SavedIPMessage;
            SavedIPMessage.setText("IP設定を保存しています.....");
            SavedIPMessage.setStandardButtons(QMessageBox::Ok);
            SavedIPMessage.button(QMessageBox::Ok)->animateClick(250);
            if(SavedIPMessage.exec()== QMessageBox::Ok)
            {
                //workaround solution to Save IPaddress and subnet mask command needs to be executed twice to display as current IP
                QProcess Save_IP;
                Save_IP.start(ip_mask);
                Save_IP.waitForFinished();
                Save_IP.close();


                if(!((ip_empty_flag==true) &&  (mask_empty_flag ==true)))
                {
                    //  qDebug() <<"ip and mask is executed";
                    //Save IPaddress and subnet mask

                    //retainDGW
                    Retain_DefaultGateway();

                    QProcess Save_IP;
                    Save_IP.start(ip_mask);
                    Save_IP.waitForFinished(-1);
                    Save_IP.close();

                    //to retain Default gateway
                    Defaultgateway_Write();

                }

                if((ip1 == "" && ip2 == "" && ip3 == "" && ip4 == "")&&(!(mask1 == "" && mask2 == "" && mask3 == "" && mask4 == "")))
                {
                    Defaultgateway_Write();
                }

                if((mask1 == "" && mask2 == "" && mask3 == "" && mask4 == "")&&(!(ip1 == "" && ip2 == "" && ip3 == "" && ip4 == "")))
                {
                    Defaultgateway_Write();
                }

                //Clear line edit once saved and disable save button
                ui->lineEdit_1->setText("");
                ui->lineEdit_2->setText("");
                ui->lineEdit_3->setText("");
                ui->lineEdit_4->setText("");
                ui->lineEdit_5->setText("");
                ui->lineEdit_6->setText("");
                ui->lineEdit_7->setText("");
                ui->lineEdit_8->setText("");
                ui->lineEdit_9->setText("");
                ui->lineEdit_10->setText("");
                ui->lineEdit_11->setText("");
                ui->lineEdit_12->setText("");
                ui->btn_SaveIP->setEnabled(false);


                //EEPROM------------------------------EEPROM
                //make a file  to write to EEPROM


                QFile filePath_ip("/opt/J1Tool/scripts/eeprom_ip");
                filePath_ip.open(QIODevice::WriteOnly);
                QTextStream in(&filePath_ip);
                in<<ip_mask;
                filePath_ip.close();

                //make link file from application
                QString link=";/etc/profile.d/./eeprom_dgw.sh";
                QFile filePath_link("/opt/J1Tool/scripts/link_IptoDgw.sh");
                filePath_link.open(QIODevice::WriteOnly);
                QTextStream input(&filePath_link);
                input<<link;
                filePath_link.close();

                //clear eep script
                QString cmd1="dd if=/dev/zero of=/etc/profile.d/eeprom_ip.sh bs=1 count=80  ";
                QProcess cleareep;
                cleareep.start(cmd1);
                cleareep.waitForFinished(-1);
                cleareep.close();


                //clear eeprom
                QString cmd_eep_clr="dd if=/dev/zero of=/sys/bus/spi/devices/spi0.0/eeprom bs=1 count=80 seek=80  ";
                QProcess cleareeprom;
                cleareeprom.start(cmd_eep_clr);
                cleareeprom.waitForFinished(-1);
                cleareeprom.close();



                //FILE TO eeprom write
                QString cmd2="dd if=/opt/J1Tool/scripts/eeprom_ip of=/sys/bus/spi/devices/spi0.0/eeprom bs=1 count=80 seek=80";
                QProcess eeprom_ip;
                eeprom_ip.start(cmd2);
                eeprom_ip.waitForFinished(-1);
                eeprom_ip.close();


                //delete previous script


                QString cmd3="dd if=/dev/zero of=/etc/profile.d/eeprom_ip.sh bs=1 count=80";
                QProcess clear;
                clear.start(cmd3);
                clear.waitForFinished(-1);
                clear.close();



                //eeprom to profile.d to retain changes on reboot
                int countIP= ip_mask.size();
                QString cmd4="dd if=/sys/bus/spi/devices/spi0.0/eeprom of=/etc/profile.d/eeprom_ip.sh bs=1 skip=80 count="+QString::number(countIP);
                QProcess profile_ip;
                profile_ip.start(cmd4);
                profile_ip.waitForFinished(-1);
                profile_ip.close();


                //Default gateway to EEPROM   (THIS IS COMMENTED OUT BECAUSE ON REBOOT DEFAULT GATEWAY CANNOT FIND RTLLINK, so always set from tool


                if(gateway_deleted==false)
                {
                    QFile filePath_DGW("/opt/J1Tool/scripts/eeprom_DefGateway");
                    filePath_DGW.open(QIODevice::WriteOnly);
                    QTextStream inputgw(&filePath_DGW);
                    inputgw<< DefaultGW ;
                    filePath_DGW.close();

                    QString cmd9="dd if=/opt/J1Tool/scripts/link_IptoDgw.sh of=/etc/profile.d/eeprom_ip.sh bs=1 count=31 seek="+QString::number(countIP);
                    QProcess linkip_dgw;
                    linkip_dgw.start(cmd9);
                    linkip_dgw.waitForFinished(-1);
                    linkip_dgw.close();

                }

                //clear dgw
                QString cmd5="dd if=/dev/zero of=/etc/profile.d/eeprom_dgw.sh bs=1 count=80  ";
                QProcess cleardgw;
                cleardgw.start(cmd5);
                cleardgw.waitForFinished(-1);
                cleardgw.close();

                //FILE TO eeprom write
                QString cmd6="dd if=/opt/J1Tool/scripts/eeprom_DefGateway of=/sys/bus/spi/devices/spi0.0/eeprom bs=1 count=80 seek=160";
                QProcess eeprom_defaultG;
                eeprom_defaultG.start(cmd6);
                eeprom_defaultG.waitForFinished(-1);
                eeprom_defaultG.close();

                //delete previous script

                if(gateway_deleted==true)
                {
                    QString cmd7="dd if=/dev/zero of=/etc/profile.d/eeprom_dgw.sh bs=1 count=80  ";

                    QProcess cleardg;
                    cleardg.start(cmd7);
                    cleardg.waitForFinished(-1);
                    cleardg.close();
                }

                if(gateway_deleted==false)
                {
                    countDGW= DefaultGW.size();
                    //eeprom to profile.d to retain changes on reboot
                    QString cmd8="dd if=/sys/bus/spi/devices/spi0.0/eeprom of=/etc/profile.d/eeprom_dgw.sh bs=1 skip=160 count="+QString::number(countDGW);
                    //qDebug()<<"Count"<<cmd8;
                    QProcess profile_dgw;
                    profile_dgw.start(cmd8);
                    profile_dgw.waitForFinished(-1);
                    profile_dgw.close();
                }
            }


            //after animation if error exists show message
            if(Save_IPSettings.close())
            {
                //Default gateway setting error log

                if(!(err.isEmpty()))
                {    //get rid of unwanted errors
                    QRegExp rx("\\b(rather|than)\\b");
                    int pos=rx.indexIn(err);
                    if(pos == -1)
                    {//wanted error
                        QMessageBox gateway_error;
                        gateway_error.setText("デフォルトゲートウェイを設定できませんでした：\n"+err);
                        gateway_error.exec();
                    }
                }
            }else
            {}
        }
    }
}


void MainWindow::on_btn_SaveBrightness_pressed()
{

}

void MainWindow::on_btn_SaveBrightness_released()
{
    //make a file  to write to EEPROM
    int value_to_save=ui->brightness_Value->text().toInt();
    switch(value_to_save)
    {
    case 100:  value_to_save=7;
        break;

    case 90:   value_to_save=6;
        break;

    case 80:   value_to_save=5;
        break;

    case 70:   value_to_save=4;
        break;

    case 60:   value_to_save=3;
        break;

    case 50:   value_to_save=2;
        break;

    case 25:   value_to_save=1;
        break;

    case 0:    value_to_save=0;
        break;
    }

    QFile filePath_backlight("/opt/J1Tool/scripts/eeprom_backlight");
    filePath_backlight.open(QIODevice::WriteOnly);
    QTextStream in(&filePath_backlight);
    in<<"echo "+QString::number(value_to_save)+" > "+"/sys/devices/soc0/backlight/backlight/backlight/brightness"+";"+"\n";
    filePath_backlight.close();


    QMessageBox Bck_eeprom;
    Bck_eeprom.setIcon(QMessageBox::Question);
    Bck_eeprom.setWindowTitle("Save Settings");
    Bck_eeprom.setText("バックライト設定をEEPROMに保存しますか？");
    Bck_eeprom.setStandardButtons(QMessageBox::Yes);
    Bck_eeprom.setButtonText(QMessageBox::Yes, tr("はい"));
    Bck_eeprom.addButton(QMessageBox::No);
    Bck_eeprom.setDefaultButton(QMessageBox::No);
    Bck_eeprom.setButtonText(QMessageBox::No, tr("いいえ"));
    if(Bck_eeprom.exec()== QMessageBox::Yes)
    {
        //FILE TO eeprom write
        QString cmd1="dd if=/opt/J1Tool/scripts/eeprom_backlight of=/sys/bus/spi/devices/spi0.0/eeprom bs=1 count=80";
        QProcess eeprom_backlight;
        eeprom_backlight.start(cmd1);
        eeprom_backlight.waitForFinished();
        eeprom_backlight.close();

        //eeprom to profile.d to retain changes on reboot
        QString cmd2="dd if=/sys/bus/spi/devices/spi0.0/eeprom of=/etc/profile.d/eeprom_backlight.sh bs=1 count=80";
        QProcess profile_backlight;
        profile_backlight.start(cmd2);
        profile_backlight.waitForFinished();
        profile_backlight.close();

    }else
    {}
}

void MainWindow::on_btn_FactoryReset_pressed()
{
    //eeprom to profile.d to retain changes on reboot
    QString cmd5="echo \"echo 6 > /sys/devices/soc0/backlight/backlight/backlight/brightness\" > /etc/profile.d/eeprom.sh";
    QProcess brght_fr;
    brght_fr.start(cmd5);
    brght_fr.waitForFinished();
    brght_fr.close();
}

void MainWindow::on_btn_FactoryReset_released()
{
    QMessageBox factory_reset;
    factory_reset.setIcon(QMessageBox::Question);
    factory_reset.setWindowTitle("Factory Reset");
    factory_reset.setText("初期化してよろしですか？");
    factory_reset.setStandardButtons(QMessageBox::Yes);
    factory_reset.setButtonText(QMessageBox::Yes, tr("はい"));
    factory_reset.addButton(QMessageBox::No);
    factory_reset.setDefaultButton(QMessageBox::No);
    factory_reset.setButtonText(QMessageBox::No, tr("いいえ"));
    if(factory_reset.exec()== QMessageBox::Yes)
    {
        //FILE TO eeprom write
        QString cmd3="dd if=/dev/zero of=/sys/bus/spi/devices/spi0.0/eeprom bs=1 count=400";
        QProcess eeprom_fr;
        eeprom_fr.start(cmd3);
        eeprom_fr.waitForFinished();
        eeprom_fr.close();

        //eeprom to profile.d to retain changes on reboot
        QString cmdrs4="dd if=/sys/bus/spi/devices/spi0.0/eeprom of=/etc/profile.d/eeprom_backlight.sh bs=1 count=80";
        QString cmdrs5="dd if=/sys/bus/spi/devices/spi0.0/eeprom of=/etc/profile.d/eeprom_ip.sh bs=1 count=80";
        QString cmdrs6="dd if=/sys/bus/spi/devices/spi0.0/eeprom of=/etc/profile.d/eeprom_dgw.sh bs=1 count="+QString::number(countDGW);
        QString cmdrs7="dd if=/sys/bus/spi/devices/spi0.0/eeprom of=/etc/profile.d/eeprom_volume.sh bs=1 count="+QString::number(Volumelevel.count());
        QProcess profile_fr;
        profile_fr.start(cmdrs4);
        profile_fr.waitForFinished(-1);
        profile_fr.start(cmdrs5);
        profile_fr.waitForFinished(-1);
        profile_fr.start(cmdrs6);
        profile_fr.waitForFinished(-1);
        profile_fr.start(cmdrs7);
        profile_fr.waitForFinished(-1);
        profile_fr.close();

        QMessageBox factory_rst_confirm;
        factory_rst_confirm.setText("レジストリが初期化しました。再起動してください。");
        if(factory_rst_confirm.exec()==QMessageBox::Ok)
        {
            factory_reset.close();
        }
    }
    else
    {}
}

void MainWindow::on_SaveIP_DHCP_pressed()
{

}

void MainWindow::on_SaveIP_DHCP_released()
{
    QMessageBox dhcp_msg;
    dhcp_msg.setText("設定を保存しますか？");
    dhcp_msg.setStandardButtons(QMessageBox::Yes);
    dhcp_msg.setButtonText(QMessageBox::Yes, tr("はい"));
    dhcp_msg.addButton(QMessageBox::No);
    dhcp_msg.setDefaultButton(QMessageBox::No);
    dhcp_msg.setButtonText(QMessageBox::No, tr("いいえ"));
    if(dhcp_msg.exec()== QMessageBox::Yes)
    {
        QProcess dhcp;
        dhcp.start("udhcpc -f -n -q -t5 -i eth0");
        dhcp.waitForFinished();
        dhcp.close();
    }
}

void MainWindow::Retain_DefaultGateway()
{

    //Process to get DGW
    QProcess process1dg;
    QProcess process2dg;

    process1dg.setStandardOutputProcess(&process2dg);

    process1dg.start("ip r");
    process2dg.start("grep \"default\"");
    process2dg.setProcessChannelMode(QProcess::ForwardedChannels);

    bool retval2 = false;
    QByteArray buffer2;
    while ((retval2 = process2dg.waitForFinished()))
        buffer2.append(process2dg.readAll());



    if(!(buffer2==""))
    {
        QRegExp rx ("[0-9]{1,3}");
        QStringList captured;

        int pos = 0;

        while ((pos = rx.indexIn(buffer2, pos)) != -1)
        {
            captured << rx.cap(0);
            pos+=rx.matchedLength();
        }



        // Separate Default Gateway in xxx.xxx.xxx.xxx format
        if(captured.length()==1)
        {

        }else{
            gw1=captured[0];
            gw2=captured[1];
            gw3=captured[2];
            gw4=captured[3];
        }

        process1dg.close();
        process2dg.close();

        def_gateway = "route add default gw "+gw1+"."+gw2+"."+gw3+"."+gw4;
        DefaultGW= "route add default gw "+gw1+"."+gw2+"."+gw3+"."+gw4;

        QProcess Def_Gateway;
        Def_Gateway.start("ip r del default ");
        Def_Gateway.waitForFinished(-1);
        Def_Gateway.start(def_gateway);
        Def_Gateway.waitForFinished(-1);
        Def_Gateway.close();
    }

}


void MainWindow::Defaultgateway_Write()
{
    QProcess Def_GatewayRetain;
    Def_GatewayRetain.start(DefaultGW);
    Def_GatewayRetain.waitForFinished(-1);
    Def_GatewayRetain.close();
}



//Audio Setting
void MainWindow::on_Btn_testSound_clicked()
{

    QProcess play;
    play.startDetached("aplay /opt/StarWars10.wav");
    play.waitForFinished(-1);
    play.close();
}


void MainWindow::on_pushButton_VolumeMute_clicked()
{

    if(mutecontrol==0)
    {
        ui->pushButton_VolumeMute->setIcon(QIcon(":/new/prefix1/V0.png"));
        QProcess audioMute;
        QString Mute="amixer set PCM 0%";
        audioMute.start(Mute);
        audioMute.waitForFinished(-1);
        mutecontrol++;

    }
    else
    {
        int prevValue=ui->horizontalSlider->value();
        on_horizontalSlider_valueChanged(prevValue);
        mutecontrol--;
    }
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{

    QProcess audio;
    QString val=QString::number(value);
    QString volume="amixer set PCM "+val+"%";
    audio.start(volume);
    audio.waitForFinished(-1);
    audio.close();

    if(value == 0)
    {
        ui->pushButton_VolumeMute->setIcon(QIcon(":/new/prefix1/V0.png"));

    }
    else if(value >= 0 && value < 30)
    {
        ui->pushButton_VolumeMute->setIcon(QIcon(":/new/prefix1/V1.png"));

    }
    else if (value >= 30 && value < 60){
        ui->pushButton_VolumeMute->setIcon(QIcon(":/new/prefix1/V2.png"));
    }
    else
    {
        ui->pushButton_VolumeMute->setIcon(QIcon(":/new/prefix1/V3.png"));

    }
}


void MainWindow::displayValue()
{
    ui->label_34->setText(QString::number(ui->horizontalSlider->value()));


}

void MainWindow::on_btn_saveVolume_pressed()
{

}

void MainWindow::on_btn_saveVolume_released()
{

    QString VolSys= QString::number(ui->horizontalSlider->value())+"%";
    QString VolHw= QString::number(ui->horizontalSlider_Speaker->value())+"%";
    Volumelevel="amixer set PCM "+VolSys+" -q;amixer set Digital "+VolHw+" -q";
    QFile filePath_volume("/opt/J1Tool/scripts/eeprom_volume");
    filePath_volume.open(QIODevice::WriteOnly);
    QTextStream in(&filePath_volume);
    in<<Volumelevel;
    filePath_volume.close();


    QMessageBox Vol_eeprom;
    Vol_eeprom.setIcon(QMessageBox::Question);
    Vol_eeprom.setWindowTitle("Save Settings");
    Vol_eeprom.setText("サウンド設定をEEPROMに保存しますか？");
    Vol_eeprom.setStandardButtons(QMessageBox::Yes);
    Vol_eeprom.setButtonText(QMessageBox::Yes, tr("はい"));
    Vol_eeprom.addButton(QMessageBox::No);
    Vol_eeprom.setDefaultButton(QMessageBox::No);
    Vol_eeprom.setButtonText(QMessageBox::No, tr("いいえ"));
    if(Vol_eeprom.exec()== QMessageBox::Yes)
    {
        //clear eep script
        QString cmd1="dd if=/dev/zero of=/etc/profile.d/eeprom_volume.sh bs=1 count=80  ";
        QProcess cleareep;
        cleareep.start(cmd1);
        cleareep.waitForFinished(-1);
        cleareep.close();


        //clear eeprom
        QString cmd_eep_clr="dd if=/dev/zero of=/sys/bus/spi/devices/spi0.0/eeprom bs=1 count=80 seek=320  ";
        QProcess cleareeprom;
        cleareeprom.start(cmd_eep_clr);
        cleareeprom.waitForFinished(-1);
        cleareeprom.close();





        //FILE TO eeprom write
        QString cmd2="dd if=/opt/J1Tool/scripts/eeprom_volume of=/sys/bus/spi/devices/spi0.0/eeprom bs=1 count=80 seek=320";
        QProcess eeprom_vol;
        eeprom_vol.start(cmd2);
        eeprom_vol.waitForFinished();
        eeprom_vol.close();

        //eeprom to profile.d to retain changes on reboot
        QString cmd3="dd if=/sys/bus/spi/devices/spi0.0/eeprom of=/etc/profile.d/eeprom_volume.sh bs=1  skip=320 count="+QString::number(Volumelevel.count()+5);
        QProcess profile_vol;
        profile_vol.start(cmd3);
        profile_vol.waitForFinished();
        profile_vol.close();

    }else
    {}
}

void MainWindow::on_horizontalSlider_Speaker_valueChanged(int value)
{

    QString valSpeaker=QString::number(value);
    QProcess audioSpeaker;
    QString volumeSpeaker="amixer set Digital "+valSpeaker+"%";
    audioSpeaker.start(volumeSpeaker);
    audioSpeaker.waitForFinished(-1);
    audioSpeaker.close();


}

void MainWindow::displayValueHW()
{
    ui->label_36->setText(QString::number(ui->horizontalSlider_Speaker->value()));
}

void MainWindow::on_pushButton_2_pressed()
{

}

void MainWindow::on_pushButton_2_released()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_4_pressed()
{

}

void MainWindow::on_pushButton_4_released()
{
    ui->stackedWidget->setCurrentIndex(0);
}
