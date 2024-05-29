#include <QEventLoop>
#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include "about.hpp"
#include "patchpanels.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->patchPanelsEdit->setPlaceholderText(tr("Número"));

    auto *about = new About(this);

    connect(ui->actionAboutQt, &QAction::triggered, about, &About::aboutQt);
    connect(ui->actionAbout, &QAction::triggered, about, &About::about);
    connect(ui->patchPanelsEdit, &QLineEdit::returnPressed, this, &MainWindow::onReturnReleased);
    connect(ui->continueBotton, &QPushButton::clicked, this, &MainWindow::onContinueBottonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::map<int, int> MainWindow::getPortsPerPatchPanel(int nPatchPanels)
{
    std::map<int, int> portsPerPatchPanel;

    for (int i {}; i < nPatchPanels; ++i) {
        while (true) {
            auto portsQuantity =
                QInputDialog::getInt(this,
                                QString(tr("Cantidad de Puertos")),
                                QString(tr("¿Cuántos puertos tiene el Patch Panel %1?").arg(i + 1))
                );

            if (portsQuantity <= 0) {
                QMessageBox::critical(this, tr("Error"), tr("Debe haber al menos un puerto."));
                continue;
            }

            portsPerPatchPanel[i + 1] = portsQuantity;
            break;
        }
    }

    return portsPerPatchPanel;
}

void MainWindow::onReturnReleased()
{
    ui->continueBotton->click();
}

void MainWindow::onContinueBottonClicked()
{
    QEventLoop loop;
    auto name = ui->nameEdit->text();
    auto text = ui->patchPanelsEdit->text();

    if (name.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Debes indicar el nombre del distrito."));
        return;
    }

    if (text.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Debes indicar cuántos Patch Panels hay."));
        return;
    }

    int quantity = text.toInt();

    if (quantity <= 0) {
        QMessageBox::critical(this, tr("Error"), tr("Debe haber al menos un Patch Panel."));
        return;
    }

    ui->patchPanelsEdit->setText("");

    auto portsPerPatchPanel = getPortsPerPatchPanel(quantity);

    auto patchPanels = PatchPanels(name, portsPerPatchPanel);
    connect(&patchPanels, &PatchPanels::closed, &loop, &QEventLoop::quit);
    patchPanels.show();
    close();
    loop.exec();
}
