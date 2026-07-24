#include "vmcontrol.h"

#include <QDateTime>
#include <QMetaType>

/**
 * @brief 构造 VMControl 模块入口。
 * @param parent Qt 父对象。
 *
 * 输入数据：Qt 父对象。
 * 输出数据：创建 VMbase，连接底层回调，并发送“未加载”初始状态。
 */
VMControl::VMControl(QObject *parent)
    : QObject(parent)
    , m_vmbase(new VMbase(this))
{
    // 连接 VMbase 的流程回调信号到 VMControl 的处理槽。
    initializeConnect();

    // 向上层发送初始状态，提示当前尚未加载方案。
    emitStatus(VisionStatus::NotLoaded, QStringLiteral("视觉方案未加载"));
}

/**
 * @brief 初始化底层回调连接。
 *
 * 输入数据：无。
 * 输出数据：把 VMbase 的流程开始/结束信号连接到本类槽函数。
 */
void VMControl::initializeConnect()
{
    // 接收流程开始回调，当前只保留扩展入口。
    connect(m_vmbase, &VMbase::processBeginCallBack, this, &VMControl::proBeginReceive);

    // 接收流程结束回调，用于解析 VM 输出数据。
    connect(m_vmbase, &VMbase::processEndCallBack, this, &VMControl::proEndReceive);
}

/**
 * @brief 析构 VMControl。
 *
 * 输入数据：无。
 * 输出数据：停止并释放所有流程超时定时器。
 */
VMControl::~VMControl()
{
    // 遍历当前记录的所有流程定时器。
    for (QTimer* timer : m_procTimers.values()) {
        // 停止定时器，避免析构过程中继续触发 timeout。
        timer->stop();

        // 延迟释放定时器对象，遵循 Qt 对象生命周期。
        timer->deleteLater();
    }

    // 清空流程名到定时器的映射。
    m_procTimers.clear();

    // 清空流程成功标记。
    m_procSuccessFlags.clear();
}

/**
 * @brief 发送统一消息事件。
 * @param message 消息文本。
 *
 * 输入数据：消息文本。
 * 输出数据：messageReady 信号。
 */
void VMControl::emitMessage(const QString& message)
{
    // 组装消息事件并附带当前时间。
    emit messageReady(VisionMessageEvent{
        QDateTime::currentDateTime(),
        message
    });
}

/**
 * @brief 发送统一数据事件。
 * @param dataType 数据类型。
 * @param pack 数据包。
 *
 * 输入数据：数据类型和 QVariantMap 数据包。
 * 输出数据：dataReady 信号。
 */
void VMControl::emitData(VisionDataType dataType, const QVariantMap &pack)
{
    // 组装数据事件并附带当前时间。
    emit dataReady(VisionDataEvent{
        QDateTime::currentDateTime(),
        dataType,
        pack
    });
}

/**
 * @brief 发送统一状态事件。
 * @param status 新状态。
 * @param message 状态说明。
 *
 * 输入数据：视觉状态枚举和说明文本。
 * 输出数据：statusChanged 信号，同时更新 oldStatus/nowStatus。
 */
void VMControl::emitStatus(VisionStatus status, const QString& message)
{
    // 保存状态切换前的旧状态。
    oldStatus = nowStatus;

    // 更新当前状态。
    nowStatus = status;

    // 组装状态事件并发送给上层。
    emit statusChanged(VisionStatusEvent{
        QDateTime::currentDateTime(),
        status,
        message
    });
}

/**
 * @brief 执行方案级命令。
 * @param command 方案操作类型。
 * @param params 方案参数包。
 *
 * 输入数据：SolMotionType 和可选 params；Load/SaveAs/Switching 读取 solPath、password。
 * 输出数据：根据命令触发状态、数据或消息信号。
 */
