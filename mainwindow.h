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
#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDateTime>
#include <QProcess>
#include "common.h"
#include "logger.h"
#include "mdichild.h"
#include "qcustomplot.h"
#include "pciesubwindow.h"
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

    void showCurveGraph(QCustomPlot *customPlot);

    QCustomPlot *curveWidget;

    double num[10];
    int n;

    enum ITypes {
        IDriver = 1999,
        IFolder = 2000,
        IFile = 2001
    };

private:
    void createStatusBar();
    void createDockWindows();
    void createSubWindows();
    void updateFileView(const QString strDir);

protected:
    void closeEvent(QCloseEvent *event);
signals:
    void loggerWrite(const QString strContext);

public slots:
    void showGraph();
    void loggerOutput(const QString strContext);

private slots:
    void on_action_AddHighSpee_triggered();
    void on_action_AddMediumSpeed_triggered();
    void on_action_AddSingleWire_triggered();
    void on_action_AddTripleWire_triggered();
    void on_action_Save_triggered();
    void on_action_Database_triggered();
    void on_action_Parameter_triggered();
    void on_action_Back_triggered();
    void on_action_Forward_triggered();
    void on_action_Upper_triggered();
    void on_action_Lookup_triggered();
    void on_action_Curve_triggered();
    //
    void on_comboWorkspace_currentTextChanged(const QString &arg1);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);   

private:
    Logger *logger;
    QMdiArea *mdiArea;
    Ui::MainWindow *ui;
    QLabel *label;
    QListWidget *listWidget;
    QListWidget *logWidget;
    QTableView* tableView;
    QComboBox *comboWorkspace;
    QStringList comboDirList;
    QString strCurrentWorkspace;
    QDirModel *dirModel;

private:
    bool bControlWorkspace;
    QIcon m_iconDriver;
    QIcon m_iconFolder;
    QIcon m_iconFile;
    QDir  m_dirCur;
    void showItem(const QDir& dir);
    QString getFileInfo(const QFileInfo& fi);
    QString getFolderInfo(const QFileInfo& fi);
};

#endif // MAINWINDOW_H
