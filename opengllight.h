#ifndef OPENGLLIGHT_H
#define OPENGLLIGHT_H

#include <QColor>
#include <QVector3D>

class OpenGLLight
{
public:
    OpenGLLight();
    OpenGLLight(QVector3D pos, QColor color);

    QVector3D getLightPos() const;
    void setLightPos(const QVector3D &newLightPos);
    QColor getLightColor() const;
    void setLightColor(const QColor &newLightColor);
    qreal getDistance() const;
    void setDistance(qreal newDistance);
    qreal getK1() const;
    qreal getK2() const;
    bool getEnableFlash() const;
    void setEnableFlash(bool newEnableFlash);
    QVector3D getFlashDirection() const;
    void setFlashDirection(const QVector3D &newFlashDirection);
    qreal getCutOff(bool cos = false) const;
    void setCutOff(qreal newCutOff);
    qreal getCutWidth() const;
    void setCutWidth(qreal newCutWidth);

private:
    QColor lightColor;
    QVector3D lightPos;
    qreal distance;
    qreal k0,k1,k2;
    bool enableFlash;
    qreal cutOff,cutWidth;
    QVector3D flashDirection;

};

#endif // OPENGLLIGHT_H