void VMControl::executeSolutionCommand(SolMotionType command, const QVariantMap& params)
{
    // 根据方案命令类型分发到具体实现函数。
    switch (command) {
    case SolMotionType::Load:
        // 加载方案需要路径和密码。
        loadSolution(params.value("solPath").toString(), params.value("password").toString());
        break;
    case SolMotionType::Close:
        // 关闭当前方案。
        closeSolution();
        break;
    case SolMotionType::Save:
        // 保存当前方案。
        saveSolution();
        break;
    case SolMotionType::SaveAs:
        // 将当前方案另存为指定路径。
        saveSolutionAs(params.value("solPath").toString(), params.value("password").toString());
        break;
    case SolMotionType::Run:
        // 整套方案执行一次。
        runSolution();
        break;
    case SolMotionType::Runing:
        // 整套方案连续执行。
        runSolutionContinuously();
        break;
    case SolMotionType::Stop:
        // 停止整套方案。
        stopSolution();
        break;
    case SolMotionType::Switching:
        // 切换方案需要新方案路径和密码。
        switchSolution(params.value("solPath").toString(), params.value("password").toString());
        break;
    default:
        // 未识别方案命令只输出消息，不中断程序。
        emitMessage(QStringLiteral("收到未知方案命令"));
        break;
    }
}

/**
 * @brief 执行流程级命令。
 * @param command 流程操作类型。
 * @param processName 流程名称。
 *
 * 输入数据：ProcMotionType 和流程名。
 * 输出数据：根据命令触发渲染数据、参数数据、超时数据或消息信号。
 */
void VMControl::executeProcedureCommand(ProcMotionType command, const QString& processName)
{
    // 根据流程命令类型分发到具体实现函数。
    switch (command) {
    case ProcMotionType::Run:
        // 执行一次指定流程。
        runProcedure(processName);
        break;
    case ProcMotionType::Runing:
        // 连续执行指定流程。
        runProcedureContinuously(processName);
        break;
    case ProcMotionType::Stop:
        // 停止指定流程。
        stopProcedure(processName);
        break;
    case ProcMotionType::Set:
        // 打开指定流程参数页。
        setProcedure(processName);
        break;
    default:
        // 预留但未实现的流程命令统一提示。
        emitMessage(QStringLiteral("收到暂不支持的流程命令"));
        break;
    }
}

/**
 * @brief 设置指定流程的连续采集模式。
 * @param processName 流程名称。
 * @param enabled true 启用（阻止自动 StopRun），false 恢复默认行为。
 */
void VMControl::setContinuousCollection(const QString &processName, bool enabled)
{
    if (enabled) {
        m_continuousProcs.insert(processName);
    } else {
        m_continuousProcs.remove(processName);
    }
}

/**
 * @brief 重置流程成功标记，允许后续回调通过拦截。
 * @param processName 流程名称。
 */
void VMControl::resetProcSuccessFlag(const QString &processName)
{
    m_procSuccessFlags.remove(processName);
}

/**
 * @brief 执行模块参数页命令。
 * @param command 模块操作类型。
 * @param params 模块参数包。
 *
 * 输入数据：Set 读取 processName/moduleName；GroupSet 读取 processName/groupName/moduleName。
 * 输出数据：SetData 数据事件和消息事件。
 */
void VMControl::executeModuleCommand(ModuMotionType command, const QVariantMap& params)
{
    // 所有模块操作都需要流程名称。
    const QString processName = params.value("processName").toString();

    // 根据模块命令类型决定打开普通模块还是组合模块内部模块。
    switch (command) {
    case ModuMotionType::Set:
        // 打开流程直属模块参数页。
        setModule(processName, params.value("moduleName").toString());
        break;
    case ModuMotionType::GroupSet:
        // 打开组合模块内部模块参数页。
        setGroupModule(processName,
                       params.value("groupName").toString(),
                       params.value("moduleName").toString());
        break;
    default:
        // 模块命令枚举当前只支持 Set 和 GroupSet。
        emitMessage(QStringLiteral("收到暂不支持的模块命令"));
        break;
    }
}

