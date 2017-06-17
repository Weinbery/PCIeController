#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
#include <QLabel>
#include <QDockWidget>
#include <QListWidget>
#include <QTreeView>
#include <QTableView>
#include <QComboBox>
#include <QMdiArea>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDirModel>
#include "mdichild.h"
#include "highspeedwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString getCurrentWorkspace() const;

    void setCurretWorkspace(QString strWorkspace);

private:
    void createStatusBar();

    void createDockWindows();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    HighSpeedWindow *createMdiChild();

private slots:
    void on_action_Add_Device_triggered();
    void on_action_Back_triggered();
    void on_action_Forward_triggered();
    void on_action_Upper_triggered();
    void on_action_Lookup_triggered();

    void on_action_Curve_triggered();

private:
    QMdiArea *mdiArea;
    Ui::MainWindow *ui;
    QLabel *label;
    QListWidget *customerList;
    QTreeView* treeView;
    QTableView* tableView;
    QComboBox *comboWorkspace;
    QStringList comboDirList;
    QString strCurrentWorkspace;
    QDirModel *dirModel;

private:
    bool bControlWorkspace;
};

#endif // MAINWINDOW_H
