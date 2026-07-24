/*********************************************************************
 * 文件名称: drawing_window.cpp
 * 文件功能: 六轴机械臂执行画板模块
 * 
 * 主要功能说明:
 * 1. DrawingWindow 类 - 绘图主窗口，提供完整的轨迹绘制和管理界面
 *    - 工具栏：清空、撤销、重做、多种绘图工具选择
 *    - 绘图工具：自由线、直线、矩形、圆形、圆弧
 *    - 点位操作：提取点位、发送执行、停止执行、查看点位文件、保存点位
 *    - 坐标系管理：支持工件坐标系和世界坐标系切换
 *    - 运动参数设置：速度、加速度、减速度、加加速度、拐角过渡、末端姿态
 * 
 * 2. DrawingCanvas 类 - 画布组件，处理底层绘图逻辑
 *    - 鼠标事件处理：按下、移动、释放
 *    - 多种图形绘制：自由笔、直线、矩形、圆形、三点圆弧
 *    - 网格绘制和工件坐标系平面显示
 *    - 点位提取功能（支持自定义分辨率）
 *    - 撤销/重做操作栈管理
 * 
 * 3. 核心交互流程：
 *    - 用户在画布上绘制轨迹 → 提取点位 → 转换为3D坐标 → 发送给机械臂执行
 *    - 支持基于工件坐标系的坐标转换，实现任意平面的轨迹绘制
 * 
 * 项目背景: 自主研发六轴机械臂上位机控制软件
 * 开发环境: Qt 6.9.0 + MSVC2022_64bit + Windows 10
 *********************************************************************/
#include "drawing_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDialog>
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>
#include <QGroupBox>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>
#include <QThread>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QRegularExpression>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <functional>
#include <cmath>

static QString ensureDir(const QString &dirPath) {
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(dirPath);
    }
    return dirPath;
}

// 实心笔画：高分辨率渲染 + Zhang-Suen 细化 + 骨架追踪
QPainterPath TextItemData::computeTrajectoryPath() const
{
    if (!solidStroke)
        return toPath();

    QPainterPath outline = toPath();
    QRectF bounds = outline.boundingRect();
    if (bounds.isEmpty() || text.isEmpty()) return outline;

    // 高分辨率渲染：fontSize*2，最小 80px，保证笔画有足够像素宽度
    const double targetH = qBound(80.0, (double)fontSize * 2.0, 250.0);
    double scale = targetH / bounds.height();
    int pad = 10;
    int w = qBound(4, (int)(bounds.width() * scale) + pad * 2, 4000);
    int h = qBound(4, (int)(bounds.height() * scale) + pad * 2, 4000);

    qDebug() << "[实心笔画] 文字:" << text << "字体:" << fontFamily
             << "size:" << fontSize << "bounds:" << bounds
             << "渲染:" << w << "x" << h << "targetH:" << targetH;

    // 渲染填充轮廓到灰度图
    QImage img(w, h, QImage::Format_Grayscale8);
    img.fill(0);
    {
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setBrush(Qt::white);
        p.setPen(Qt::NoPen);
        p.translate(-bounds.x() * scale + pad, -bounds.y() * scale + pad);
        p.scale(scale, scale);
        p.drawPath(outline);
    }

    // 转二值数组
    QVector<QVector<bool>> px(h, QVector<bool>(w));
    int fgCount = 0;
    for (int y = 0; y < h; ++y) {
        const uchar *line = img.scanLine(y);
        for (int x = 0; x < w; ++x) {
            px[y][x] = (line[x] >= 128);
            if (px[y][x]) fgCount++;
        }
    }
    qDebug() << "[实心笔画] 前景像素:" << fgCount << "/" << (w*h);

    // Zhang-Suen 细化（不填洞，保留内部结构）
    {
        bool changed = true;
        int iterCount = 0;
        QVector<QPoint> toRemove;
        while (changed && iterCount < 50) {
            changed = false; iterCount++;
            // Step 1
            for (int y = 1; y < h - 1; ++y) {
                for (int x = 1; x < w - 1; ++x) {
                    if (!px[y][x]) continue;
                    int p2 = px[y-1][x],   p3 = px[y-1][x+1], p4 = px[y][x+1],
                        p5 = px[y+1][x+1], p6 = px[y+1][x],   p7 = px[y+1][x-1],
                        p8 = px[y][x-1],   p9 = px[y-1][x-1];
                    int bp = p2+p3+p4+p5+p6+p7+p8+p9;
                    if (bp < 2 || bp > 6) continue;
                    int ap = (p2==0&&p3==1)+(p3==0&&p4==1)+(p4==0&&p5==1)+(p5==0&&p6==1)
                           +(p6==0&&p7==1)+(p7==0&&p8==1)+(p8==0&&p9==1)+(p9==0&&p2==1);
                    if (ap != 1) continue;
                    if (p2 && p4 && p6) continue;
                    if (p4 && p6 && p8) continue;
                    toRemove.append(QPoint(x, y));
                }
            }
            for (const auto &pt : toRemove) { px[pt.y()][pt.x()] = false; changed = true; }
            toRemove.clear();
            // Step 2
            for (int y = 1; y < h - 1; ++y) {
                for (int x = 1; x < w - 1; ++x) {
                    if (!px[y][x]) continue;
                    int p2 = px[y-1][x],   p3 = px[y-1][x+1], p4 = px[y][x+1],
                        p5 = px[y+1][x+1], p6 = px[y+1][x],   p7 = px[y+1][x-1],
                        p8 = px[y][x-1],   p9 = px[y-1][x-1];
                    int bp = p2+p3+p4+p5+p6+p7+p8+p9;
                    if (bp < 2 || bp > 6) continue;
                    int ap = (p2==0&&p3==1)+(p3==0&&p4==1)+(p4==0&&p5==1)+(p5==0&&p6==1)
                           +(p6==0&&p7==1)+(p7==0&&p8==1)+(p8==0&&p9==1)+(p9==0&&p2==1);
                    if (ap != 1) continue;
                    if (p2 && p4 && p8) continue;
                    if (p2 && p6 && p8) continue;
                    toRemove.append(QPoint(x, y));
                }
            }
            for (const auto &pt : toRemove) { px[pt.y()][pt.x()] = false; changed = true; }
            toRemove.clear();
        }
        fgCount = 0;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                if (px[y][x]) fgCount++;
        qDebug() << "[实心笔画] 细化" << iterCount << "轮, 骨架像素:" << fgCount;
    }

    // 追踪骨架路径（预标记交汇点，确保不可删除）
    QVector<QVector<QPoint>> paths;
    {
        const int dx8[8] = {1, 1, 0, -1, -1, -1, 0, 1};
        const int dy8[8] = {0, -1, -1, -1, 0, 1, 1, 1};
        auto countNb = [&](int x, int y) -> int {
            int c = 0;
            for (int i = 0; i < 8; ++i) {
                int nx = x + dx8[i], ny = y + dy8[i];
                if (nx >= 0 && nx < w && ny >= 0 && ny < h && px[ny][nx]) c++;
            }
            return c;
        };
        auto findNext = [&](int x, int y, int &nx, int &ny) -> bool {
            for (int i = 0; i < 8; ++i) {
                nx = x + dx8[i]; ny = y + dy8[i];
                if (nx >= 0 && nx < w && ny >= 0 && ny < h && px[ny][nx]) return true;
            }
            return false;
        };

        // 预标记交汇点（追踪前快照，避免动态变化）
        QVector<QVector<bool>> junction(h, QVector<bool>(w, false));
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                if (px[y][x] && countNb(x, y) >= 3)
                    junction[y][x] = true;

        // 单方向追踪（从当前点走到终点/交汇点）
        auto traceOneWay = [&](int sx, int sy, QVector<QPoint> &out, bool forward) {
            int cx = sx, cy = sy;
            while (true) {
                if (!junction[cy][cx]) px[cy][cx] = false;
                int nx, ny;
                if (!findNext(cx, cy, nx, ny)) {
                    out.append(QPoint(cx, cy));
                    break;
                }
                if (junction[ny][nx]) {
                    out.append(QPoint(cx, cy));       // 当前点
                    out.append(QPoint(nx, ny));       // 交汇点（共用的终点）
                    break;
                }
                out.append(QPoint(cx, cy));
                cx = nx; cy = ny;
            }
        };

        for (int round = 0; round < 2; ++round) {
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    if (!px[y][x] || junction[y][x]) continue;
                    int nb = countNb(x, y);
                    if (nb == 0) { px[y][x] = false; continue; }
                    if (round == 0 && nb != 1) continue;

                    QVector<QPoint> path;
                    if (round == 0 || nb == 1) {
                        // 端点出发：单向追踪
                        traceOneWay(x, y, path, true);
                    } else {
                        // 中间点出发（nb==2）：先清起点，再双向追踪，保证整段连续
                        px[y][x] = false;
                        // 找两个邻居
                        int n1x = -1, n1y = -1, n2x = -1, n2y = -1;
                        for (int i = 0; i < 8; ++i) {
                            int nx = x + dx8[i], ny = y + dy8[i];
                            if (nx >= 0 && nx < w && ny >= 0 && ny < h && px[ny][nx]) {
                                if (n1x < 0) { n1x = nx; n1y = ny; }
                                else { n2x = nx; n2y = ny; break; }
                            }
                        }
                        // 方向1追踪（反向放到起点前面）
                        if (n1x >= 0) {
                            QVector<QPoint> part;
                            traceOneWay(n1x, n1y, part, true);
                            // part = [N1, ..., J1/端点], 反转变为 [J1/端点, ..., N1]
                            for (int i = part.size() - 1; i >= 0; --i)
                                path.append(part[i]);
                        }
                        // 起点
                        path.append(QPoint(x, y));
                        // 方向2追踪（正向放到起点后面）
                        if (n2x >= 0) {
                            traceOneWay(n2x, n2y, path, true);
                        }
                    }
                    if (path.size() >= 2) paths.append(path);
                }
            }
        }
    }
    qDebug() << "[实心笔画] 追踪路径数:" << paths.size();

    // Douglas-Peucker 折线简化（消除像素级锯齿）
    std::function<QVector<QPoint>(const QVector<QPoint>&, double)> simplifyPath;
    simplifyPath = [&](const QVector<QPoint> &pts, double eps) -> QVector<QPoint> {
        if (pts.size() <= 2) return pts;
        double maxDist = 0;
        int maxIdx = 0;
        QPointF a(pts.first().x(), pts.first().y());
        QPointF b(pts.last().x(), pts.last().y());
        double abLen = QLineF(a, b).length();
        for (int i = 1; i < pts.size() - 1; ++i) {
            double dist;
            if (abLen < 0.001) {
                dist = QLineF(a, QPointF(pts[i].x(), pts[i].y())).length();
            } else {
                double cross = std::abs((b.x()-a.x())*(a.y()-pts[i].y()) - (a.x()-pts[i].x())*(b.y()-a.y()));
                dist = cross / abLen;
            }
            if (dist > maxDist) { maxDist = dist; maxIdx = i; }
        }
        if (maxDist <= eps) {
            QVector<QPoint> r; r.append(pts.first()); r.append(pts.last());
            return r;
        }
        QVector<QPoint> left = simplifyPath(pts.mid(0, maxIdx + 1), eps);
        QVector<QPoint> right = simplifyPath(pts.mid(maxIdx), eps);
        left.removeLast();
        left.append(right);
        return left;
    };

    // 转回原坐标系
    double invScale = 1.0 / scale;
    // 简化阈值：与渲染尺寸成比例（2% = 对100px高字体约2px），只消除像素锯齿
    double simplifyEps = std::max(1.5, targetH * 0.02);
    QPainterPath result;
    int totalPts = 0;
    for (const auto &pts : paths) {
        if (pts.size() < 2) continue;
        QVector<QPoint> simplified = simplifyPath(pts, simplifyEps);
        totalPts += simplified.size();
        QPainterPath sp;
        sp.moveTo((simplified[0].x() - pad) * invScale + bounds.x(),
                   (simplified[0].y() - pad) * invScale + bounds.y());
        for (int i = 1; i < simplified.size(); ++i)
            sp.lineTo((simplified[i].x() - pad) * invScale + bounds.x(),
                       (simplified[i].y() - pad) * invScale + bounds.y());
        result.addPath(sp);
    }
    qDebug() << "[实心笔画] 简化后总点数:" << totalPts << "路径数:" << paths.size();

    return result.isEmpty() ? outline : result;
}