/**
 * @brief 加载视觉方案。
 * @param solPath 方案路径。
 * @param password 方案密码。
 *
 * 输入数据：方案文件路径和密码。
 * 输出数据：状态信号、流程列表数据、SDK 消息信号。
 */
void VMControl::loadSolution(const QString& solPath, const QString& password)
{
    // 调用底层 SDK 封装加载方案。
    VMbaseInfo info = m_vmbase->loadSol(solPath, password);

    // 把加载结果消息发给上层。
    emitMessage(info.message);

    // 加载失败时更新状态并结束后续流程列表读取。
    if (!info.success) {
        emitStatus(VisionStatus::FailToLoad, QStringLiteral("视觉方案加载失败"));
        return;
    }

    // 方案加载成功后读取流程列表。
    VMbaseInfo processInfo = m_vmbase->getprocedureObjList();

    // 输出流程列表读取结果消息。
    emitMessage(processInfo.message);

    // 流程列表读取失败时仍保持方案已加载状态，方便用户检查方案。
    if (!processInfo.success) {
        emitStatus(VisionStatus::Loaded, QStringLiteral("流程信息列表获取失败"));
    }

    // 将流程列表数据发送给界面刷新流程下拉框。
    emitData(VisionDataType::SolData, processInfo.pack);

    // 启用 SDK 模块回调，便于后续流程执行完成后返回数据。
    VMbaseInfo callbackInfo = m_vmbase->EnableCallback();

    // 输出回调启用结果消息。
    emitMessage(callbackInfo.message);

    // 回调启用失败时给出状态提示，但不卸载方案。
    if (!callbackInfo.success) {
        emitStatus(VisionStatus::Loaded, QStringLiteral("模块回调启用失败，无法获取回调数据"));
    }

    // 最终发送方案已加载状态。
    emitStatus(VisionStatus::Loaded, QStringLiteral("视觉方案已加载"));
}

/**
 * @brief 关闭当前方案。
 *
 * 输入数据：无。
 * 输出数据：消息信号；关闭成功时发送 NotLoaded 状态。
 */
void VMControl::closeSolution()
{
    // 调用底层关闭方案。
    VMbaseInfo info = m_vmbase->closeSol();

    // 输出关闭结果消息。
    emitMessage(info.message);

    // 关闭成功后更新状态为未加载。
    if (info.success) {
        emitStatus(VisionStatus::NotLoaded, QStringLiteral("视觉方案已关闭，无法执行视觉指令"));
    }
}

/**
 * @brief 保存当前方案。
 *
 * 输入数据：无。
 * 输出数据：消息信号。
 */
void VMControl::saveSolution()
{
    // 调用底层保存方案。
    VMbaseInfo info = m_vmbase->SaveSol();

    // 输出保存结果消息。
    emitMessage(info.message);
}

/**
 * @brief 当前方案另存为。
 * @param solPath 新方案路径。
 * @param password 新方案密码。
 *
 * 输入数据：保存路径和密码。
 * 输出数据：消息信号。
 */
void VMControl::saveSolutionAs(const QString& solPath, const QString& password)
{
    // 调用底层另存为方案。
    VMbaseInfo info = m_vmbase->SaveAsSol(solPath, password);

    // 输出另存为结果消息。
    emitMessage(info.message);
}

/**
 * @brief 切换视觉方案。
 * @param solPath 新方案路径。
 * @param password 新方案密码。
 *
 * 输入数据：新方案路径和密码。
 * 输出数据：关闭旧方案结果；关闭成功后继续加载新方案。
 */
void VMControl::switchSolution(const QString& solPath, const QString& password)
{
    // 先关闭当前方案。
    VMbaseInfo info = m_vmbase->closeSol();

    // 输出关闭旧方案结果。
    emitMessage(info.message);

    // 旧方案关闭成功后加载新方案。
    if (info.success) {
        loadSolution(solPath, password);
        return;
    }

    // 旧方案关闭失败时保留旧方案并提示切换失败。
    emitStatus(VisionStatus::Loaded, QStringLiteral("视觉方案切换失败：旧方案关闭失败，继续使用旧方案"));
}

