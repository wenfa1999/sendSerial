#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include <QDebug>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_status(new QLabel)
    , m_sendStatus(new QLabel)
    , m_serial(new QSerialPort(this))
{
    ui->setupUi(this);

    this->setWindowTitle("串口接发工具");

    initActionConnections();

    initSerials();

    initParams();

    showPortInfo(ui->serialsBox->currentIndex());

    ui->textEdit->setEnabled(false);
    ui->connetcBtn->setEnabled(true);
    ui->actionConnect->setEnabled(true);
    ui->disconnectBtn->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);
    ui->sendBtn->setEnabled(false);
    ui->clearTextBtn->setEnabled(false);

    ui->statusbar->addWidget(m_status);
    ui->statusbar->addWidget(m_sendStatus);

    showStatus("等待连接...");

    connect(ui->serialsBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &this->showPortInfo);







    /*
     * 测试
    */
//    ui->receiveBrowser->setText("慈母手中线，游子身上衣。"
//                                "临行密密缝，意恐迟迟归。"
//                                "谁言寸草心，报得三春晖。");

    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::receiveData);
//    connect(m_serial, QOverload<Str>, this, &MainWindow::receiveData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//设置Action的信号槽连接
void MainWindow::initActionConnections()
{
    // 关于Qt
    connect(ui->actionQt, &QAction::triggered, qApp,&QApplication::aboutQt);
    // 使用帮助
    connect(ui->actionHelp, &QAction::triggered, this, &MainWindow::help);
    // 打开串口
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerial);
    // 关闭串口
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerial);
}

//初始化串口信息
void MainWindow::initSerials()
{
    ui->serialsBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << description
             << manufacturer
             << serialNumber
             << info.systemLocation();

        ui->serialsBox->addItem(list.first(), list);
    }
}

