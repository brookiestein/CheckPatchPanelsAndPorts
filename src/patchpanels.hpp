#ifndef PATCHPANELS_HPP
#define PATCHPANELS_HPP

#include <map>
#include <QCheckBox>
#include <QCloseEvent>
#include <QGridLayout>
#include <QTabWidget>
#include <vector>

#include <xlsxwriter.h>

namespace Ui {
class PatchPanels;
}

class PatchPanels : public QTabWidget
{
    Q_OBJECT
    Ui::PatchPanels *ui;
    QString m_filename;
    std::vector<QGridLayout *> m_layouts;
    std::map<int, int> m_portsPerPatchPanel;
    std::vector<std::map<int, bool>> m_verifiedPorts;
    std::map<QCheckBox *, QCheckBox *> m_checkBoxesPerLine;
    bool m_verified;

    void writeSpreadSheet();
public:
    explicit PatchPanels(QString name, std::map<int, int> portsPerPatchPanel, QWidget *parent = nullptr);
    ~PatchPanels();
protected:
    void closeEvent(QCloseEvent *event) override;
signals:
    void closed();
private slots:
    void onPortClicked();
    void onConfirmButtonClicked();
    void onCheckBoxClicked();
};

#endif // PATCHPANELS_HPP