/**
 * @brief 整套方案执行一次。
 *
 * 输入数据：无。
 * 输出数据：消息信号。
 */
void VMControl::runSolution()
{
    // 调用底层方案执行一次接口。
    VMbaseInfo info = m_vmbase->runSol();

    // 输出执行结果消息。
    emitMessage(info.message);
}

/**
 * @brief 整套方案连续执行。
 *
 * 输入数据：无。
 * 输出数据：消息信号。
 */
void VMControl::runSolutionContinuously()
{
    // 调用底层方案连续执行接口。
    VMbaseInfo info = m_vmbase->runingSol();

    // 输出启动结果消息。
    emitMessage(info.message);
}

/**
 * @brief 停止整套方案执行。
 *
 * 输入数据：无。
 * 输出数据：消息信号。
 */
void VMControl::stopSolution()
{
    // 调用底层方案停止接口。
    VMbaseInfo info = m_vmbase->stopRunSol();

    // 输出停止结果消息。
    emitMessage(info.message);
}

/**
 * @brief 指定流程执行一次。
 * @param processName 流程名称。
 *
 * 输入数据：流程名。
 * 输出数据：流程渲染数据和消息信号。
 */
void VMControl::runProcedure(const QString& processName)
{
    // 流程名为空时无法定位 SDK 流程对象。
    if (processName.isEmpty()) {
        emitMessage(QStringLiteral("流程名称为空，无法执行流程"));
        return;
    }

    // 标记该流程尚未收到有效结果。
    m_procSuccessFlags[processName] = false;

    // 调用底层流程执行一次接口。
    VMbaseInfo info = m_vmbase->runPrc(processName);

    // 输出执行结果消息。
    emitMessage(info.message);

    // 执行成功时把渲染控件句柄发给界面。
    if (info.success) {
        emitData(VisionDataType::RenderData, info.pack);
    }
}

/**
 * @brief 指定流程连续执行。
 * @param processName 流程名称。
 *
 * 输入数据：流程名。
 * 输出数据：流程渲染数据、超时数据和消息信号。
 */
void VMControl::runProcedureContinuously(const QString& processName)
{
    // 流程名为空时无法定位 SDK 流程对象。
    if (processName.isEmpty()) {
        emitMessage(QStringLiteral("流程名称为空，无法连续执行流程"));
        return;
    }

    // 第一次连续执行该流程时创建单次超时定时器。
    if (!m_procTimers.contains(processName)) {
        // 创建定时器并交给 VMControl 管理生命周期。
        QTimer* timer = new QTimer(this);

        // 设置为单次触发，避免超时消息重复发送。
        timer->setSingleShot(true);

        // 定时器到期时按流程名执行超时处理。
        connect(timer, &QTimer::timeout, this, [this, processName]() {
            onTimeoutForProc(processName);
        });

        // 记录流程名与定时器的对应关系。
        m_procTimers[processName] = timer;

        // 初始化流程结果成功标记。
        m_procSuccessFlags[processName] = false;
    }

    // 每次启动连续执行前重置成功标记。
    m_procSuccessFlags[processName] = false;

    // 启动 5 秒超时计时，等待 VM 回调返回有效数据。
    m_procTimers[processName]->start(5000);

    // 输出定时器启动消息。
    emitMessage(QStringLiteral("流程[%1]定时器开启").arg(processName));

    // 调用底层流程连续执行接口。
    VMbaseInfo info = m_vmbase->runningPrc(processName);

    // 输出执行结果消息。
    emitMessage(info.message);

    // 启动成功时把渲染控件句柄发给界面。
    if (info.success) {
        emitData(VisionDataType::RenderData, info.pack);
    }
}

/**
 * @brief 停止指定流程。
 * @param processName 流程名称。
 *
 * 输入数据：流程名。
 * 输出数据：流程渲染数据和消息信号。
 */
