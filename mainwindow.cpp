#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    iniWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSelectedObject(const OpenGLObject *obj)
{
    QString model_file = obj->getFileName();
    ui->object_list->setCurrentText(model_file.section('/',-1));
}

void MainWindow::on_load_object_clicked()
{
    QString model_file = QFileDialog::getOpenFileName(this,"选择一个模型文件","D:/",tr("model files(*.obj;*.gltf;*.fbx)"));
    OpenGLObject *obj = new OpenGLObject;
    obj->loadModel(model_file);
    if(obj->isValid())
    {
        ui->openGLWidget->addObject(obj);
        ui->object_list->addItem(model_file.section('/',-1));
        currentObj = obj;
    }
}


void MainWindow::on_unload_object_clicked()
{
    int idx = ui->object_list->currentIndex();
    ui->openGLWidget->removeObject(idx);
    ui->object_list->removeItem(idx);
}


void MainWindow::on_start_run_clicked()
{
    ui->openGLWidget->startRun();
}

void MainWindow::iniWindow()
{
    isRuing = false;
    currentObj = nullptr;
    connect(ui->openGLWidget,SIGNAL(selectedObject(const OpenGLObject*)),this,SLOT(onSelectedObject(const OpenGLObject*)));
}


void MainWindow::on_reset_cam_clicked()
{
    OpenGLCamera cam;
    ui->openGLWidget->setCamera(cam);
}


void MainWindow::on_object_list_currentIndexChanged(int index)
{
    if(ui->openGLWidget->getObjectsRef().size() > index && index > -1)
    {
        currentObj = ui->openGLWidget->getObjectsRef()[index];
        ui->openGLWidget->setCurrentObject(currentObj);
    }
    else
    {
        currentObj = nullptr;
    }
}


void MainWindow::on_scale_valueChanged(double arg1)
{
    static double last = 0;
    if(currentObj)
    {
        double dx = arg1 - last;
        currentObj->scale(1 + dx);
        last = arg1;
    }
}


void MainWindow::on_cammspeed_valueChanged(double arg1)
{
    ui->openGLWidget->setCameraMoveSpeed(arg1);
}


void MainWindow::on_camrspeed_valueChanged(double arg1)
{
    ui->openGLWidget->setCameraRotateSpeed(arg1);
}


void MainWindow::on_fod_valueChanged(int value)
{
    ui->openGLWidget->setFod(value);
}


void MainWindow::on_fov_valueChanged(int value)
{
    ui->openGLWidget->setFov(value);
}


void MainWindow::on_rx_valueChanged(int value)
{
    static int last = 0;
    if(currentObj)
    {
        int dx = value - last;
        currentObj->rotate(dx,{1,0,0});
        last = value;
    }
}


void MainWindow::on_ry_valueChanged(int value)
{
    static int last = 0;
    if(currentObj)
    {
        int dx = value - last;
        currentObj->rotate(dx,{0,1,0});
        last = value;
    }
}


void MainWindow::on_rz_valueChanged(int value)
{
    static int last = 0;
    if(currentObj)
    {
        int dx = value - last;
        currentObj->rotate(dx,{0,0,1});
        last = value;
    }
}


void MainWindow::on_px_valueChanged(int value)
{
    if(currentObj)
    {
        QVector3D pos = currentObj->getPosition();
        pos.setX(value);
        currentObj->setPosition(pos);
    }
}


void MainWindow::on_py_valueChanged(int value)
{
    if(currentObj)
    {
        QVector3D pos = currentObj->getPosition();
        pos.setY(value);
        currentObj->setPosition(pos);
    }
}


void MainWindow::on_pz_valueChanged(int value)
{
    if(currentObj)
    {
        QVector3D pos = currentObj->getPosition();
        pos.setZ(value);
        currentObj->setPosition(pos);
    }
}


void MainWindow::on_reset_obj_clicked()
{
    currentObj->reset();
}

