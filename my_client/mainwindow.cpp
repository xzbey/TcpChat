#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->name->setPlaceholderText("Имя...");
    ui->message->setPlaceholderText("Сообщение...");
    ui->ip->setPlaceholderText("Ip...");
    ui->port->setPlaceholderText("Port...");

    ui->message->setEnabled(0);
    ui->btn_send->setEnabled(0);
    ui->ip->setVisible(0);
    ui->port->setVisible(0);

    connect_socket();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connect_socket() {
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    //connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::connected, this, [this]() {
        qDebug() << "connected to server";
        ui->btn_connect->setEnabled(0);
        ui->name->setEnabled(0);
        ui->message->setEnabled(1);
        ui->btn_send->setEnabled(0);
        ui->ip->setVisible(0);
        ui->port->setVisible(0);
        ui->changeHostAddress->setVisible(0);
        ui->chat->append("<table align='center'>"
                         "<tr><td style='color:green; text-align:center; padding: 10px'>"
                         "<b><i>Connected to server.</i></b>"
                         "</td></tr>"
                         "</table>");
    });
    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "disconnected from server";
        socket = 0;
        ui->message->setEnabled(0);
        ui->name->setEnabled(1);
        ui->btn_connect->setEnabled(1);
        ui->btn_send->setEnabled(0);
        ui->changeHostAddress->setVisible(1);
        if (ui->changeHostAddress->isChecked()) {
            ui->ip->setVisible(1);
            ui->port->setVisible(1);
        }
        ui->chat->append("<table align='center'>"
                         "<tr><td style='color:red; text-align:center; padding: 10px'>"
                         "<b><i>Disconnected from server.</i></b>"
                         "</td></tr>"
                         "</table>");

    });
    connect(socket, &QAbstractSocket::errorOccurred, this, [this]() {
        if (!socket) {
            qDebug() << "error after disconnect (ignored)";
            return;
        }

        qDebug() << "socket error:" << socket->errorString();
        ui->message->setEnabled(0);
        ui->name->setEnabled(1);
        ui->btn_connect->setEnabled(1);
        ui->btn_send->setEnabled(0);
        ui->changeHostAddress->setVisible(1);
        if (ui->changeHostAddress->isChecked()) {
            ui->ip->setVisible(1);
            ui->port->setVisible(1);
        }
        ui->chat->append(QString("<table align='center'>"
                                 "<tr><td style='color:red; text-align:center; padding: 10px'>"
                                 "<b><i>Disconnected | Error: %1</i></b>"
                                 "</td></tr>"
                                 "</table>").arg(socket->errorString()));
    });

}

QColor MainWindow::ColorDialog() {
    QColorDialog* ColorDialog = new QColorDialog(this);
    ColorDialog->exec();

    QColor selectedColor = ColorDialog->selectedColor();
    delete ColorDialog;

    if (!selectedColor.isValid())
        selectedColor = Qt::black;
    return selectedColor;
}

bool MainWindow::isValid(QString str_ip, QHostAddress& address) {
    if (!address.setAddress(str_ip)) {
        qDebug() << "error: invalid ip =" << str_ip;
        return false;
    }
    return true;
}

bool MainWindow::isValid(QString str_port, quint16& port) {
    if (str_port.toInt()) {
        port = str_port.toInt();
        if (port > 0 and port <= 65535)
            return true;
    }
    qDebug() << "error: invalid port =" << str_port;
    return false;
}

void MainWindow::slotReadyRead() {
    if (!socket)
        return;
    QDataStream stream(socket);
    if (stream.status() == QDataStream::Ok) {
        Datagram data;
        stream >> data;
        qDebug() << "datagram received:" << data.Get_name() << data.Get_color().name() << data.Get_message();
        QString align = "left";
        if (data.Get_name() == ui->name->text())
            align = "right";

        ui->chat->append(QString("<table style='border: 2px solid %1; border-collapse: collapse; -qt-table-type: frame; width: 1px; table-layout: fixed' align=%2>"
                                  "<tr><td style='color:%1; text-align:%2; padding: 10px'>"
                                  "<b>%3</b><br>%4"
                                  "</td></tr>"
                                  "</table>")
                             .arg(data.Get_color().name()).arg(align).arg(data.Get_name()).arg(data.Get_message()));
    } else {
        ui->chat->append("read error");
    }
}

void MainWindow::sendToServer() {
    if (!socket or socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "error: cannot send - not connected";
        return;
    }
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
    QHostAddress address = QHostAddress::LocalHost; quint16 port = 50000;
    if (ui->changeHostAddress->isChecked())
        if (!isValid(ui->ip->text(), address) or !isValid(ui->port->text(), port))
            return;

    qDebug() << "address accepted:" << address.toString() << QString::number(port);

    if (datagram != nullptr)
        delete datagram;

    datagram = new Datagram(ui->name->text(), ColorDialog());
    qDebug() << "datagram created:" << datagram->Get_name() << datagram->Get_color().name();

    if (!socket)
        connect_socket();

    socket->connectToHost(address, port);
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


void MainWindow::on_changeHostAddress_toggled(bool checked)
{
    if (checked) {
        ui->ip->setVisible(1);
        ui->port->setVisible(1);
    } else {
        ui->ip->setVisible(0);
        ui->port->setVisible(0);
    }
}

