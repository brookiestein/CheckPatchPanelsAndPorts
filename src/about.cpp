#include <QMessageBox>

#include "about.hpp"

About::About(QObject *parent)
    : QObject{parent}
{
    /* Perfectly safe to do this cast.
     * qobject_cast will return nullptr if the cast couldn't be done.
     */
    m_parent = qobject_cast<QWidget *>(parent);
}

void About::aboutQt(bool triggered)
{
    QMessageBox::aboutQt(m_parent);
}

void About::about(bool triggered)
{
    auto message = tr("\
<html>\
  <body>\
    <p>\
    <b>Autor:</b> %1<br>\
    <b>Versión:</b> %2<br><br>\
    <b>Bibliotecas Usadas:</b>\
    <ol>\
        <li>Qt6</li>\
        <li>libxlsxwriter</li>\
    </ol>\
    <a href='%3'>Licencia de libxlsxwriter:</a></p>\
  </body>\
</html>\
").arg(AUTHOR, VERSION, LIBXLSXWRITER_LICENSE);

    QMessageBox msg(m_parent);
    msg.setWindowTitle(tr("Acerca de"));
    msg.setTextFormat(Qt::RichText);
    msg.setText(message);
    msg.exec();
}
