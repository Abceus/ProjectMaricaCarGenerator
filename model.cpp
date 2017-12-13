#include "model.h"

#include <QFile>
#include <memory>

Model::Model(QString filename, QOpenGLFunctions *f, QOpenGLExtraFunctions *ef)
{

    VChunkHeader GeneralHeader;
    VChunkHeader PointsHeader;
    std::unique_ptr<VPoint[]> PointsData;
    VChunkHeader WedgesHeader;
    std::unique_ptr<VVertex[]> WedgesData;
    VChunkHeader FacesHeader;
    std::unique_ptr<VTriangle[]> FacesData;
    VChunkHeader MaterialsHeader;
    std::unique_ptr<VMaterial[]> MaterialsData;

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
        return;


    file.read((char*)&GeneralHeader, sizeof(VChunkHeader));

    qDebug() << GeneralHeader.ChunkID << " " << GeneralHeader.DataCount << " " << GeneralHeader.DataSize << " " << GeneralHeader.TypeFlags;


    file.read((char*)&PointsHeader, sizeof(VChunkHeader));

    qDebug() << PointsHeader.ChunkID << " " << PointsHeader.DataCount << " " << PointsHeader.DataSize << " " << PointsHeader.TypeFlags;

    PointsData = std::unique_ptr<VPoint[]>(new VPoint[PointsHeader.DataCount]);

    for (int i = 0; i < PointsHeader.DataCount; i++)
    {
        file.read((char*)&PointsData[i], PointsHeader.DataSize);
        //qDebug() << PointsData[i].X << " " << PointsData[i].Y << " " << PointsData[i].Z;
    }


    file.read((char*)&WedgesHeader, sizeof(VChunkHeader));

    qDebug() << WedgesHeader.ChunkID << " " << WedgesHeader.DataCount << " " << WedgesHeader.DataSize << " " << WedgesHeader.TypeFlags;

    WedgesData = std::unique_ptr<VVertex[]>(new VVertex[WedgesHeader.DataCount]);

    for (int i = 0; i < WedgesHeader.DataCount; i++)
    {
        file.read((char*)&WedgesData[i], WedgesHeader.DataSize);
        //qDebug() << WedgesData[i].PointIndex << " " << WedgesData[i].U << " " << WedgesData[i].V;
    }

    file.read((char*)&FacesHeader, sizeof(VChunkHeader));

    qDebug() << FacesHeader.ChunkID << " " << FacesHeader.DataCount << " " << FacesHeader.DataSize << " " << FacesHeader.TypeFlags;

    FacesData = std::unique_ptr<VTriangle[]>(new VTriangle[FacesHeader.DataCount]);

    for (int i = 0; i < FacesHeader.DataCount; i++)
    {
        file.read((char*)&FacesData[i], FacesHeader.DataSize);
        //qDebug() << FacesData[i].WedgeIndex[0] << " " << FacesData[i].WedgeIndex[1] << " " << FacesData[i].WedgeIndex[2];
    }

    file.read((char*)&MaterialsHeader, sizeof(VChunkHeader));

    qDebug() << MaterialsHeader.ChunkID << " " << MaterialsHeader.DataCount << " " << MaterialsHeader.DataSize << " " << MaterialsHeader.TypeFlags;

    MaterialsData = std::unique_ptr<VMaterial[]>(new VMaterial[MaterialsHeader.DataCount]);

    for (int i = 0; i < MaterialsHeader.DataCount; i++)
    {
        file.read((char*)&MaterialsData[i], MaterialsHeader.DataSize);
        //qDebug() << MaterialsData[i].MaterialName << " " << MaterialsData[i].TextureIndex;
    }


    std::unique_ptr<GLfloat[]> vertices(new GLfloat[WedgesHeader.DataCount*5]);

    for(int i=0; i<WedgesHeader.DataCount; i++)
    {
        vertices[i*5] = PointsData[WedgesData[i].PointIndex].X;
        vertices[i*5+1] = PointsData[WedgesData[i].PointIndex].Y;
        vertices[i*5+2] = PointsData[WedgesData[i].PointIndex].Z;
        vertices[i*5+3] = WedgesData[i].U;
        vertices[i*5+4] = WedgesData[i].V;
    }

    std::unique_ptr<GLuint[]> indices(new GLuint[FacesHeader.DataCount*3]);

    for(int i=0; i<FacesHeader.DataCount; i++)
    {
        for(int j=0; j<3; j++)
        {
            indices[i*3+j] = FacesData[i].WedgeIndex[j];
        }
    }

    this->VAO = std::unique_ptr<QOpenGLVertexArrayObject>(new QOpenGLVertexArrayObject());
    this->VAO->create();
    this->VBO = std::unique_ptr<QOpenGLBuffer>(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));
    this->VBO->create();
    this->VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
    this->EBO = std::unique_ptr<QOpenGLBuffer>(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer));
    this->EBO->create();
    this->EBO->setUsagePattern(QOpenGLBuffer::StaticDraw);

    this->VAO->bind();

    this->VBO->bind();
    this->VBO->allocate(vertices.get(), sizeof(vertices.get())*WedgesHeader.DataCount*5);

    this->EBO->bind();
    this->EBO->allocate(indices.get(), sizeof(indices.get())*FacesHeader.DataCount*3);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    f->glEnableVertexAttribArray(1);

    this->VAO->release();



    file.close();

    this->setTexture(MaterialsData[0].MaterialName);
    this->VAOsize = WedgesHeader.DataCount;
}
/*
GLuint Model::getVAO()
{
    return VAO;
}
*/

QOpenGLVertexArrayObject* Model::getVAO() const
{
    return VAO.get();
}

QOpenGLBuffer* Model::getVBO() const
{
    return VBO.get();
}

QOpenGLBuffer* Model::getEBO() const
{
    return EBO.get();
}

int Model::getVAOsize()
{
    return this->VAOsize;
}

QOpenGLTexture* Model::getTexture() const
{
    return this->texture.get();
}

void Model::setTexture(QString filename)
{
    QImage image = QImage(filename);
    if(!image.isNull())
    {
        this->texture = std::unique_ptr<QOpenGLTexture>(new QOpenGLTexture(image));
    }
    else
    {
        //TODO: Load default image from texture manager
        this->texture = std::unique_ptr<QOpenGLTexture>(new QOpenGLTexture(QImage("./../ProjectMaricaCarGenerator/test.jpg")));
    }
}