void VMControl::stopProcedure(const QString& processName)
{
    // 流程名为空时无法定位 SDK 流程对象。
    if (processName.isEmpty()) {
        emitMessage(QStringLiteral("流程名称为空，无法停止流程"));
        return;
    }

    // 调用底层流程停止接口。
    VMbaseInfo info = m_vmbase->stopPrc(processName);

    // 如果该流程存在超时定时器，停止流程后释放定时器。
    if (m_procTimers.contains(processName)) {
        QTimer* timer = m_procTimers.take(processName);
        timer->deleteLater();
    }

    // 清除流程成功标记。
    m_procSuccessFlags.remove(processName);

    // 输出停止结果消息。
    emitMessage(info.message);

    // 把底层返回的数据包交给界面更新渲染控件。
    emitData(VisionDataType::RenderData, info.pack);
}

/**
 * @brief 打开指定流程参数页。
 * @param processName 流程名称。
 *
 * 输入数据：流程名。
 * 输出数据：SetData 数据事件和消息信号。
 */
void VMControl::setProcedure(const QString& processName)
{
    // 流程名为空时无法定位 SDK 流程对象。
    if (processName.isEmpty()) {
        emitMessage(QStringLiteral("流程名称为空，无法打开流程参数"));
        return;
    }

    // 调用底层流程参数页句柄获取接口。
    VMbaseInfo info = m_vmbase->setProcess(processName);

    // 输出参数页获取结果消息。
    emitMessage(info.message);

    // 把参数页句柄数据发送给界面参数控件。
    emitData(VisionDataType::SetData, info.pack);
}

/**
 * @brief 打开流程直属模块参数页。
 * @param processName 流程名称。
 * @param moduleName 模块名称。
 *
 * 输入数据：流程名和模块名。
 * 输出数据：SetData 数据事件和消息信号。
 */
void VMControl::setModule(const QString& processName, const QString& moduleName)
{
    // 流程名或模块名为空时无法拼出 VM 模块路径。
    if (processName.isEmpty() || moduleName.isEmpty()) {
        emitMessage(QStringLiteral("流程或模块名称为空，无法打开模块参数"));
        return;
    }

    // 调用底层流程直属模块参数页句柄获取接口。
    VMbaseInfo info = m_vmbase->ModuSet(processName, moduleName);

    // 输出参数页获取结果消息。
    emitMessage(info.message);

    // 把参数页句柄数据发送给界面参数控件。
    emitData(VisionDataType::SetData, info.pack);
}

/**
 * @brief 打开组合模块内部模块参数页。
 * @param processName 流程名称。
 * @param groupName 组合模块名称。
 * @param moduleName 组合模块内部模块名称。
 *
 * 输入数据：流程名、组合模块名和模块名。
 * 输出数据：SetData 数据事件和消息信号。
 */
void VMControl::setGroupModule(const QString& processName, const QString& groupName, const QString& moduleName)
{
    // 任一名称为空时无法拼出 VM 组合模块路径。
    if (processName.isEmpty() || groupName.isEmpty() || moduleName.isEmpty()) {
        emitMessage(QStringLiteral("流程、分组或模块名称为空，无法打开模块参数"));
        return;
    }

    // 调用底层组合模块内部参数页句柄获取接口。
    VMbaseInfo info = m_vmbase->GroupSet(processName, groupName, moduleName);

    // 输出参数页获取结果消息。
    emitMessage(info.message);

    // 把参数页句柄数据发送给界面参数控件。
    emitData(VisionDataType::SetData, info.pack);
}

/**
 * @brief 接收流程开始回调。
 * @param backData VMbase 回调数据包。
 *
 * 输入数据：流程开始回调数据。
 * 输出数据：当前不输出，保留扩展入口。
 */
void VMControl::proBeginReceive(const QVariantMap &backData)
{
    // 当前示例不需要处理流程开始事件，显式标记参数未使用。
    Q_UNUSED(backData);
}

