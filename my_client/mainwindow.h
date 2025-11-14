#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QProcess>
#include <QCloseEvent>
#include <QColorDialog>
#include <QAbstractSocket>
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
    void on_btn_connect_clicked();

    void on_btn_send_clicked();

    void slotReadyRead();

    void on_message_returnPressed();

    void on_newProcess_clicked();

    void on_message_textEdited(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket = 0;
    QList<QProcess*>processes;
    Datagram*datagram = 0;

    QColor ColorDialog();
    void sendToServer();
};
#endif // MAINWINDOW_H
