// windows.h 必须在 VM SDK 头文件之前包含，否则 SDK 的
// using namespace std 与 rpcndr.h 的 typedef unsigned char byte 冲突
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "vmbase.h"
#include "VMException.h"

#include <QString>
#include <vector>

#ifdef _WIN32
// 显式 GBK(CP936) 编解码，绕开 /utf-8 编译标志对 QString::fromLocal8Bit
// 的影响。SDK 内部以 GBK 索引流程名，只有 CP936 才能保证双向一致。
static QString gbkToQString(const char* s)
{
    if (!s) return QString();
    int wlen = MultiByteToWideChar(936, 0, s, -1, nullptr, 0);
    if (wlen <= 0) return QString();
    std::vector<wchar_t> wbuf(static_cast<size_t>(wlen));
    MultiByteToWideChar(936, 0, s, -1, wbuf.data(), wlen);
    return QString::fromWCharArray(wbuf.data(), wlen - 1);
}

static QByteArray qstringToGbk(const QString& s)
{
    if (s.isEmpty()) return QByteArray();
    std::wstring w = s.toStdWString();
    int len = WideCharToMultiByte(936, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (len <= 0) return QByteArray();
    QByteArray out;
    out.resize(len - 1);
    WideCharToMultiByte(936, 0, w.c_str(), -1, out.data(), len, nullptr, nullptr);
    return out;
}
#endif

/**
 * @brief 构造 VMbase。
 * @param parent Qt 父对象。
 *
 * 输入数据：Qt 父对象。
 * 输出数据：创建未加载方案状态的 VMbase 实例。
 */
VMbase::VMbase(QObject *parent)
    : QObject(parent)
    , pVmSol(nullptr)
{
    /** 注意：只为空解决方案创建工具实例。此解决方案中没有过程。*/
    // pVmSol = CreateSolutionInstance();

    // 初始化方案有效标记，只有 loadSol 成功后才认为方案可用。
    m_solutionValid = false;
}

/**
 * @brief 析构 VMbase 并释放 SDK 资源。
 *
 * 输入数据：无。
 * 输出数据：销毁方案实例并释放 VisionMaster SDK 全局资源。
 */
VMbase::~VMbase()
{
    // 如果方案实例存在，先销毁方案实例。
    if (pVmSol) {
        DestroySolutionInstance(pVmSol);
        pVmSol = nullptr;
    }

    // 释放 SDK 内部资源。
    DisposeResource();
}

/**
 * @brief 加载视觉方案，为解决方案创建工具实例。
 * @param solPath 方案路径。
 * @param password 方案密码。
 * @return VMbaseInfo：success 表示加载是否成功，message 表示执行结果。
 */
VMbaseInfo VMbase::loadSol(const QString& solPath, const QString& password)
{
    // 初始化返回状态。
    bool success = false;
    QString message;

    try {
        // 加载新方案前先释放原有方案实例，避免 SDK 句柄泄漏。
        if (pVmSol) {
            DestroySolutionInstance(pVmSol);
            pVmSol = nullptr;
        }

        // 调用 SDK 加载方案，返回方案实例指针。
        pVmSol = LoadSolution(solPath.toStdString().c_str(),
                              password.toStdString().c_str());

        // 判断 SDK 是否成功创建方案实例。
        if (!pVmSol) {
            message = "加载解决方案失败: 实例创建失败";
        } else {
            success = true;
            message = "加载解决方案成功";
        }
    } catch (CVmException e) {
        // 捕获 VisionMaster SDK 异常并输出错误码。
        message = QString("加载解决方案失败: 0x%1").arg(QString::number(e.GetErrorCode(), 16));
    } catch (...) {
        // 捕获其他未知异常，保证调用方不会因为 SDK 异常崩溃。
        message = "加载解决方案失败: 未知异常";
    }

    // 返回加载结果。
    return {success, message};
}

/**
 * @brief 读取当前方案是否已加载。
 *
 * 输入数据：无。
 * 输出数据：true 表示方案实例存在，false 表示未加载。
 */
bool VMbase::readSol()
{
    // 方案实例为空表示未加载。
    if (!pVmSol) {
        return false;
    } else {
        return true;
    }
}

/**
 * @brief 将当前方案另存为新文件。
 * @param solPath 新方案路径。
 * @param password 新方案密码。
 * @return VMbaseInfo：pack["solPath"] 返回另存为路径。
 */
VMbaseInfo VMbase::SaveAsSol(const QString& solPath, const QString& password)
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    try {
        // 调用 SDK 另存为接口。
        pVmSol->SaveAsSolution(solPath.toStdString().c_str(), password.toStdString().c_str());

        // 记录成功状态和输出路径。
        success = true;
        message = "保存解决方案成功";
        pack["solPath"] = solPath;
    } catch (...) {
        // 捕获另存为过程中的未知异常。
        message = "保存解决方案失败: 未知异常";
    }

    // 返回另存为结果。
    return {success, message, pack};
}