static QString getProjectRoot() {
    return QCoreApplication::applicationDirPath() + "/..";
}

DrawingWindow::DrawingWindow(QWidget *parent) : QMainWindow(parent), m_workPieceCoord(nullptr) {
    // 创建绘图区域
    m_canvas = new DrawingCanvas;
    setCentralWidget(m_canvas);

    // 创建工具栏
    createToolBar();

    // 创建状态栏坐标标签
    m_coordLabel = new QLabel(this);
    m_coordLabel->setAlignment(Qt::AlignRight);
    m_coordLabel->setMinimumWidth(150);

    // 创建当前工具标签
    m_toolLabel = new QLabel("当前工具: 自由线", this);
    m_toolLabel->setAlignment(Qt::AlignCenter);
    m_toolLabel->setMinimumWidth(150);

    // 创建坐标系状态标签
    m_coordSysLabel = new QLabel(this);
    m_coordSysLabel->setAlignment(Qt::AlignLeft);
    m_coordSysLabel->setMinimumWidth(200);

    // 使用QHBoxLayout来布局状态栏中的小部件
    QWidget* statusWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(statusWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_coordSysLabel);
    layout->addStretch();
    layout->addWidget(m_toolLabel);
    layout->addStretch();

    // 将布局小部件添加到状态栏
    statusBar()->addWidget(statusWidget, 1);
    statusBar()->addPermanentWidget(m_coordLabel);

    // 连接鼠标位置变化信号
    connect(m_canvas, &DrawingCanvas::mousePositionChanged, this, [this](const QPointF &pos){
        m_coordLabel->setText(QString("X: %1, Y: %2").arg(pos.x(), 0, 'f', 0).arg(pos.y(), 0, 'f', 0));
    });

    // 初始化坐标系标签
    updateCoordSysLabel();

    resize(800, 600);
    setWindowTitle("执行画板");
}

DrawingWindow::~DrawingWindow() {
    delete m_canvas;
}

void DrawingWindow::createToolBar() {
    auto toolbar = addToolBar("绘图工具");

    // 清空按钮
    auto clearBtn = new QAction("清空", this);
    connect(clearBtn, &QAction::triggered, this, &DrawingWindow::onClearCanvas);
    toolbar->addAction(clearBtn);

    // 撤销按钮
    auto undoBtn = new QAction("撤销", this);
    connect(undoBtn, &QAction::triggered, this, &DrawingWindow::onUndo);
    toolbar->addAction(undoBtn);

    // 重做按钮
    auto redoBtn = new QAction("重做", this);
    connect(redoBtn, &QAction::triggered, this, &DrawingWindow::onRedo);
    toolbar->addAction(redoBtn);

    toolbar->addSeparator();

    // 绘图工具按钮
    auto freePenBtn = new QAction("自由线", this);
    connect(freePenBtn, &QAction::triggered,
            [this](){
                m_canvas->setCurrentTool(DrawingTool::FreePen);
                m_toolLabel->setText("当前工具: 自由线");
            });
    toolbar->addAction(freePenBtn);

    auto lineBtn = new QAction("直线", this);
    connect(lineBtn, &QAction::triggered,
            [this](){
                m_canvas->setCurrentTool(DrawingTool::Line);
                m_toolLabel->setText("当前工具: 直线");
            });
    toolbar->addAction(lineBtn);

    auto rectBtn = new QAction("矩形", this);
    connect(rectBtn, &QAction::triggered,
            [this](){
                m_canvas->setCurrentTool(DrawingTool::Rectangle);
                m_toolLabel->setText("当前工具: 矩形");
            });
    toolbar->addAction(rectBtn);

    auto circleBtn = new QAction("圆形", this);
    connect(circleBtn, &QAction::triggered,
            [this](){
                m_canvas->setCurrentTool(DrawingTool::Circle);
                m_toolLabel->setText("当前工具: 圆形");
            });
    toolbar->addAction(circleBtn);

    auto arcBtn = new QAction("圆弧", this);
    connect(arcBtn, &QAction::triggered,
            [this](){
                m_canvas->setCurrentTool(DrawingTool::Arc);
                m_toolLabel->setText("当前工具: 圆弧");
            });
    toolbar->addAction(arcBtn);

    toolbar->addSeparator();

    // 点位提取按钮
    auto extractBtn = new QAction("提取点位", this);
    connect(extractBtn, &QAction::triggered, this, &DrawingWindow::onExtractPoints);
    toolbar->addAction(extractBtn);

    // 开始按钮
    auto startBtn = new QAction("开始", this);
    connect(startBtn, &QAction::triggered, this, &DrawingWindow::onSendPoints);
    toolbar->addAction(startBtn);

    // 停止按钮
    auto stopBtn = new QAction("停止", this);
    connect(stopBtn, &QAction::triggered, this, &DrawingWindow::onStopPoints);
    toolbar->addAction(stopBtn);

    // 查看点位文件按钮
    auto viewBtn = new QAction("查看点位文件", this);
    connect(viewBtn, &QAction::triggered, this, &DrawingWindow::onExportPoints);
    toolbar->addAction(viewBtn);

    // 保存点位按钮
    auto saveBtn = new QAction("保存点位", this);
    connect(saveBtn, &QAction::triggered, this, &DrawingWindow::onSavePoints);
    toolbar->addAction(saveBtn);

    // 选择工具（移动/缩放文字）
    auto selectBtn = new QAction("选择", this);
    connect(selectBtn, &QAction::triggered,
            [this](){
                m_canvas->setCurrentTool(DrawingTool::Select);
                m_toolLabel->setText("当前工具: 选择");
            });
    toolbar->addAction(selectBtn);

    // 文字输入按钮（弹窗打字）
    auto textBtn = new QAction("文字", this);
    connect(textBtn, &QAction::triggered, this, &DrawingWindow::onTextDialog);
    toolbar->addAction(textBtn);

    // 工件坐标系设置按钮
    auto coordBtn = new QAction("坐标系", this);
    connect(coordBtn, &QAction::triggered, this, &DrawingWindow::onSetWorkPieceCoord);
    toolbar->addAction(coordBtn);

    // 设置按钮（含运动参数和工件平面信息）
    auto settingsBtn = new QAction("设置", this);
    connect(settingsBtn, &QAction::triggered, this, &DrawingWindow::onMotionSettings);
    toolbar->addAction(settingsBtn);
}

void DrawingWindow::onClearCanvas() {
    m_canvas->clearAll();
    statusBar()->showMessage("画布已清空", 2000);
}

void DrawingWindow::onUndo() {
    if (!m_canvas->undo()) {
        statusBar()->showMessage("没有可撤销的操作", 2000);
    } else {
        statusBar()->showMessage("已撤销上一步操作", 2000);
    }
}

void DrawingWindow::onRedo() {
    if (!m_canvas->redo()) {
        statusBar()->showMessage("没有可重做的操作", 2000);
    } else {
        statusBar()->showMessage("已重做上一步操作", 2000);
    }
}

void DrawingWindow::onExtractPoints() {
    QVector<QVector3D> points3D = getDrawingPoints3D(2.0);

    if (points3D.isEmpty()) {
        QMessageBox::information(this, "点位提取结果", "没有可提取的点位");
        return;
    }

    double rx = (m_workPieceCoord ? m_workPieceCoord->getRXOriginal() : m_motionParams.rx);
    double ry = (m_workPieceCoord ? m_workPieceCoord->getRYOriginal() : m_motionParams.ry);
    double rz = (m_workPieceCoord ? m_workPieceCoord->getRZOriginal() : m_motionParams.rz);

    // 创建一个对话框来显示所有点位
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(QString("点位列表 (%1 个点)").arg(points3D.size()));
    dialog->resize(900, 400);

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea(dialog);
    scrollArea->setWidgetResizable(true);

    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

    // 添加表头
    QLabel *header = new QLabel(QString("<b>序号 | X坐标 | Y坐标 | Z坐标 | RX(°) | RY(°) | RZ(°)</b>"));
    header->setStyleSheet("font-size: 12px; padding: 5px; background-color: #f0f0f0;");
    scrollLayout->addWidget(header);

    // 添加分割线
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    scrollLayout->addWidget(line);

    // 添加所有点位（显示3D坐标 + 姿态）
    int index = 1;
    for (const QVector3D &p : points3D) {
        QString text = QString("%1 | %2 | %3 | %4 | %5 | %6 | %7")
            .arg(index++, 4)
            .arg(p.x(), 0, 'f', 3)
            .arg(p.y(), 0, 'f', 3)
            .arg(p.z(), 0, 'f', 3)
            .arg(rx, 0, 'f', 2)
            .arg(ry, 0, 'f', 2)
            .arg(rz, 0, 'f', 2);
        QLabel *label = new QLabel(text);
        label->setStyleSheet("font-size: 11px; padding: 2px 5px;");
        // 交替行颜色
        if (index % 2 == 0) {
            label->setStyleSheet("font-size: 11px; padding: 2px 5px; background-color: #fafafa;");
        }
        scrollLayout->addWidget(label);
    }

    scrollArea->setWidget(scrollWidget);
    layout->addWidget(scrollArea);

    // 添加坐标系信息
    QLabel *coordInfo = new QLabel();
    if (m_workPieceCoord) {
        coordInfo->setText(QString("坐标系: 工件坐标系 (原点: (%1, %2, %3))  姿态: RX=%4° RY=%5° RZ=%6°")
            .arg(m_workPieceCoord->getOrigin().x(), 0, 'f', 3)
            .arg(m_workPieceCoord->getOrigin().y(), 0, 'f', 3)
            .arg(m_workPieceCoord->getOrigin().z(), 0, 'f', 3)
            .arg(rx, 0, 'f', 2)
            .arg(ry, 0, 'f', 2)
            .arg(rz, 0, 'f', 2));
    } else {
        coordInfo->setText(QString("坐标系: 世界坐标系  姿态: RX=%1° RY=%2° RZ=%3°").arg(rx, 0, 'f', 2).arg(ry, 0, 'f', 2).arg(rz, 0, 'f', 2));
    }
    coordInfo->setStyleSheet("font-size: 11px; padding: 5px; background-color: #e3f2fd;");
    layout->addWidget(coordInfo);

    // 添加运动参数信息
    QLabel *motionInfo = new QLabel(QString("运动参数: 速度=%1 mm/s, 加速度=%2 mm/s², 减速度=%3 mm/s²")
        .arg(m_motionParams.speed, 0, 'f', 1)
        .arg(m_motionParams.acceleration, 0, 'f', 1)
        .arg(m_motionParams.deceleration, 0, 'f', 1));
    motionInfo->setStyleSheet("font-size: 11px; padding: 5px; background-color: #fff3e0;");
    layout->addWidget(motionInfo);

    // 添加关闭按钮
    QPushButton *closeBtn = new QPushButton("关闭", dialog);
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::close);
    layout->addWidget(closeBtn);

    dialog->exec();

    statusBar()->showMessage(QString("已提取 %1 个3D点位 (姿态 RX=%2° RY=%3° RZ=%4°)").arg(points3D.size()).arg(rx, 0, 'f', 1).arg(ry, 0, 'f', 1).arg(rz, 0, 'f', 1), 3000);
}

