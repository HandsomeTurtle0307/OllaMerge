#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void on_selectBaseBtn_clicked();
    void on_selectLoraBtn_clicked();
    void on_mergeBtn_clicked();
    void appendLog(const QString &message);
    void setStatus(const QString &status);
    void on_aboutBtn_clicked();
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