//初始串口连接参数设置
void MainWindow::initParams()
{
    // 波特率
    ui->baudRateBox->addItem(QString("1200"), QSerialPort::Baud1200);
    ui->baudRateBox->addItem(QString("2400"), QSerialPort::Baud2400);
    ui->baudRateBox->addItem(QString("4800"), QSerialPort::Baud4800);
    ui->baudRateBox->addItem(QString("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QString("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QString("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QString("57600"), QSerialPort::Baud57600);
    ui->baudRateBox->addItem(QString("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->setCurrentIndex(3);

    //数据位
    ui->dataBitsBox->addItem("5",QSerialPort::Data5);
    ui->dataBitsBox->addItem("6",QSerialPort::Data6);
    ui->dataBitsBox->addItem("7",QSerialPort::Data7);
    ui->dataBitsBox->addItem("8",QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    //校验方式
    ui->parityBox->addItem(QString("无"), QSerialPort::NoParity);
    ui->parityBox->addItem(QString("奇校验"), QSerialPort::OddParity);
    ui->parityBox->addItem(QString("偶校验"), QSerialPort::EvenParity);
    ui->parityBox->addItem(QString("空格"), QSerialPort::SpaceParity);
    ui->parityBox->addItem(QString("标记"), QSerialPort::MarkParity);
    ui->parityBox->setCurrentIndex(0);

    //停止位
    ui->stopBitsBox->addItem(QString("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    //仅在Windows下存在1.5的停止位
    ui->stopBitsBox->addItem(QString("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QString("2"), QSerialPort::TwoStop);
    ui->stopBitsBox->setCurrentIndex(0);

    //串口控制流
    ui->flowControlBox->addItem(QString("无"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(QString("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(QString("XON/XOFF"), QSerialPort::SoftwareControl);
    ui->flowControlBox->setCurrentIndex(0);

}

//状态栏--显示串口连接状态
void MainWindow::showStatus(const QString message)
{
    m_status->setText(message);
}

//决定参数是否可进行设置
void MainWindow::updateBoxStatus(bool status)
{
    ui->serialsBox->setEnabled(status);
    ui->baudRateBox->setEnabled(status);
    ui->dataBitsBox->setEnabled(status);
    ui->parityBox->setEnabled(status);
    ui->stopBitsBox->setEnabled(status);
    ui->flowControlBox->setEnabled(status);
}

//帮助文档
void MainWindow::help()
{
    QMessageBox::information(this, "使用帮助",
                             "选择串口号，设置参数后点击“连接串口”，"
                             "打开成功后在输入区输入，·点击“发送”按钮进行发送。");
}

//显示串口相关信息
void MainWindow::showPortInfo(int index)
{
    if (index == -1)
    {
        return ;
    }

    const QStringList list = ui->serialsBox->itemData(index).toStringList();

    ui->desLabel->setText(tr("描述: %1").arg(list.count() > 1 ? list.at(1) : tr("未知")));
    ui->manufacturerLabel->setText(tr("生产厂商: %1").arg(list.count() > 2 ? list.at(2) : tr("未知")));
    ui->serialNumberLabel->setText(tr("串口号: %1").arg(list.count() > 3 ? list.at(3) : tr("未知")));
    ui->locationLabel->setText(tr("位置: %1").arg(list.count() > 4 ? list.at(4) : tr("未知")));
}

//打开串口
void MainWindow::openSerial()
{
    qDebug() << "即将打开串口";
    updateSettings();
    const Settings settings = m_currentSettings;
    m_serial->setPortName(settings.name);
    m_serial->setBaudRate(settings.baudRate);
    m_serial->setDataBits(settings.dataBits);
    m_serial->setStopBits(settings.stopBits);
    m_serial->setParity(settings.parity);
    m_serial->setFlowControl(settings.flowControl);

    if (m_serial->open(QIODevice::ReadWrite))
    {
        qDebug() << "打开串口成功！";
        ui->textEdit->setEnabled(true); //启用编辑区
        ui->actionDisconnect->setEnabled(true); //开启断开连接按钮
        ui->disconnectBtn->setEnabled(true);
        ui->actionConnect->setEnabled(false);   //禁用连接按钮
        ui->connetcBtn->setEnabled(false);
        ui->sendBtn->setEnabled(true);          //启用发送按钮
        ui->clearTextBtn->setEnabled(true);     //启用清空文本按钮

        updateBoxStatus(false);

        showStatus(tr("%1 已连接 : 波特率：%2，数据位：%3，校验方式：%4，停止位：%5，流控制：%6")
                          .arg(settings.name).arg(settings.stringBaudRate).arg(settings.stringDataBits)
                          .arg(settings.stringParity).arg(settings.stringStopBits).arg(settings.stringFlowControl));

    }
    else
    {
        qDebug() << "打开串口失败！";
        QMessageBox errorMsg(QMessageBox::Critical, "错误", "串口打开失败!\n原因：" + m_serial->errorString());
        errorMsg.exec();
    }
}

//关闭串口
void MainWindow::closeSerial()
{
    qDebug() << "试图关闭串口";
    if (m_serial->isOpen())
    {
        updateBoxStatus(true);
        qDebug() << "串口已打开，正在关闭";
        m_serial->close();
        showStatus("已关闭，等待连接...");
        ui->connetcBtn->setEnabled(true);
        ui->actionConnect->setEnabled(true);
        ui->disconnectBtn->setEnabled(false);
        ui->actionDisconnect->setEnabled(false);
        ui->sendBtn->setEnabled(false);
        ui->clearTextBtn->setEnabled(false);
        ui->textEdit->setEnabled(false);

        m_sendStatus->clear();
        m_sendStatus->setStyleSheet(nullptr);
    }
}

//更新串口
void MainWindow::updateSettings()
{
    m_currentSettings.name = ui->serialsBox->currentText();

    m_currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    m_currentSettings.stringBaudRate = QString::number(m_currentSettings.baudRate);

    m_currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    m_currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    m_currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    m_currentSettings.stringParity = ui->parityBox->currentText();

    m_currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    m_currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    m_currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    m_currentSettings.stringFlowControl = ui->flowControlBox->currentText();
    
}

// 接收数据
void MainWindow::receiveData()
{
    qDebug() << "进入receiveData函数" ;
    QByteArray recData = m_serial->readAll();
    QString recStr = recData.toHex();

//    qDebug() << "接收数据：";
//    QTextCodec *codecUtf8 = QTextCodec::codecForName("utf-8");
//    QTextCodec *codecBig5 = QTextCodec::codecForName("Big5");
//    QTextCodec *codecGB18030 = QTextCodec::codecForName("GB18030");
//    qDebug() << "UTF-8：" << codecUtf8->toUnicode(recData);

//    qDebug() << "Big5：" << codecBig5->toUnicode(recData);
//    qDebug() << "GB18030：" << codecGB18030->toUnicode(recData);

    QTextCodec *codec = nullptr;

#ifdef Q_OS_WINDOWS
    codec = QTextCodec::codecForName("GB18030");
#else
    codec = QTextCodec::codecForName("utf-8");
#endif

    QString finalStr = codec->toUnicode(recData);

    qDebug() << codec->name() << ":" << finalStr;

//    ui->receiveBrowser->setText(finalStr);
    ui->receiveBrowser->append(finalStr);
}

// 点击发送按钮{槽函数}
void MainWindow::on_sendBtn_clicked()
{
    if (!ui->textEdit->isEnabled())
    {
        this->m_status->setText("未连接！");
    }
    QByteArray data = ui->textEdit->toPlainText().toLocal8Bit();
    qDebug() << "试图发送###";
    qDebug() << data;
    if (data.length() == 0)
    {
        qDebug() << "输入为空!";
        m_sendStatus->setStyleSheet(QString("background-color:")+"grey");
        m_sendStatus->setText("***输入为空！***");
        return;
    }
    qint64 res = m_serial->write(data);
    qDebug() << "返回值：" << res;
    if (res == -1)
    {
        m_sendStatus->setStyleSheet(QString("background-color:")+"red");
        m_sendStatus->setText("***发送失败！***");
    }
    else
    {
        ui->textEdit->clear();
        m_sendStatus->setStyleSheet(QString("background-color:")+"yellow");
        m_sendStatus->setText("***发送成功！***");
    }
}
