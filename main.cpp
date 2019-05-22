#include <QGuiApplication>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DRender/QMaterial>
#include <QVector3D>
#include <QColor>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QOrbitCameraController>

int main(int argc, char *argv[])
{
    // Application
    QGuiApplication app(argc, argv);

    // Window
    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 50.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // Camera control
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setLookSpeed(180.0f);
    camController->setLinearSpeed(50.0f);
    camController->setCamera(camera);

    // Cuboid mesh data
    Qt3DExtras::QCuboidMesh *cuboid = new Qt3DExtras::QCuboidMesh();
    cuboid->setXExtent(2);
    cuboid->setYExtent(2);
    cuboid->setZExtent(2);

    // Cuboid mesh transform
    Qt3DCore::QTransform *cuboidTransform = new Qt3DCore::QTransform();
    cuboidTransform->setScale(3.0);
    cuboidTransform->setTranslation(QVector3D(10.0f, 10.0f, 0.0f));

    // Cuboid material
    Qt3DExtras::QPhongAlphaMaterial *cuboidMaterial = new Qt3DExtras::QPhongAlphaMaterial(rootEntity);
    cuboidMaterial->setDiffuse(QColor(0, 0, 255, 255));
    cuboidMaterial->setAmbient(QColor(0, 0, 255, 255));
    cuboidMaterial->setSpecular(QColor(0, 0, 255, 255));
    cuboidMaterial->setAlpha(0.5);
    cuboidMaterial->setShininess(1.0);

    // Cuboid entity
    Qt3DCore::QEntity *cuboidEntity = new Qt3DCore::QEntity(rootEntity);
    cuboidEntity->addComponent(cuboid);
    cuboidEntity->addComponent(cuboidTransform);
    cuboidEntity->addComponent(cuboidMaterial);
    cuboidEntity->setEnabled(true);

    view->setRootEntity(rootEntity);
    view->show();
    view->resize(1600, 1200);

    return app.exec();
}
