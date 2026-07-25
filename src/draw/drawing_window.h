#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QToolBar>
#include <QAction>
#include <QStack>
#include <QStatusBar>
#include <QLabel>
#include <QVector>
#include <QPointF>
#include <QVector3D>
#include <QVector2D>
#include <QLineEdit>
#include <QPair>
#include <QColor>
#include <QSet>
#include "WorkPieceCoord.h"

/**
 * @brief 绘图工具枚举类型
 */
enum class DrawingTool {
    FreePen,    ///< 自由钢笔
    Line,       ///< 直线
    Rectangle,  ///< 矩形
    Circle,     ///< 圆形
    Arc,        ///< 圆弧
    Select,     ///< 选择/移动
    Eraser      ///< 橡皮擦
};

/**
 * @brief 运动参数结构体
 */
struct DrawMotionParams {
    double speed = 50.0;      // 运行速度 (mm/s)
    double acceleration = 200.0; // 加速度 (mm/s²)
    double deceleration = 200.0; // 减速度 (mm/s²)
    double jerk = 1000.0;     // 加加速度 (mm/s³)
    double sramp = 100.0;     // S曲线时间 (ms)，0=梯形加减速
    int blendMode = 1;        // 拐角过渡模式 (0:无过渡, 1:圆弧过渡, 2:样条过渡) 默认开启平滑
    double blendRadius = 5.0; // 拐角过渡半径 (mm)
    double rx = 0.0;          // 工具姿态 RX (度)
    double ry = 0.0;          // 工具姿态 RY (度)
    double rz = 0.0;          // 工具姿态 RZ (度)
};

/**
 * @brief 圆形参数：圆心 + 半径（执行时用圆弧插补指令，不走采样点）
 */
struct CircleData {
    QPointF center;
    double radius;
};

/**
 * @brief 弧线参数：三点弧 (起点 + 经过点 + 终点)
 */
struct ArcData {
    QPointF start;
    QPointF mid;
    QPointF end;
};

/**
 * @brief 文字项数据
 */
struct TextItemData {
    QString text;
    QPointF pos;        // 左上角位置
    QString fontFamily = "KaiTi";
    int fontSize = 50;
    bool solidStroke = true;  // true=轨迹用细体(单线实心), false=轨迹用正常体(轮廓空心)
    double scale = 1.0;

    // 屏幕显示用路径
    QPainterPath toPath() const {
        return toPathNormal();
    }

    // 轨迹执行用路径（solidStroke 时骨架化 → 单线），结果缓存
    QPainterPath toTrajectoryPath() const {
        if (!m_trajCached || m_cachedScale != scale || m_cachedFontSize != fontSize
            || m_cachedPos != pos) {
            m_trajCache = computeTrajectoryPath();
            m_cachedScale = scale;
            m_cachedFontSize = fontSize;
            m_cachedPos = pos;
            m_trajCached = true;
        }
        return m_trajCache;
    }

    QRectF boundingRect() const {
        return toPath().boundingRect();
    }

private:
    QPainterPath computeTrajectoryPath() const;
    mutable QPainterPath m_trajCache;
    mutable bool m_trajCached = false;
    mutable double m_cachedScale = 1.0;
    mutable int m_cachedFontSize = 50;
    mutable QPointF m_cachedPos;

private:
    QPainterPath toPathNormal() const {
        QPainterPath p;
        QFont f(fontFamily, fontSize);
        double s = scale;
        if (s < 0.1) s = 0.1;
        if (s > 10.0) s = 10.0;
        QPainterPath raw;
        raw.addText(0, 0, f, text);
        QTransform t;
        t.translate(pos.x(), pos.y());
        t.scale(s, s);
        p = t.map(raw);
        return p;
    }
};

/**
 * @brief 剪贴板数据结构（深拷贝）
 */
struct ClipboardData {
    QList<QPainterPath> paths;
    QVector<CircleData> circles;
    QVector<ArcData> arcs;
    QVector<TextItemData> texts;
    bool isEmpty() const {
        return paths.isEmpty() && circles.isEmpty() && arcs.isEmpty() && texts.isEmpty();
    }
    void clear() { paths.clear(); circles.clear(); arcs.clear(); texts.clear(); }
};

