/*
 * @file HomingController.h
 * @brief 声明回原点控制器类，负责六轴机械臂的回零/回原点操作。
 *
 * 本文件定义 HomingController 类，该类负责：
 * - 串行预处理（索引2/1/4，即机械臂第3/2/5轴，依次移开原点/反向限位）
 * - 并行回零（6轴同时执行回零流程）
 * - 回零进度和结果通知
 *
 * 回零流程（参考 HomeManager 设计）：
 * 1. 串行预处理：索引2/1/4（机械臂第3/2/5轴）依次检查传感器、移开原点
 * 2. 并行回零：6轴同时执行回零状态机
 * 3. 每轴状态机：INIT→CHECK_SENSOR→MOVE_AWAY_ORIGIN→WAIT→
 *               MOVE_AWAY_REVERSE→WAIT→START_DATUM→WAIT_DATUM→SUCCESS/FAIL
 *
 * 架构位置：
 * MainWindow -> RobotController -> HomingController -> ZmotionAPI -> 硬件
 */
#ifndef HOMINGCONTROLLER_H
#define HOMINGCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QList>

class ZmotionAPI;
class RobotConfig;

enum HomeAxisState
{
    HOME_INIT = 0,
    HOME_CHECK_SENSOR,
    HOME_MOVE_AWAY_ORIGIN,
    HOME_WAIT_MOVE_AWAY_ORIGIN,
    HOME_MOVE_AWAY_REVERSE,
    HOME_WAIT_MOVE_AWAY_REVERSE,
    HOME_START_DATUM,
    HOME_WAIT_DATUM_START,
    HOME_WAIT_DATUM,
    HOME_SUCCESS,
    HOME_FAIL
};

class HomingController : public QObject
{
    Q_OBJECT

public:
    static constexpr int kAxisCount = 6;

    explicit HomingController(ZmotionAPI* zmotionAPI, RobotConfig* config, QObject *parent = nullptr);
    ~HomingController();

    void reset();
    bool startHoming();
    void stopHoming();
    void step();

    bool isAllHomed() const;
    bool isHoming() const { return m_isHoming; }

    void moveAllAxesToPositions(const QList<float>& positions, float speed = 60.0f, float accel = 80.0f, float decel = 100.0f);
    // 等时到达版本：每轴独立速度（speeds 与 positions 一一对应），
    // 由调用方按行程比例分配速度，使各轴同时到位
    void moveAllAxesToPositions(const QList<float>& positions, const QList<float>& speeds, float accel, float decel);


signals:
    void homingStarted();
    void homingProgress(int axisIndex, HomeAxisState state);
    void homingAxisCompleted(int axisIndex);
    void homingAxisFailed(int axisIndex, const QString& errorMsg);
    void homingCompleted();
    void homingFailed(const QString& errorMsg);

private slots:
    void onPollTimeout();

private:
    ZmotionAPI* m_zmotionAPI;
    RobotConfig* m_config;

    bool m_isHoming;
    bool m_serialPrehomeCompleted;

    QList<HomeAxisState> m_homeState;
    QList<qint64> m_homeStartTimeMs;
    QList<qint64> m_moveStartTimeMs;
    QList<bool> m_isHomed;

    bool m_j2MovedAway;
    bool m_j3MovedAway;
    bool m_j5MovedAway;

    QTimer* m_pollTimer;

    void stepSerial();
    void processPrehomeAxis(int i);
    void stepParallel();

    bool initAxisParams(int i);
    int checkAxisSensors(int i, bool checkNegIn);
    bool isAxisIdle(int i);
    bool moveAxisRelative(int i, float distance);
    bool startAxisDatum(int i);
};

#endif // HOMINGCONTROLLER_H
