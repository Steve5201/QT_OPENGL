#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    fov = 45.0f;
    fod = 555.1f;
    AmbientColor = {255,255,255,255};
    OpenGLLight light1,light2;
    light1.setDistance(60000);
    light2.setLightPos({0,-50,0});
    pointLights.append(light1);
    light2.setDistance(60000);
    light2.setEnableFlash(true);
    light2.setLightPos({0,50,0});
    pointLights.append(light2);
    AmbientStrength = 0.3f;
    DiffuseStrength = 0.7f;
    SpecularStrength = 0.7f;
    cameraMoveSpeed = 1.0f;
    cameraRotateSpeed = 0.1f;
    rotating = input_enabled = false;
    moveR = moveL = moveF = moveB = false;
    texture = nullptr;
    currentObject = nullptr;
    program = new QOpenGLShaderProgram(this);
    projectionMat.perspective(fov,width()/(float)height(),0.1f,fod);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(refresh()));
}

OpenGLWidget::~OpenGLWidget()
{
    delete timer;
    delete program;
    delete texture;
}

void OpenGLWidget::startRun()
{
    timer->start(15);
}

void OpenGLWidget::stopRun()
{
    timer->stop();
}

void OpenGLWidget::addObject(OpenGLObject *obj)
{
    objects.push_back(obj);
    currentObject = obj;
    this->update();
}

void OpenGLWidget::removeObject(const int objidx)
{
    if(objects[objidx] == currentObject)
    {
        currentObject = nullptr;
    }
    if(objidx > -1)
    {
        objects.removeAt(objidx);
    }
    this->update();
}

void OpenGLWidget::removeObject(const OpenGLObject *obj)
{
    int idx = objects.indexOf(obj);
    if(objects[idx] == currentObject)
    {
        currentObject = nullptr;
    }
    if(idx > -1)
    {
        objects.removeAt(idx);
    }
    this->update();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    configProgram();
}

void OpenGLWidget::paintGL()
{
    enableSelectMode(false);
    updateAmbientColor(AmbientColor);
    updateAmbientStrength(AmbientStrength);
    updatePointLights(pointLights);
    updateDiffuseStrength(DiffuseStrength);
    updateSpecularStrength(SpecularStrength);
    updateProgramView(camera.getViewMat());
    updateProgramprojection(projectionMat);
    if(program_valid)
    {
        if(objects.size()>0)
        {
            foreach (auto obj, objects)
            {
                updateProgramModel(obj->getTransformationMat());
                drawObject(*obj);
            }
        }
    }
    if(currentObject)
    {
        enableSelectMode(true);
        setSelectId({1.0,0.0,0.0,1.0});
        updateProgramModel(currentObject->getTransformationMat());
        QVector<QVector3D> rect3D = currentObject->getRect3D();
        float vertices [24] = {rect3D[0].x(),rect3D[0].y(),rect3D[0].z(),
                            rect3D[1].x(),rect3D[1].y(),rect3D[1].z(),
                            rect3D[2].x(),rect3D[2].y(),rect3D[2].z(),
                            rect3D[3].x(),rect3D[3].y(),rect3D[3].z(),
                            rect3D[4].x(),rect3D[4].y(),rect3D[4].z(),
                            rect3D[5].x(),rect3D[5].y(),rect3D[5].z(),
                            rect3D[6].x(),rect3D[6].y(),rect3D[6].z(),
                            rect3D[7].x(),rect3D[7].y(),rect3D[7].z()};
        unsigned int indices[24] = {0,1,3,2, 3,2,6,7, 6,7,5,4, 5,4,0,1, 0,4,6,2, 1,3,7,5};
        unsigned int VBO;
        unsigned int EBO;
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)16);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)32);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)48);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)64);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)80);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void OpenGLWidget::resizeGL(int w, int h)
{
    QMatrix4x4 prj;
    prj.perspective(fov,width()/(float)height(),0.1f,fod);
    projectionMat = prj;
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 120;
    if (!numDegrees.isNull())
    {
        float size = numDegrees.y() / 10.1 + 1.0;
        if(size < 0)
        {
            size = 0.01f;
        }
        currentObject->scale(size);
    }
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W:
        moveF = true;
        break;
    case Qt::Key_S:
        moveB = true;
        break;
    case Qt::Key_A:
        moveL = true;
        break;
    case Qt::Key_D:
        moveR = true;
        break;
    case Qt::Key_Escape:
        input_enabled = false;
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        break;
    default:
        break;
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W:
        moveF = false;
        break;
    case Qt::Key_S:
        moveB = false;
        break;
    case Qt::Key_A:
        moveL = false;
        break;
    case Qt::Key_D:
        moveR = false;
        break;
    default:
        break;
    }
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        lastPos = QCursor::pos();
        selectStartPos = event->pos();
        startRun();
        setFocus();
        rotating = true;
        input_enabled = true;
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }
    if(event->button() == Qt::RightButton)
    {
        OpenGLObject *obj = getSelectObject(event->position());
        if(obj)
        {
            emit selectedObject(obj);
            currentObject = obj;
        }
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        rotating = false;
        selectStopPos = event->pos();
    }
}

