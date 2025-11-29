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
#include <QFileDialog>
#include <QDir>
#include <QBuffer>
#include <QInputDialog>
#include <QMessageBox>
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

    void on_newProcess_clicked();

    void on_message_textEdited(const QString &arg1);

    void on_select_avatar_clicked();


private:
    Ui::MainWindow *ui;

    QTcpSocket* socket = 0;
    QList<QProcess*>processes;

    Datagram*datagram = 0;
    QRandomGenerator*rg;

    qint64 nextBlockSize = 0;

    QSet<QString>blacklist;

    QColor ColorDialog();

    void sendToServer();
    void connect_socket();
    void trySendMessage();
    void commandProcessing(const QString &message);
    inline void warning_nullFunc(const QString &command) const;
    void socketError(QAbstractSocket::SocketError error);

    bool isValid(QString str_ip, QHostAddress& address);
    bool isValid(QString str_port, quint16& port);
};
#endif // MAINWINDOW_H