/**
 * @brief 接收并分发流程结束回调。
 * @param backData VMbase 回调数据包。
 *
 * 输入数据：包含 message、bool、dataStatus、坐标数据和流程对象的数据包。
 * 输出数据：根据 dataStatus 分发 TargetData、PointData 或通用流程处理。
 */
void VMControl::proEndReceive(const QVariantMap &backData)
{
    // 先把底层解析出的消息发给上层。
    emitMessage(backData["message"].toString());

    // 判断本次流程结果是否有效。
    const bool success = backData["bool"].toBool();

    // 无效结果直接结束，不再继续解析数据。
    if (!success) {
        return;
    }

    // 读取 VM 流程输出的数据类型标记。
    const QString dataStatus = backData["dataStatus"].toString();

    // 多组数据转换为 TargetData 列表。
    if (dataStatus == QStringLiteral("多组数据")) {
        hanleTargetData(backData);

    // 单点数据转换为 PointData。
    } else if (dataStatus == QStringLiteral("单点数据")) {
        hanlePointData(backData);

    // 其他有效结果只停止流程和定时器。
    } else {
        hanleprocData(backData);
    }
}

/**
 * @brief 处理多目标数据。
 * @param backData VMbase 回调数据包。
 *
 * 输入数据：流程名、id/status/x/y/r 数组和 procedure 指针。
 * 输出数据：TargetData 数据事件，并停止流程连续执行。
 */
void VMControl::hanleTargetData(const QVariantMap &backData)
{
    // 读取结果所属流程名。
    const QString processName = backData["processName"].toString();

    // 如果该流程已经处理过有效数据，则忽略重复回调。
    if (m_procSuccessFlags.value(processName, false)) {
        return;
    }

    // 从回调数据包中取出 VM 输出数组。
    IntDataArray id = backData["id"].value<IntDataArray>();
    IntDataArray status = backData["status"].value<IntDataArray>();
    FloatDataArray x = backData["x"].value<FloatDataArray>();
    FloatDataArray y = backData["y"].value<FloatDataArray>();
    FloatDataArray r = backData["r"].value<FloatDataArray>();

    // 创建上层通用的目标数据列表。
    QList<TargetData> targetDataList;

    // 按 VM 返回数量逐个转换目标数据。
    for (int i = 0; i < id.nValueNum; ++i) {
        TargetData pack;
        pack.id = id.pIntVal[i];
        pack.status = status.pIntVal[i];
        pack.x = x.pFloatVal[i];
        pack.y = y.pFloatVal[i];
        pack.r = r.pFloatVal[i];
        targetDataList.append(pack);
    }

    // 组装对外数据包。
    QVariantMap packData;
    packData["processName"] = processName;
    packData["TargetDataList"] = QVariant::fromValue(targetDataList);

    // 发送多目标数据事件。
    emitData(VisionDataType::TargetData, packData);

    // 取出流程对象，用于在拿到有效数据后停止连续执行。
    IVmProcedure *procedure = backData["procedure"].value<IVmProcedure*>();

    // 流程对象有效时停止流程。
    if (procedure) {
        procedure->StopRun();
    }

    // 标记该流程已经收到有效数据。
    m_procSuccessFlags[processName] = true;

    // 如果存在超时定时器，收到数据后停止计时。
    if (m_procTimers.contains(processName)) {
        m_procTimers[processName]->stop();
        emitMessage(QStringLiteral("流程[%1]数据获取成功，停止定时器").arg(processName));
    }
}

/**
 * @brief 处理单点数据。
 * @param backData VMbase 回调数据包。
 *
 * 输入数据：流程名、x/y/r 数组和 procedure 指针。
 * 输出数据：PointData 数据事件，并停止流程连续执行。
 */