void OpenGLWidget::refresh()
{
    if(input_enabled)
    {
        QPoint mousePos = QCursor::pos();
        QPoint widgetPos = this->mapToGlobal(QPoint(0,0));
        if(mousePos.x() > widgetPos.x() + this->width())
        {
            lastPos.setX(widgetPos.x());
            mousePos.setX(widgetPos.x() + 1);
        }
        if(mousePos.x() < widgetPos.x())
        {
            lastPos.setX(widgetPos.x() + this->width());
            mousePos.setX(widgetPos.x() + this->width() - 1);
        }
        if(mousePos.y() > widgetPos.y() + this->height())
        {
            lastPos.setY(widgetPos.y());
            mousePos.setY(widgetPos.y() + 1);
        }
        if(mousePos.y() < widgetPos.y())
        {
            lastPos.setY(widgetPos.y() + this->height());
            mousePos.setY(widgetPos.y() + this->height() - 1);
        }
        QCursor::setPos(mousePos);
        if(lastPos.x() == 0 && lastPos.y() ==0)
        {
            lastPos = mousePos;
            return;
        }
        int dx = mousePos.x() - lastPos.x();
        int dy = mousePos.y() - lastPos.y();
        if(abs(dx) > abs(dy))
        {
            if(!rotating)
            {
                camera.rotate(dx * cameraRotateSpeed,{0,1,0});
            }
            if(rotating && currentObject)
            {
                currentObject->rotate(dx * 0.1,{0,1,0});
            }
        }
        else if(abs(dx) < abs(dy))
        {
            if(!rotating)
            {
                camera.rotate(dy * cameraRotateSpeed,camera.getCameraRight());
            }
            if(rotating && currentObject)
            {
                currentObject->rotate(dy * 0.1,{1,0,0});
            }
        }
        lastPos = mousePos;
        if(moveF)
        {
            camera.moveFront(cameraMoveSpeed);
        }
        if(moveB)
        {
            camera.moveBack(cameraMoveSpeed);
        }
        if(moveR)
        {
            camera.moveRight(cameraMoveSpeed);
        }
        if(moveL)
        {
            camera.moveLeft(cameraMoveSpeed);
        }
    }
    this->update();
}

float OpenGLWidget::getSpecularStrength() const
{
    return SpecularStrength;
}

void OpenGLWidget::setSpecularStrength(float newSpecularStrength)
{
    SpecularStrength = newSpecularStrength;
}

float OpenGLWidget::getAmbientStrength() const
{
    return AmbientStrength;
}

void OpenGLWidget::setAmbientStrength(float newAmbientStrength)
{
    AmbientStrength = newAmbientStrength;
}

QColor OpenGLWidget::getAmbientColor() const
{
    return AmbientColor;
}

void OpenGLWidget::setAmbientColor(const QColor &newAmbientColor)
{
    AmbientColor = newAmbientColor;
}

OpenGLObject *OpenGLWidget::getCurrentObject() const
{
    return currentObject;
}

void OpenGLWidget::setCurrentObject(OpenGLObject *newCurrentObject)
{
    currentObject = newCurrentObject;
}

OpenGLCamera OpenGLWidget::getCamera() const
{
    return camera;
}

void OpenGLWidget::setCamera(const OpenGLCamera &newCamera)
{
    camera = newCamera;
}

float OpenGLWidget::getFod() const
{
    return fod;
}