/**
 * @brief 保存视觉方案到源文件。
 *
 * 输入数据：无。
 * 输出数据：VMbaseInfo，message 描述保存结果。
 */
VMbaseInfo VMbase::SaveSol()
{
    // 初始化返回状态。
    bool success = false;
    QString message;

    try {
        // 调用 SDK 保存当前方案。
        pVmSol->SaveSolution();

        // 记录保存成功状态。
        success = true;
        message = "保存方案成功";
    } catch (const CVmException e) {
        // 捕获 SDK 异常并输出错误码。
        message = QString("保存方案失败: 0x%1").arg(QString::number(e.GetErrorCode(), 16));
    } catch (const std::exception& e) {
        // 捕获标准库异常并输出异常文本。
        message = QString("保存方案失败 (系统异常): %1").arg(e.what());
    } catch (...) {
        // 捕获未知异常。
        message = "保存方案失败: 未知异常";
    }

    // 返回保存结果。
    return {success, message};
}

/**
 * @brief 关闭当前解决方案。
 *
 * 输入数据：无。
 * 输出数据：VMbaseInfo，success 表示关闭是否成功。
 */
VMbaseInfo VMbase::closeSol()
{
    // 初始化返回状态。
    bool success = false;
    QString message;

    try {
        // 销毁当前方案实例。
        DestroySolutionInstance(pVmSol);

        // 清空方案指针，避免后续误用悬空指针。
        pVmSol = nullptr;

        // 记录关闭成功状态。
        success = true;
        message = "关闭解决方案成功";
    } catch (const CVmException e) {
        // 捕获 SDK 异常并输出错误码。
        message = QString("关闭解决方案失败: 0x%1").arg(QString::number(e.GetErrorCode(), 16));
    } catch (const std::exception& e) {
        // 捕获标准库异常并输出异常文本。
        message = QString("关闭解决方案失败 (系统异常): %1").arg(e.what());
    } catch (...) {
        // 捕获未知异常。
        message = "关闭解决方案失败: 未知异常";
    }

    // 返回关闭结果。
    return {success, message};
}

/**
 * @brief 整套方案执行一次。
 *
 * 输入数据：无。
 * 输出数据：VMbaseInfo，message 描述执行结果。
 */
VMbaseInfo VMbase::runSol()
{
    // 初始化返回状态。
    bool success = false;
    QString message;

    try {
        // 调用 SDK 方案执行一次接口。
        pVmSol->Run();

        // 记录执行消息。
        message = "全方案执行一次";
    } catch (CVmException e) {
        // 捕获 SDK 异常并输出错误码。
        message = QString("运行方案失败: 0x%1").arg(QString::number(e.GetErrorCode(), 16));
    } catch (...) {
        // 捕获未知异常。
        message = "运行方案失败: 未知异常";
    }

    // 返回执行结果。
    return {success, message};
}

/**
 * @brief 整套方案连续执行。
 *
 * 输入数据：无。
 * 输出数据：VMbaseInfo，success 表示连续执行是否启动成功。
 */
VMbaseInfo VMbase::runingSol()
{
    // 初始化返回状态。
    bool success = false;
    QString message;

    try {
        // 调用 SDK 方案连续执行接口。
        pVmSol->Runing();

        // 记录启动成功状态。
        success = true;
        message = "正在连续运行方案中";
    } catch (CVmException e) {
        // 捕获 SDK 异常并输出错误码。
        message = QString("启动连续运行方案失败: 0x%1").arg(QString::number(e.GetErrorCode(), 16));
    } catch (...) {
        // 捕获未知异常。
        message = "启动连续运行方案失败: 未知异常";
    }

    // 返回连续执行启动结果。
    return {success, message};
}

/**
 * @brief 停止整套方案执行。
 *
 * 输入数据：无。
 * 输出数据：VMbaseInfo，success 表示停止是否成功。
 */
