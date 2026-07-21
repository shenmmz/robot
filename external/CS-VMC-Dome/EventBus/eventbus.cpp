/**
 * @file eventbus.cpp
 * @brief EventBus事件总线框架实现文件
 * @details 实现线程安全的事件总线系统，支持事件的发布、订阅和取消订阅
 * @author AI Assistant
 * @date 2025-01-17
 * @version 1.0.0
 */

#include "eventbus.h"
#include <algorithm>
#include <string>

// ============================================================================
// EventBus类实现
// ============================================================================

/**
 * @brief 获取EventBus单例实例
 * @return EventBus单例引用
 * @details 使用Meyer's Singleton模式实现线程安全的单例
 *          C++11标准保证静态局部变量的初始化是线程安全的
 */
EventBus& EventBus::getInstance() {
    // 创建函数内静态单例对象，C++11 保证初始化线程安全。
    static EventBus instance;  // 线程安全的单例实现

    // 返回事件总线单例引用。
    return instance;
}



/**
 * @brief 清空所有订阅
 * @details 移除所有订阅者的订阅信息，通常在系统关闭时调用
 *          线程安全操作
 */
void EventBus::clear() {
    // 加锁保护订阅者容器，避免清空时与发布/订阅并发访问。
    std::lock_guard<std::mutex> lock(m_mutex);

    // 清空所有事件类型下的订阅者列表。
    m_subscribers.clear();
}