void OpenGLWidget::setFod(float newFod)
{
    fod = newFod;
    QMatrix4x4 prj;
    prj.perspective(fov,width()/(float)height(),0.1f,fod);
    projectionMat = prj;
}

float OpenGLWidget::getFov() const
{
    return fov;
}

void OpenGLWidget::setFov(float newFov)
{
    fov = newFov;
    QMatrix4x4 prj;
    prj.perspective(fov,width()/(float)height(),0.1f,fod);
    projectionMat = prj;
}

float OpenGLWidget::getCameraRotateSpeed() const
{
    return cameraRotateSpeed;
}

void OpenGLWidget::setCameraRotateSpeed(float newCameraRotateSpeed)
{
    cameraRotateSpeed = newCameraRotateSpeed;
}

float OpenGLWidget::getCameraMoveSpeed() const
{
    return cameraMoveSpeed;
}

void OpenGLWidget::setCameraMoveSpeed(float newCameraMoveSpeed)
{
    cameraMoveSpeed = newCameraMoveSpeed;
}

QVector<OpenGLObject*> OpenGLWidget::getObjects() const
{
    return objects;
}

QVector<OpenGLObject*> &OpenGLWidget::getObjectsRef()
{
    return objects;
}

QMatrix4x4 OpenGLWidget::getProjectionMat() const
{
    return projectionMat;
}

void OpenGLWidget::setProjectionMat(const QMatrix4x4 &newProjectionMat)
{
    projectionMat = newProjectionMat;
}

void OpenGLWidget::configMesh(OpenGLMesh &mesh, QOpenGLFramebufferObject *fbo)
{
    makeCurrent();
    if(fbo)
    {
        fbo->bind();
    }
    QOpenGLBuffer *vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer *ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    QOpenGLVertexArrayObject *vao = new QOpenGLVertexArrayObject;
    if(!vbo->create() || !ebo->create() || !vao->create())
    {
        delete vbo;
        delete ebo;
        delete vao;
        return;
    }
    QVector<quint32> &indices = mesh.getIndicesRef();
    QVector<OpenGLMesh::Vertex> &vertexs = mesh.getVerticesRef();
    vao->bind();
    ebo->bind();
    vbo->bind();
    ebo->allocate(indices.data(),indices.size()*(int)sizeof(quint32));
    vbo->allocate(vertexs.data(),vertexs.size()*(int)sizeof(OpenGLMesh::Vertex));
    updateProgramPointer();
    vao->release();
    ebo->release();
    vbo->release();
    mesh.setVAO(vao);
    mesh.setEBO(ebo);
    mesh.setVBO(vbo);

    QImage image;
    QString str_path;
    QVector<OpenGLMesh::Texture> &textures = mesh.getTexturesRef();
    for (int i = 0; i < textures.size(); ++i)
    {
        OpenGLMesh::Texture &tex = textures[i];
        str_path = mesh.getFileName().mid(0, mesh.getFileName().lastIndexOf('/') + 1) + tex.path;
        if(loaded_textures.contains(str_path))
        {
            tex.ptex = loaded_textures.value(str_path);
        }
        else
        {
            image.load(str_path);
            if(!image.isNull())
            {
                texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
                if(texture && texture->create())
                {
                    texture->bind();
                    texture->setData(image);
                    texture->allocateStorage();
                    texture->release();
                    tex.ptex = texture;
                    loaded_textures.insert(str_path,texture);
                    texture = nullptr;
                }
            }
        }
    }
}

void OpenGLWidget::drawMesh(OpenGLMesh &mesh, QOpenGLFramebufferObject *fbo)
{
    makeCurrent();
    if(fbo)
    {
        fbo->bind();
    }
    if(!mesh.getConfiged())
    {
        configMesh(mesh);
    }
    mesh.getVAO()->bind();
    QVector<OpenGLMesh::Texture> textures = mesh.getTextures();
    QString sampler;
    int idx_diff = 0, idx_spec = 0;
    for (int i = 0; i < textures.size() && i < 15; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        OpenGLMesh::Texture &tex = textures[i];
        tex.ptex->bind();
        if(tex.type == OpenGLMesh::DIFFUSE && idx_diff < 1)
        {
            sampler = "DIFFUSE" + QString::asprintf("%02d",idx_diff);
            idx_diff++;
        }
        else if(tex.type == OpenGLMesh::SPECULAR && idx_spec < 1)
        {
            sampler = "SPECULAR" + QString::asprintf("%02d",idx_spec);
            idx_spec++;
        }
        int pos = program->uniformLocation(sampler);
        if(pos > -1)
        {
            program->setUniformValue(pos,i);
        }
        sampler = "";
    }
    glActiveTexture(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES,mesh.getIndicesSize(),GL_UNSIGNED_INT,(void*)0);
    mesh.getVAO()->release();
    for (int i = 0; i < textures.size() && i < 15; ++i)
    {
        textures[i].ptex->release();
    }
}