void VMControl::hanlePointData(const QVariantMap &backData)
{
    // 读取结果所属流程名。
    const QString processName = backData["processName"].toString();

    // 如果该流程已经处理过有效数据且不在连续采集模式下，则忽略重复回调。
    if (m_procSuccessFlags.value(processName, false) && !m_continuousProcs.contains(processName)) {
        return;
    }

    // 从回调数据包中取出坐标数组。
    FloatDataArray x = backData["x"].value<FloatDataArray>();
    FloatDataArray y = backData["y"].value<FloatDataArray>();
    //FloatDataArray r = backData["r"].value<FloatDataArray>();

    // 转换为上层通用单点结构。
    PointData point;
    point.x = x.pFloatVal[0];
    point.y = y.pFloatVal[0];
    //point.r = r.pFloatVal[0];

    // 组装对外数据包。
    QVariantMap packData;
    packData["processName"] = processName;
    packData["PointData"] = QVariant::fromValue(point);

    // 发送单点数据事件。
    emitData(VisionDataType::PointData, packData);

    // 取出流程对象，用于在拿到有效数据后停止连续执行。
    IVmProcedure *procedure = backData["procedure"].value<IVmProcedure*>();

    // 流程对象有效时停止流程（连续采集模式下不自动停止）。
    if (procedure && !m_continuousProcs.contains(processName)) {
        procedure->StopRun();
    }

    // 连续采集模式下不设置成功标记，允许后续回调继续通过。
    if (!m_continuousProcs.contains(processName)) {
        m_procSuccessFlags[processName] = true;
    }

    // 如果存在超时定时器，收到数据后停止计时。
    if (m_procTimers.contains(processName)) {
        m_procTimers[processName]->stop();
        emitMessage(QStringLiteral("流程[%1]数据获取成功，停止定时器").arg(processName));
    }
}

/**
 * @brief 处理通用流程结束数据。
 * @param backData VMbase 回调数据包。
 *
 * 输入数据：流程名和 procedure 指针。
 * 输出数据：停止流程连续执行，并停止对应超时定时器。
 */
void VMControl::hanleprocData(const QVariantMap &backData)
{
    // 读取结果所属流程名。
    const QString processName = backData["processName"].toString();

    // 如果该流程已经处理过有效数据，则忽略重复回调。
    if (m_procSuccessFlags.value(processName, false)) {
        return;
    }

    // 取出流程对象，用于停止连续执行。
    IVmProcedure *procedure = backData["procedure"].value<IVmProcedure*>();

    // 流程对象有效时停止流程。
    if (procedure) {
        procedure->StopRun();
    }

    // 标记该流程已经完成有效回调。
    m_procSuccessFlags[processName] = true;

    // 如果存在超时定时器，收到回调后停止计时。
    if (m_procTimers.contains(processName)) {
        m_procTimers[processName]->stop();
        emitMessage(QStringLiteral("流程[%1]数据获取成功，停止定时器").arg(processName));
    }
}

/**
 * @brief 处理流程执行超时。
 * @param procName 流程名称。
 *
 * 输入数据：流程名。
 * 输出数据：停止流程，发送 Timeout 数据事件和消息事件，并释放定时器。
 */
void VMControl::onTimeoutForProc(const QString& procName)
{
    // 超时时先尝试停止底层流程。
    VMbaseInfo info = m_vmbase->stopPrc(procName);

    // 当前只需要停止动作，不需要使用返回结构。
    Q_UNUSED(info);

    // 组装超时数据包。
    QVariantMap timeoutPack;
    timeoutPack["processName"] = procName;
    timeoutPack["message"] = QStringLiteral("流程[%1]执行超时").arg(procName);

    // 发送超时消息。
    emitMessage(QStringLiteral("流程[%1]数据获取已超时").arg(procName));

    // 发送超时数据事件给界面。
    emitData(VisionDataType::Timeout, timeoutPack);

    // 释放流程对应的超时定时器并清除状态。
    if (m_procTimers.contains(procName)) {
        QTimer* timer = m_procTimers.take(procName);
        timer->deleteLater();
        m_procSuccessFlags.remove(procName);
    }
}
