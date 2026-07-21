/**
 * @file eventbus.h
 * @brief 线程安全的轻量事件总线示例。
 *
 * 说明：当前标准模块示例已经改为 MainWindow 与 VMControl 直接使用信号槽通信，
 * EventBus 文件仅作为历史广播方案示例保留，不参与主程序构建。
 */
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <typeindex>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <QDebug>
#include "eventTypes.h"

/**
 * @brief 基于事件类型的进程内发布订阅总线。
 *
 * 输入数据：事件类型 Event 和事件对象。
 * 输出数据：订阅 ID 或订阅者回调执行结果。
 */
class EventBus {
public:
    /**
     * @brief 获取 EventBus 单例。
     * @return EventBus 全局唯一实例。
     */
    static EventBus& instance() {
        // 使用函数内静态变量实现线程安全单例。
        static EventBus bus;

        // 返回单例引用。
        return bus;
    }

    /**
     * @brief 订阅指定类型事件。
     * @tparam Event 事件类型。
     * @param callback 事件回调函数。
     * @return 订阅 ID，可用于取消订阅。
     */
    template<typename Event>
    size_t subscribe(std::function<void(const Event&)> callback) {

        // 将类型化回调包装成统一基类指针。
        auto callbackPtr = std::make_shared<Callback<Event>>(callback);

        // 使用回调对象地址生成订阅 ID。
        size_t subscriptionId = reinterpret_cast<size_t>(callbackPtr.get());

        {
            // 加锁保护订阅者容器。
            std::lock_guard<std::mutex> lock(mutex_);

            // 按事件类型获取订阅列表。
            auto& eventSubscribers = subscribers_[typeid(Event)];

            // 保存订阅 ID 和回调对象。
            eventSubscribers.push_back({subscriptionId, callbackPtr});
        }

        // 返回订阅 ID 给调用方。
        return subscriptionId;
    }

    /**
     * @brief 取消指定订阅。
     * @tparam Event 事件类型。
     * @param subscriptionId subscribe 返回的订阅 ID。
     * @return true 表示找到并移除订阅，false 表示没有找到。
     */
    template<typename Event>
    bool unsubscribe(size_t subscriptionId) {
        // 加锁保护订阅者容器。
        std::lock_guard<std::mutex> lock(mutex_);

        // 查找指定事件类型的订阅列表。
        auto it = subscribers_.find(typeid(Event));

        // 事件类型存在时尝试删除匹配 ID 的订阅者。
        if (it != subscribers_.end()) {
            // 取出当前事件类型的订阅者列表。
            auto& eventSubscribers = it->second;

            // 查找需要删除的订阅者范围。
            auto removeIt = std::remove_if(eventSubscribers.begin(), eventSubscribers.end(),
                                           [subscriptionId](const auto& sub) {
                                               // 按订阅 ID 匹配。
                                               return sub.first == subscriptionId;
                                           });

            // 找到匹配项时从列表中删除。
            if (removeIt != eventSubscribers.end()) {
                eventSubscribers.erase(removeIt, eventSubscribers.end());
                return true;
            }
        }

        // 未找到匹配订阅。
        return false;
    }

    /**
     * @brief 发布指定类型事件。
     * @tparam Event 事件类型。
     * @param event 事件数据。
     *
     * 输入数据：事件对象。
     * 输出数据：依次触发所有订阅该事件类型的回调。
     */
    template<typename Event>
    void publish(const Event& event) {
        // 临时保存回调列表，避免在锁内执行用户代码。
        std::vector<std::shared_ptr<ICallback>> callbacks;

        {
            // 先复制一份回调列表，再释放锁
            std::lock_guard<std::mutex> lock(mutex_);

            // 查找当前事件类型的订阅列表。
            auto it = subscribers_.find(typeid(Event));

            // 存在订阅者时复制回调对象。
            if (it != subscribers_.end()) {
                for (auto& [id, sub] : it->second) {
                    // 当前发布逻辑不使用 id，显式标记避免误解。
                    Q_UNUSED(id);

                    // 复制 shared_ptr，确保锁释放后回调对象仍然有效。
                    callbacks.push_back(sub); // 拷贝出来
                }
            }
        }

        // 在锁外执行回调，避免死锁
        for (auto& sub : callbacks) {
            // 将统一基类指针转换回当前事件类型的回调包装器。
            auto func = std::static_pointer_cast<Callback<Event>>(sub);

            // 回调对象存在时执行。
            if (func) {
                try {
                    // 调用订阅者回调并传入事件数据。
                    (*func)(event);
                } catch (...) {
                    // 防止订阅者抛异常导致其他订阅者无法执行
                    // 这里可以加日志
                }
            }
        }
    }

private:
    /**
     * @brief 回调基类。
     *
     * 用于把不同 Event 类型的回调统一存储到同一个容器中。
     */
    struct ICallback { virtual ~ICallback() = default; };

    /**
     * @brief 类型化回调包装器。
     * @tparam Event 事件类型。
     */
    template<typename Event>
    struct Callback : ICallback {
        std::function<void(const Event&)> func; ///< 实际订阅回调。

        /** @brief 保存回调函数。 */
        Callback(std::function<void(const Event&)> f) : func(std::move(f)) {}

        /** @brief 执行回调函数。 */
        void operator()(const Event& e) { func(e); }
    };

    // 按事件类型保存订阅者列表，每个订阅者由订阅 ID 和回调对象组成。
    std::unordered_map<std::type_index,
                       std::vector<std::pair<size_t, std::shared_ptr<ICallback>>>> subscribers_;

    // 保护 subscribers_ 的互斥锁。
    std::mutex mutex_;
};