void OpenGLWidget::configObject(OpenGLObject &obj, QOpenGLFramebufferObject *fbo)
{
    for (int i = 0; i < obj.getMeshsRef().size(); ++i)
    {
        OpenGLMesh &mesh = obj.getMeshsRef()[i];
        configMesh(mesh, fbo);
    }
}

void OpenGLWidget::drawObject(OpenGLObject &obj, QOpenGLFramebufferObject *fbo)
{
    for (int i = 0; i < obj.getMeshsRef().size(); ++i)
    {
        OpenGLMesh &mesh = obj.getMeshsRef()[i];
        drawMesh(mesh, fbo);
    }
}

void OpenGLWidget::configProgram()
{
    makeCurrent();
    program_valid = false;
    if(!program->isLinked())
    {
        if(!program->addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/Vertex.shd"))
        {
            log = "顶点着色器编译失败";
        }
        if(!program->addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/Fragment.shd"))
        {
            log = "片断着色器编译失败";
        }
        if(!program->link())
        {
            log = "着色器程序链接失败";
        }
        program->removeAllShaders();
    }
    if(!program->bind())
    {
        log = "着色器绑定失败";
    }
    program_valid = true;
}

void OpenGLWidget::updateProgramPointer()
{
    makeCurrent();
    if(program_valid)
    {
        int idx = program->attributeLocation("aPos");
        if(idx == -1)
        {
            log = "aPos设置失败";
        }
        else
        {
            program->setAttributeBuffer(idx,GL_FLOAT,0,3,32);
            program->enableAttributeArray(idx);
        }
        idx = program->attributeLocation("aNormal");
        if(idx == -1)
        {
            log = "aNormal设置失败";
        }
        else
        {
            program->setAttributeBuffer(idx,GL_FLOAT,12,3,32);
            program->enableAttributeArray(idx);
        }
        idx = program->attributeLocation("aTexCoords");
        if(idx == -1)
        {
            log = "aTexCoords设置失败";
        }
        else
        {
            program->setAttributeBuffer(idx,GL_FLOAT,24,2,32);
            program->enableAttributeArray(idx);
        }
    }
}

void OpenGLWidget::updateProgramModel(const QMatrix4x4 &mat)
{
    makeCurrent();
    if(program_valid)
    {
        int idx = program->uniformLocation("model");
        if(idx != -1)
        {
            program->setUniformValue(idx,mat);
        }
        idx = program->uniformLocation("normalMatrix");
        if(idx != -1)
        {
            program->setUniformValue(idx,mat.inverted().transposed());
        }
    }
}

void OpenGLWidget::updateProgramView(const QMatrix4x4 &mat)
{
    makeCurrent();
    if(program_valid)
    {
        int idx = program->uniformLocation("view");
        if(idx != -1)
        {
            program->setUniformValue(idx,mat);
        }
    }
}

void OpenGLWidget::updateProgramprojection(const QMatrix4x4 &mat)
{
    makeCurrent();
    if(program_valid)
    {
        int idx = program->uniformLocation("projection");
        if(idx != -1)
        {
            program->setUniformValue(idx,mat);
        }
    }
}

void OpenGLWidget::updateAmbientColor(const QColor &color)
{
    makeCurrent();
    if(program_valid)
    {
        int idx = program->uniformLocation("AmbientColor");
        if(idx != -1)
        {
            program->setUniformValue(idx,color);
        }
    }
}

void OpenGLWidget::updateAmbientStrength(const float strength)
{
    makeCurrent();
    if(program_valid)
    {
        int idx = program->uniformLocation("AmbientStrength");
        if(idx != -1)
        {
            program->setUniformValue(idx,strength);
        }
    }
}

void OpenGLWidget::updateDiffuseStrength(const float strength)
{
    makeCurrent();
    if(program_valid)
    {
        int idx = program->uniformLocation("DiffuseStrength");
        if(idx != -1)
        {
            program->setUniformValue(idx,strength);
        }
    }
}

void OpenGLWidget::updateSpecularStrength(const float strength)
{
    makeCurrent();
    if(program_valid)
    {
        int idx = program->uniformLocation("SpecularStrength");
        if(idx != -1)
        {
            program->setUniformValue(idx,strength);
        }
    }
}

void OpenGLWidget::updatePointLights(const QVector<OpenGLLight> &lights)
{
    makeCurrent();
    int num = lights.size();
    int idx = program->uniformLocation("numPointLights");
    if(idx != -1)
    {
        program->setUniformValue(idx,num);
    }
    idx = program->uniformLocation("eyePos");
    if(idx != -1)
    {
        program->setUniformValue(idx,camera.getCameraPosition());
    }
    for (int i = 0; i < num; ++i)
    {
        QString name;
        if(program_valid)
        {
            name = QString::asprintf("pointLights[%d].lightPos",i);
            idx = program->uniformLocation(name);
            if(idx != -1)
            {
                program->setUniformValue(idx,lights[i].getLightPos());
            }
            name = QString::asprintf("pointLights[%d].lightColor",i);
            idx = program->uniformLocation(name);
            if(idx != -1)
            {
                program->setUniformValue(idx,lights[i].getLightColor());
            }
            name = QString::asprintf("pointLights[%d].k1",i);
            idx = program->uniformLocation(name);
            if(idx != -1)
            {
                program->setUniformValue(idx,(float)lights[i].getK1());
            }
            name = QString::asprintf("pointLights[%d].k2",i);
            idx = program->uniformLocation(name);
            if(idx != -1)
            {
                program->setUniformValue(idx,(float)lights[i].getK2());
            }
            if(lights[i].getEnableFlash())
            {
                name = QString::asprintf("pointLights[%d].flash",i);
                idx = program->uniformLocation(name);
                if(idx != -1)
                {
                    program->setUniformValue(idx,1);
                }
                name = QString::asprintf("pointLights[%d].cutOff",i);
                idx = program->uniformLocation(name);
                if(idx != -1)
                {
                    program->setUniformValue(idx,(float)lights[i].getCutOff(true));
                }
                name = QString::asprintf("pointLights[%d].cutWidth",i);
                idx = program->uniformLocation(name);
                if(idx != -1)
                {
                    program->setUniformValue(idx,(float)lights[i].getCutWidth());
                }
                name = QString::asprintf("pointLights[%d].lightDirection",i);
                idx = program->uniformLocation(name);
                if(idx != -1)
                {
                    program->setUniformValue(idx,lights[i].getFlashDirection().normalized());
                }
            }
        }
    }
}

void OpenGLWidget::enableSelectMode(bool select)
{
    makeCurrent();
    int idx = program->uniformLocation("select");
    if(idx != -1)
    {
        program->setUniformValue(idx,select);
    }
}

void OpenGLWidget::setSelectId(const QVector4D &id)
{
    makeCurrent();
    int idx = program->uniformLocation("selectId");
    if(idx != -1)
    {
        program->setUniformValue(idx,id);
    }
}

OpenGLObject *OpenGLWidget::getSelectObject(QPointF pos)
{
    OpenGLObject *result = nullptr;
    QHash<quint32,OpenGLObject*> mapper;
    QOpenGLFramebufferObject *fbo = new QOpenGLFramebufferObject(this->grabFramebuffer().size(),QOpenGLFramebufferObject::Depth);
    glClearColor(1.f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    enableSelectMode(true);
    if(objects.size()>0)
    {
        for (int i = 0; i < objects.size(); ++i)
        {
            updateProgramModel(objects[i]->getTransformationMat());
            QColor id(i);
            setSelectId({id.redF(),id.greenF(),id.blueF(),id.alphaF()});
            mapper.insert(i,objects[i]);
            drawObject(*objects[i],fbo);
        }
    }
    QImage img = fbo->toImage().scaled(this->width(),this->height());
    quint32 key = img.pixel(pos.toPoint());
    key &= 16777215;
    result = mapper.value(key);
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    return result;
}
