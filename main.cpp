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

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <QPropertyAnimation>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qorbitcameracontroller.h>

#include<QDebug>

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


    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);
    // Torus
    Qt3DCore::QEntity *customMeshEntity = new Qt3DCore::QEntity(rootEntity);
    // Transform
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
    transform->setScale(8.0f);
    // Custom Mesh (TetraHedron)
    Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
    Qt3DRender::QGeometry *customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);
    Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);

    QByteArray vertexBufferData;

    // Three vertex, three float and color each
    vertexBufferData.resize(3 * (3 + 3) * sizeof(float));

    // Vertices
    QVector3D v0(-0.5f, -0.5f, 0.0f);
    QVector3D v1(0.5f, -0.5f, 0.0f);
    QVector3D v2(0.0f,  0.5f, 0.0f);

    // Color
    QVector3D red(1.0f, 0.0f, 0.0f);

    QVector<QVector3D> vertices = QVector<QVector3D>()
            << v0 << red
            << v1 << red
            << v2 << red;

    // What is this step?
    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;
    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }

    vertexDataBuffer->setData(vertexBufferData);

    // Attributes
    // Position Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setCount(3);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    // Color Attributes
    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    colorAttribute->setVertexSize(3);
    colorAttribute->setByteOffset(3 * sizeof(float));
    colorAttribute->setByteStride(6 * sizeof(float));
    colorAttribute->setCount(3);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(colorAttribute);

    customMeshRenderer->setInstanceCount(1);
    customMeshRenderer->setIndexOffset(0);
    customMeshRenderer->setFirstInstance(0);
    customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    customMeshRenderer->setGeometry(customGeometry);
    customMeshRenderer->setVertexCount(3);

    customMeshEntity->addComponent(customMeshRenderer);
    customMeshEntity->addComponent(transform);
    customMeshEntity->addComponent(material);

    view->setRootEntity(rootEntity);
    view->show();
    view->resize(1600, 1200);

    return app.exec();
}
