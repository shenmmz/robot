/*
 * @file ServoTestWidget.h
 * @brief 舵机设置界面组件
 *
 * 提供6个舵机通道的独立测试界面，支持：
 * - 勾选要控制的舵机
 * - 设置速度和角度
 * - 实时显示当前角度
 */
#ifndef SERVOTESTWIDGET_H
#define SERVOTESTWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <functional>

class ServoController;
class UiController;
class QLabel;
class QTimer;

class ServoTestWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param servo      舵机控制器指针
     * @param ui         UiController 指针（用于 "插入到编辑器" 功能，可空）
     * @param parent     父窗口
     */
    explicit ServoTestWidget(ServoController* servo, UiController* ui = nullptr, QWidget *parent = nullptr);
    ~ServoTestWidget();

protected:
    /**
     * @brief 弹窗每次显示时同步一次 ServoController 当前角度到 UI
     *
     * 解决"刚打开舵机设置显示默认 90°"：把 ServoController::getCurrentAngle() 的值
     * 回写到 6 路"当前"标签、滑块、spinbox（仅未勾选通道，避免打断用户输入）。
     */
    void showEvent(QShowEvent* event) override;

private slots:
    /**
     * @brief 角度更新回调
     */
    void onAngleChanged(int channel, float angle);

private:
    /**
     * @brief 初始化UI
     */
    void setupUI();

    /**
     * @brief 刷新某通道的勾选/锁定状态：勾选后启用角度控件；未勾选则禁用。
     * @param channel 通道号 (0-5)
     */
    void updateChannelLockState(int channel);

    /**
     * @brief "📋 插入 SERVO 到编辑器"槽：按勾选通道生成 SERVO(ch=ang:Tms) 插入主编辑器
     */
    void onInsertToEditor();

private:
    ServoController* m_servo;  // 舵机控制器指针
    UiController*    m_ui;     // 主 UI 控制器（用于插入文本到编辑器；可空）

    // UI组件
    QCheckBox* m_checkBoxes[6];       // 勾选框
    QSlider* m_angleSliders[6];        // 角度滑块
    QLineEdit* m_angleSpins[6];        // 角度输入
    QLineEdit* m_speedSpins[6];        // 速度输入
    QLabel* m_currentLabels[6];         // 当前角度显示

    // 插入到编辑器的时间参数（ms，默认 500）
    QLineEdit* m_insertTimeSpin = nullptr;

    // 记录哪个通道被勾选
    bool m_checkedChannels[6];

    // 操作反馈 toast(顶部居中浮层)
    QLabel*    m_toastLabel = nullptr;
    QTimer*    m_toastTimer = nullptr;

private slots:
    /**
     * @brief 隐藏 toast 浮层(QTimer 超时回调)
     */
    void onToastTimeout();

public:
    /**
     * @brief 在窗口顶部弹出临时提示,2 秒后自动消失
     * @param text 提示文本(空字符串则立即隐藏)
     */
    void showToast(const QString& text);
};

#endif // SERVOTESTWIDGET_H
