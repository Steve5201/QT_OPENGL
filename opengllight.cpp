#include "opengllight.h"

OpenGLLight::OpenGLLight()
{
    lightPos = {0,0,0};
    lightColor = {255,255,255};
    distance = 100;
    k0 = 1.0;
    k1 = 0.045;
    k2 = 0.0075;
    cutWidth = 0.1;
    cutOff = qCos(60.0 / 180.0 * M_PI);
    enableFlash = false;
    flashDirection = {0,-1,0};
}

OpenGLLight::OpenGLLight(QVector3D pos, QColor color)
{
    lightPos = pos;
    lightColor = color;
    distance = 100;
    k0 = 1.0;
    k1 = 0.045;
    k2 = 0.0075;
    cutWidth = 0.5;
    cutOff = qCos(60.0 / 180.0 * M_PI);
    enableFlash = false;
    flashDirection = {0,0,-1};
}

QVector3D OpenGLLight::getLightPos() const
{
    return lightPos;
}

void OpenGLLight::setLightPos(const QVector3D &newLightPos)
{
    lightPos = newLightPos;
}

QColor OpenGLLight::getLightColor() const
{
    return lightColor;
}

void OpenGLLight::setLightColor(const QColor &newLightColor)
{
    lightColor = newLightColor;
}

qreal OpenGLLight::getDistance() const
{
    return distance;
}

void OpenGLLight::setDistance(qreal newDistance)
{
    if(newDistance <= 0)
    {
        newDistance = 0.01;
    }
    distance = newDistance;
    qreal dliner = distance * 0.1;
    k2 = (99 * 0.1 - 1) / (distance * distance - dliner * dliner);
    k1 = (99 - distance * distance * k2) / newDistance;
}

qreal OpenGLLight::getK1() const
{
    return k1;
}

qreal OpenGLLight::getK2() const
{
    return k2;
}

bool OpenGLLight::getEnableFlash() const
{
    return enableFlash;
}

void OpenGLLight::setEnableFlash(bool newEnableFlash)
{
    enableFlash = newEnableFlash;
}

QVector3D OpenGLLight::getFlashDirection() const
{
    return flashDirection;
}

void OpenGLLight::setFlashDirection(const QVector3D &newFlashDirection)
{
    flashDirection = newFlashDirection;
}

qreal OpenGLLight::getCutOff(bool cos) const
{
    if(cos)
    {
        return cutOff;
    }
    else
    {
        return qAcos(cutOff) / M_PI * 180;
    }
}

void OpenGLLight::setCutOff(qreal newCutOff)
{
    if(newCutOff > 89.9)
    {
        newCutOff = 89.9;
    }
    if(newCutOff < 0.1)
    {
        newCutOff = 0.1;
    }
    cutOff = qCos(newCutOff / 180.0 * M_PI);
}

qreal OpenGLLight::getCutWidth() const
{
    return cutWidth;
}

void OpenGLLight::setCutWidth(qreal newCutWidth)
{
    cutWidth = newCutWidth;
}
