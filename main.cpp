#include <QColor>
#include <QObject>
#include <QDebug>
#include <QPropertyAnimation>
#include <QThread>

#include <QVector3D>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/qorbitcameracontroller.h>
#include <Qt3DExtras/qt3dwindow.h>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QMaterial>

#include <QtGui/QScreen>

#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

// Reference: https://code.woboq.org/qt5/qt3d/tests/manual/custom-mesh-cpp/main.cpp.html#37rawIndexArray

int main(int argc, char *argv[])
{
    // Application
    QApplication app(argc, argv);

    // Window
    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    QWidget *container = QWidget::createWindowContainer(view);
    QSize screenSize = view->screen()->size();
    container->setMinimumSize(QSize(400, 300));
    container->setMaximumSize(screenSize);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    widget->setWindowTitle(QStringLiteral("Hello Cube"));

    // Camera
    Qt3DRender::QCamera *camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    QVector3D originalPosition(0, 0, 50.0f);
    camera->setPosition(originalPosition);
    QVector3D originalViewCenter(0, 0, 0);
    camera->setViewCenter(originalViewCenter);

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
    cuboidTransform->setTranslation(QVector3D(15.0f, 0.0f, 0.0f));

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
    // Custom Mesh
    Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
    Qt3DRender::QGeometry *customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);
    Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);
    Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);

    QByteArray vertexBufferData;

    // Three vertex, three float and color each
    vertexBufferData.resize(8 * (3 + 3) * sizeof(float));

    // Vertices
    QVector3D v0(0.0f, 0.0f, 0.0f);
    QVector3D v1(1.0f, 0.0f, 0.0f);
    QVector3D v2(1.0f, 0.0f, 1.0f);
    QVector3D v3(0.0f, 0.0f, 1.0f);
    QVector3D v4(0.0f, 1.0f, 0.0f);
    QVector3D v5(1.0f, 1.0f, 0.0f);
    QVector3D v6(1.0f, 1.0f, 1.0f);
    QVector3D v7(0.0f, 1.0f, 1.0f);

    // Color
    QVector3D red(1.0f, 0.0f, 0.0f);
    QVector3D green(0.0f, 1.0f, 0.0f);
    QVector3D blue(0.0f, 0.0f, 1.0f);
    QVector3D white(0.0f, 0.0f, 0.0f);

    QVector<QVector3D> vertices = QVector<QVector3D>()
            << v0 << red
            << v1 << green
            << v2 << blue
            << v3 << white
            << v4 << blue
            << v5 << white
            << v6 << green
            << v7 << red;

    // What is this step?
    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;
    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }

    vertexDataBuffer->setData(vertexBufferData);

    // Indices
    QByteArray indexBufferData;
    indexBufferData.resize(2 * 6 * 3 * sizeof(ushort));
    ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

    // The order of vertext is important!!

    // Front square, triangle 1
    rawIndexArray[0] = 4;
    rawIndexArray[1] = 1;
    rawIndexArray[2] = 0;

    // Front square, triangle 2
    rawIndexArray[3] = 4;
    rawIndexArray[4] = 5;
    rawIndexArray[5] = 1;

    // Bottom square, triangle 1
    rawIndexArray[6] = 0;
    rawIndexArray[7] = 1;
    rawIndexArray[8] = 3;

    // Bottom square, triangle 2
    rawIndexArray[9] = 1;
    rawIndexArray[10] = 2;
    rawIndexArray[11] = 3;

    // Right square, triangle 1
    rawIndexArray[12] = 5;
    rawIndexArray[13] = 2;
    rawIndexArray[14] = 1;

    // Right square, triangle 2
    rawIndexArray[15] = 2;
    rawIndexArray[16] = 5;
    rawIndexArray[17] = 6;

    // Left square, triangle 1
    rawIndexArray[18] = 0;
    rawIndexArray[19] = 3;
    rawIndexArray[20] = 4;

    // Left square, triangle 2
    rawIndexArray[21] = 7;
    rawIndexArray[22] = 4;
    rawIndexArray[23] = 3;

    // Top square, triangle 1
    rawIndexArray[24] = 7;
    rawIndexArray[25] = 5;
    rawIndexArray[26] = 4;

    // Top square, triangle 2
    rawIndexArray[27] = 7;
    rawIndexArray[28] = 6;
    rawIndexArray[29] = 5;

    // Back square, triangle 1
    rawIndexArray[30] = 7;
    rawIndexArray[31] = 3;
    rawIndexArray[32] = 2;

    // Back square, triangle 2
    rawIndexArray[33] = 2;
    rawIndexArray[34] = 6;
    rawIndexArray[35] = 7;

    indexDataBuffer->setData(indexBufferData);

    // Attributes
    // Position Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setCount(8);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    // Color Attributes
    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    colorAttribute->setVertexSize(3);
    colorAttribute->setByteOffset(3 * sizeof(float));
    colorAttribute->setByteStride(6 * sizeof(float));
    colorAttribute->setCount(8);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setVertexSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(36);

    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(colorAttribute);
    customGeometry->addAttribute(indexAttribute);

    customMeshRenderer->setInstanceCount(2);
    customMeshRenderer->setIndexOffset(0);
    customMeshRenderer->setFirstInstance(0);
    customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    customMeshRenderer->setGeometry(customGeometry);
    customMeshRenderer->setVertexCount(36);

    customMeshEntity->addComponent(customMeshRenderer);
    customMeshEntity->addComponent(transform);
    customMeshEntity->addComponent(material);

    view->setRootEntity(rootEntity);
