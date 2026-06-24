/*
 * @file ServoTestWidget.h
 * @brief 舵机测试界面组件
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
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

class ServoController;

class ServoTestWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param servo 舵机控制器指针
     * @param parent 父窗口
     */
    explicit ServoTestWidget(ServoController* servo, QWidget *parent = nullptr);
    ~ServoTestWidget();

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

private:
    ServoController* m_servo;  // 舵机控制器指针

    // UI组件
    QCheckBox* m_checkBoxes[6];       // 勾选框
    QSlider* m_angleSliders[6];        // 角度滑块
    QDoubleSpinBox* m_angleSpins[6];   // 角度输入
    QDoubleSpinBox* m_speedSpins[6];   // 速度输入
    QLabel* m_currentLabels[6];         // 当前角度显示

    // 记录哪个通道被勾选
    bool m_checkedChannels[6];
};

#endif // SERVOTESTWIDGET_H
