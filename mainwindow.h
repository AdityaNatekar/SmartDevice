#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QProcess>
#include<QMouseEvent>
#include<QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QProcess *OProcess_Backlight;

    int backlight_Flag;

    void brightness_percentage_display(int);

    void Retain_DefaultGateway();

    void Defaultgateway_Write();

private slots:
    void on_pushButton_pressed();

    void on_pushButton_released();

    void on_Btn_Dark_pressed();

    void on_Btn_Dark_released();

    void on_Btn_Bright_pressed();

    void on_Btn_Bright_released();

    void on_titlebar_Close_pressed();

    void on_titlebar_Close_released();

    void on_checkBox_Buzzer_stateChanged(int arg1);

    void showTime();



    void on_checkBox_setTime_stateChanged(int arg1);

    void on_btn_saveTime_released();

    void on_btn_saveTime_pressed();



    void on_radioButton_currentip_clicked();

    void on_radioButton_setip_clicked();

    void on_btn_Touch_pressed();

    void on_btn_Touch_released();

  void on_lineEdit_1_textChanged(const QString &arg1);



  void on_lineEdit_2_textChanged(const QString &arg1);

  void on_lineEdit_3_textChanged(const QString &arg1);

  void on_lineEdit_4_textChanged(const QString &arg1);

  void on_lineEdit_5_textChanged(const QString &arg1);

  void on_lineEdit_7_textChanged(const QString &arg1);

  void on_lineEdit_8_textChanged(const QString &arg1);

  void on_lineEdit_9_textChanged(const QString &arg1);

  void on_lineEdit_10_textChanged(const QString &arg1);

  void on_lineEdit_11_textChanged(const QString &arg1);

  void on_lineEdit_12_textChanged(const QString &arg1);

  void on_lineEdit_6_textChanged(const QString &arg1);

  //void on_lineEdit_textChanged(const QString &arg1);



  void on_btn_SaveIP_pressed();

  void on_btn_SaveIP_released();

 // void on_lineEdit_dgw_textChanged(const QString &arg1);

  void on_btn_SaveBrightness_pressed();

  void on_btn_SaveBrightness_released();

  void on_btn_FactoryReset_pressed();

  void on_btn_FactoryReset_released();

  void on_SaveIP_DHCP_pressed();

  void on_SaveIP_DHCP_released();

  void on_Btn_testSound_clicked();


  void on_pushButton_VolumeMute_clicked();

  void on_horizontalSlider_valueChanged(int value);

  void on_btn_saveVolume_pressed();

  void on_btn_saveVolume_released();

  void on_horizontalSlider_Speaker_valueChanged(int value);

  void on_pushButton_2_pressed();

  void on_pushButton_2_released();

  void on_pushButton_4_pressed();

  void on_pushButton_4_released();

public slots:
    void mousePressEvent(QMouseEvent *ev);
   void displayValue();
      void displayValueHW();


private:
    Ui::MainWindow *ui;

    QTimer *timer;


    QString err;

    QString  def_gateway;   //to not let dgw delete in ip add
    int countDGW;
    QString gw1,gw2,gw3,gw4;

    QString DefaultGW;

    QString style_0_60      = "QProgressBar::chunk{ background-color: qlineargradient(spread:pad, x1:1, y1:0.506, x2:0, y2:0.517, stop:0.0724638 rgba(0, 166, 0, 255), stop:1 rgba(255, 255, 255, 255)); }";
    QString style_60_80     = "QProgressBar::chunk{ background-color: qlineargradient(spread:pad, x1:1, y1:0.506, x2:0, y2:0.517, stop:0.0724638 rgba(0, 72, 255, 255), stop:1 rgba(255, 255, 255, 255)); }";
    QString style_80_100    = "QProgressBar::chunk{ background-color: qlineargradient(spread:pad, x1:1, y1:0.506, x2:0, y2:0.517, stop:0.0724638 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255)); }";

float volflag;
float volLeft;
float volRight;
float BalanceConstant=1.000;

int mutecontrol=0;   //mute control

 QString Volumelevel;


};
#endif // MAINWINDOW_H