VMbaseInfo VMbase::stopRunSol()
{
    // 初始化返回状态。
    bool success = false;
    QString message;

    try {
        // 调用 SDK 停止方案执行接口。
        pVmSol->StopRun();

        // 记录停止成功状态。
        success = true;
        message = "已停止运行方案";
    } catch (CVmException e) {
        // 捕获 SDK 异常并输出错误码。
        message = QString("已停止运行方案: 0x%1").arg(QString::number(e.GetErrorCode(), 16));
    } catch (...) {
        // 捕获未知异常。
        message = "已停止运行方案: 未知异常";
    }

    // 返回停止结果。
    return {success, message};
}

/**
 * @brief 获取当前方案中的所有流程信息列表。
 *
 * 输入数据：无。
 * 输出数据：pack["number"] 为流程数量，pack["groupNameN"] 为流程名。
 */
VMbaseInfo VMbase::getprocedureObjList()
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    try {
        // 从 SDK 获取方案内所有流程对象。
        ObjectList ObjList = pVmSol->GetAllProcedureObjects();

        // 读取流程数量。
        int number = ObjList.nNum;

        // 没有流程对象时直接返回失败信息。
        if (number == 0) {
            message = "流程对象数量为空，请检查方案文件";
            return {success, message};
        }

        // 标记流程列表获取成功。
        success = true;

        // 保存 SDK 原始流程对象列表，供需要时扩展使用。
        pack["procedureObjList"] = QVariant::fromValue(ObjList);

        // 记录流程数量消息。
        message = QString("已获取所有流程对象,数量: [%1]个\n").arg(number);

        // 保存流程数量。
        pack["number"] = number;

        // 遍历所有流程对象并提取流程名称。
        for (int i = 0; i < number; ++i) {
            // 为每个流程生成稳定的字段名。
            QString groupName = QString("groupName%1").arg(i);

            // 将 SDK 对象指针转换为 IVmProcedure 指针。
            IVmProcedure* pProcedure = static_cast<IVmProcedure*>(ObjList.pObjects[i]);

            // 从 SDK 流程对象中读取流程名（SDK 返回 UTF-8）
            QString procName = QString::fromUtf8(pProcedure->GetName());

            // 把流程名写入返回数据包。
            pack[groupName] = procName;

            // 追加流程字段映射说明。
            message.append(QString("[%1]的流程对象对应提取词为：[%2]\n").arg(procName).arg(groupName));
        }
    } catch (...) {
        // 捕获流程列表获取过程中的未知异常。
        message = QString("获取所有流程对象失败：未知错误");
    }

    // 返回流程列表结果。
    return {success, message, pack};
}

/**
 * @brief 禁用所有模块结果回调。
 *
 * 输入数据：无。
 * 输出数据：VMbaseInfo，success 表示禁用是否成功。
 */
VMbaseInfo VMbase::DisableCallback()
{
    // 初始化返回状态。
    bool success = false;
    QString message;

    try {
        // 调用 SDK 禁用全部模块回调。
        pVmSol->DisableModulesCallback();

        // 记录禁用成功状态。
        success = true;
        message = QString("已禁用所有模块回调");
    } catch (...) {
        // 捕获禁用回调过程中的未知异常。
        message = QString("禁用所有模块回调失败,请检查方案对象");
    }

    // 返回禁用回调结果。
    return {success, message};
}

/**
 * @brief 启用所有模块结果回调。
 *
 * 输入数据：无。
 * 输出数据：VMbaseInfo，success 表示启用是否成功。
 */
VMbaseInfo VMbase::EnableCallback()
{
    // 初始化返回状态。
    bool success = false;
    QString message;

    try {
        // 调用 SDK 启用全部模块回调。
        pVmSol->EnableModulesCallback();

        // 记录启用成功状态。
        success = true;
        message = QString("启用所有模块回调");
    } catch (...) {
        // 捕获启用回调过程中的未知异常。
        message = QString("启用所有模块回调失败,请检查方案对象");
    }

    // 返回启用回调结果。
    return {success, message};
}

/**
 * @brief 使用绝对路径加载流程文件。
 * @param procPath 流程文件绝对路径。
 * @return VMbaseInfo：pack["IVmProcedure"] 返回流程对象指针。
 */
