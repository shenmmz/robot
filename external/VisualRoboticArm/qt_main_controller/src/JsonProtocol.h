#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace armcomm {

// Qt 主控与 Python 视觉进程共用的通讯协议。
//
// 传输方式：默认本机 TCP。
// 分帧方式：每条消息是一个紧凑 JSON 对象，并以 '\n' 结尾。
// 通用必备字段：
//   type：消息类型，例如 hello / heartbeat / pose / cmd / ack。
//   seq：发送端自增序号。
//   ts：Unix 时间戳，单位秒。
//   role：发送端身份，"qt" 或 "python"。
//
// 此头文件保持自包含，方便后续直接复制到正式 Qt 项目中，
// 不需要同时引入测试 UI 或 QProcess 管理代码。
inline QJsonObject buildMessage(const QString &type,
                                qint64 seq,
                                const QString &role,
                                const QJsonObject &payload = QJsonObject(),
                                const QString &msg = QString(),
                                const QJsonObject &extra = QJsonObject())
{
    QJsonObject message;
    message.insert(QStringLiteral("type"), type);
    message.insert(QStringLiteral("seq"), static_cast<double>(seq));
    message.insert(QStringLiteral("ts"), QDateTime::currentMSecsSinceEpoch() / 1000.0);
    message.insert(QStringLiteral("role"), role);

    if (!payload.isEmpty()) {
        message.insert(QStringLiteral("payload"), payload);
    }
    if (!msg.isEmpty()) {
        message.insert(QStringLiteral("msg"), msg);
    }
    for (auto it = extra.begin(); it != extra.end(); ++it) {
        message.insert(it.key(), it.value());
    }

    return message;
}

inline QByteArray encodeMessage(const QJsonObject &message)
{
    return QJsonDocument(message).toJson(QJsonDocument::Compact) + '\n';
}

inline bool decodeMessage(const QByteArray &line, QJsonObject *out, QString *error = nullptr)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (error) {
            *error = parseError.errorString();
        }
        return false;
    }
    if (!doc.isObject()) {
        if (error) {
            *error = QStringLiteral("JSON message must be an object");
        }
        return false;
    }
    if (out) {
        *out = doc.object();
    }
    return true;
}

inline QString messageType(const QJsonObject &message)
{
    return message.value(QStringLiteral("type")).toString();
}

inline qint64 messageSeq(const QJsonObject &message)
{
    return static_cast<qint64>(message.value(QStringLiteral("seq")).toDouble());
}

} // 命名空间 armcomm
