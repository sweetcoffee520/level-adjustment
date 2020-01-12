#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionopenfile_triggered();

    void on_pushButton_clicked();

    void on_actionsave_triggered();

private:
    Ui::MainWindow *ui;
    QString path;
};
#endif // MAINWINDOW_H
