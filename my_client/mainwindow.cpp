#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    rg = QRandomGenerator::global();

    ui->name->setPlaceholderText("Имя...");
    ui->message->setPlaceholderText("Сообщение...");
    ui->ip->setPlaceholderText("Ip...");
    ui->port->setPlaceholderText("Port...");

    ui->message->setEnabled(0);
    ui->btn_send->setEnabled(0);
    ui->select_avatar->setEnabled(0);

    QImage img(164, 164, QImage::Format_RGB32);
    //img.fill("#000000");
    ui->avatar_label->setPixmap(QPixmap::fromImage(img));
    ui->avatar_label->setFixedSize(164, 164);

    connect_socket();
    connect(ui->message, &QLineEdit::returnPressed, this, &MainWindow::trySendMessage);
    connect(ui->btn_send, &QPushButton::clicked, this, &MainWindow::trySendMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    if (str_ip == "") {
        address = QHostAddress::LocalHost;
        ui->ip->setText("127.0.0.1");
        return true;
    }
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
        ui->ip->setEnabled(0);
        ui->port->setEnabled(0);
        ui->select_avatar->setEnabled(1);
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
        ui->ip->setEnabled(1);
        ui->port->setEnabled(1);
        ui->btn_connect->setEnabled(1);
        ui->btn_send->setEnabled(0);
        ui->select_avatar->setEnabled(0);
        ui->chat->append("<table align='center'>"
                         "<tr><td style='color:red; text-align:center; padding: 10px'>"
                         "<b><i>Disconnected from server.</i></b>"
                         "</td></tr>"
                         "</table>");

    });
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

}

void MainWindow::socketError(QAbstractSocket::SocketError error) {
    if (!socket) {
        qDebug() << "error after disconnect (ignored)";
        return;
    }

    qDebug() << "socket error:" << socket->errorString();
    ui->message->setEnabled(0);
    ui->name->setEnabled(1);
    ui->btn_connect->setEnabled(1);
    ui->btn_send->setEnabled(0);
    ui->ip->setEnabled(1);
    ui->port->setEnabled(1);
    ui->select_avatar->setEnabled(0);

    ui->chat->append(QString("<table align='center'>"
                             "<tr><td style='color:red; text-align:center; padding: 10px'>"
                             "<b><i>Disconnected | Error: %1</i></b>"
                             "</td></tr>"
                             "</table>").arg(socket->errorString()));
}

void MainWindow::slotReadyRead() {
    QDataStream stream(socket);
    while (socket->bytesAvailable() > 0) {
        if (nextBlockSize == 0) {
            if (socket->bytesAvailable() < sizeof(quint64))
                return;
            stream >> nextBlockSize;
        }

        if (socket->bytesAvailable() < nextBlockSize)
            return;

        QByteArray fullBlock = socket->read(nextBlockSize);
        Datagram data = Datagram::fromByteArray(fullBlock);

        if (data.Get_type() == 0) {
            qDebug() << "datagram type - user data";
            qDebug() << "info" << data.Get_name() << data.Get_color().name() << data.Get_message();

            if (blacklist.contains(data.Get_name())) {
                qDebug() << "[Заблокировано]";
                nextBlockSize = 0;
                return;
            }

            QString recipient = data.Get_recipient();
            if (!recipient.isEmpty() and recipient != "@a") {
                if (recipient == ui->name->text())
                    data.Set_message("<b>[ЛС]</b> " + data.Get_message());
                else {
                    qDebug() << "[Заблокировано]";
                    nextBlockSize = 0;
                    return;
                }
            }

            QString align = "left";
            if (data.Get_name() == ui->name->text())
                align = "right";

            QImage image;
            image.loadFromData(data.Get_avatar());
            QTextDocument *doc = ui->chat->document();
            QString imageKey = data.Get_name() + "_" + QString::number(reinterpret_cast<quintptr>(&image), 16);
            doc->addResource(QTextDocument::ImageResource, QUrl(imageKey), image);

            QString time = QTime::currentTime().toString("hh:mm:ss");
            ui->chat->append(QString("<table style='border: 2px solid %1; border-collapse: collapse; -qt-table-type: frame; width: 1px; table-layout: fixed' align=%2>"
                                     "<tr><td style='color:%1; text-align:%2; padding: 10px'>"
                                     "<b>%3 / %4</b><br>"
                                     "<img src='%6' style='max-width:10%; max-height:10%'><br>%5"
                                     "</td></tr>"
                                     "</table>")
                                 .arg(data.Get_color().name()).arg(align).arg(data.Get_name()).arg(time).arg(data.Get_message()).arg(imageKey));
        }
        else { // data.type == 1
            qDebug() << "datagram type - list connections";
            QString userIp = "-";
            ui->userList->clear();
            ui->userList->addItem(QString("Подключенных: %1\n").arg(data.Get_list().count()));
            for (QHostAddress user: data.Get_list()) {
                userIp = user.toString().remove(':').remove('f');
                qDebug() << userIp;
                ui->userList->addItem(userIp);
            }
        }

        nextBlockSize = 0;
    }
}

void MainWindow::sendToServer() {
    if (!socket or socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "error: cannot send - not connected";
        return;
    }
    /*QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << *datagram;*/
    QByteArray data = datagram->toByteArray();
    socket->write(data);
    ui->message->clear();
    qDebug() << "send" << data.size() << "bytes | datagram:" << datagram->Get_name() << datagram->Get_color().name() << datagram->Get_message();
    ui->btn_send->setEnabled(0);
}