VMbaseInfo VMbase::loadProc(const QString& procPath)
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    try {
        // 调用 SDK 按文件路径加载流程对象。
        IVmProcedure *pVmPrc = LoadProcedure(procPath.toUtf8().constData());

        // SDK 返回空指针时表示流程加载失败。
        if (NULL == pVmPrc)
        {
            message = "加载[%1]失败:流程对象为空";
            return {success, message};
        }

        // 把流程指针放入 QVariantMap，供上层需要时获取。
        pack["IVmProcedure"] = QVariant::fromValue(pVmPrc);

        // 记录加载成功状态。
        message = "加载流程成功";
        success = true;
    } catch (...) {
        // 捕获流程加载过程中的未知异常。
        message = QString("加载[%1]失败:未知错误").arg(procPath);
    }

    // 返回流程加载结果。
    return {success, message, pack};
}

/**
 * @brief 根据流程名称获取流程对象。
 * @param processName 流程名称。
 * @return VMbaseInfo：pack["IVmProcedure"] 返回流程对象指针。
 */
VMbaseInfo VMbase::getIVmProcedure(QString processName)
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    try {
        // 使用流程名称从方案对象中索引 SDK 流程对象。
        IVmProcedure* pVmPrc = (IVmProcedure *)(*pVmSol)[qstringToGbk(processName).constData()];

        // 记录获取成功状态。
        success = true;
        message = QString("已获取[%1]的流程对象").arg(processName);

        // 保存流程对象指针。
        pack["IVmProcedure"] = QVariant::fromValue(pVmPrc);
    } catch (...) {
        // 捕获流程对象获取失败。
        message = QString("获取[%1]的流程对象失败，请检查流程名称").arg(processName);
    }

    // 返回流程对象获取结果。
    return {success, message, pack};
}

/**
 * @brief 根据流程名称执行流程一次。
 * @param processName 流程名称。
 * @return VMbaseInfo：pack["RenderData"] 返回流程渲染控件句柄。
 */
VMbaseInfo VMbase::runPrc(QString processName)
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    // 记录流程名称，便于上层识别本次操作对应的流程。
    pack["processName"] = QVariant::fromValue(processName);

    try {
        // 使用流程名称获取流程对象。
        IVmProcedure* pVmPrc = (IVmProcedure *)(*pVmSol)[qstringToGbk(processName).constData()];

        // 注册流程回调，流程执行结束后回到 OnWorkEndStatusCallBack。
        pVmPrc->RegisterCallBackEvent(this, pVmPrc);

        // 执行流程一次。
        pVmPrc->Run();

        // 获取流程参数对象中的渲染控制信息。
        void* pParamData = pVmPrc->GetParamObj()->GetControlInfo();

        // 把控制信息指针转换为 qlonglong，方便通过 QVariant 和 ActiveX 传递。
        qlonglong pRenderData = reinterpret_cast<qlonglong>(pParamData);

        // 保存渲染控件句柄。
        pack["RenderData"] = QVariant::fromValue(pRenderData);

        // 记录执行成功状态。
        success = true;
        message = QString("执行[%1]成功").arg(processName);
    } catch (...) {
        // 捕获流程执行失败。
        message = QString("执行[%1]失败").arg(processName);
    }

    // 返回流程执行结果。
    return {success, message, pack};
}

/**
 * @brief 根据流程名称启动流程连续执行。
 * @param processName 流程名称。
 * @return VMbaseInfo：pack["RenderData"] 返回流程渲染控件句柄。
 */
VMbaseInfo VMbase::runningPrc(QString processName)
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    // 记录流程名称，便于上层识别本次操作对应的流程。
    pack["processName"] = QVariant::fromValue(processName);

    try {
        // 使用流程名称获取流程对象。
        IVmProcedure* pVmPrc = (IVmProcedure *)(*pVmSol)[qstringToGbk(processName).constData()];

        // 获取流程参数对象中的渲染控制信息。
        void* pParamData = pVmPrc->GetParamObj()->GetControlInfo();

        // 把控制信息指针转换为 qlonglong，方便通过 QVariant 和 ActiveX 传递。
        qlonglong pRenderData = reinterpret_cast<qlonglong>(pParamData);

        // 保存渲染控件句柄。
        pack["RenderData"] = QVariant::fromValue(pRenderData);

        // 注册流程回调，流程执行结束后回到 OnWorkEndStatusCallBack。
        pVmPrc->RegisterCallBackEvent(this, pVmPrc);

        // 启动流程连续执行。
        pVmPrc->Runing();

        // 记录启动成功状态。
        success = true;
        message = QString("启动连续运行[%1]成功").arg(processName);
    } catch (...) {
        // 捕获流程连续执行启动失败。
        message = QString("启动连续运行[%1]失败").arg(processName);
    }

    // 返回连续执行启动结果。
    return {success, message, pack};
}

