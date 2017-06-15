#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
#include <QLabel>
#include <QDockWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QMdiArea>
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

private:
    void createStatusBar();

    void createDockWindows();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    HighSpeedWindow *createMdiChild();

private slots:
    void on_action_Add_Device_triggered();

private:
    QMdiArea *mdiArea;
    Ui::MainWindow *ui;
    QLabel *label;
    QListWidget *customerList;
    QTreeWidget treeWidget;
    QTableWidget tableWidget;
};

#endif // MAINWINDOW_H
