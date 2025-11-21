#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QColor>
#include <QHostAddress>

class Datagram {
public:
    Datagram(bool type, QList<QHostAddress> list, QString name = "Null_", QColor color = "#000000", QString message = "None"):
        type(type), list(list), name(name), color(color), message(message) {}
    Datagram(bool type):
        type(type), name("---"), color("#000000"), message("---") {}
    Datagram() {}

    void Set_type(bool type) { this->type = type; }
    void Set_list(QList<QHostAddress> list) { this->list = list; }
    void Set_name(QString name) { this->name = name; }
    void Set_color(QColor color) { this->color = color; }
    void Set_message(QString message) { this->message = message; }

    bool Get_type() const { return type; }
    QList<QHostAddress> Get_list() const { return list; }
    QString Get_name() const { return name; }
    QColor Get_color() const { return color; }
    QString Get_message() const { return message; }

private:
    bool type; // 0 - датаграмма с данными пользователя | 1 - датаграмма со списком клиентов
    QList<QHostAddress> list;
    QString name;
    QColor color;
    QString message;

    friend QDataStream &operator<<(QDataStream &out, const Datagram &datagram) {
        out << datagram.type << datagram.list << datagram.name << datagram.color << datagram.message;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, Datagram &datagram) {
        in >> datagram.type >> datagram.list >> datagram.name >> datagram.color >> datagram.message;
        return in;
    }
};

/*
    friend QDataStream &operator<<(QDataStream &out, const Datagram &datagram) {
    if (datagram.type == 0)
        out << datagram.type << datagram.name << datagram.color << datagram.message;
    else
        out << datagram.type << datagram.list;

    return out;
}

friend QDataStream &operator>>(QDataStream &in, Datagram &datagram) {
    if (datagram.type == 0)
        in >> datagram.type >> datagram.name >> datagram.color >> datagram.message;
    else
        in >> datagram.type >> datagram.list;

    return in;
}
*/

#endif // DATAGRAM_H
