#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QProcess>
#include <QCloseEvent>

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
    void closeEvent(QCloseEvent*);

private slots:
    void on_btn_connect_clicked();

    void on_btn_send_clicked();

    void slotReadyRead();

    void on_message_returnPressed();

    void on_newProcess_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QByteArray data;
    QList<QProcess*>processes;

    void sendToServer(QString buffer);
};
#endif // MAINWINDOW_H
