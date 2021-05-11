#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    struct Settings {
        QString name;                           //串口号
        qint32 baudRate;                        //波特率
        QString stringBaudRate;                 //波特率字符串
        QSerialPort::DataBits dataBits;         //数据位
        QString stringDataBits;                 //数据位字符串
        QSerialPort::Parity parity;             //校验位
        QString stringParity;                   //校验位字符串
        QSerialPort::StopBits stopBits;         //停止位
        QString stringStopBits;                 //停止位字符串
        QSerialPort::FlowControl flowControl;   //流控制
        QString stringFlowControl;              //流控制字符串
    };


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void help();                    //显示帮助
    void showPortInfo(int index);   //显示串口信息
    void openSerial();              //打开串口
    void closeSerial();             //关闭串口
    void updateSettings();          //更新串口设置
    void receiveData();             //接收信息

    void on_sendBtn_clicked();      //按键按下

private:
    void initActionConnections();           //设置Action的信号槽连接
    void initSerials();                     //初始化串口信息
    void initParams();                      //初始化串口连接参数设置
    void showStatus(const QString message); //状态栏--显示串口连接状态
    void updateBoxStatus(bool status);      //决定参数是否可进行设置

private:
    Ui::MainWindow *ui;

    QLabel *m_status;           //状态栏
    QLabel *m_sendStatus;       //发送结果
    QSerialPort *m_serial;      //保存当前串口
    Settings m_currentSettings; //当前串口设置
};
#endif // MAINWINDOW_H