void DrawingWindow::onSendPoints() {
    auto pointGroups = getDrawingPoints3DGrouped(2.0);
    QVector<QVector3D> circleCenters, arcStarts, arcMids, arcEnds;
    QVector<double> circleRadii;
    getDrawingCircles3D(circleCenters, circleRadii);
    getDrawingArcs3D(arcStarts, arcMids, arcEnds);

    // 计算总点位
    int totalPoints = 0;
    for (const auto &g : pointGroups) totalPoints += g.size();

    if (pointGroups.isEmpty() && circleCenters.isEmpty() && arcStarts.isEmpty()) {
        QMessageBox::warning(this, "警告", "没有可发送的轨迹，请先绘制");
        return;
    }

    double rx = (m_workPieceCoord ? m_workPieceCoord->getRXOriginal() : m_motionParams.rx);
    double ry = (m_workPieceCoord ? m_workPieceCoord->getRYOriginal() : m_motionParams.ry);
    double rz = (m_workPieceCoord ? m_workPieceCoord->getRZOriginal() : m_motionParams.rz);

    if (!pointGroups.isEmpty()) {
        emit sendDrawingPoints(pointGroups, rx, ry, rz);
    }
    if (!circleCenters.isEmpty()) {
        emit sendDrawingCircles(circleCenters, circleRadii);
    }
    if (!arcStarts.isEmpty()) {
        emit sendDrawingArcs(arcStarts, arcMids, arcEnds);
    }

    QString coordType = m_workPieceCoord ? "工件坐标系" : "世界坐标系";
    statusBar()->showMessage(QString("已发送 %1 组轨迹(%2点) + %3 个圆形 + %4 个弧线")
        .arg(pointGroups.size()).arg(totalPoints).arg(circleCenters.size()).arg(arcStarts.size()), 3000);
}

void DrawingWindow::onStopPoints() {
    emit requestStopMotion();
    statusBar()->showMessage("已发送停止指令", 3000);
}

void DrawingWindow::setWorkPieceCoord(const WorkPieceCoord *coord) {
    m_workPieceCoord = coord;
    updateCoordSysLabel();

    if (m_workPieceCoord && m_workPieceCoord->isValid()) {
        m_canvas->setWorkPiecePlaneSize(m_canvas->size());
    } else {
        m_canvas->clearWorkPiecePlane();
    }
}

void DrawingWindow::setDefaultPose(double rx, double ry, double rz) {
    m_motionParams.rx = rx;
    m_motionParams.ry = ry;
    m_motionParams.rz = rz;
}

void DrawingWindow::updateCoordSysLabel() {
    if (m_workPieceCoord && m_workPieceCoord->isValid()) {
        QVector3D origin = m_workPieceCoord->getOrigin();
        QString text = QString("坐标系: 工件坐标 [原点: (%1, %2, %3)]")
                       .arg(origin.x(), 0, 'f', 2)
                       .arg(origin.y(), 0, 'f', 2)
                       .arg(origin.z(), 0, 'f', 2);
        m_coordSysLabel->setText(text);
        m_coordSysLabel->setStyleSheet("color: #2E7D32;");
    } else {
        m_coordSysLabel->setText("坐标系: 世界坐标");
        m_coordSysLabel->setStyleSheet("color: #757575;");
    }
}

QVector<QVector3D> DrawingWindow::getDrawingPoints3D(double resolution) const {
    QVector<QPointF> points2D = m_canvas->extractPoints(resolution);
    QVector<QVector3D> points3D;

    for (const QPointF &p : points2D) {
        if (m_workPieceCoord && m_workPieceCoord->isValid()) {
            QVector3D p3D = m_workPieceCoord->transform2DTo3D(p.x(), p.y());
            points3D.append(p3D);
        } else {
            points3D.append(QVector3D(p.x(), p.y(), 0));
        }
    }

    return points3D;
}

QVector<QVector<QVector3D>> DrawingWindow::getDrawingPoints3DGrouped(double resolution) const {
    QVector<QVector<QPointF>> groups2D = m_canvas->extractPointsGrouped(resolution);
    QVector<QVector<QVector3D>> result;
    for (const auto &group : groups2D) {
        QVector<QVector3D> group3D;
        for (const QPointF &p : group) {
            if (m_workPieceCoord && m_workPieceCoord->isValid())
                group3D.append(m_workPieceCoord->transform2DTo3D(p.x(), p.y()));
            else
                group3D.append(QVector3D(p.x(), p.y(), 0));
        }
        if (!group3D.isEmpty()) result.append(group3D);
    }
    return result;
}

QVector<QPointF> DrawingWindow::getDrawingPoints(double resolution) const {
    return m_canvas->extractPoints(resolution);
}

void DrawingWindow::getDrawingCircles3D(QVector<QVector3D> &centers, QVector<double> &radii) const {
    QVector<CircleData> circles = m_canvas->getCircles();
    for (const auto &c : circles) {
        if (m_workPieceCoord && m_workPieceCoord->isValid()) {
            QVector3D c3D = m_workPieceCoord->transform2DTo3D(c.center.x(), c.center.y());
            centers.append(c3D);
        } else {
            centers.append(QVector3D(c.center.x(), c.center.y(), 0));
        }
        radii.append(c.radius);
    }
}

void DrawingWindow::getDrawingArcs3D(QVector<QVector3D> &starts, QVector<QVector3D> &mids, QVector<QVector3D> &ends) const {
    QVector<ArcData> arcs = m_canvas->getArcs();
    for (const auto &a : arcs) {
        auto to3D = [this](const QPointF &p) -> QVector3D {
            if (m_workPieceCoord && m_workPieceCoord->isValid())
                return m_workPieceCoord->transform2DTo3D(p.x(), p.y());
            return QVector3D(p.x(), p.y(), 0);
        };
        starts.append(to3D(a.start));
        mids.append(to3D(a.mid));
        ends.append(to3D(a.end));
    }
}

void DrawingWindow::onSetWorkPieceCoord() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(QStringLiteral("当前工件坐标系信息"));
    dialog->resize(500, 400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    
    if (m_workPieceCoord && m_workPieceCoord->isValid()) {
        // 坐标系名称
        QGroupBox *nameGroup = new QGroupBox(QStringLiteral("工件坐标系名称"));
        QHBoxLayout *nameLayout = new QHBoxLayout(nameGroup);
        QLabel *nameLabel = new QLabel(QStringLiteral("坐标系名称:"));
        QString coordName = m_workPieceCoord->getName();
        if (coordName.isEmpty()) {
            coordName = QStringLiteral("未命名坐标系");
        }
        QLabel *nameValue = new QLabel(coordName);
        nameValue->setStyleSheet("font-weight: bold; color: #1976D2;");
        nameLayout->addWidget(nameLabel);
        nameLayout->addWidget(nameValue);
        nameLayout->addStretch();
        mainLayout->addWidget(nameGroup);
        
        // 坐标系参数
        QGroupBox *paramGroup = new QGroupBox(QStringLiteral("坐标系参数"));
        QFormLayout *formLayout = new QFormLayout(paramGroup);
        
        QVector3D origin = m_workPieceCoord->getOrigin();
        QVector3D xAxis = m_workPieceCoord->getXAxis();
        QVector3D yAxis = m_workPieceCoord->getYAxis();
        QVector3D zAxis = m_workPieceCoord->getZAxis();
        
        formLayout->addRow(QStringLiteral("原点坐标:"), new QLabel(QString("(%1, %2, %3)").arg(origin.x(), 0, 'f', 2).arg(origin.y(), 0, 'f', 2).arg(origin.z(), 0, 'f', 2)));
        formLayout->addRow(QStringLiteral("X轴方向:"), new QLabel(QString("(%1, %2, %3)").arg(xAxis.x(), 0, 'f', 4).arg(xAxis.y(), 0, 'f', 4).arg(xAxis.z(), 0, 'f', 4)));
        formLayout->addRow(QStringLiteral("Y轴方向:"), new QLabel(QString("(%1, %2, %3)").arg(yAxis.x(), 0, 'f', 4).arg(yAxis.y(), 0, 'f', 4).arg(yAxis.z(), 0, 'f', 4)));
        formLayout->addRow(QStringLiteral("Z轴方向:"), new QLabel(QString("(%1, %2, %3)").arg(zAxis.x(), 0, 'f', 4).arg(zAxis.y(), 0, 'f', 4).arg(zAxis.z(), 0, 'f', 4)));
        
        mainLayout->addWidget(paramGroup);
        
        // 定义平面的三个点（包含姿态）
        QGroupBox *pointsGroup = new QGroupBox(QStringLiteral("定义平面的三个点"));
        QFormLayout *pointsLayout = new QFormLayout(pointsGroup);
        
        // 使用原始保存的三个定义点和姿态
        QVector3D p1 = m_workPieceCoord->getPoint1();
        QVector3D p2 = m_workPieceCoord->getPoint2();
        QVector3D p3 = m_workPieceCoord->getPoint3();
        double rx = m_workPieceCoord->getRXOriginal();  // 使用原始值
        double ry = m_workPieceCoord->getRYOriginal();
        double rz = m_workPieceCoord->getRZOriginal();
        
        pointsLayout->addRow(QStringLiteral("点1 (原点):"), new QLabel(QString("(%1, %2, %3)").arg(p1.x(), 0, 'f', 3).arg(p1.y(), 0, 'f', 3).arg(p1.z(), 0, 'f', 3)));
        pointsLayout->addRow(QStringLiteral("点2 (X轴方向):"), new QLabel(QString("(%1, %2, %3)").arg(p2.x(), 0, 'f', 3).arg(p2.y(), 0, 'f', 3).arg(p2.z(), 0, 'f', 3)));
        pointsLayout->addRow(QStringLiteral("点3 (XY平面):"), new QLabel(QString("(%1, %2, %3)").arg(p3.x(), 0, 'f', 3).arg(p3.y(), 0, 'f', 3).arg(p3.z(), 0, 'f', 3)));
        
        // 添加姿态信息显示（显示原始值）
        pointsLayout->addRow(QStringLiteral("姿态 RX:"), new QLabel(QString("%1°").arg(rx, 0, 'f', 3)));
        pointsLayout->addRow(QStringLiteral("姿态 RY:"), new QLabel(QString("%1°").arg(ry, 0, 'f', 3)));
        pointsLayout->addRow(QStringLiteral("姿态 RZ:"), new QLabel(QString("%1°").arg(rz, 0, 'f', 3)));
        
        mainLayout->addWidget(pointsGroup);
        
        // 使用说明
        QLabel *hintLabel = new QLabel(QStringLiteral("<b>使用说明:</b><br>"
                                                      "- 画板上绘制的 (X, Y) 坐标将映射到工件坐标系平面<br>"
                                                      "- 实际3D坐标 = 原点 + X * X轴向量 + Y * Y轴向量<br>"
                                                      "- 点击下方按钮打开坐标系设置对话框"));
        hintLabel->setWordWrap(true);
        hintLabel->setStyleSheet("padding: 10px; background-color: #e3f2fd; border-radius: 5px;");
        mainLayout->addWidget(hintLabel);
    } else {
        QLabel *warningLabel = new QLabel(QStringLiteral("当前未设置工件坐标系"));
        warningLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #dc3545; padding: 10px;");
        mainLayout->addWidget(warningLabel);
        
        QLabel *infoLabel = new QLabel(QStringLiteral("点击下方按钮进行三点标定设置。"));
        infoLabel->setWordWrap(true);
        infoLabel->setStyleSheet("padding: 10px; background-color: #fff3cd; border-radius: 5px;");
        mainLayout->addWidget(infoLabel);
        
        QLabel *hintLabel = new QLabel(QStringLiteral("<b>当前状态:</b> 使用世界坐标系"));
        hintLabel->setStyleSheet("padding: 10px;");
        mainLayout->addWidget(hintLabel);
    }

    // 按钮栏
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *setupBtn = new QPushButton(QStringLiteral("设置坐标系"), dialog);
    setupBtn->setStyleSheet("padding: 8px 20px; border-radius: 5px; background-color: #1976D2; color: white;");
    QPushButton *closeBtn = new QPushButton(QStringLiteral("关闭"), dialog);
    closeBtn->setStyleSheet("padding: 8px 20px; border-radius: 5px; background-color: #4CAF50; color: white;");
    btnLayout->addWidget(setupBtn);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);

    connect(setupBtn, &QPushButton::clicked, this, [this, dialog]() {
        dialog->close();
        emit requestWorkpieceCoordSetup();
    });
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::close);

    dialog->exec();
}

