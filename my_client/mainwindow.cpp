#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->message->setEnabled(0);
    ui->btn_send->setEnabled(0);

    socket = new QTcpSocket;
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::connected, this, [this]() {
        qDebug() << "connected to server";
        ui->message->setEnabled(1);
        ui->btn_send->setEnabled(0);
        ui->chat->append("<br><span style='color:green'>Connected to server.</span><br>");
    });
    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "disconnected from server";
        ui->message->setEnabled(0);
        ui->name->setEnabled(1);
        ui->btn_connect->setEnabled(1);
        ui->btn_send->setEnabled(0);
        ui->chat->append("<br><span style='color:red'>Disconnected from server.</span><br>");
    });
    connect(socket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        qDebug() << "socket error:" << socket->errorString();
        ui->message->setEnabled(0);
        ui->name->setEnabled(1);
        ui->btn_connect->setEnabled(1);
        ui->btn_send->setEnabled(0);
        ui->chat->append(QString("<br><span style='color:red'>Disconnected | Error: %1</span><br>").arg(socket->errorString()));
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

QColor MainWindow::ColorDialog() {
    QColorDialog* ColorDialog;
    ColorDialog = new QColorDialog(this);
    ColorDialog->exec();
    QColor selectedColor = ColorDialog->selectedColor();
    ColorDialog->deleteLater();

    if (!selectedColor.isValid())
        selectedColor = Qt::black;
    return selectedColor;
}

void MainWindow::slotReadyRead() {
    QDataStream stream(socket);
    if (stream.status() == QDataStream::Ok) {
        Datagram data;
        stream >> data;
        qDebug() << "datagram received:" << data.Get_name() << data.Get_color().name() << data.Get_message();
        ui->chat->append(QString("<span style='color:%1'>%2<br>%3<br></span>").arg(data.Get_color().name()).arg(data.Get_name()).arg(data.Get_message()));
    } else {
        ui->chat->append("read error");
    }
}

void MainWindow::sendToServer() {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << *datagram;
    socket->write(data);
    ui->message->clear();
    qDebug() << "datagram sended:" << datagram->Get_name() << datagram->Get_color().name() << datagram->Get_message();
    ui->btn_send->setEnabled(0);
}


void MainWindow::on_btn_connect_clicked()
{
    datagram = new Datagram(ui->name->text(), ColorDialog());
    qDebug() << "datagram created:" << datagram->Get_name() << datagram->Get_color().name();
    socket->connectToHost(QHostAddress::LocalHost, 50000);

    ui->btn_connect->setEnabled(0);
    ui->name->setEnabled(0);

    //ui->message->setEnabled(1);
}


void MainWindow::on_btn_send_clicked()
{
    datagram->Set_message(ui->message->text());
    sendToServer();
}


void MainWindow::on_message_returnPressed()
{
    datagram->Set_message(ui->message->text());
    sendToServer();
}


void MainWindow::on_newProcess_clicked()
{
    QProcess *process = new QProcess(this);
    processes.append(process);
    process->start(qApp->applicationFilePath());
}

void MainWindow::closeEvent(QCloseEvent* e) {

    if (socket != nullptr)
        socket->deleteLater();
    qDebug() << "socket deleted";

    if (datagram != nullptr)
        delete datagram;
    qDebug() << "datagram deleted";

    for(auto &process : processes)
        process->terminate();
    qDebug() << "processes clear";

    QWidget::closeEvent(e);
}

void MainWindow::on_message_textEdited(const QString &arg1)
{
    ui->btn_send->setEnabled(!arg1.isEmpty());
}

