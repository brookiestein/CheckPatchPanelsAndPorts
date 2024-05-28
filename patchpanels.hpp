#ifndef PATCHPANELS_HPP
#define PATCHPANELS_HPP

#include <map>
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
    QString m_name;
    std::vector<QGridLayout *> m_layouts;
    std::map<int, int> m_portsPerPatchPanel;
    std::vector<std::map<int, bool>> m_verifiedPorts;

    void writeSpreadSheet();
public:
    explicit PatchPanels(QString name, std::map<int, int> portsPerPatchPanel, QWidget *parent = nullptr);
    ~PatchPanels();
protected:
    void closeEvent(QCloseEvent *event) override;
signals:
    void closed();
private slots:
    void onConfirmButtonClicked();
};

#endif // PATCHPANELS_HPP