void DrawingWindow::onExportPoints() {
    ensureDir(getProjectRoot() + "/点位保存文件处");
    QString defaultDir = getProjectRoot() + "/点位保存文件处";
    QString fileName = QFileDialog::getOpenFileName(this, "选择点位文件", 
                                                   defaultDir, 
                                                   "JSON文件 (*.json);;文本文件 (*.txt);;CSV文件 (*.csv);;所有文件 (*.*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件进行读取");
        return;
    }
    
    QVector<QPointF> points;
    QVector<QVector3D> loadedCircleCenters; // 从文件加载的圆形圆心(已是3D)
    QVector<double> loadedCircleRadii;       // 对应的半径
    QVector<QPointF> loadedCircle2DCenters;  // 2D画布坐标(用于加载回画板)
    QVector<double> loadedCircle2DRadii;
    QString fileInfo;
    QString circleInfo;
    double filePoseRX = 0.0, filePoseRY = 0.0, filePoseRZ = 0.0;
    bool hasPoseInfo = false;
    QString suffix = QFileInfo(fileName).suffix().toLower();

    if (suffix == "json") {
        // JSON格式解析
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isNull()) {
            QMessageBox::warning(this, "错误", "无效的JSON文件");
            file.close();
            return;
        }

        QJsonObject jsonObj = doc.object();

        // 提取文件信息
        if (jsonObj.contains("type")) fileInfo += "类型: " + jsonObj["type"].toString() + "\n";
        if (jsonObj.contains("createTime")) fileInfo += "生成时间: " + jsonObj["createTime"].toString() + "\n";
        if (jsonObj.contains("pointCount")) fileInfo += "点位数量: " + QString::number(jsonObj["pointCount"].toInt()) + "\n";
        if (jsonObj.contains("resolution")) fileInfo += "分辨率: " + QString::number(jsonObj["resolution"].toDouble()) + " 点/毫米\n";
        if (jsonObj.contains("unit")) fileInfo += "单位: " + jsonObj["unit"].toString() + "\n";

        // 读取姿态信息（文件级别）
        if (jsonObj.contains("rx") && jsonObj.contains("ry") && jsonObj.contains("rz")) {
            filePoseRX = jsonObj["rx"].toString().toDouble();
            filePoseRY = jsonObj["ry"].toString().toDouble();
            filePoseRZ = jsonObj["rz"].toString().toDouble();
            hasPoseInfo = true;
            fileInfo += QString("姿态: RX=%1° RY=%2° RZ=%3°\n").arg(filePoseRX, 0, 'f', 2).arg(filePoseRY, 0, 'f', 2).arg(filePoseRZ, 0, 'f', 2);
        }

        // 读取运动参数信息
        if (jsonObj.contains("speed")) fileInfo += "速度: " + jsonObj["speed"].toString() + " mm/s\n";
        if (jsonObj.contains("acceleration")) fileInfo += "加速度: " + jsonObj["acceleration"].toString() + " mm/s²\n";
        if (jsonObj.contains("deceleration")) fileInfo += "减速度: " + jsonObj["deceleration"].toString() + " mm/s²\n";

        // 读取圆形数据
        if (jsonObj.contains("circles") && jsonObj["circles"].isArray()) {
            QJsonArray circlesArray = jsonObj["circles"].toArray();
            circleInfo = QString("圆形数量: %1\n").arg(circlesArray.size());
            for (int i = 0; i < circlesArray.size(); ++i) {
                QJsonObject cObj = circlesArray[i].toObject();
                double cx = cObj["cx"].toString().toDouble();
                double cy = cObj["cy"].toString().toDouble();
                double cz = cObj["cz"].toString().toDouble();
                double r = cObj["r"].toString().toDouble();
                loadedCircleCenters.append(QVector3D(cx, cy, cz)); // 已是3D坐标
                loadedCircleRadii.append(r);
                // 读取2D画布坐标（用于加载回画板显示）
                if (cObj.contains("cx2d") && cObj.contains("cy2d")) {
                    loadedCircle2DCenters.append(QPointF(cObj["cx2d"].toString().toDouble(), cObj["cy2d"].toString().toDouble()));
                    loadedCircle2DRadii.append(r);
                }
                circleInfo += QString("  圆%1: 圆心(%2, %3, %4) 半径=%5mm\n")
                    .arg(i + 1)
                    .arg(QString::number(cx, 'f', 2))
                    .arg(QString::number(cy, 'f', 2))
                    .arg(QString::number(cObj["cz"].toString().toDouble(), 'f', 2))
                    .arg(QString::number(r, 'f', 2));
            }
            fileInfo += circleInfo;
        }

        // 提取点位数据
        if (jsonObj.contains("points") && jsonObj["points"].isArray()) {
            QJsonArray pointsArray = jsonObj["points"].toArray();
            for (const QJsonValue &val : pointsArray) {
                if (val.isObject()) {
                    QJsonObject pointObj = val.toObject();
                    double x = pointObj["x"].toString().toDouble();
                    double y = pointObj["y"].toString().toDouble();
                    points.append(QPointF(x, y));
                }
            }
        }
    } else {
        // 文本格式解析
        QTextStream in(&file);
        int lineNum = 0;

        while (!in.atEnd()) {
            QString line = in.readLine();
            lineNum++;

            // 跳过注释行和空行
            if (line.startsWith("#") || line.trimmed().isEmpty()) {
                fileInfo += line + "\n";
                // 尝试从注释中读取姿态信息
                if (line.contains("姿态 RX:") && line.contains("RY:") && line.contains("RZ:")) {
                    QRegularExpression rx("RX:\\s*([\\d.\\-]+)");
                    QRegularExpression ry("RY:\\s*([\\d.\\-]+)");
                    QRegularExpression rz("RZ:\\s*([\\d.\\-]+)");
                    auto matchRX = rx.match(line);
                    auto matchRY = ry.match(line);
                    auto matchRZ = rz.match(line);
                    if (matchRX.hasMatch()) filePoseRX = matchRX.captured(1).toDouble();
                    if (matchRY.hasMatch()) filePoseRY = matchRY.captured(1).toDouble();
                    if (matchRZ.hasMatch()) filePoseRZ = matchRZ.captured(1).toDouble();
                    hasPoseInfo = true;
                }
                continue;
            }

            // 解析点位数据 (兼容旧的4列和新的7列格式)
            QStringList parts = line.split(",");
            if (parts.size() >= 3) {
                bool ok1, ok2;
                int idx = parts[0].trimmed().toInt(&ok1);
                double x = parts[1].trimmed().toDouble(&ok2);
                if (ok1 && ok2 && parts.size() >= 3) {
                    double y = parts[2].trimmed().toDouble();
                    points.append(QPointF(x, y));
                    // 从点位行读取姿态（7列格式：序号, X, Y, Z, RX, RY, RZ）
                    if (!hasPoseInfo && parts.size() >= 7) {
                        filePoseRX = parts[4].trimmed().toDouble();
                        filePoseRY = parts[5].trimmed().toDouble();
                        filePoseRZ = parts[6].trimmed().toDouble();
                        hasPoseInfo = true;
                    }
                }
            }
        }
    }
    
    file.close();
    
    if (points.isEmpty() && circleInfo.isEmpty()) {
        QMessageBox::warning(this, "警告", "文件中没有有效的轨迹数据");
        return;
    }
    
    // 创建一个对话框来显示点位文件内容
    QDialog *dialog = new QDialog(this);
    QString titleExtra;
    int circleCount = circleInfo.count('\n') - 1; // 第一行是"圆形数量: N"，后面每行一个圆
    if (circleCount < 0) circleCount = 0;
    if (!points.isEmpty()) titleExtra += QString("%1 个点").arg(points.size());
    if (circleCount > 0) titleExtra += QString("%1%2 个圆").arg(titleExtra.isEmpty() ? "" : " + ").arg(circleCount);
    dialog->setWindowTitle(QString("轨迹文件: %1 (%2)").arg(QFileInfo(fileName).fileName()).arg(titleExtra));
    dialog->resize(650, 500);
    
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    
    // 文件信息区域
    QGroupBox *infoBox = new QGroupBox("文件信息");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoBox);
    QLabel *infoLabel = new QLabel(fileInfo);
    infoLabel->setFont(QFont("Consolas", 10));
    infoLabel->setStyleSheet("color: #666;");
    infoLayout->addWidget(infoLabel);
    layout->addWidget(infoBox);
    
    // 创建表格标题
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    QLabel *label1 = new QLabel("<b>序号</b>");
    QLabel *label2 = new QLabel("<b>X坐标</b>");
    QLabel *label3 = new QLabel("<b>Y坐标</b>");
    label1->setAlignment(Qt::AlignCenter);
    label2->setAlignment(Qt::AlignCenter);
    label3->setAlignment(Qt::AlignCenter);
    label1->setFixedWidth(80);
    label2->setFixedWidth(200);
    label3->setFixedWidth(200);
    headerLayout->addWidget(label1);
    headerLayout->addWidget(label2);
    headerLayout->addWidget(label3);
    headerWidget->setStyleSheet("background-color: #4a90d9; color: white; padding: 8px;");
    layout->addWidget(headerWidget);
    
    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea(dialog);
    scrollArea->setWidgetResizable(true);
    
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
    
    // 添加所有点位
    int index = 1;
    for (const QPointF &p : points) {
        QWidget *rowWidget = new QWidget();
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
        
        QLabel *idxLabel = new QLabel(QString("%1").arg(index++, 4));
        QLabel *xLabel = new QLabel(QString("%1").arg(p.x(), 0, 'f', 2));
        QLabel *yLabel = new QLabel(QString("%1").arg(p.y(), 0, 'f', 2));
        
        idxLabel->setAlignment(Qt::AlignCenter);
        xLabel->setAlignment(Qt::AlignCenter);
        yLabel->setAlignment(Qt::AlignCenter);
        
        idxLabel->setFixedWidth(80);
        xLabel->setFixedWidth(200);
        yLabel->setFixedWidth(200);
        
        // 交替行颜色
        if (index % 2 == 0) {
            rowWidget->setStyleSheet("background-color: #f8f8f8;");
        } else {
            rowWidget->setStyleSheet("background-color: #ffffff;");
        }
        
        rowLayout->addWidget(idxLabel);
        rowLayout->addWidget(xLabel);
        rowLayout->addWidget(yLabel);
        rowLayout->setContentsMargins(0, 2, 0, 2);
        
        scrollLayout->addWidget(rowWidget);
    }
    
    scrollArea->setWidget(scrollWidget);
    layout->addWidget(scrollArea);
    
    // 添加操作按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *loadBtn = new QPushButton("加载到画布", dialog);
    loadBtn->setStyleSheet("background-color: #4CAF50; color: white; border: none; padding: 8px 20px; border-radius: 4px;");
    connect(loadBtn, &QPushButton::clicked, [this, dialog, points, loadedCircle2DCenters, loadedCircle2DRadii]() {
        // 清除画布并绘制加载的轨迹
        m_canvas->clearAll();

        // 绘制点位（线段）
        if (points.size() >= 2) {
            QPainterPath path;
            path.moveTo(points.first());
            for (int i = 1; i < points.size(); i++) {
                path.lineTo(points[i]);
            }
            m_canvas->addPath(path);
        }

        // 绘制圆形
        for (int i = 0; i < loadedCircle2DCenters.size(); ++i) {
            m_canvas->addCircle(loadedCircle2DCenters[i], loadedCircle2DRadii[i]);
        }

        int totalItems = points.size() + loadedCircle2DCenters.size();
        QMessageBox::information(this, "加载成功", QString("已加载 %1 个轨迹到画布 (点位:%2 圆形:%3)").arg(totalItems).arg(points.size()).arg(loadedCircle2DCenters.size()));
        dialog->close();
    });
    btnLayout->addWidget(loadBtn);
    
    QPushButton *sendBtn = new QPushButton("开始", dialog);
    sendBtn->setStyleSheet("background-color: #4CAF50; color: white; border: none; padding: 8px 20px; border-radius: 4px;");
    connect(sendBtn, &QPushButton::clicked, [this, dialog, points, loadedCircleCenters, loadedCircleRadii, hasPoseInfo, filePoseRX, filePoseRY, filePoseRZ]() {
        // 弹出确认对话框
        QString confirmMsg = QString("确定要执行轨迹吗？\n\n");
        if (!points.isEmpty()) confirmMsg += QString("点位: %1 个\n").arg(points.size());
        if (!loadedCircleCenters.isEmpty()) confirmMsg += QString("圆形: %1 个\n").arg(loadedCircleCenters.size());
        QMessageBox::StandardButton reply = QMessageBox::question(this, "确认执行", confirmMsg, QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes) {
            return;
        }

        qDebug() << "[执行轨迹] ========== 开始执行轨迹 ==========";
        qDebug() << "[执行轨迹] 点位:" << points.size() << "个, 圆形:" << loadedCircleCenters.size() << "个";

        emit requestInverseKinematics();
        QThread::msleep(500);

        double execRX = hasPoseInfo ? filePoseRX : ((m_workPieceCoord ? m_workPieceCoord->getRXOriginal() : m_motionParams.rx));
        double execRY = hasPoseInfo ? filePoseRY : ((m_workPieceCoord ? m_workPieceCoord->getRYOriginal() : m_motionParams.ry));
        double execRZ = hasPoseInfo ? filePoseRZ : ((m_workPieceCoord ? m_workPieceCoord->getRZOriginal() : m_motionParams.rz));

        // 发送点位
        if (!points.isEmpty()) {
            QVector<QVector3D> points3D;
            for (const QPointF &p : points) {
                if (m_workPieceCoord && m_workPieceCoord->isValid())
                    points3D.append(m_workPieceCoord->transform2DTo3D(p.x(), p.y()));
                else
                    points3D.append(QVector3D(p.x(), p.y(), 0));
            }
            QVector<QVector<QVector3D>> groups;
            if (!points3D.isEmpty()) groups.append(points3D);
            emit sendDrawingPoints(groups, execRX, execRY, execRZ);
        }

        // 发送圆形（从文件加载的圆心已是3D坐标，无需再转换）
        if (!loadedCircleCenters.isEmpty()) {
            emit sendDrawingCircles(loadedCircleCenters, loadedCircleRadii);
        }

        qDebug() << "[执行轨迹] ========== 轨迹执行请求已发送 ==========";
        dialog->close();
    });
    btnLayout->addWidget(sendBtn);
    
    QPushButton *stopBtn = new QPushButton("停止", dialog);
    stopBtn->setStyleSheet("background-color: #f44336; color: white; border: none; padding: 8px 20px; border-radius: 4px;");
    connect(stopBtn, &QPushButton::clicked, [this, dialog]() {
        emit requestStopMotion();
        dialog->close();
    });
    btnLayout->addWidget(stopBtn);
    
    QPushButton *closeBtn = new QPushButton("关闭", dialog);
    closeBtn->setStyleSheet("background-color: #9E9E9E; color: white; border: none; padding: 8px 20px; border-radius: 4px;");
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::close);
    btnLayout->addWidget(closeBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    dialog->exec();
    
    statusBar()->showMessage(QString("已查看点位文件: %1").arg(QFileInfo(fileName).fileName()), 3000);
}

