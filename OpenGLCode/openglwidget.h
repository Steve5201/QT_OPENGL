#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QHash>
#include <QTimer>
#include <QLabel>
#include <QImage>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include "opengllight.h"
#include "openglcamera.h"
#include "openglobject.h"

class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    void startRun();
    void stopRun();
    void addObject(OpenGLObject *obj);
    void removeObject(const int objidx);
    void removeObject(const OpenGLObject *obj);
    QMatrix4x4 getProjectionMat() const;
    void setProjectionMat(const QMatrix4x4 &newProjectionMat);
    QVector<OpenGLObject*> getObjects() const;
    QVector<OpenGLObject*>& getObjectsRef();
    float getCameraMoveSpeed() const;
    void setCameraMoveSpeed(float newCameraMoveSpeed);
    float getCameraRotateSpeed() const;
    void setCameraRotateSpeed(float newCameraRotateSpeed);
    float getFov() const;
    void setFov(float newFov);
    float getFod() const;
    void setFod(float newFod);
    OpenGLCamera getCamera() const;
    void setCamera(const OpenGLCamera &newCamera);
    OpenGLObject *getCurrentObject() const;
    void setCurrentObject(OpenGLObject *newCurrentObject);
    QColor getAmbientColor() const;
    void setAmbientColor(const QColor &newAmbientColor);
    float getAmbientStrength() const;
    void setAmbientStrength(float newAmbientStrength);
    float getSpecularStrength() const;
    void setSpecularStrength(float newSpecularStrength);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void selectedObject(const OpenGLObject *obj);

private slots:
    void refresh();

private:
    bool program_valid;
    bool input_enabled;
    bool rotating;
    float fov,fod;
    float cameraMoveSpeed;
    float cameraRotateSpeed;
    bool moveR,moveL,moveF,moveB;
    QString log;
    QTimer *timer;
    QPoint lastPos;
    QPoint selectStartPos;
    QPoint selectStopPos;
    QColor AmbientColor;
    float AmbientStrength;
    float DiffuseStrength;
    float SpecularStrength;
    OpenGLCamera camera;
    OpenGLObject *currentObject;
    QOpenGLTexture *texture;
    QOpenGLShaderProgram *program;
    QVector<OpenGLLight> pointLights;
    QVector<OpenGLObject*> objects;
    QHash<QString,QOpenGLTexture*> loaded_textures;
    QMatrix4x4 modelMat,viewMat,projectionMat;

    void configMesh(OpenGLMesh &mesh, QOpenGLFramebufferObject *fbo = nullptr);
    void drawMesh(OpenGLMesh &mesh, QOpenGLFramebufferObject *fbo = nullptr);
    void configObject(OpenGLObject &obj, QOpenGLFramebufferObject *fbo = nullptr);
    void drawObject(OpenGLObject &obj, QOpenGLFramebufferObject *fbo = nullptr);
    void configProgram();
    void updateProgramPointer();
    void updateProgramModel(const QMatrix4x4 &mat);
    void updateProgramView(const QMatrix4x4 &mat);
    void updateProgramprojection(const QMatrix4x4 &mat);
    void updateAmbientColor(const QColor &color);
    void updateAmbientStrength(const float strength);
    void updateDiffuseStrength(const float strength);
    void updateSpecularStrength(const float strength);
    void updatePointLights(const QVector<OpenGLLight> &lights);
    void enableSelectMode(bool select);
    void setSelectId(const QVector4D &id);
    OpenGLObject* getSelectObject(QPointF pos);

};

#endif // OPENGLWIDGET_H