/**
 * @brief 根据流程名称停止流程执行。
 * @param processName 流程名称。
 * @return VMbaseInfo：pack["processName"] 返回流程名称。
 */
VMbaseInfo VMbase::stopPrc(QString processName)
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    // 记录流程名称。
    pack["processName"] = QVariant::fromValue(processName);

    try {
        // 使用流程名称获取流程对象。
        IVmProcedure* pVmPrc = (IVmProcedure *)(*pVmSol)[qstringToGbk(processName).constData()];

        // 调用 SDK 停止流程执行。
        pVmPrc->StopRun();

        // 记录停止成功状态。
        success = true;
        message = QString("停止[%1]成功").arg(processName);
    } catch (...) {
        // 捕获流程停止失败。
        message = QString("停止[%1]失败").arg(processName);
    }

    // 返回流程停止结果。
    return {success, message, pack};
}

/**
 * @brief 根据流程名称获取流程参数页句柄。
 * @param processName 流程名称。
 * @return VMbaseInfo：pack["qlonglongData"] 返回 ParamBaseEx 地址。
 */
VMbaseInfo VMbase::setProcess(QString processName)
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    // 记录流程名称。
    pack["processName"] = QVariant::fromValue(processName);

    try {
        // 使用流程名称获取流程对象。
        IVmProcedure* pVmPrc = (IVmProcedure *)(*pVmSol)[qstringToGbk(processName).constData()];

        // 获取流程参数控件所需的 SDK 控制信息。
        void * pParamData = pVmPrc->GetParamObj()->GetControlInfo();

        // 控制信息为空时无法打开参数页。
        if (!pParamData) {
            message = QString("[%1]中的pParamData为空").arg(processName);
            return {success, message};
        }

        // 将控制信息指针转换为 qlonglong，兼容界面层调试显示。
        qlonglong pRenderData = reinterpret_cast<qlonglong>(pParamData);

        // 保存原始控制信息地址。
        pack["参数数据"] = QVariant::fromValue(pRenderData);
        pack["qlonglongRenderData"] = pRenderData;

        // 将控制信息转换为参数控件输入结构。
        ParamCtrlInput* pParam = static_cast<ParamCtrlInput*>(pParamData);

        // 清空成员级参数结构，保证旧数据不会影响本次参数页打开。
        stParamData = {0};

        // 保存 SDK 方案或流程句柄。
        stParamData.Handle = pParam->Handle;

        // 保存当前流程模块 ID。
        stParamData.ModuId = pParam->ModuId;

        // 设置参数控件调用超时时间。
        stParamData.TimeOut = 5;

        // 将成员结构体地址转换为 qlonglong，供 ActiveX 参数控件使用。
        qlonglong pData = reinterpret_cast<qlonglong>(&stParamData);

        // 保存参数结构体地址。
        pack["参数结构体地址"] = QVariant::fromValue(pData);
        pack["qlonglongData"] = pData;
    }
    catch (...) {
        // 捕获参数页句柄获取失败。
        message = QString("设置[%1]流程参数失败: 未知异常")
                           .arg(processName);
    }

    // 返回参数页句柄数据。
    return {success, message, pack};
}

/**
 * @brief 获取指定流程的结果对象。
 * @param processName 流程名称。
 * @return VMbaseInfo：pack["results"] 返回 IMVSProcedureResults 指针。
 */
VMbaseInfo VMbase::getResult(QString processName)
{
    // 初始化返回状态和数据包。
    bool success = false;
    QString message;
    QVariantMap pack;

    // 记录流程名称。
    pack["processName"] = QVariant::fromValue(processName);

    try {
        // 使用流程名称获取流程对象。
        IVmProcedure* pVmPrc = (IVmProcedure *)(*pVmSol)[qstringToGbk(processName).constData()];

        // 从流程对象读取结果对象。
        IMVSProcedureResults *results = pVmPrc->GetResult();

        // 保存结果对象指针。
        pack["results"] = QVariant::fromValue(results);

        // 记录获取成功状态。
        success = true;
        message = QString("获取[%1]结果成功").arg(processName);
    } catch (...) {
        // 捕获流程结果获取失败。
        message = QString("获取[%1]结果失败").arg(processName);
    }

    // 返回流程结果对象。
    return {success, message, pack};
}