void DrawingWindow::onSavePoints() {
    // 使用分组点位（与执行轨迹保持一致）
    auto pointGroups = getDrawingPoints3DGrouped(2.0);
    QVector<QVector3D> circleCenters, arcStarts, arcMids, arcEnds;
    QVector<double> circleRadii;
    getDrawingCircles3D(circleCenters, circleRadii);
    getDrawingArcs3D(arcStarts, arcMids, arcEnds);

    // 扁平化统计
    QVector<QVector3D> points3D;
    for (const auto &g : pointGroups) points3D.append(g);

    if (points3D.isEmpty() && circleCenters.isEmpty() && arcStarts.isEmpty()) {
        QMessageBox::warning(this, "警告", "没有可保存的轨迹，请先绘制");
        return;
    }

    ensureDir(getProjectRoot() + "/点位保存文件处");
    QString defaultDir = getProjectRoot() + "/点位保存文件处";
    QString fileName = QFileDialog::getSaveFileName(this, "保存点位文件",
                                                    defaultDir + "/points.json",
                                                    "JSON文件 (*.json);;文本文件 (*.txt);;CSV文件 (*.csv);;所有文件 (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件进行写入");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    double rx = (m_workPieceCoord ? m_workPieceCoord->getRXOriginal() : m_motionParams.rx);
    double ry = (m_workPieceCoord ? m_workPieceCoord->getRYOriginal() : m_motionParams.ry);
    double rz = (m_workPieceCoord ? m_workPieceCoord->getRZOriginal() : m_motionParams.rz);

    // 判断文件格式
    QString suffix = QFileInfo(fileName).suffix().toLower();

    if (suffix == "json") {
        // JSON格式（按组分段保存）
        QJsonObject jsonObj;
        jsonObj["type"] = "轨迹点位文件";
        jsonObj["createTime"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        jsonObj["groupCount"] = pointGroups.size();
        jsonObj["pointCount"] = points3D.size();
        jsonObj["resolution"] = 2.0;
        jsonObj["unit"] = "毫米";
        jsonObj["useWorkPieceCoord"] = (m_workPieceCoord && m_workPieceCoord->isValid());
        jsonObj["rx"] = QString::number(rx, 'f', 2);
        jsonObj["ry"] = QString::number(ry, 'f', 2);
        jsonObj["rz"] = QString::number(rz, 'f', 2);
        jsonObj["speed"] = QString::number(m_motionParams.speed, 'f', 1);
        jsonObj["acceleration"] = QString::number(m_motionParams.acceleration, 'f', 1);
        jsonObj["deceleration"] = QString::number(m_motionParams.deceleration, 'f', 1);

        QJsonArray groupsArray;
        int globalIdx = 1;
        for (int g = 0; g < pointGroups.size(); ++g) {
            QJsonArray pts;
            for (const QVector3D &p : pointGroups[g]) {
                QJsonObject pointObj;
                pointObj["index"] = globalIdx++;
                pointObj["g"] = g + 1;
                pointObj["x"] = QString::number(p.x(), 'f', 2);
                pointObj["y"] = QString::number(p.y(), 'f', 2);
                pointObj["z"] = QString::number(p.z(), 'f', 2);
                pointObj["rx"] = QString::number(rx, 'f', 2);
                pointObj["ry"] = QString::number(ry, 'f', 2);
                pointObj["rz"] = QString::number(rz, 'f', 2);
                pts.append(pointObj);
            }
            groupsArray.append(pts);
        }
        jsonObj["groups"] = groupsArray;
            QJsonArray circlesArray;
            QVector<CircleData> circles2D = m_canvas->getCircles();
            for (int i = 0; i < circleCenters.size(); ++i) {
                QJsonObject cObj;
                cObj["type"] = "circle";
                cObj["cx"] = QString::number(circleCenters[i].x(), 'f', 2);
                cObj["cy"] = QString::number(circleCenters[i].y(), 'f', 2);
                cObj["cz"] = QString::number(circleCenters[i].z(), 'f', 2);
                cObj["r"] = QString::number(circleRadii[i], 'f', 2);
                cObj["rx"] = QString::number(rx, 'f', 2);
                cObj["ry"] = QString::number(ry, 'f', 2);
                cObj["rz"] = QString::number(rz, 'f', 2);
                // 保存2D画布坐标，方便加载回画板显示
                if (i < circles2D.size()) {
                    cObj["cx2d"] = QString::number(circles2D[i].center.x(), 'f', 2);
                    cObj["cy2d"] = QString::number(circles2D[i].center.y(), 'f', 2);
                }
                circlesArray.append(cObj);
            }
            jsonObj["circles"] = circlesArray;
            jsonObj["circleCount"] = circleCenters.size();

        // 弧线数据：三点弧（执行时用 MoveCirc2Abs）
        if (!arcStarts.isEmpty()) {
            QJsonArray arcsArray;
            for (int i = 0; i < arcStarts.size(); ++i) {
                QJsonObject aObj;
                aObj["type"] = "arc";
                aObj["sx"] = QString::number(arcStarts[i].x(), 'f', 2);
                aObj["sy"] = QString::number(arcStarts[i].y(), 'f', 2);
                aObj["sz"] = QString::number(arcStarts[i].z(), 'f', 2);
                aObj["mx"] = QString::number(arcMids[i].x(), 'f', 2);
                aObj["my"] = QString::number(arcMids[i].y(), 'f', 2);
                aObj["mz"] = QString::number(arcMids[i].z(), 'f', 2);
                aObj["ex"] = QString::number(arcEnds[i].x(), 'f', 2);
                aObj["ey"] = QString::number(arcEnds[i].y(), 'f', 2);
                aObj["ez"] = QString::number(arcEnds[i].z(), 'f', 2);
                aObj["rx"] = QString::number(rx, 'f', 2);
                aObj["ry"] = QString::number(ry, 'f', 2);
                aObj["rz"] = QString::number(rz, 'f', 2);
                arcsArray.append(aObj);
            }
            jsonObj["arcs"] = arcsArray;
            jsonObj["arcCount"] = arcStarts.size();
        }

        QJsonDocument doc(jsonObj);
        out << doc.toJson(QJsonDocument::Indented);
    } else {
        // 文本格式
        out << "# 轨迹点位文件\n";
        out << "# 生成时间: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        out << "# 总点位数量: " << points3D.size() << "\n";
        out << "# 分辨率: 2.0 点/毫米\n";
        out << "# 是否使用工件坐标系: " << (m_workPieceCoord && m_workPieceCoord->isValid() ? "是" : "否") << "\n";
        out << "# 姿态 RX: " << rx << "°, RY: " << ry << "°, RZ: " << rz << "°\n";
        out << "# 速度: " << m_motionParams.speed << " mm/s, 加速度: " << m_motionParams.acceleration << " mm/s², 减速度: " << m_motionParams.deceleration << " mm/s²\n";
        out << "# 格式: 序号, X坐标, Y坐标, Z坐标, RX, RY, RZ\n";
        out << "#========================================\n\n";

        int index = 1;
        for (const QVector3D &p : points3D) {
            out << QString("%1, %2, %3, %4, %5, %6, %7\n").arg(index++, 4).arg(p.x(), 0, 'f', 2).arg(p.y(), 0, 'f', 2).arg(p.z(), 0, 'f', 2).arg(rx, 0, 'f', 2).arg(ry, 0, 'f', 2).arg(rz, 0, 'f', 2);
        }
    }

    file.close();

    QMessageBox::information(this, "保存成功",
                            QString("已成功保存 %1 个点位 + %2 个圆形 + %3 个弧线到文件:\n%4\n姿态: RX=%5° RY=%6° RZ=%7°\n速度: %8 mm/s, 加速度: %9 mm/s², 减速度: %10 mm/s²")
                                .arg(points3D.size()).arg(circleCenters.size()).arg(arcStarts.size()).arg(fileName)
                                .arg(rx, 0, 'f', 2).arg(ry, 0, 'f', 2).arg(rz, 0, 'f', 2)
                                .arg(m_motionParams.speed, 0, 'f', 1).arg(m_motionParams.acceleration, 0, 'f', 1).arg(m_motionParams.deceleration, 0, 'f', 1));
    statusBar()->showMessage(QString("已保存 %1 个点位到文件").arg(points3D.size()), 3000);
}

void DrawingWindow::onTextInput() {
    // 保留兼容，实际使用 onTextDialog
    onTextDialog();
}

void DrawingWindow::onTextDialog() {
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle(QStringLiteral("输入文字"));
    dlg->resize(500, 400);

    QVBoxLayout *layout = new QVBoxLayout(dlg);

    // 字体选择（固定几种适合骨架化的字体）
    QHBoxLayout *fontRow = new QHBoxLayout();
    fontRow->addWidget(new QLabel(QStringLiteral("字体:")));
    QComboBox *fontCombo = new QComboBox();
    fontCombo->addItem("楷体 (KaiTi)", "KaiTi");
    fontCombo->addItem("微软雅黑 (Microsoft YaHei)", "Microsoft YaHei");
    fontCombo->addItem("黑体 (SimHei)", "SimHei");
    fontCombo->addItem("等线 (DengXian)", "DengXian");
    fontCombo->addItem("宋体 (SimSun)", "SimSun");
    fontCombo->setCurrentIndex(0);
    fontRow->addWidget(fontCombo);

    fontRow->addWidget(new QLabel(QStringLiteral("大小:")));
    QSpinBox *sizeSpin = new QSpinBox();
    sizeSpin->setRange(10, 500);
    sizeSpin->setValue(50);
    sizeSpin->setSuffix(" pt");
    fontRow->addWidget(sizeSpin);

    // 实心笔画：勾选=轨迹用细体(单线实心)，不勾选=轮廓空心
    QCheckBox *solidCheck = new QCheckBox(QStringLiteral("实心笔画"));
    solidCheck->setChecked(true);
    fontRow->addWidget(solidCheck);
    layout->addLayout(fontRow);

    // 文字输入区域
    QTextEdit *textEdit = new QTextEdit();
    textEdit->setPlaceholderText(QStringLiteral("在此输入文字，支持中文输入法..."));
    layout->addWidget(textEdit);

    // 按钮
    QHBoxLayout *btnRow = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton(QStringLiteral("生成文字"), dlg);
    okBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px 20px; border-radius: 4px;");
    QPushButton *cancelBtn = new QPushButton(QStringLiteral("取消"), dlg);
    cancelBtn->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc; padding: 8px 20px; border-radius: 4px;");

    connect(okBtn, &QPushButton::clicked, [this, dlg, textEdit, fontCombo, sizeSpin, solidCheck]() {
        QString text = textEdit->toPlainText().trimmed();
        if (text.isEmpty()) { dlg->close(); return; }

        // 自动排列：计算下一个文字的放置位置
        double x = 50.0, y = 200.0;
        const auto& items = m_canvas->textItems();
        if (!items.isEmpty()) {
            QRectF lastBounds = items.last().boundingRect();
            if (lastBounds.isValid()) {
                x = lastBounds.right() + 20.0;
                y = lastBounds.top();
                if (x > 700.0) { x = 50.0; y = lastBounds.bottom() + 30.0; }
            }
        }

        TextItemData item;
        item.text = text;
        item.pos = QPointF(x, y);
        item.fontFamily = fontCombo->currentData().toString();
        item.fontSize = sizeSpin->value();
        item.solidStroke = solidCheck->isChecked();
        item.scale = 1.0;
        m_canvas->addTextItem(item);

        dlg->close();
    });

    connect(cancelBtn, &QPushButton::clicked, dlg, &QDialog::close);

    btnRow->addStretch();
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(okBtn);
    layout->addLayout(btnRow);

    dlg->exec();
}

void DrawingWindow::onMotionSettings() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(QStringLiteral("运动参数设置"));
    dialog->resize(480, 700);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    
    // 速度设置
    QGroupBox *speedGroup = new QGroupBox(QStringLiteral("速度设置"));
    QFormLayout *speedLayout = new QFormLayout(speedGroup);
    
    QDoubleSpinBox *speedSpin = new QDoubleSpinBox();
    speedSpin->setRange(1, 500);
    speedSpin->setDecimals(1);
    speedSpin->setValue(m_motionParams.speed);
    speedSpin->setSuffix(" mm/s");
    speedLayout->addRow(QStringLiteral("运行速度:"), speedSpin);
    
    QDoubleSpinBox *accelSpin = new QDoubleSpinBox();
    accelSpin->setRange(1, 2000);
    accelSpin->setDecimals(1);
    accelSpin->setValue(m_motionParams.acceleration);
    accelSpin->setSuffix(" mm/s²");
    speedLayout->addRow(QStringLiteral("加速度:"), accelSpin);
    
    QDoubleSpinBox *decelSpin = new QDoubleSpinBox();
    decelSpin->setRange(1, 2000);
    decelSpin->setDecimals(1);
    decelSpin->setValue(m_motionParams.deceleration);
    decelSpin->setSuffix(" mm/s²");
    speedLayout->addRow(QStringLiteral("减速度:"), decelSpin);
    
    QDoubleSpinBox *jerkSpin = new QDoubleSpinBox();
    jerkSpin->setRange(1, 10000);
    jerkSpin->setDecimals(0);
    jerkSpin->setValue(m_motionParams.jerk);
    jerkSpin->setSuffix(" mm/s³");
    speedLayout->addRow(QStringLiteral("加加速度:"), jerkSpin);

    QDoubleSpinBox *srampSpin = new QDoubleSpinBox();
    srampSpin->setRange(0, 500);
    srampSpin->setDecimals(0);
    srampSpin->setValue(m_motionParams.sramp);
    srampSpin->setSuffix(" ms");
    speedLayout->addRow(QStringLiteral("S曲线时间:"), srampSpin);

    mainLayout->addWidget(speedGroup);
    
    // 拐角过渡设置
    QGroupBox *blendGroup = new QGroupBox(QStringLiteral("拐角过渡设置"));
    QFormLayout *blendLayout = new QFormLayout(blendGroup);
    
    QComboBox *blendModeCombo = new QComboBox();
    blendModeCombo->addItem(QStringLiteral("无过渡"), 0);
    blendModeCombo->addItem(QStringLiteral("圆弧过渡"), 1);
    blendModeCombo->addItem(QStringLiteral("样条过渡"), 2);
    blendModeCombo->setCurrentIndex(m_motionParams.blendMode);
    blendLayout->addRow(QStringLiteral("过渡模式:"), blendModeCombo);
    
    QDoubleSpinBox *blendRadiusSpin = new QDoubleSpinBox();
    blendRadiusSpin->setRange(0.1, 50);
    blendRadiusSpin->setDecimals(1);
    blendRadiusSpin->setValue(m_motionParams.blendRadius);
    blendRadiusSpin->setSuffix(" mm");
    blendLayout->addRow(QStringLiteral("过渡半径:"), blendRadiusSpin);
    
    mainLayout->addWidget(blendGroup);

    // 工件坐标系信息
    QGroupBox *coordGroup = new QGroupBox(QStringLiteral("工件坐标系信息"));
    QVBoxLayout *coordLayout = new QVBoxLayout(coordGroup);
    if (m_workPieceCoord && m_workPieceCoord->isValid()) {
        QVector3D origin = m_workPieceCoord->getOrigin();
        QVector3D xAxis = m_workPieceCoord->getXAxis();
        QVector3D yAxis = m_workPieceCoord->getYAxis();
        QVector3D zAxis = m_workPieceCoord->getZAxis();
        QString coordName = m_workPieceCoord->getName();
        if (coordName.isEmpty()) coordName = QStringLiteral("未命名坐标系");

        coordLayout->addWidget(new QLabel(QStringLiteral("名称: %1").arg(coordName)));
        coordLayout->addWidget(new QLabel(QString("原点: (%1, %2, %3)").arg(origin.x(), 0, 'f', 2).arg(origin.y(), 0, 'f', 2).arg(origin.z(), 0, 'f', 2)));
        coordLayout->addWidget(new QLabel(QString("X轴: (%1, %2, %3)").arg(xAxis.x(), 0, 'f', 4).arg(xAxis.y(), 0, 'f', 4).arg(xAxis.z(), 0, 'f', 4)));
        coordLayout->addWidget(new QLabel(QString("Y轴: (%1, %2, %3)").arg(yAxis.x(), 0, 'f', 4).arg(yAxis.y(), 0, 'f', 4).arg(yAxis.z(), 0, 'f', 4)));
        coordLayout->addWidget(new QLabel(QString("Z轴: (%1, %2, %3)").arg(zAxis.x(), 0, 'f', 4).arg(zAxis.y(), 0, 'f', 4).arg(zAxis.z(), 0, 'f', 4)));
    } else {
        coordLayout->addWidget(new QLabel(QStringLiteral("当前使用世界坐标系")));
    }
    mainLayout->addWidget(coordGroup);

    // 按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *okBtn = new QPushButton(QStringLiteral("确定"), dialog);
    okBtn->setStyleSheet("background-color: #4CAF50; color: white; border: none; padding: 8px 20px; border-radius: 4px;");
    connect(okBtn, &QPushButton::clicked, [this, dialog, speedSpin, accelSpin, decelSpin, jerkSpin, srampSpin, blendModeCombo, blendRadiusSpin]() {
        m_motionParams.speed = speedSpin->value();
        m_motionParams.acceleration = accelSpin->value();
        m_motionParams.deceleration = decelSpin->value();
        m_motionParams.jerk = jerkSpin->value();
        m_motionParams.sramp = srampSpin->value();
        m_motionParams.blendMode = blendModeCombo->currentData().toInt();
        m_motionParams.blendRadius = blendRadiusSpin->value();
        
        // 发送参数到主窗口
        emit requestDrawMotionParams(m_motionParams);
        
        QMessageBox::information(this, QStringLiteral("设置成功"), QStringLiteral("运动参数已更新"));
        dialog->close();
    });
    
    QPushButton *cancelBtn = new QPushButton(QStringLiteral("取消"), dialog);
    cancelBtn->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc; padding: 8px 20px; border-radius: 4px;");
    connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::close);
    
    QPushButton *resetBtn = new QPushButton(QStringLiteral("恢复默认"), dialog);
    resetBtn->setStyleSheet("background-color: #2196F3; color: white; border: none; padding: 8px 20px; border-radius: 4px;");
    connect(resetBtn, &QPushButton::clicked, [speedSpin, accelSpin, decelSpin, jerkSpin, srampSpin, blendModeCombo, blendRadiusSpin]() {
        speedSpin->setValue(50.0);
        accelSpin->setValue(200.0);
        decelSpin->setValue(200.0);
        jerkSpin->setValue(1000.0);
        srampSpin->setValue(100.0);
        blendModeCombo->setCurrentIndex(1);  // 默认圆弧过渡
        blendRadiusSpin->setValue(5.0);
    });
    
    btnLayout->addStretch();
    btnLayout->addWidget(resetBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);
    
    mainLayout->addLayout(btnLayout);
    
    dialog->exec();
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    // 中键或 Ctrl+左键 → 平移
    if (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::ControlModifier)) {
        m_isPanning = true;
        m_panStart = event->position();
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        // 选择工具：点击选中文字项
        if (m_currentTool == DrawingTool::Select) {
            QPointF pt = screenToLogical(event->position());
            int idx = hitTestText(pt);
            if (idx >= 0) {
                m_selectedText = idx;
                m_dragOffset = pt - m_textItems[idx].pos;
                m_drawing = true;
            } else {
                m_selectedText = -1;
            }
            update();
            return;
        }

        m_drawing = true;
        m_endPoint = screenToLogical(event->position());

        if (m_currentTool == DrawingTool::Arc && m_arcSecondPointSet) {
        } else {
            m_startPoint = screenToLogical(event->position());
            if (m_currentTool == DrawingTool::Arc) {
                m_arcStartPoint = screenToLogical(event->position());
            }
        }

        if (m_currentTool == DrawingTool::FreePen) {
            m_currentPath = QPainterPath();
            m_currentPath.moveTo(m_startPoint);
            m_paths.append(m_currentPath);
        }
        update();
    }
}