//    view->show();
//    view->resize(1600, 1200);

    // Create control widget
    QCommandLinkButton *info = new QCommandLinkButton();
    info->setText(QStringLiteral("Qt3D Cube"));
    info->setDescription(QString::fromLatin1("Showing custom cube and cube from cuboid."));
    info->setIconSize(QSize(0,0));

    // Zoom in button
    QPushButton *zoomInButton = new QPushButton(widget);
    zoomInButton->setText(QStringLiteral("Zoom In"));
    QPushButton *zoomOutButton = new QPushButton(widget);
    zoomOutButton->setText(QStringLiteral("Zoom Out"));
    QPushButton *moveUpButton = new QPushButton(widget);
    moveUpButton->setText(QStringLiteral("Move Up"));
    QPushButton *moveDownButton = new QPushButton(widget);
    moveDownButton->setText(QStringLiteral("Move Down"));
    QPushButton *moveLeftButton = new QPushButton(widget);
    moveLeftButton->setText(QStringLiteral("Move Left"));
    QPushButton *moveRightButton = new QPushButton(widget);
    moveRightButton->setText(QStringLiteral("Move Right"));
    QPushButton *resetViewButton = new QPushButton(widget);
    resetViewButton->setText(QStringLiteral("Reset View"));


    vLayout->addWidget(info);
    vLayout->addWidget(zoomInButton);
    vLayout->addWidget(zoomOutButton);
    vLayout->addWidget(moveUpButton);
    vLayout->addWidget(moveDownButton);
    vLayout->addWidget(moveLeftButton);
    vLayout->addWidget(moveRightButton);
    vLayout->addWidget(resetViewButton);
    float moveFactor = 5;
    float zoomFactor = 5;

    QObject::connect(zoomInButton, &QPushButton::clicked, camera, [ = ]{
        camera->translate(QVector3D(0, 0, zoomFactor));
        qInfo() << "Zoom in" << "Camera position: " << camera->position();
    });
    QObject::connect(zoomOutButton, &QPushButton::clicked,camera, [ = ]{
        camera->translate(QVector3D(0, 0, -zoomFactor));
        qInfo() << "Zoom out" << "Camera position: " << camera->position();
    });
    QObject::connect(moveUpButton, &QPushButton::clicked,camera, [ = ]{
        camera->translate(QVector3D(0, -moveFactor, 0));
        qInfo() << "Move up" << "Camera position: " << camera->position();
    });
    QObject::connect(moveDownButton, &QPushButton::clicked,camera, [ = ]{
        camera->translate(QVector3D(0, moveFactor, 0));
        qInfo() << "Move down" << "Camera position: " << camera->position();
    });
    QObject::connect(moveLeftButton, &QPushButton::clicked,camera, [ = ]{
        camera->translate(QVector3D(moveFactor, 0, 0));
        qInfo() << "Move left" << "Camera position: " << camera->position();
    });
    QObject::connect(moveRightButton, &QPushButton::clicked,camera, [ = ]{
        camera->translate(QVector3D(-moveFactor, 0, 0));
        qInfo() << "Move right" << "Camera position: " << camera->position();
    });
    QObject::connect(resetViewButton, &QPushButton::clicked,camera, [ = ]{
        camera->setPosition(originalPosition);
        camera->setViewCenter(originalViewCenter);
        qInfo() << "Reset view" << "Camera position: " << camera->position();
    });

    widget->show();
    widget->resize(1600, 1200);

    return app.exec();
}
