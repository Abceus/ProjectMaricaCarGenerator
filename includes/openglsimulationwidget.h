#pragma once

#include <vector>
#include <memory>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QString>
#include <QWheelEvent>
#include <QMatrix4x4>

#include "physics/physicworld.h"
#include "render_system/scene.h"
#include "updatable.h"

class OpenglSimulationWidget :public QOpenGLWidget
{
Q_OBJECT
public:
    explicit OpenglSimulationWidget( QWidget *parent = nullptr );
    ~OpenglSimulationWidget() override = default;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void rewriteThisShit( const QString &filename );
    void closeEvent(QCloseEvent *event) override;
    void init();
private:
    QVector<QSharedPointer<IUpdatable>> m_objects;
    QMatrix4x4 projection;
    QMap<int, bool> keys;
    QSharedPointer<PhysicWorld> physicWorld;
    Scene scene;
    std::chrono::time_point<std::chrono::high_resolution_clock> prevTime;

    QSharedPointer<PhysObject> m_tire;
protected:
    void initializeGL() override;
    void resizeGL( int w, int h ) override;
    void paintGL() override;
signals:
    void closed();
};