void DrawingCanvas::mouseMoveEvent(QMouseEvent *event) {
    // 平移拖拽
    if (m_isPanning) {
        QPointF delta = event->position() - m_panStart;
        m_viewOffset += delta;
        m_panStart = event->position();
        update();
        return;
    }

    // 发出逻辑坐标（用于状态栏显示）
    emit mousePositionChanged(screenToLogical(event->position()));

    // 选择工具：拖拽移动文字
    if (m_currentTool == DrawingTool::Select && m_drawing && m_selectedText >= 0 && (event->buttons() & Qt::LeftButton)) {
        QPointF pt = screenToLogical(event->position());
        m_textItems[m_selectedText].pos = pt - m_dragOffset;
        update();
        return;
    }

    if (m_drawing && (event->buttons() & Qt::LeftButton)) {
        m_endPoint = screenToLogical(event->position());

        if (m_currentTool == DrawingTool::FreePen) {
            m_currentPath.lineTo(m_endPoint);
            m_paths.last() = m_currentPath;
        }
        update();
    }
}

void DrawingCanvas::mouseReleaseEvent(QMouseEvent *event) {
    // 结束平移
    if (m_isPanning) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        return;
    }

    if (event->button() == Qt::LeftButton && m_drawing) {
        m_endPoint = screenToLogical(event->position());

        switch (m_currentTool) {
            case DrawingTool::Line: {
                QPainterPath path;
                path.moveTo(m_startPoint);
                path.lineTo(m_endPoint);
                m_paths.append(path);
                break;
            }
            case DrawingTool::Rectangle: {
                QPainterPath path;
                QRectF rect(m_startPoint, m_endPoint);
                path.addRect(rect);
                m_paths.append(path);
                break;
            }
            case DrawingTool::Circle: {
                QPointF center = m_startPoint;
                double radius = QLineF(center, m_endPoint).length();
                // 圆形不存 path，单独存参数；执行用圆弧插补，保存用圆心+半径
                m_circles.append({center, radius});
                break;
            }
            case DrawingTool::Arc: {
                if (!m_arcSecondPointSet) {
                    // 第一次松开：释放点是弧线经过的中间点B
                    m_arcMidPoint = m_endPoint;
                    m_arcSecondPointSet = true;
                    m_drawing = true;
                } else {
                    // 第二次松开：终点C，弧线 A(固定)→B(经过点)→C(终点)
                    // 不走采样点，单独存弧线参数，执行用 MoveCirc2Abs
                    m_arcs.append({m_arcStartPoint, m_arcMidPoint, m_endPoint});
                    m_drawing = false;
                    m_arcSecondPointSet = false;
                }
                break;
            }
            default:
                break;
        }
        update();
    }
    if (event->button() == Qt::LeftButton && m_currentTool == DrawingTool::Select) {
        m_drawing = false;
    }
}

