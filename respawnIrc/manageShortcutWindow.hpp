#ifndef MANAGESHORTCUTWINDOW_HPP
#define MANAGESHORTCUTWINDOW_HPP

#include <QWidget>
#include <QTextEdit>
#include <QTreeView>
#include <QStandardItemModel>

#include "baseDialog.hpp"

class manageShortcutWindowClass : public baseDialogClass
{
    Q_OBJECT
public:
    explicit manageShortcutWindowClass(QWidget* parent);
private:
    void initializeShortcutsModel();
    void addRowToModel(QString base, QString replacement);
    void updateShortcutRuleAndPreview();
private slots:
    void addNewShortcut(QString base, QString replacement);
    void setCurrentShortcut(QString base, QString replacement);
    void showAddShortcutWindow();
    void showEditShortcutWindow();
    void removeShortcut();
    void moveUpShortcut();
    void moveDownShortcut();
    void textInTextZoneChanged();
    void validateShortcuts();
private:
    QTreeView* shortcutsListView;
    QStandardItemModel* shortcutsItemModel;
    QTextEdit* textZone;
    QTextEdit* previewZone;
};

#endif
