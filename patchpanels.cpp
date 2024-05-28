#include "patchpanels.hpp"
#include "ui_patchpanels.h"

#include <QCheckBox>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

PatchPanels::PatchPanels(QString name, std::map<int, int> portsPerPatchPanel, QWidget *parent)
    : QTabWidget(parent)
    , ui(new Ui::PatchPanels)
    , m_name(name)
    , m_portsPerPatchPanel(portsPerPatchPanel)
{
    ui->setupUi(this);

    setWindowTitle(name);

    auto *layout = new QVBoxLayout(this);
    auto *firstWidget = new QWidget(this);
    auto *button = new QPushButton(tr("Confirmar"));
    button->setToolTip(tr("Pulsa aquí para verificar rápidamente si todos los puertos fueron verificados."));
    connect(button, &QPushButton::clicked, this, &PatchPanels::onConfirmButtonClicked);

    layout->addWidget(button);
    firstWidget->setLayout(layout);

    addTab(firstWidget, button->text());

    int nPatchPanel {};
    int row {};
    int column {};
    for (auto [patchPanel, ports] : portsPerPatchPanel) {
        auto *layout = new QGridLayout(this);
        m_layouts.push_back(layout);
        auto *patchPanelTab = new QWidget(this);

        column = 0;
        for (int i {}; i < ports; ++i) {
            auto *nPort = new QCheckBox(QString(tr("Puerto %1").arg(QString::number(i + 1))), this);
            auto *nQuestion = new QCheckBox(QString(tr("¿Es un AP?")));
            nPort->setToolTip(tr("Por favor, márcalo si pudiste verificarlo."));
            nQuestion->setToolTip(tr("Márcalo si en este puerto está instalado un AP, si sólo es una toma de datos déjalo desmarcado."));
            layout->addWidget(nPort, row, column);
            layout->addWidget(nQuestion, row, column + 1);

            ++row;
        }

        patchPanelTab->setLayout(layout);
        addTab(patchPanelTab, QString(tr("Patch Panel %1").arg(QString::number(nPatchPanel + 1))));
        ++nPatchPanel;
    }

    setCurrentIndex(1);
}

PatchPanels::~PatchPanels()
{
    delete ui;
}

void PatchPanels::writeSpreadSheet()
{
    auto filename = QString("%1%2%3")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                             QDir::separator(),
                             QString("%1.xlsx").arg(m_name)
                             );

    lxw_workbook *workbook = workbook_new(filename.toStdString().c_str());
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, nullptr);
    lxw_format *bold = workbook_add_format(workbook);
    format_set_bold(bold);

    const auto GREEN = 0x00ff00;

    int row {};
    int column {};
    int nPatchPanel {1};
    for (auto *layout : m_layouts) {
        row = 0;
        auto title = tr("Patch Panel %1").arg(QString::number(nPatchPanel)).toStdString();
        worksheet_write_string(worksheet, row, column, title.c_str(), bold);
        worksheet_set_column(worksheet, column, column, title.length(), bold);

        ++row;
        for (int i {}; i < layout->count(); ++i) {
            if (i % 2 != 0) {
                continue;
            }

            auto port = qobject_cast<QCheckBox *>(layout->itemAt(i)->widget())->text();
            auto isAnAP = qobject_cast<QCheckBox *>(layout->itemAt(i + 1)->widget())->isChecked();

            auto cell = port.mid(port.size() - 1, 1).toInt();
            lxw_format *color = workbook_add_format(workbook);
            format_set_bg_color(color, isAnAP ? LXW_COLOR_CYAN : GREEN);

            worksheet_write_number(worksheet, row, column, cell, color);
            ++row;
        }

        ++nPatchPanel;
        ++column;
    }

    lxw_format *legend_fmt = workbook_add_format(workbook);
    lxw_format *data = workbook_add_format(workbook);
    lxw_format *ap = workbook_add_format(workbook);
    format_set_bold(legend_fmt);
    format_set_bold(data);
    format_set_bold(ap);
    format_set_bg_color(data, GREEN);
    format_set_bg_color(ap, LXW_COLOR_CYAN);

    row = 0;
    column += 2;
    auto legend = tr("Leyenda").toStdString();
    worksheet_write_string(worksheet, row, column, legend.c_str(), legend_fmt);

    ++row;
    legend = tr("Toma de datos").toStdString();
    worksheet_set_column(worksheet, column, column, legend.length(), nullptr);
    worksheet_write_string(worksheet, row, column, legend.c_str(), data);

    ++row;
    legend = tr("AP").toStdString();
    worksheet_write_string(worksheet, row, column, legend.c_str(), ap);

    auto result = workbook_close(workbook);

    if (result == LXW_NO_ERROR) {
        QMessageBox::information(this, tr("Éxito"), tr("El libro de excel fue guardado correctamente."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Ocurrió un error al crear el libro de excel."));
    }
}

void PatchPanels::closeEvent(QCloseEvent *event)
{
    emit closed();
    QWidget::closeEvent(event);
}

void PatchPanels::onConfirmButtonClicked()
{
    bool ok {true};
    std::map<QString, std::vector<QString>> notVerifiedPorts;
    int q {};

    int nPatchPanel {1};
    for (auto *layout : m_layouts) {
        const auto key = tr("Patch Panel %1").arg(nPatchPanel);
        notVerifiedPorts[key] = std::vector<QString>();
        for (int i {}; i < layout->count(); ++i) {
            if (i % 2 != 0) {
                continue;
            }

            auto *widget = qobject_cast<QCheckBox *>(layout->itemAt(i)->widget());
            if (not widget->isChecked()) {
                ok = false;
                notVerifiedPorts.at(tr("Patch Panel %1").arg(nPatchPanel)).push_back(widget->text());
                ++q;
            }
        }

        ++nPatchPanel;
    }

    if (ok) {
        QMessageBox::information(this, tr("Confirmado"), tr("Todos los puertos están verificados."));
        auto reply = QMessageBox::question(this, tr("Generar Excel"), tr("¿Quieres generar una hoja de cálculo?"));
        if (reply == QMessageBox::No) {
            return;
        }

        writeSpreadSheet();
    } else {
        auto message = q == 1 ? tr("El siguiente puerto no está verificado: ") : tr("Los siguientes puertos no están verificados: ");
        for (auto [patchPanel, ports] : notVerifiedPorts) {
            for (auto port : ports) {
                message += QString("\n%1: %2").arg(patchPanel, port);
            }
        }

        QMessageBox::warning(this, tr("Advertencia"), message);
    }
}
