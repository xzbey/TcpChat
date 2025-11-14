#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket;
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotReadyRead() {
    QDataStream stream(socket);
    if (stream.status() == QDataStream::Ok) {
        QString buffer;
        stream >> buffer;
        ui->chat->append(buffer);
    } else {
        ui->chat->append("read error");
    }
}

void MainWindow::sendToServer(QString buffer) {
    data.clear();
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << buffer;
    socket->write(data);
    ui->message->clear();
}


void MainWindow::on_btn_connect_clicked()
{
    socket->connectToHost(QHostAddress::LocalHost, 50000);
}


void MainWindow::on_btn_send_clicked()
{
    sendToServer(ui->message->text());
}


void MainWindow::on_message_returnPressed()
{
    sendToServer(ui->message->text());
}


void MainWindow::on_newProcess_clicked()
{
    QProcess *process = new QProcess(this);
    processes.append(process);
    process->start(qApp->applicationFilePath());
}

void MainWindow::closeEvent(QCloseEvent* e) {

    for(auto &process : processes)
        process->terminate();
    qDebug() << "processes clear";

    QWidget::closeEvent(e);
}
