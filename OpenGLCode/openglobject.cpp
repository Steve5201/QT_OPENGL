#include "openglobject.h"

OpenGLObject::OpenGLObject()
{
    is_valid = false;
    scaleFactor = 1.0;
    transMat = new QMatrix4x4;
    rotMat = new QMatrix4x4;
    position = {0.0,0.0,0.0};
}

OpenGLObject::OpenGLObject(QString file)
{
    loadModel(file);
}

OpenGLObject::OpenGLObject(const OpenGLObject &other)
{
    is_valid = other.is_valid;
    file_name = other.file_name;
    position = other.position;
    transMat = new QMatrix4x4;
    *transMat = *other.transMat;
    rotMat = new QMatrix4x4;
    *rotMat = *other.rotMat;
    meshs = other.meshs;
}

OpenGLObject::~OpenGLObject()
{
    delete transMat;
}

OpenGLObject& OpenGLObject::operator=(const OpenGLObject &other)
{
    is_valid = other.is_valid;
    file_name = other.file_name;
    position = other.position;
    *transMat = *other.transMat;
    *rotMat = *other.rotMat;
    meshs = other.meshs;
    return *this;
}

void OpenGLObject::reset()
{
    scaleFactor = 1.0;
    if(transMat)
    {
        delete transMat;
        transMat = new QMatrix4x4;
    }
    if(rotMat)
    {
        delete rotMat;
        rotMat = new QMatrix4x4;
    }
}

bool OpenGLObject::isValid() const
{
    return is_valid;
}

bool OpenGLObject::getConfiged() const
{
    bool obj_configed = true;
    foreach (auto v, meshs)
    {
        if(v.getConfiged() == false)
        {
            obj_configed = false;
            break;
        }
    }
    return obj_configed;
}

void OpenGLObject::loadModel(QString file)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(file.toStdString(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        is_valid = false;
        return;
    }
    file_name = file;
    processNode(scene->mRootNode, scene);
    is_valid = true;
}

QString OpenGLObject::getFileName() const
{
    return file_name;
}

QMatrix4x4 OpenGLObject::getTransformationMat() const
{
    return (*transMat) * (*rotMat);
}

QVector<OpenGLMesh> OpenGLObject::getMeshs() const
{
    return meshs;
}

QVector<OpenGLMesh> &OpenGLObject::getMeshsRef()
{
    return meshs;
}

void OpenGLObject::scale(float factor)
{
    if(transMat)
    {
        scaleFactor *= factor;
        delete transMat;
        transMat = new QMatrix4x4;
        transMat->translate(position);
        QMatrix4x4 sc;
        sc.scale(scaleFactor);
        *transMat *= sc;
    }
}

float OpenGLObject::getScaleFactor() const
{
    return scaleFactor;
}

QVector3D OpenGLObject::getPosition() const
{
    return position;
}

void OpenGLObject::setPosition(const QVector3D &newPosition)
{
    if(transMat)
    {
        position = newPosition;
        delete transMat;
        transMat = new QMatrix4x4;
        transMat->translate(position);
        QMatrix4x4 sc;
        sc.scale(scaleFactor);
        *transMat *= sc;
    }
}

void OpenGLObject::rotate(float angle, const QVector3D &vector)
{
    if(rotMat)
    {
        QMatrix4x4 rot;
        rot.rotate(angle,vector);
        *rotMat *= rot;
    }
}

QVector<QVector3D> OpenGLObject::getRect3D() const
{
    QVector<QVector3D> result;
    QVector<QVector3D> vertices;
    foreach (auto v, meshs)
    {
        vertices.append(v.getRect3D());
    }
    qreal minx = vertices[0].x();
    qreal miny = vertices[0].y();
    qreal minz = vertices[0].z();
    qreal maxx = minx;
    qreal maxy = miny;
    qreal maxz = minz;
    foreach (auto vet, vertices)
    {
        qreal x = vet.x();
        qreal y = vet.y();
        qreal z = vet.z();
        if(x > maxx)
        {
            maxx = x;
        }
        if(x < minx)
        {
            minx = x;
        }
        if(y > maxy)
        {
            maxy = y;
        }
        if(y < miny)
        {
            miny = y;
        }
        if(z > maxz)
        {
            maxz = z;
        }
        if(z < minz)
        {
            minz = z;
        }
    }
    result.append(QVector3D(minx,miny,minz));
    result.append(QVector3D(minx,maxy,minz));
    result.append(QVector3D(minx,miny,maxz));
    result.append(QVector3D(minx,maxy,maxz));
    result.append(QVector3D(maxx,miny,minz));
    result.append(QVector3D(maxx,maxy,minz));
    result.append(QVector3D(maxx,miny,maxz));
    result.append(QVector3D(maxx,maxy,maxz));
    return result;
}

void OpenGLObject::processNode(aiNode *node, const aiScene *scene)
{
    if(!node || !scene)
    {
        return;
    }
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        OpenGLMesh amesh = processMesh(mesh, scene);
        if(!amesh.isNull())
        {
            amesh.setFileName(file_name);
            meshs.push_back(amesh);
        }
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

OpenGLMesh OpenGLObject::processMesh(aiMesh *mesh, const aiScene *scene)
{
    if(!mesh || !scene)
    {
        return OpenGLMesh();
    }
    QVector<OpenGLMesh::Vertex> vertices;
    QVector<unsigned int> indices;
    QVector<OpenGLMesh::Texture> textures;

    unsigned int count_vertices = mesh->mNumVertices;
    for(unsigned int i = 0; i < count_vertices; i++)
    {
        OpenGLMesh::Vertex v;
        QVector3D pos;
        if(mesh->mVertices)
        {
            pos.setX(mesh->mVertices[i].x);
            pos.setY(mesh->mVertices[i].y);
            pos.setZ(mesh->mVertices[i].z);
            v.position = pos;
        }
        QVector3D nor;
        if(mesh->mNormals)
        {
            nor.setX(mesh->mNormals[i].x);
            nor.setY(mesh->mNormals[i].y);
            nor.setZ(mesh->mNormals[i].z);
            v.normal = nor;
        }
        QVector2D tex;
        if(mesh->mTextureCoords[0])
        {
            tex.setX(mesh->mTextureCoords[0][i].x);
            tex.setY(mesh->mTextureCoords[0][i].y);
            v.texCoords = tex;
        }
        vertices.push_back(v);
    }

    unsigned int count_faces = mesh->mNumFaces;
    for(unsigned int i = 0; i < count_faces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        if(material)
        {
            QVector<OpenGLMesh::Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
            textures.append(diffuseMaps);
            QVector<OpenGLMesh::Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
            textures.append(specularMaps);
        }
    }

    return OpenGLMesh(vertices, indices, textures);
}

QVector<OpenGLMesh::Texture> OpenGLObject::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
    OpenGLMesh::Texture texture;
    QVector<OpenGLMesh::Texture> result;
    unsigned int count = mat->GetTextureCount(type);
    for(unsigned int i = 0; i < count; i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        texture.path = str.C_Str();
        if(type == aiTextureType_DIFFUSE)
        {
            texture.type = OpenGLMesh::DIFFUSE;
        }
        else if(type == aiTextureType_SPECULAR)
        {
            texture.type = OpenGLMesh::SPECULAR;
        }
        texture.ptex = nullptr;
        result.push_back(texture);
    }
    return result;
}
