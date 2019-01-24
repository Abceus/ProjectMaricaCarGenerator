#ifndef MODEL_H
#define MODEL_H

#include <memory>

#include <QString>
#include <QVector>
#include <QVector4D>
#include <QQuaternion>
#include <QPicture>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <GL/glu.h>
#include <GL/gl.h>

#include "unanimation.h"

class Model
{
public:
    Model();
    Model(const QString& filename, QOpenGLFunctions *f, QOpenGLExtraFunctions* ef);
    ~Model();
    void bindVAO();
    void releaseVAO();
    void bindTexture( size_t index );
    void setTexture( QString filename, size_t index );
    void addTexture(QString filename);
    size_t getTexturesSize();
    size_t getVAOsize();
    size_t getTextureQueue( size_t index );
    void setTextureQueue( size_t index, float average );
private:
    std::unique_ptr<QOpenGLVertexArrayObject> VAO;
    std::unique_ptr<QOpenGLBuffer> VBO, EBO;
    std::vector<std::unique_ptr<QOpenGLTexture>> textures;
    std::vector<std::pair<size_t, float>> textureQueue;
    size_t VAOsize;
    void sortTextures();
    float averageAlpha(QImage image);
};

#endif // MODEL_H