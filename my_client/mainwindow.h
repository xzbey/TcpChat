#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QProcess>
#include <QCloseEvent>
#include <QColorDialog>
#include <QAbstractSocket>
#include <QTime>
#include <QThread>
#include <QRandomGenerator>
#include "datagram.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent*) override;

private slots:
    void slotReadyRead();

    void on_btn_connect_clicked();

    void on_btn_send_clicked();

    void on_message_returnPressed();

    void on_newProcess_clicked();

    void on_message_textEdited(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket = 0;
    QList<QProcess*>processes;
    Datagram*datagram = 0;
    QRandomGenerator*rg;
    qint64 nextBlockSize = 0;

    QColor ColorDialog();

    void sendToServer();
    void connect_socket();

    bool isValid(QString str_ip, QHostAddress& address);
    bool isValid(QString str_port, quint16& port);
};
#endif // MAINWINDOW_H
