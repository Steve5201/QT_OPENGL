#ifndef OPENGLOBJECT_H
#define OPENGLOBJECT_H

#include <QMatrix4x4>
#include "openglmesh.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class OpenGLObject
{
public:
    OpenGLObject();
    OpenGLObject(QString file);
    OpenGLObject(const OpenGLObject &other);
    ~OpenGLObject();
    OpenGLObject& operator=(const OpenGLObject &other);

    void reset();
    bool isValid() const;
    bool getConfiged() const;
    void loadModel(QString file);
    QString getFileName() const;
    QMatrix4x4 getTransformationMat() const;
    QVector<OpenGLMesh> getMeshs() const;
    QVector<OpenGLMesh>& getMeshsRef();
    void scale(float factor);
    float getScaleFactor() const;
    QVector3D getPosition() const;
    void setPosition(const QVector3D &newPosition);
    void rotate(float angle, const QVector3D &vector);
    QVector<QVector3D> getRect3D() const;

private:
    bool is_valid;
    float scaleFactor;
    QString file_name;
    QVector3D position;
    QMatrix4x4 *transMat,*rotMat;
    QVector<OpenGLMesh> meshs;

    void processNode(aiNode *node, const aiScene *scene);
    OpenGLMesh processMesh(aiMesh *mesh, const aiScene *scene);
    QVector<OpenGLMesh::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

};

#endif // OPENGLOBJECT_H