/**
 * @brief 绘图画布类
 */
class DrawingCanvas : public QWidget {
    Q_OBJECT
public:
    explicit DrawingCanvas(QWidget *parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);
        setBackgroundRole(QPalette::Base);
        setAutoFillBackground(true);
        m_showWorkPiecePlane = false;
        m_showGrid = true;
    }

    void setCurrentTool(DrawingTool tool) {
        if (m_currentTool == DrawingTool::Select) deselectAll();
        if (m_currentTool == DrawingTool::Eraser && tool != DrawingTool::Eraser) {
            m_eraserPoints.clear();
            m_eraserCoveragePath = QPainterPath();
        }
        m_currentTool = tool;
        m_drawing = false;
        m_arcSecondPointSet = false;
    }
    void setWorkPiecePlaneSize(const QSizeF &size) { m_workPiecePlaneSize = size; m_showWorkPiecePlane = true; update(); }
    void clearWorkPiecePlane() { m_showWorkPiecePlane = false; update(); }
    void drawWorkpieceCoordRegion(const QPointF &origin, const QPointF &xPt, const QPointF &yPt);
    QPointF screenToLogical(const QPointF &screenPos) const;
    double viewScale() const { return m_viewScale; }
    void zoomToFit(const QRectF &bounds);
    void clearAll() {
        m_paths.clear();
        m_coloredPaths.clear();
        m_textItems.clear();
        m_circles.clear();
        m_arcs.clear();
        m_undoStack.clear();
        m_eraserPoints.clear();
        m_eraserCoveragePath = QPainterPath();
        m_drawing = false;
        m_arcSecondPointSet = false;
        m_selectedText = -1;
        deselectAll();
        update();
    }

    bool undo() {
        if (!m_textItems.isEmpty()) {
            m_textItems.removeLast();
            update();
            return true;
        }
        if (m_paths.isEmpty()) return false;
        m_undoStack.push(m_paths.last());
        m_paths.removeLast();
        m_drawing = false;
        m_arcSecondPointSet = false;
        update();
        return true;
    }

    bool redo() {
        if (m_undoStack.isEmpty()) return false;
        m_paths.append(m_undoStack.pop());
        update();
        return true;
    }

    void addPath(const QPainterPath &path) {
        m_paths.append(path);
        update();
    }

    void addTextItem(const TextItemData &item) {
        m_textItems.append(item);
        update();
    }

    const QVector<TextItemData>& textItems() const { return m_textItems; }

    QVector<CircleData> getCircles() const { return m_circles; }
    QVector<ArcData> getArcs() const { return m_arcs; }
    void addCircle(const QPointF &center, double radius) {
        m_circles.append({center, radius});
        update();
    }

    const QList<QPainterPath>& paths() const { return m_paths; }

    QVector<QPointF> extractPoints(double resolution = 1.0) const;
    QVector<QVector<QPointF>> extractPointsGrouped(double resolution = 1.0) const;

    void deselectAll() {
        m_selectedText = -1;
        m_selectedPathIndices.clear();
        m_selectedCircleIndices.clear();
        m_selectedArcIndices.clear();
        m_selectedTextIndices.clear();
        update();
    }

