#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <map>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;

    std::map<int, int> getPortsPerPatchPanel(int nPatchPanels);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onReturnReleased();
    void onContinueBottonClicked();
};
#endif // MAINWINDOW_HPP
