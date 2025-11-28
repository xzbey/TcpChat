#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QColor>
#include <QByteArray>
#include <QHostAddress>

class Datagram {
public:
    Datagram(bool type, QList<QHostAddress> list, QString name = "Null_", QColor color = "#000000", QString message = "None", QByteArray imageData = nullptr):
        type(type), list(list), name(name), color(color), message(message), imageData(imageData) {}
    Datagram(bool type):
        type(type), name("---"), color("#000000"), message("---") {}
    Datagram() {}

    void Set_type(bool type) { this->type = type; }
    void Set_list(QList<QHostAddress> list) { this->list = list; }
    void Set_name(QString name) { this->name = name; }
    void Set_color(QColor color) { this->color = color; }
    void Set_message(QString message) { this->message = message; }
    void Set_avatar(QByteArray avatar) { this->imageData = avatar; }

    bool Get_type() const { return type; }
    QList<QHostAddress> Get_list() const { return list; }
    QString Get_name() const { return name; }
    QColor Get_color() const { return color; }
    QString Get_message() const { return message; }
    QByteArray Get_avatar() const { return imageData; }


    QByteArray toByteArray() const {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << quint64(0);
        out << *this;
        out.device()->seek(0);
        out << quint64(data.size() - sizeof(quint64));
        return data;
    }

    static Datagram fromByteArray(QByteArray&data) {
        Datagram datagram;
        QDataStream in(&data, QIODevice::ReadOnly);
        in >> datagram;
        return datagram;
    }

private:
    bool type; // 0 - датаграмма с данными пользователя | 1 - датаграмма со списком клиентов
    QList<QHostAddress> list;
    QString name;
    QColor color;
    QString message;
    QByteArray imageData;

    friend QDataStream &operator<<(QDataStream &out, const Datagram &datagram) {
        out << datagram.type << datagram.list << datagram.name << datagram.color << datagram.message << datagram.imageData;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, Datagram &datagram) {
        in >> datagram.type >> datagram.list >> datagram.name >> datagram.color >> datagram.message >> datagram.imageData;
        return in;
    }
};


#endif // DATAGRAM_H