void MainWindow::on_btn_connect_clicked()
{
    QHostAddress address = QHostAddress::LocalHost; quint16 port = 50000;
    if (!isValid(ui->ip->text(), address) or !isValid(ui->port->text(), port))
        return;

    qDebug() << "address accepted:" << address.toString() << QString::number(port);

    if (datagram != nullptr)
        delete datagram;

    datagram = new Datagram(0);
    QString name = ui->name->text().trimmed();
    if (name == "") {
        int id = rg->bounded(1000, 100000);
        ui->name->setText(QString("user%1").arg(id));
    }
    datagram->Set_name(ui->name->text());
    datagram->Set_color(ColorDialog());

    QByteArray imageData;
    QBuffer imageBuffer(&imageData);
    QImage img(164, 164, QImage::Format_RGB32);
    img.fill(datagram->Get_color());
    ui->avatar_label->setPixmap(QPixmap::fromImage(img));
    img.save(&imageBuffer, "JPG", 80);
    imageBuffer.close();
    datagram->Set_avatar(imageData);


    qDebug() << "datagram created:" << datagram->Get_name() << datagram->Get_color().name();

    if (!socket)
        connect_socket();

    socket->connectToHost(address, port);
}


void MainWindow::trySendMessage() {
    QString message = ui->message->text().trimmed();
    ui->message->clear();

    if (message == "")
        return;

    if (message.startsWith('/'))
        commandProcessing(message);
    else {
        datagram->Set_message(message);
        sendToServer();
    }
}

void MainWindow::warning_nullFunc(const QString& command) const {
    ui->chat->append(QString("<i style='color: gray'>Пустая команда [%1]!</i>").arg(command));
}

void MainWindow::commandProcessing(const QString& message) { // /mute & /msg
    QStringList commandParts = message.split(' ', Qt::SkipEmptyParts);
    // QStringList commandParts = message.split(' '); //для qt 4.8.1
    if (commandParts.isEmpty())
        return;


    QString command = commandParts[0].toLower();
    commandParts.pop_front();

    if (command == "/mute") {
        if (commandParts.isEmpty()) {
            warning_nullFunc("/mute");
            return; }
        if (commandParts[0] == ui->name->text()) {
            ui->chat->append("<i style='color: gray'><b>[mute]</b> Вы не можете замутить самого себя!</i>");
            return; }

        blacklist.insert(commandParts[0]);
        ui->chat->append(QString("<i style='color: gray'><b>[mute]</b> Вы замутили %1!</i>").arg(commandParts[0]));
    }

    else if (command == "/unmute") {
        if (commandParts.isEmpty()) {
            warning_nullFunc("/unmute");
            return; }

        if (commandParts[0] == "@a") {
            blacklist.clear();
            ui->chat->append("<i style='color: gray'><b>[unmute]</b> Все пользователи удалены из мута!</i>");
            return; }

        if (commandParts[0] == ui->name->text()) {
            ui->chat->append("<i style='color: gray'><b>[unmute]</b> Вы не можете размутить самого себя!</i>");
            return; }

        int ok = blacklist.remove(commandParts[0]);
        if (ok == 0)
            ui->chat->append("<i style='color: gray'><b>[unmute]</b> Пользователя не существует!</i>");
        else
            ui->chat->append(QString("<i style='color: gray'><b>[unmute]</b> Пользователь %1 успешно убран из мута!</i>")
                                 .arg(commandParts[0]));
    }

    else if (command == "/mutelist") {
        if (blacklist.isEmpty())
            ui->chat->append("<i style='color: gray'><b>[mutelist]</b> Список пуст!</i>");
        else {
            ui->chat->append("<i style='color: gray'><b>[mutelist]</b> <u>Черный список:</u></i>");
            for (QString user: blacklist)
                ui->chat->append(QString("<i style='color: gray'><b>[mutelist]</b> %1</i>").arg(user));
            ui->chat->append(QString("<i style='color: gray'><b>[mutelist]</b> Количество: %1</i>").arg(blacklist.size()));
        }
    }

    else if (command == "/msg") {
        if (commandParts.size() < 2) {
            ui->chat->append("<i style='color: gray'><b>[msg]</b> Пустое сообщение или ник!</i>");
            return; }
        if (commandParts[0] == ui->name->text()) {
            ui->chat->append("<i style='color: gray'><b>[msg]</b> Вы не можете отправить личное сообщение самому себе!</i>");
            return;
        }

        QString user = commandParts[0],
            text = commandParts.mid(1).join(' ');
        datagram->Set_recipient(user);
        datagram->Set_message(text);
        sendToServer();
        datagram->Set_recipient("");
        ui->chat->append(QString("<i style='color: gray'><b>[msg]</b> Отправлено личное сообщение %1: %2</i>").arg(user).arg(text));
    }

    else
        ui->chat->append("<i style='color: gray'>Неизвестная команда!</i>");


}


void MainWindow::on_newProcess_clicked()
{
    QProcess *process = new QProcess(this);
    processes.append(process);
    process->start(qApp->applicationFilePath());
}

void MainWindow::on_message_textEdited(const QString &arg1)
{
    ui->btn_send->setEnabled(!arg1.isEmpty());
}

void MainWindow::on_select_avatar_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file", QDir::homePath(), "Images (*.png *.jpg)");
    ui->avatar_label->setPixmap(QPixmap(filename));
    ui->avatar_label->setScaledContents(1);
    ui->avatar_label->setFixedSize(164, 164);

    QByteArray imageData;
    QBuffer imageBuffer(&imageData);
    QImage image = ui->avatar_label->pixmap().toImage();
    image.save(&imageBuffer, "JPG", 80);
    imageBuffer.close();
    datagram->Set_avatar(imageData);
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




