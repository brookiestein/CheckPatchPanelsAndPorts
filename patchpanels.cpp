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
    , m_portsPerPatchPanel(portsPerPatchPanel)
{
    ui->setupUi(this);

    m_filename = tr("Verificación de tomas de datos - %1").arg(name);

    setWindowTitle(m_filename);

    auto *layout = new QVBoxLayout(this);
    auto *firstWidget = new QWidget(this);
    auto *button = new QPushButton(tr("Confirmar"), this);
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
            auto *nPort = new QCheckBox(tr("Puerto %1").arg(QString::number(i + 1)), this);
            nPort->setStyleSheet("QCheckBox::checked { color: #00ff00; }");
            nPort->setToolTip(tr("Por favor, márcalo si pudiste verificarlo."));

            auto *nIsAnAP = new QCheckBox(tr("¿Es un AP?"), this);
            nIsAnAP->setStyleSheet("QCheckBox::checked { color: #00ffff; }");
            nIsAnAP->setToolTip(
                tr("%1: Márcalo si en este puerto está instalado un AP, si sólo es una toma de datos, déjalo desmarcado.")
                    .arg(nPort->text())
            );

            auto *nIsACamera = new QCheckBox(tr("¿Es una cámara?"), this);
            nIsACamera->setStyleSheet("QCheckBox::checked { color: #e69138; }");
            nIsACamera->setToolTip(
                tr("%1: Márcalo si en este puerto está instalada una cámara, si sólo es una toma de datos, déjalo desmarcado.")
                    .arg(nPort->text())
            );

            m_checkBoxesPerLine[nIsAnAP] = nIsACamera;

            connect(nIsAnAP, &QCheckBox::clicked, this, &PatchPanels::onCheckBoxClicked);
            connect(nIsACamera, &QCheckBox::clicked, this, &PatchPanels::onCheckBoxClicked);

            layout->addWidget(nPort, row, column);
            layout->addWidget(nIsAnAP, row, column + 1);
            layout->addWidget(nIsACamera, row, column + 2);

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
                             QString("%1.xlsx").arg(m_filename)
                             );

    lxw_workbook *workbook = workbook_new(filename.toStdString().c_str());
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, nullptr);
    lxw_format *bold = workbook_add_format(workbook);
    format_set_bold(bold);

    const auto GREEN = 0x00ff00;
    const auto ORANGE = 0xe69138;

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
            auto port = qobject_cast<QCheckBox *>(layout->itemAt(i)->widget())->text();
            /* Check starting on the left-most widget. */
            if (not port.contains(tr("Puerto"))) {
                continue;
            }

            auto isAnAP = qobject_cast<QCheckBox *>(layout->itemAt(i + 1)->widget())->isChecked();
            auto isACamera = qobject_cast<QCheckBox *>(layout->itemAt(i + 2)->widget())->isChecked();

            /* First check if port number is >= 10, i.e, has two numbers. */
            bool ok {};
            int portNumber = port.mid(port.size() - 2, port.size()).toInt(&ok);

            if (not ok) {
                portNumber = port.mid(port.size() - 1, 1).toInt();
            }

            lxw_format *color = workbook_add_format(workbook);
            int rgb {};

            if (isAnAP) {
                rgb = LXW_COLOR_CYAN;
            } else if (isACamera) {
                rgb = ORANGE;
            } else {
                rgb = GREEN;
            }

            format_set_bg_color(color, rgb);

            worksheet_write_number(worksheet, row, column, portNumber, color);
            ++row;
        }

        ++nPatchPanel;
        ++column;
    }

    lxw_format *legend_fmt = workbook_add_format(workbook);
    lxw_format *data = workbook_add_format(workbook);
    lxw_format *ap = workbook_add_format(workbook);
    lxw_format *camera = workbook_add_format(workbook);

    format_set_bold(legend_fmt);
    format_set_bold(data);
    format_set_bold(ap);
    format_set_bold(camera);
    format_set_bg_color(data, GREEN);
    format_set_bg_color(ap, LXW_COLOR_CYAN);
    format_set_bg_color(camera, ORANGE);

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

    ++row;
    legend = tr("Cámara").toStdString();
    worksheet_write_string(worksheet, row, column, legend.c_str(), camera);

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
            auto *widget = qobject_cast<QCheckBox *>(layout->itemAt(i)->widget());
            if (not widget->text().contains(tr("Puerto"))) {
                continue;
            }

            if (not widget->isChecked()) {
                ok = false;
                notVerifiedPorts.at(tr("Patch Panel %1").arg(nPatchPanel)).push_back(widget->text());
                ++q;
            }
        }

        ++nPatchPanel;
    }

    if (ok) {
        QMessageBox::information(this,
                                 tr("Confirmado"),
                                 tr("Todos los puertos están verificados.")
                                 );

        auto reply = QMessageBox::question(this,
                                           tr("Generar Excel"),
                                           tr("¿Quieres generar una hoja de cálculo?")
                                           );

        if (reply == QMessageBox::No) {
            return;
        }

        writeSpreadSheet();
    } else {
        auto message = q == 1 ? tr("El siguiente puerto no está verificado: ")
                              : tr("Los siguientes puertos no están verificados: ");

        for (auto [patchPanel, ports] : notVerifiedPorts) {
            for (auto port : ports) {
                message += QString("\n%1: %2").arg(patchPanel, port);
            }
        }

        QMessageBox::warning(this, tr("Advertencia"), message);
    }
}

void PatchPanels::onCheckBoxClicked()
{
    auto *obj = qobject_cast<QCheckBox *>(sender());
    bool checked = obj->isChecked();

    for (auto [ap, camera] : m_checkBoxesPerLine) {
        if (obj == ap) {
            if (checked) {
                camera->setChecked(false);
                break;
            }
        } else if (obj == camera) {
            if (checked) {
                ap->setChecked(false);
                break;
            }
        }
    }
}
