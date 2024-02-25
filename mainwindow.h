#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "openglobject.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectedObject(const OpenGLObject *obj);
    void on_load_object_clicked();
    void on_unload_object_clicked();
    void on_start_run_clicked();
    void on_reset_cam_clicked();
    void on_object_list_currentIndexChanged(int index);
    void on_scale_valueChanged(double arg1);
    void on_cammspeed_valueChanged(double arg1);
    void on_camrspeed_valueChanged(double arg1);
    void on_fod_valueChanged(int value);
    void on_fov_valueChanged(int value);
    void on_rx_valueChanged(int value);
    void on_ry_valueChanged(int value);
    void on_rz_valueChanged(int value);
    void on_px_valueChanged(int value);
    void on_py_valueChanged(int value);
    void on_pz_valueChanged(int value);
    void on_reset_obj_clicked();

private:
    Ui::MainWindow *ui;
    bool isRuing;
    OpenGLObject *currentObj;

    void iniWindow();

};
#endif // MAINWINDOW_H
