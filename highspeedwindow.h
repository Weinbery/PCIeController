#ifndef HIGHSPEEDWINDOW_H
#define HIGHSPEEDWINDOW_H

#include <QWidget>

namespace Ui {
class HighSpeedWindow;
}

class HighSpeedWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HighSpeedWindow(QWidget *parent = 0);
    ~HighSpeedWindow();

    QSize sizeHint() const;

    static int sequenceNumber;

    QString getFile() const;

    QString getWindowTitle() const;

private slots:
    void on_comboBoxSendType_currentIndexChanged(int index);

    void on_pushButtonChooseFile_clicked();

private:
    Ui::HighSpeedWindow *ui;
    QString strWindowTitle;
    QString strFile;
};

#endif // HIGHSPEEDWINDOW_H