void DrawingCanvas::wheelEvent(QWheelEvent *event) {
    // 选择工具下滚轮缩放选中的文字
    if (m_currentTool == DrawingTool::Select && m_selectedText >= 0) {
        double delta = event->angleDelta().y() / 120.0;
        double newScale = m_textItems[m_selectedText].scale + delta * 0.1;
        if (newScale < 0.1) newScale = 0.1;
        if (newScale > 10.0) newScale = 10.0;
        m_textItems[m_selectedText].scale = newScale;
        update();
        return;
    }
    // 视图缩放（Ctrl + 滚轮，或以鼠标位置为中心）
    double delta = event->angleDelta().y() / 120.0;
    double factor = (delta > 0) ? 1.15 : (1.0 / 1.15);
    double newScale = m_viewScale * factor;
    if (newScale < 0.01) newScale = 0.01;
    if (newScale > 100.0) newScale = 100.0;
    QPointF cursor = event->position();
    // 保持光标下的逻辑点不变
    QPointF logical = screenToLogical(cursor);
    m_viewScale = newScale;
    m_viewOffset = cursor - logical * m_viewScale;
    update();
}

int DrawingCanvas::hitTestText(const QPointF &pt) const {
    for (int i = m_textItems.size() - 1; i >= 0; --i) {
        if (m_textItems[i].boundingRect().contains(pt))
            return i;
    }
    return -1;
}

void DrawingCanvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    if (!painter.isActive()) {
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);

    // 应用视图变换（平移 + 缩放）
    painter.save();
    painter.translate(m_viewOffset);
    painter.scale(m_viewScale, m_viewScale);

    // 绘制工件坐标系平面（底层）
    if (m_showWorkPiecePlane) {
        drawWorkPiecePlane(painter);
    }

    // 绘制网格线（逻辑坐标）
    drawGrid(painter);

    // 绘制圆形
    painter.setPen(QPen(Qt::blue, 2));
    painter.setBrush(Qt::NoBrush);
    for (const auto &c : m_circles) {
        painter.drawEllipse(c.center, c.radius, c.radius);
    }

    // 绘制弧线
    for (const auto &a : m_arcs) {
        QPainterPath arcPath;
        arcPath.moveTo(a.start);
        drawArcThroughThreePoints(arcPath, a.start, a.mid, a.end);
        painter.drawPath(arcPath);
    }

    // 绘制已完成的路径
    painter.setPen(QPen(Qt::blue, 2));
    for (const auto &path : m_paths) {
        painter.drawPath(path);
    }

    // 绘制带颜色的路径（如坐标系轴线）
    for (const auto &cp : m_coloredPaths) {
        painter.setPen(QPen(cp.second, 2));
        painter.drawPath(cp.first);
    }

    // 绘制文字项（退出视图变换，用原生 drawText 在屏幕坐标绘制，绕开骨架化+viewScale 双重压缩）
    painter.restore();
    painter.save();
    for (int i = 0; i < m_textItems.size(); ++i) {
        const auto &item = m_textItems[i];
        // 逻辑坐标 → 屏幕坐标
        QPointF screenPos(item.pos.x() * m_viewScale + m_viewOffset.x(),
                           item.pos.y() * m_viewScale + m_viewOffset.y());
        QFont font(item.fontFamily, item.fontSize);
        painter.setFont(font);
        if (i == m_selectedText) {
            painter.setPen(Qt::red);
            painter.drawText(screenPos, item.text);
            QFontMetrics fm(font);
            QRectF br = fm.boundingRect(item.text);
            QRectF selRect(screenPos.x() + br.x(), screenPos.y() + br.y(),
                           br.width(), br.height());
            painter.setPen(QPen(Qt::red, 1, Qt::DashLine));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(selRect.adjusted(-4, -4, 4, 4));
        } else {
            painter.setPen(item.solidStroke ? Qt::blue : Qt::darkGreen);
            painter.drawText(screenPos, item.text);
        }
    }
    painter.restore();
    painter.save();
    painter.translate(m_viewOffset);
    painter.scale(m_viewScale, m_viewScale);

    // 绘制预览
    if (m_drawing) {
        painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
        switch (m_currentTool) {
            case DrawingTool::Line:
                painter.drawLine(m_startPoint, m_endPoint);
                break;
            case DrawingTool::Rectangle:
                painter.drawRect(QRectF(m_startPoint, m_endPoint));
                break;
            case DrawingTool::Circle: {
                double radius = QLineF(m_startPoint, m_endPoint).length();
                painter.drawEllipse(m_startPoint, radius, radius);
                break;
            }
            case DrawingTool::Arc:
                if (m_arcSecondPointSet) {
                    QPainterPath arcPath;
                    arcPath.moveTo(m_arcStartPoint);
                    drawArcThroughThreePoints(arcPath, m_arcStartPoint, m_arcMidPoint, m_endPoint);
                    painter.drawPath(arcPath);
                    painter.setPen(QPen(Qt::red, 4));
                    painter.drawPoint(m_arcStartPoint);
                    painter.drawPoint(m_arcMidPoint);
                    painter.drawPoint(m_endPoint);
                } else {
                    painter.drawLine(m_startPoint, m_endPoint);
                }
                break;
            default:
                break;
        }
    }

    painter.restore();

    // 固定 UI 元素：右下角坐标指示器（不受视图变换影响）
    drawCoordinateAxes(painter);
}

void DrawingCanvas::drawGrid(QPainter &painter) {
    if (!m_showGrid) return;

    painter.save();
    QPen gridPen(QColor(200, 200, 200));
    gridPen.setStyle(Qt::DashLine);
    gridPen.setWidth(1);
    painter.setPen(gridPen);

    // 计算可视区域的逻辑坐标范围
    QPointF logicalTL = screenToLogical(QPointF(0, 0));
    QPointF logicalBR = screenToLogical(QPointF(width(), height()));
    int gs = m_gridSpacing;
    // 自适应网格间距：缩放太小时增大间距避免过密
    double showSpacing = gs;
    while (showSpacing * m_viewScale < 8.0) showSpacing *= 2;
    double startX = floor(logicalTL.x() / showSpacing) * showSpacing;
    double startY = floor(logicalTL.y() / showSpacing) * showSpacing;
    for (double x = startX; x <= logicalBR.x(); x += showSpacing) {
        painter.drawLine(QPointF(x, logicalTL.y()), QPointF(x, logicalBR.y()));
    }
    for (double y = startY; y <= logicalBR.y(); y += showSpacing) {
        painter.drawLine(QPointF(logicalTL.x(), y), QPointF(logicalBR.x(), y));
    }
    painter.restore();
}

void DrawingCanvas::drawCoordinateAxes(QPainter &painter) {
    painter.save();

    // 右下角坐标指示器
    int margin = 30;
    int ox = width() - margin - 80;
    int oy = height() - margin - 80;
    int axisLen = 60;

    // X轴：红色，向右
    QPen xPen(QColor(220, 50, 50), 2);
    painter.setPen(xPen);
    painter.drawLine(ox, oy, ox + axisLen, oy);
    painter.drawLine(ox + axisLen, oy, ox + axisLen - 8, oy - 5);
    painter.drawLine(ox + axisLen, oy, ox + axisLen - 8, oy + 5);
    QFont font("Arial", 10, QFont::Bold);
    painter.setFont(font);
    painter.setPen(QColor(220, 50, 50));
    painter.drawText(ox + axisLen + 5, oy + 5, "X+");

    // Y轴：绿色，向下（画布坐标Y正方向）
    QPen yPen(QColor(50, 180, 50), 2);
    painter.setPen(yPen);
    painter.drawLine(ox, oy, ox, oy + axisLen);
    painter.drawLine(ox, oy + axisLen, ox - 5, oy + axisLen - 8);
    painter.drawLine(ox, oy + axisLen, ox + 5, oy + axisLen - 8);
    painter.setPen(QColor(50, 180, 50));
    painter.drawText(ox + 10, oy + axisLen + 15, "Y+");

    // 原点标记
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(ox, oy), 4, 4);
    painter.setPen(Qt::black);
    painter.drawText(ox + 8, oy - 5, "O");

    painter.restore();
}

void DrawingCanvas::drawWorkPiecePlane(QPainter &painter) {
    if (!m_showWorkPiecePlane || !m_workPiecePlaneSize.isValid()) {
        return;
    }

    painter.save();

    QRectF planeRect(m_workPiecePlaneOffset, m_workPiecePlaneSize);

    QLinearGradient gradient(planeRect.topLeft(), planeRect.bottomRight());
    gradient.setColorAt(0, QColor(135, 206, 250, 100));
    gradient.setColorAt(1, QColor(135, 206, 250, 60));

    painter.setPen(QPen(QColor(70, 130, 180, 180), 2));
    painter.setBrush(QBrush(gradient));
    painter.drawRect(planeRect);

    painter.restore();
}