/**
 * @brief SDK 流程开始回调。
 * @param status SDK 工作状态结构。
 * @param user 注册回调时传入的用户指针。
 *
 * 输入数据：SDK 回调状态和用户数据。
 * 输出数据：processBeginCallBack 信号。
 */
void VMbase::OnWorkBeginStatusCallBack(const IMVS_PF_MODULE_WORK_STAUS *status, void *user)
{
    // 当前示例不解析开始状态，显式标记未使用参数。
    Q_UNUSED(status);
    Q_UNUSED(user);

    // 创建流程开始回调数据包。
    QVariantMap pack;

    // 将流程开始事件转发给 VMControl。
    emit processBeginCallBack(pack);
}

/**
 * @brief SDK 流程结束回调。
 * @param status SDK 工作状态结构。
 * @param user 注册回调时传入的流程对象指针。
 *
 * 输入数据：SDK 回调状态和流程对象。
 * 输出数据：processEndCallBack 信号，包含流程名、结果状态和坐标数组。
 */
void VMbase::OnWorkEndStatusCallBack(const IMVS_PF_MODULE_WORK_STAUS *status, void *user)
{
    // 当前示例不直接解析 status，显式标记未使用参数。
    Q_UNUSED(status);

    // 准备输出消息和回调数据包。
    QString message;
    QVariantMap backData;

    // 将用户指针还原为流程对象。
    IVmProcedure *procedure = static_cast<IVmProcedure*>(user);

    // 读取流程名称并写入回调包（SDK 返回 UTF-8）
    QString processName = QString::fromUtf8(procedure->GetName());
    backData["processName"] = processName;

    // 从流程对象读取流程结果。
    IMVSProcedureResults *results = procedure->GetResult();

    // 保存流程对象指针，供 VMControl 在拿到数据后停止流程。
    backData["procedure"] = QVariant::fromValue(procedure);

    try {
        // 读取 VM 输出字符串，NG 表示本次结果无效，直接返回。
        QString stringValue = results->GetOutputString("out_string").astStringVal[0].strValue;
        if (stringValue == "NG") {
            return;
        }

        // 读取数据状态，用于区分多组数据、单点数据或普通结果。
        QString dataStatus = results->GetOutputString("out_dataStatus").astStringVal[0].strValue;
        backData["dataStatus"] = dataStatus;

        // 多组数据额外读取 id 和 status 数组。
        if (dataStatus == "多组数据") {
            // 读取目标编号数组。
            IntDataArray idValue = results->GetOutputInt("out_id");

            // 读取目标状态数组。
            IntDataArray statusValue = results->GetOutputInt("out_status");

            // 保存目标编号数组。
            backData["id"] = QVariant::fromValue(idValue);

            // 保存目标状态数组。
            backData["status"] = QVariant::fromValue(statusValue);
        }

        // 读取 X 坐标数组。
        FloatDataArray x = results->GetOutputFloat("out_X");

        // 读取 Y 坐标数组。
        FloatDataArray y = results->GetOutputFloat("out_Y");

        // 如方案输出 R，可在这里打开读取。
        //FloatDataArray r = results->GetOutputFloat("out_R");

        // 记录数据读取成功消息。
        message = (QString("[%1]获取数据成功").arg(processName));

        // 标记本次回调数据有效。
        backData["bool"] = true;

        // 保存 X 坐标数组。
        backData["x"] = QVariant::fromValue(x);

        // 保存 Y 坐标数组。
        backData["y"] = QVariant::fromValue(y);

        // 如读取 R，可在这里写入回调包。
        //backData["r"] = QVariant::fromValue(r);
    } catch (...) {
        // 标记本次回调数据无效。
        backData["bool"] = false;

        // 记录数据读取失败消息。
        message = (QString("[%1]获取数据失败:检查订阅与名称").arg(processName));
    }

    // 保存回调消息。
    backData["message"] = message;

    // 将流程结束数据转发给 VMControl。
    emit processEndCallBack(backData);
}

/**
 * @brief 获取流程直属模块参数页句柄。
 * @param processName 流程名称。
 * @param moduName 模块名称。
 * @return VMbaseInfo：pack["setData"] 返回 ParamBaseEx 地址。
 */
