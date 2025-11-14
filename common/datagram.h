#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QColor>

class Datagram {
public:
    Datagram(QString name, QColor color, QString message = "None"):
        name(name), color(color), message(message) {}
    Datagram() {}

    void Set_name(QString name) { this->name = name; }
    void Set_color(QColor color) { this->color = color; }
    void Set_message(QString message) { this->message = message; }

    QString Get_name() const { return name; }
    QColor Get_color() const { return color; }
    QString Get_message() const { return message; }

private:
    QString name;
    QColor color;
    QString message;

    friend QDataStream &operator<<(QDataStream &out, const Datagram &datagram) {
        out << datagram.name << datagram.color << datagram.message;
        return out; }
    friend QDataStream &operator>>(QDataStream &in, Datagram &datagram) {
        in >> datagram.name >> datagram.color >> datagram.message;
        return in; };
};

#endif // DATAGRAM_H