signals:
    void mousePositionChanged(const QPointF &pos);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    DrawingTool m_currentTool{DrawingTool::FreePen};
    QPointF m_startPoint;
    QPointF m_endPoint;
    bool m_drawing{false};
    QList<QPainterPath> m_paths;
    QVector<QPair<QPainterPath, QColor>> m_coloredPaths;  // 带颜色属性的路径
    QVector<TextItemData> m_textItems;
    int m_selectedText = -1;
    QPointF m_dragOffset;
    QVector<CircleData> m_circles;
    QVector<ArcData> m_arcs;
    QPainterPath m_currentPath;
    QStack<QPainterPath> m_undoStack;
    // 橡皮擦
    QVector<QPointF> m_eraserPoints;
    QPainterPath m_eraserCoveragePath;
    double m_eraserRadius{10.0};
    bool m_showGrid{false};
    int m_gridSpacing{20};
    QPointF m_arcStartPoint;
    QPointF m_arcMidPoint;
    bool m_arcSecondPointSet{false};
    bool m_showWorkPiecePlane{false};
    QSizeF m_workPiecePlaneSize;
    QPointF m_workPiecePlaneOffset;

    // 视图变换
    double m_viewScale{1.0};
    QPointF m_viewOffset{0, 0};
    bool m_isPanning{false};
    QPointF m_panStart;

    // 选择功能
    QSet<int> m_selectedPathIndices;
    QSet<int> m_selectedCircleIndices;
    QSet<int> m_selectedArcIndices;
    QSet<int> m_selectedTextIndices;
    bool m_isSelectingRect{false};
    QPointF m_selectRectStart;
    QPointF m_selectRectEnd;
    QPointF m_moveLastPoint;      // 移动操作的上一个鼠标位置
    bool m_isMovingElements{false};

    // 剪贴板
    ClipboardData m_clipboard;

    // 辅助方法
    int hitTestPath(const QPointF &pt) const;
    int hitTestCircle(const QPointF &pt) const;
    int hitTestArc(const QPointF &pt) const;
    bool isAnySelected() const;
    void clearSelection();
    void copySelection();
    void pasteSelection();
    QRectF computeSelectionBounds() const;
    QRectF pathBoundingRect(const QPainterPath &path) const;
    void drawBoundingBox(QPainter &painter, const QRectF &rect, const QColor &color) const;

    void drawGrid(QPainter &painter);
    void drawArcThroughThreePoints(QPainterPath &path, const QPointF &p1, const QPointF &p2, const QPointF &p3) const;
    void drawWorkPiecePlane(QPainter &painter);
    void drawCoordinateAxes(QPainter &painter);
    int hitTestText(const QPointF &pt) const;
};

/**
 * @brief 绘图窗口类
 */
class DrawingWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit DrawingWindow(QWidget *parent = nullptr);
    ~DrawingWindow();

    void setWorkPieceCoord(const WorkPieceCoord *coord);
    void setDefaultPose(double rx, double ry, double rz);
    void drawWorkpieceCoordRegion(const QPointF &origin, const QPointF &xPt, const QPointF &yPt);
    void addCanvasTextItem(const TextItemData &item);
    double canvasViewScale() const;

    QVector<QVector3D> getDrawingPoints3D(double resolution = 1.0) const;
    QVector<QPointF> getDrawingPoints(double resolution = 1.0) const;
    QVector<QVector<QVector3D>> getDrawingPoints3DGrouped(double resolution = 1.0) const;
    void getDrawingCircles3D(QVector<QVector3D> &centers, QVector<double> &radii) const;
    void getDrawingArcs3D(QVector<QVector3D> &starts, QVector<QVector3D> &mids, QVector<QVector3D> &ends) const;

signals:
    void sendDrawingPoints(const QVector<QVector<QVector3D>> &pointGroups, double rx, double ry, double rz);
    void sendDrawingCircles(const QVector<QVector3D> &centers, const QVector<double> &radii);
    void sendDrawingArcs(const QVector<QVector3D> &starts, const QVector<QVector3D> &mids, const QVector<QVector3D> &ends);
    void requestInverseKinematics();
    void requestStopMotion();
    void requestDrawMotionParams(const DrawMotionParams &params);
    void requestWorkpieceCoordSetup();              // 请求打开工件坐标系设置对话框

private slots:
    void onClearCanvas();
    void onUndo();
    void onRedo();
    void onExtractPoints();
    void onSendPoints();
    void onStopPoints();
    void onExportPoints();
    void onSavePoints();
    void onSetWorkPieceCoord();
    void onMotionSettings();
    void onTextInput();
    void onTextDialog();

private:
    DrawingCanvas *m_canvas;
    QLabel *m_coordLabel;
    QLabel *m_toolLabel;
    QLabel *m_coordSysLabel;
    const WorkPieceCoord *m_workPieceCoord;
    DrawMotionParams m_motionParams;

    void createToolBar();
    void updateCoordSysLabel();
};