void DrawingCanvas::drawWorkpieceCoordRegion(const QPointF &origin,
                                              const QPointF &xPt,
                                              const QPointF &yPt) {
    // 计算工件平面矩形范围（基于三个标定点）
    double minX = std::min({origin.x(), xPt.x(), yPt.x()});
    double maxX = std::max({origin.x(), xPt.x(), yPt.x()});
    double minY = std::min({origin.y(), xPt.y(), yPt.y()});
    double maxY = std::max({origin.y(), xPt.y(), yPt.y()});
    double marginW = (maxX - minX) * 0.1;
    double marginH = (maxY - minY) * 0.1;
    if (marginW < 5) marginW = 5;
    if (marginH < 5) marginH = 5;
    QSizeF planeSize(maxX - minX + 2 * marginW, maxY - minY + 2 * marginH);
    m_workPiecePlaneSize = planeSize;
    m_workPiecePlaneOffset = QPointF(minX - marginW, minY - marginH);
    setWorkPiecePlaneSize(planeSize);

    // 自动适配视图：将坐标系区域居中并缩放到合适大小
    QRectF bounds(QPointF(minX - marginW, minY - marginH), planeSize);
    zoomToFit(bounds);
}

void DrawingCanvas::drawArcThroughThreePoints(QPainterPath &path, const QPointF &p1, const QPointF &p2, const QPointF &p3) {
    double A = 2 * (p2.x() - p1.x());
    double B = 2 * (p2.y() - p1.y());
    double C = p2.x() * p2.x() + p2.y() * p2.y() - p1.x() * p1.x() - p1.y() * p1.y();
    double D = 2 * (p3.x() - p2.x());
    double E = 2 * (p3.y() - p2.y());
    double F = p3.x() * p3.x() + p3.y() * p3.y() - p2.x() * p2.x() - p2.y() * p2.y();

    double det = A * E - B * D;
    if (qAbs(det) < 1e-6) {
        path.lineTo(p3);
        return;
    }

    double cx = (C * E - B * F) / det;
    double cy = (A * F - C * D) / det;
    QPointF center(cx, cy);

    double r = QLineF(center, p1).length();

    double startAngle = atan2(p1.y() - cy, p1.x() - cx) * 180 / M_PI;
    double endAngle = atan2(p3.y() - cy, p3.x() - cx) * 180 / M_PI;

    double angle = endAngle - startAngle;
    if (angle < 0) angle += 360;

    path.arcTo(cx - r, cy - r, 2 * r, 2 * r, startAngle, angle);
}

QVector<QPointF> DrawingCanvas::extractPoints(double resolution) const {
    QVector<QPointF> allPoints;
    
    for (const QPainterPath &path : m_paths) {
        QVector<QPointF> pathPoints;
        
        // 获取路径的元素
        for (int i = 0; i < path.elementCount(); ++i) {
            QPainterPath::Element elem = path.elementAt(i);
            
            if (elem.isMoveTo()) {
                if (!pathPoints.isEmpty()) {
                    allPoints.append(pathPoints);
                    pathPoints.clear();
                }
                pathPoints.append(QPointF(elem.x, elem.y));
            } else if (elem.isLineTo()) {
                QPointF newPt(elem.x, elem.y);
                QPointF lastPt = pathPoints.last();
                double dist = QLineF(lastPt, newPt).length();
                double minSpacing = (resolution > 0) ? (1.0 / resolution) : 0.5;
                if (dist >= minSpacing) {
                    pathPoints.append(newPt);
                }
            } else if (elem.isCurveTo()) {
                // 贝塞尔曲线需要更多处理
                QPointF start = pathPoints.last();
                QPointF c1(elem.x, elem.y);
                QPointF c2(path.elementAt(i+1).x, path.elementAt(i+1).y);
                QPointF end(path.elementAt(i+2).x, path.elementAt(i+2).y);
                i += 2; // 跳过两个控制点
                
                double dist = QLineF(start, end).length() * 1.5; // 曲线长度估算
                int steps = std::max(1, (int)(dist * resolution));
                
                for (int j = 1; j <= steps; ++j) {
                    double t = (double)j / steps;
                    double t2 = t * t;
                    double t3 = t2 * t;
                    double mt = 1 - t;
                    double mt2 = mt * mt;
                    double mt3 = mt2 * mt;
                    
                    double x = mt3 * start.x() + 3 * mt2 * t * c1.x() + 3 * mt * t2 * c2.x() + t3 * end.x();
                    double y = mt3 * start.y() + 3 * mt2 * t * c1.y() + 3 * mt * t2 * c2.y() + t3 * end.y();
                    pathPoints.append(QPointF(x, y));
                }
            }
        }
        
        if (!pathPoints.isEmpty()) {
            allPoints.append(pathPoints);
        }
    }

    // 处理文字项路径（轨迹用 toTrajectoryPath，实心时强制细体）
    for (const auto &item : m_textItems) {
        QPainterPath tp = item.toTrajectoryPath();
        QVector<QPointF> pathPoints;
        for (int i = 0; i < tp.elementCount(); ++i) {
            QPainterPath::Element elem = tp.elementAt(i);
            if (elem.isMoveTo()) {
                if (!pathPoints.isEmpty()) { allPoints.append(pathPoints); pathPoints.clear(); }
                pathPoints.append(QPointF(elem.x, elem.y));
            } else if (elem.isLineTo()) {
                QPointF newPt(elem.x, elem.y);
                QPointF lastPt = pathPoints.last();
                double dist = QLineF(lastPt, newPt).length();
                double minSpacing = (resolution > 0) ? (1.0 / resolution) : 0.5;
                if (dist >= minSpacing) pathPoints.append(newPt);
            } else if (elem.isCurveTo()) {
                QPointF start = pathPoints.last();
                QPointF c1(elem.x, elem.y);
                QPointF c2(tp.elementAt(i+1).x, tp.elementAt(i+1).y);
                QPointF end(tp.elementAt(i+2).x, tp.elementAt(i+2).y);
                i += 2;
                double dist = QLineF(start, end).length() * 1.5;
                int steps = std::max(1, (int)(dist * resolution));
                for (int j = 1; j <= steps; ++j) {
                    double t = (double)j / steps;
                    double x = (1-t)*(1-t)*(1-t)*start.x() + 3*(1-t)*(1-t)*t*c1.x() + 3*(1-t)*t*t*c2.x() + t*t*t*end.x();
                    double y = (1-t)*(1-t)*(1-t)*start.y() + 3*(1-t)*(1-t)*t*c1.y() + 3*(1-t)*t*t*c2.y() + t*t*t*end.y();
                    pathPoints.append(QPointF(x, y));
                }
            }
        }
        if (!pathPoints.isEmpty()) allPoints.append(pathPoints);
    }

    return allPoints;
}

QVector<QVector<QPointF>> DrawingCanvas::extractPointsGrouped(double resolution) const {
    QVector<QVector<QPointF>> allGroups;

    for (const QPainterPath &path : m_paths) {
        QVector<QPointF> pathPoints;

        for (int i = 0; i < path.elementCount(); ++i) {
            QPainterPath::Element elem = path.elementAt(i);

            if (elem.isMoveTo()) {
                if (!pathPoints.isEmpty()) {
                    allGroups.append(pathPoints);
                    pathPoints.clear();
                }
                pathPoints.append(QPointF(elem.x, elem.y));
            } else if (elem.isLineTo()) {
                // 距离过滤：只添加间距 >= minSpacing 的点，避免自由钢笔生成过密点群
                QPointF newPt(elem.x, elem.y);
                QPointF lastPt = pathPoints.last();
                double dist = QLineF(lastPt, newPt).length();
                double minSpacing = (resolution > 0) ? (1.0 / resolution) : 0.5;
                if (dist >= minSpacing) {
                    pathPoints.append(newPt);
                }
            } else if (elem.isCurveTo()) {
                QPointF start = pathPoints.last();
                QPointF c1(elem.x, elem.y);
                QPointF c2(path.elementAt(i+1).x, path.elementAt(i+1).y);
                QPointF end(path.elementAt(i+2).x, path.elementAt(i+2).y);
                i += 2;
                double dist = QLineF(start, end).length() * 1.5;
                int steps = std::max(1, (int)(dist * resolution));
                for (int j = 1; j <= steps; ++j) {
                    double t = (double)j / steps;
                    double t2 = t * t, t3 = t2 * t;
                    double mt = 1 - t, mt2 = mt * mt, mt3 = mt2 * mt;
                    double x = mt3 * start.x() + 3 * mt2 * t * c1.x() + 3 * mt * t2 * c2.x() + t3 * end.x();
                    double y = mt3 * start.y() + 3 * mt2 * t * c1.y() + 3 * mt * t2 * c2.y() + t3 * end.y();
                    pathPoints.append(QPointF(x, y));
                }
            }
        }

        if (!pathPoints.isEmpty()) {
            allGroups.append(pathPoints);
        }
    }

    // 文字项路径：分笔画提取（轨迹用 toTrajectoryPath）
    for (const auto &item : m_textItems) {
        QPainterPath tp = item.toTrajectoryPath();
        QVector<QPointF> strokePoints;
        for (int i = 0; i < tp.elementCount(); ++i) {
            QPainterPath::Element elem = tp.elementAt(i);
            if (elem.isMoveTo()) {
                // 碰到新笔画，先保存上一笔
                if (!strokePoints.isEmpty()) { allGroups.append(strokePoints); strokePoints.clear(); }
                strokePoints.append(QPointF(elem.x, elem.y));
            } else if (elem.isLineTo()) {
                QPointF pt(elem.x, elem.y);
                double dist = QLineF(strokePoints.last(), pt).length();
                double minSpacing = (resolution > 0) ? (1.0 / resolution) : 0.5;
                if (dist >= minSpacing) strokePoints.append(pt);
            } else if (elem.isCurveTo()) {
                // 曲线段简化为直线（取端点），避免贝塞尔采样出大量点
                QPointF end(tp.elementAt(i+2).x, tp.elementAt(i+2).y);
                i += 2;
                double dist = QLineF(strokePoints.last(), end).length();
                double minSpacing = (resolution > 0) ? (1.0 / resolution) : 0.5;
                if (dist >= minSpacing) strokePoints.append(end);
            }
        }
        if (!strokePoints.isEmpty()) allGroups.append(strokePoints);
    }

    return allGroups;
}

void DrawingWindow::drawWorkpieceCoordRegion(const QPointF &origin,
                                              const QPointF &xPt,
                                              const QPointF &yPt) {
    m_canvas->drawWorkpieceCoordRegion(origin, xPt, yPt);
}

void DrawingWindow::addCanvasTextItem(const TextItemData &item) {
    m_canvas->addTextItem(item);
}

double DrawingWindow::canvasViewScale() const {
    return m_canvas->viewScale();
}

QPointF DrawingCanvas::screenToLogical(const QPointF &screenPos) const {
    return (screenPos - m_viewOffset) / m_viewScale;
}

void DrawingCanvas::zoomToFit(const QRectF &bounds) {
    if (bounds.isEmpty()) return;
    double margin = 40.0;
    double availW = width() - 2 * margin;
    double availH = height() - 2 * margin;
    if (availW <= 0 || availH <= 0) return;
    double scaleW = availW / bounds.width();
    double scaleH = availH / bounds.height();
    m_viewScale = std::min(scaleW, scaleH);
    if (m_viewScale < 0.01) m_viewScale = 0.01;
    if (m_viewScale > 100.0) m_viewScale = 100.0;
    // 居中
    QPointF center = bounds.center();
    m_viewOffset = QPointF(width() / 2.0, height() / 2.0) - center * m_viewScale;
    update();
}
