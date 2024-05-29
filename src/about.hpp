#ifndef ABOUT_HPP
#define ABOUT_HPP

#include <QObject>
#include <QWidget>

class About : public QObject
{
    Q_OBJECT
    QWidget *m_parent;
public:
    explicit About(QObject *parent = nullptr);
public slots:
    void aboutQt(bool triggered);
    void about(bool triggered);
};

#endif // ABOUT_HPP
