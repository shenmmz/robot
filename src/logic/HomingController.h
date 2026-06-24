/*
 * @file HomingController.h
 * @brief 声明回原点控制器类，负责六轴机械臂的回零/回原点操作。
 *
 * 本文件定义 HomingController 类，该类负责：
 * - 串行预处理（J3→J2→J5 依次移开原点/反向限位）
 * - 并行回零（6轴同时执行回零流程）
 * - 回零进度和结果通知
 *
 * 回零流程（参考 HomeManager 设计）：
 * 1. 串行预处理：J3→J2→J5 依次检查传感器、移开原点
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
#include <QElapsedTimer>
#include <QCoreApplication>

class ZmotionAPI;
class RobotConfig;
class RobotState;

struct HomeAxisHWConfig
{
    int axis;              ///< 轴号
    int atype;             ///< 轴类型（1=旋转轴）
    int dir;               ///< 轴方向（1=正向，-1=反向）
    int homeIn;            ///< 原点传感器输入IO编号
    int negIn;             ///< 反向限位传感器输入IO编号（-1表示无）
};

struct HomeStrategy
{
    float moveAwayOrigin;
    float moveAwayReverse;
    float homeSpeed;
    float homeAccel;
    float homeDecel;
    float homeLspeed;
    float homeSramp;
    float homeCreep;
    int datumMode;
};

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

    explicit HomingController(ZmotionAPI* zmotionAPI, RobotConfig* config, RobotState* state, QObject *parent = nullptr);
    ~HomingController();

    void bindZmotionAPI(ZmotionAPI* api);

    void reset();
    bool startHoming();
    void stopHoming();
    void step();

    bool isAllHomed() const;
    bool isHoming() const { return m_isHoming; }
    const QList<bool>& homedFlags() const { return m_isHomed; }

    void setHomingTimeoutMs(int ms) { m_homingTimeoutMs = ms; }
    int getHomingTimeoutMs() const { return m_homingTimeoutMs; }
    void setPollInterval(int ms) { m_pollIntervalMs = ms; }
    int getPollInterval() const { return m_pollIntervalMs; }

    void moveAllAxesToPositions(const QList<float>& positions, float speed = 60.0f, float accel = 80.0f, float decel = 100.0f);


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
    RobotState* m_state;

    bool m_isHoming;
    bool m_serialPrehomeCompleted;
                                    // 仅执行串行预处理（不进入并行回零）

    QList<HomeAxisState> m_homeState;
    QList<qint64> m_homeStartTimeMs;
    QList<qint64> m_moveStartTimeMs;
    QList<bool> m_isHomed;

    bool m_j2MovedAway;
    bool m_j3MovedAway;
    bool m_j5MovedAway;

    QTimer* m_pollTimer;
    int m_pollIntervalMs;
    int m_homingTimeoutMs;

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