VMbaseInfo VMbase::ModuSet(const QString& processName
                            , const QString& moduName)
{
    // 准备返回消息和数据包。
    QString message;
    QVariantMap resultData;

    // 构建流程直属模块完整路径，格式：流程名.模块名。
    QString fullName = processName + "." + moduName;

    try {
        // 使用流程名称获取流程对象。
        IVmProcedure* proTool = (IVmProcedure*)(*pVmSol)[qstringToGbk(processName).constData()];

        // 流程对象为空时记录错误消息。
        if (proTool == NULL) {
            message = QString("[%1]流程对象获取失败，请检查视觉方案").arg(processName);
        }

        // 获取流程参数控件所需的 SDK 控制信息。
        ParamCtrlInput* pParam = (ParamCtrlInput*)(proTool->GetParamObj()->GetControlInfo());

        // 初始化模块 ID。
        unsigned int nID = 0;

        // 根据完整模块路径从 SDK 中查询模块 ID。
        IMVS_PF_GetModuleIDByFullName(pParam->Handle, qstringToGbk(fullName).constData(), false, &nID);

        // 清空成员级参数结构体，避免旧数据影响本次打开参数页。
        stParamData = { 0 };

        // 设置 SDK 控制句柄。
        stParamData.Handle = pParam->Handle;

        // 设置目标模块 ID。
        stParamData.ModuId = nID;

        // 设置参数控件调用超时时间，单位秒。
        stParamData.TimeOut = 5;

        // 将参数结构体地址转换为 qlonglong，供 ActiveX 参数控件使用。
        qlonglong pData = (qlonglong)&stParamData;

        // 保存参数页句柄数据。
        resultData["setData"] = pData;

        // 记录获取成功消息。
        message = QString("[%1]地址参数获取成功").arg(fullName);

        // 返回成功结果。
        return {true, message, resultData};
    } catch (...) {
        // 捕获模块参数页句柄获取失败。
        message = QString("[%1]地址参数获取失败").arg(fullName);

        // 返回失败结果。
        return {false, message};
    }
}

/**
 * @brief 获取组合模块内部模块参数页句柄。
 * @param processName 流程名称。
 * @param groupName 组合模块名称。
 * @param moduName 组合模块内部模块名称。
 * @return VMbaseInfo：pack["setData"] 返回 ParamBaseEx 地址。
 */
VMbaseInfo VMbase::GroupSet(const QString& processName
                            , const QString& groupName
                            , const QString& moduName)
{
    // 准备返回消息和数据包。
    QString message;
    QVariantMap resultData;

    // 构建组合模块内部完整路径，格式：流程名.Group名.内部模块名。
    QString fullModuleName = processName + "." + groupName + "." + moduName;

    try {
        // 使用流程名称获取流程对象。
        IVmProcedure* proTool = (IVmProcedure*)(*pVmSol)[qstringToGbk(processName).constData()];

        // 流程对象为空时记录错误消息。
        if (proTool == NULL) {
            message = QString("[%1]流程对象获取失败，请检查视觉方案").arg(processName);
        }

        // 获取流程参数控件所需的 SDK 控制信息。
        ParamCtrlInput* pParam = (ParamCtrlInput*)(proTool->GetParamObj()->GetControlInfo());

        // 初始化模块 ID。
        unsigned int nID = 0;

        // 根据完整模块路径从 SDK 中查询模块 ID。
        IMVS_PF_GetModuleIDByFullName(pParam->Handle, qstringToGbk(fullModuleName).constData(), false, &nID);

        // 清空成员级参数结构体，避免旧数据影响本次打开参数页。
        stParamData = { 0 };

        // 设置 SDK 控制句柄。
        stParamData.Handle = pParam->Handle;

        // 设置目标模块 ID。
        stParamData.ModuId = nID;

        // 设置参数控件调用超时时间，单位秒。
        stParamData.TimeOut = 5;

        // 将参数结构体地址转换为 qlonglong，供 ActiveX 参数控件使用。
        qlonglong pData = (qlonglong)&stParamData;

        // 保存参数页句柄数据。
        resultData["setData"] = pData;

        // 记录获取成功消息。
        message = QString("[%1]地址参数获取成功").arg(fullModuleName);

        // 返回成功结果。
        return {true, message, resultData};
    } catch (...) {
        // 捕获组合模块参数页句柄获取失败。
        message = QString("[%1]地址参数获取失败").arg(fullModuleName);

        // 返回失败结果。
        return {false, message};
    }
}
