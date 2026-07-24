// windows.h 必须在 mainwindow.h 之前包含：
// VM SDK 的 VmModuleParamBase.h 中有 using namespace std; 会使 std::byte
// 进入全局命名空间，与 rpcndr.h 的 typedef unsigned char byte 产生歧义。
// 先包含 windows.h 可确保 rpcndr.h 处理 byte 时 using namespace std 尚未生效。
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "VmWindow.h"
#include "ui_VmWindow.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QTextCursor>

static void outputVisionLine(const QString &line)
{
    const QByteArray utf8 = (line + QLatin1Char('\n')).toLocal8Bit();
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h && h != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(h, utf8.constData(), (DWORD)utf8.size(), &written, NULL);
    }
#else
    fwrite(utf8.constData(), 1, utf8.size(), stdout);
    fflush(stdout);
#endif
}

/**
 * @brief 构造主窗口并创建视觉模块。
 * @param parent Qt 父窗口。
 *
 * 输入数据：Qt 父对象。
 * 输出数据：完成 UI 初始化、VMControl 连接和初始状态显示。
 */
VmWindow::VmWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VmWindow)
    , m_vmcontrol(new VMControl(this))
{
    // 创建并布局 Qt Designer 中定义的界面控件。
    ui->setupUi(this);

    // 初始化控件状态，例如 ActiveX 控件和密码输入框。
    initializeUI();

    // 建立 MainWindow 与 VMControl 的命令和结果信号连接。
    connectVMControl();

    // 主动写入初始状态，让界面启动后立即显示当前视觉方案状态。
    handleVisionStatusEvent(VisionStatusEvent{
        QDateTime::currentDateTime(),
        VisionStatus::NotLoaded,
        QStringLiteral("视觉方案未加载")
    });
}

/**
 * @brief 析构主窗口。
 *
 * 输入数据：无。
 * 输出数据：释放 Qt Designer 生成的 UI 对象。
 */
VmWindow::~VmWindow()
{
    // Qt 子对象会随父对象释放，这里只需要释放 ui 指针。
    delete ui;
}

/**
 * @brief 初始化主窗口控件。
 *
 * 输入数据：无。
 * 输出数据：ActiveX 控件完成对象指针初始化，密码框改为密码显示模式。
 */
void VmWindow::initializeUI()
{
    // 触发渲染 ActiveX 控件创建内部对象。
    ui->axWidgetRender->dynamicCall("GetObjectPointer()");

    // 触发参数 ActiveX 控件创建内部对象。
    ui->axWidgetParam->dynamicCall("GetObjectPointer()");

    // 将方案密码输入框设置为密码回显。
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
}

/**
 * @brief 连接 MainWindow 与 VMControl。
 *
 * 输入数据：无。
 * 输出数据：命令信号连接到 VMControl，VMControl 输出信号连接回界面处理函数。
 */
void VmWindow::connectVMControl()
{
    // 方案命令由 MainWindow 发起，VMControl 统一执行。
    connect(this, &VmWindow::requestSolutionCommand, m_vmcontrol, &VMControl::executeSolutionCommand);

    // 流程命令由 MainWindow 发起，VMControl 统一执行。
    connect(this, &VmWindow::requestProcedureCommand, m_vmcontrol, &VMControl::executeProcedureCommand);

    // 模块参数页命令由 MainWindow 发起，VMControl 统一执行。
    connect(this, &VmWindow::requestModuleCommand, m_vmcontrol, &VMControl::executeModuleCommand);

    // VMControl 状态变化统一回传到状态显示区域。
    connect(m_vmcontrol, &VMControl::statusChanged, this, &VmWindow::handleVisionStatusEvent);

    // VMControl 数据结果统一回传到数据处理入口。
    connect(m_vmcontrol, &VMControl::dataReady, this, &VmWindow::handleVisionDataEvent);

    // VMControl 日志消息统一回传到消息显示区域。
    connect(m_vmcontrol, &VMControl::messageReady, this, &VmWindow::handleVisionMessageEvent);
}

/**
 * @brief 处理视觉状态事件。
 * @param event VMControl 返回的状态事件。
 *
 * 输入数据：状态时间、状态枚举、状态消息。
 * 输出数据：向状态文本框追加一行状态记录。
 */
void VmWindow::handleVisionStatusEvent(const VisionStatusEvent &event)
{
    // 将状态时间转换为毫秒级可读文本。
    const QString timestamp = event.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");

    // 将状态枚举转换为文本并追加显示。
    ui->textEdit_status->append(
        QString("[%1] %2: %3").arg(timestamp, userRoleToString(event.Status), event.message));

    // 向外部进程输出状态（stdout 管道，供 Robot_R06 捕获）
    outputVisionLine(QStringLiteral("VISION_STATUS|%1").arg(event.message));
}

/**
 * @brief 处理视觉数据事件。
 * @param event VMControl 返回的数据事件。
 *
 * 输入数据：数据类型和 QVariantMap 数据包。
 * 输出数据：根据数据类型更新结果文本框、渲染控件或参数控件。
 */
void VmWindow::handleVisionDataEvent(const VisionDataEvent &event)
{
    // 根据数据类型选择对应的界面处理逻辑。
    switch (event.DataType) {
    case VisionDataType::TargetData:
        // 多目标数据交给列表展示函数处理。
        appendTargetData(event.pack);
        break;
    case VisionDataType::PointData:
        // 单点数据交给点位展示函数处理。
        appendPointData(event.pack);
        break;
    case VisionDataType::RenderData:
        // 渲染数据包含流程运行画面的 ActiveX 句柄。
        if (event.pack.contains("RenderData")) {
            // 将句柄传入渲染控件显示流程运行画面。
            ui->axWidgetRender->dynamicCall("SetParamsInfo(qlonglong)", event.pack["RenderData"].toLongLong());
        }
        break;
    case VisionDataType::SetData:
    {
        // 参数页数据优先读取模块参数句柄，兼容旧字段 qlonglongData。
        const qlonglong paramHandle = event.pack.value("setData", event.pack.value("qlonglongData")).toLongLong();

        // 句柄有效时，把参数页句柄传给参数控件。
        if (paramHandle != 0) {
            ui->axWidgetParam->dynamicCall("SetParamsInfo(qlonglong)", paramHandle);
        }
        break;
    }
    case VisionDataType::Timeout:
        // 超时事件直接把消息显示到状态框。
        ui->textEdit_status->append(
            event.pack.value("message",
                             QString("[%1] 数据超时").arg(event.pack["processName"].toString())).toString());
        break;
    case VisionDataType::SolData:
        // 方案数据包含流程列表，交给流程下拉框处理函数。
        handleSolData(event.pack);
        break;
    default:
        // 未识别的数据类型只做提示，不影响程序继续运行。
        ui->textEdit_data->append(QStringLiteral("收到未处理的数据类型"));
        break;
    }
}

/**
 * @brief 处理视觉日志消息。
 * @param event VMControl 返回的消息事件。
 *
 * 输入数据：消息时间和消息文本。
 * 输出数据：向消息文本框追加一行日志。
 */
void VmWindow::handleVisionMessageEvent(const VisionMessageEvent &event)
{
    // 将消息时间转换为毫秒级可读文本。
    const QString timestamp = event.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");

    // 追加消息内容。
    ui->textEdit_message->append(QString("[%1]: %2").arg(timestamp, event.message));
}

/**
 * @brief 选择方案文件。
 *
 * 输入数据：用户从文件对话框选择的 .sol 路径。
 * 输出数据：路径输入框。
 */
void VmWindow::on_pushButton_chosse_clicked()
{
    // 读取程序所在目录，作为相对方案目录的基准路径。
    QDir appDir(QCoreApplication::applicationDirPath());

    // 拼出程序目录下的“视觉方案”文件夹。
    const QString solutionDirPath = appDir.filePath(QStringLiteral("视觉方案"));

    // 如果“视觉方案”文件夹不存在，则自动创建，方便用户放置 .sol 文件。
    if (!QDir(solutionDirPath).exists()) {
        appDir.mkpath(QStringLiteral("视觉方案"));
    }

    // 打开方案文件选择对话框。
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("选择方案"),
        solutionDirPath,
        QStringLiteral("SOL(*.sol)"));

    // 用户选择有效文件后，把路径写入界面。
    if (!fileName.isEmpty()) {
        ui->lineEdit_path->setText(QDir::toNativeSeparators(fileName));
    }
}

/**
 * @brief 加载方案。
 *
 * 输入数据：界面中的方案路径和密码。
 * 输出数据：requestSolutionCommand(Load, params) 信号。
 */
void VmWindow::on_pushButton_load_clicked()
{
    // 从路径输入框读取方案路径。
    const QString solPath = ui->lineEdit_path->text();

    // 路径为空时直接提示并结束本次加载。
    if (solPath.isEmpty()) {
        ui->textEdit_status->append(QStringLiteral("解决方案路径不能为空"));
        ui->textEdit_status->moveCursor(QTextCursor::End);
        ui->textEdit_status->ensureCursorVisible();
        return;
    }

    // 构造方案加载参数包。
    QVariantMap params;
    params["solPath"] = solPath;
    params["password"] = ui->lineEdit_password->text();

    // 在状态区域显示即将加载的方案路径。
    ui->textEdit_status->append(solPath);

    // 发送加载方案命令，实际加载由 VMControl 完成。
    emit requestSolutionCommand(SolMotionType::Load, params);
}

/**
 * @brief 关闭当前方案。
 *
 * 输入数据：无。
 * 输出数据：requestSolutionCommand(Close) 信号。
 */
void VmWindow::on_pushButton_close_clicked()
{
    // 发送关闭方案命令。
    emit requestSolutionCommand(SolMotionType::Close);
}

/**
 * @brief 整套方案执行一次。
 *
 * 输入数据：无。
 * 输出数据：requestSolutionCommand(Run) 信号。
 */
void VmWindow::on_pushButton_run_clicked()
{
    // 发送方案执行一次命令。
    emit requestSolutionCommand(SolMotionType::Run);
}

/**
 * @brief 整套方案连续执行。
 *
 * 输入数据：无。
 * 输出数据：requestSolutionCommand(Runing) 信号。
 */
void VmWindow::on_pushButton_rining_clicked()
{
    // 发送方案连续执行命令。
    emit requestSolutionCommand(SolMotionType::Runing);
}

/**
 * @brief 停止整套方案执行。
 *
 * 输入数据：无。
 * 输出数据：requestSolutionCommand(Stop) 信号。
 */
void VmWindow::on_pushButton_stop_clicked()
{
    // 发送方案停止命令。
    emit requestSolutionCommand(SolMotionType::Stop);
}

/**
 * @brief 保存当前方案。
 *
 * 输入数据：无。
 * 输出数据：requestSolutionCommand(Save) 信号。
 */
void VmWindow::on_pushButton_save_clicked()
{
    // 发送方案保存命令。
    emit requestSolutionCommand(SolMotionType::Save);
}

/**
 * @brief 当前方案另存为。
 *
 * 输入数据：用户选择的新方案路径和界面密码。
 * 输出数据：requestSolutionCommand(SaveAs, params) 信号。
 */
void VmWindow::on_pushButton_saveAS_clicked()
{
    // 打开另存为对话框，并默认使用当前路径作为初始位置。
    const QString solPath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("方案另存为"),
        ui->lineEdit_path->text(),
        QStringLiteral("SOL(*.sol)"));

    // 用户取消保存时直接退出。
    if (solPath.isEmpty()) {
        return;
    }

    // 构造另存为参数包。
    QVariantMap params;
    params["solPath"] = solPath;
    params["password"] = ui->lineEdit_password->text();

    // 发送方案另存为命令。
    emit requestSolutionCommand(SolMotionType::SaveAs, params);
}

/**
 * @brief 当前流程执行一次。
 *
 * 输入数据：流程下拉框当前文本。
 * 输出数据：requestProcedureCommand(Run, processName) 信号。
 */
void VmWindow::on_runPro_Button_clicked()
{
    // 发送当前流程执行一次命令。
    emit requestProcedureCommand(ProcMotionType::Run, selectedProcessName());
}

/**
 * @brief 当前流程连续执行。
 *
 * 输入数据：流程下拉框当前文本。
 * 输出数据：requestProcedureCommand(Runing, processName) 信号。
 */
void VmWindow::on_runingPro_Button_clicked()
{
    // 发送当前流程连续执行命令。
    emit requestProcedureCommand(ProcMotionType::Runing, selectedProcessName());
}

/**
 * @brief 停止当前流程。
 *
 * 输入数据：流程下拉框当前文本。
 * 输出数据：requestProcedureCommand(Stop, processName) 信号。
 */
void VmWindow::on_stopPro_Button_clicked()
{
    // 发送当前流程停止命令。
    emit requestProcedureCommand(ProcMotionType::Stop, selectedProcessName());
}

/**
 * @brief 打开粗定位模块参数页示例。
 *
 * 输入数据：当前流程名、示例内置组合模块名和模块名。
 * 输出数据：requestModuleCommand(GroupSet, params) 信号。
 */
void VmWindow::on_pushButton_cuSet_clicked()
{
    // 构造组合模块内部模块参数包。
    QVariantMap params;
    params["processName"] = selectedProcessName();
    params["groupName"] = QStringLiteral("组合模块1");
    params["moduleName"] = QStringLiteral("轮廓匹配1");

    // 发送组合模块内部参数页打开命令。
    emit requestModuleCommand(ModuMotionType::GroupSet, params);
}

/**
 * @brief 打开精定位模块参数页示例。
 *
 * 输入数据：当前流程名、示例内置组合模块名和模块名。
 * 输出数据：requestModuleCommand(GroupSet, params) 信号。
 */
void VmWindow::on_pushButton_jingSet_clicked()
{
    // 构造组合模块内部模块参数包。
    QVariantMap params;
    params["processName"] = selectedProcessName();
    params["groupName"] = QStringLiteral("组合模块1");
    params["moduleName"] = QStringLiteral("轮廓匹配2");

    // 发送组合模块内部参数页打开命令。
    emit requestModuleCommand(ModuMotionType::GroupSet, params);
}

/**
 * @brief 打开直线查找模块参数页示例。
 *
 * 输入数据：当前流程名、示例内置组合模块名和模块名。
 * 输出数据：requestModuleCommand(GroupSet, params) 信号。
 */
void VmWindow::on_pushButton_lineSet_clicked()
{
    // 构造组合模块内部模块参数包。
    QVariantMap params;
    params["processName"] = selectedProcessName();
    params["groupName"] = QStringLiteral("组合模块1");
    params["moduleName"] = QStringLiteral("直线查找1");

    // 发送组合模块内部参数页打开命令。
    emit requestModuleCommand(ModuMotionType::GroupSet, params);
}

/**
 * @brief 打开标定转换模块参数页示例。
 *
 * 输入数据：当前流程名、示例内置组合模块名和模块名。
 * 输出数据：requestModuleCommand(GroupSet, params) 信号。
 */
void VmWindow::on_pushButton_turnSet_clicked()
{
    // 构造组合模块内部模块参数包。
    QVariantMap params;
    params["processName"] = selectedProcessName();
    params["groupName"] = QStringLiteral("组合模块1");
    params["moduleName"] = QStringLiteral("标定转换1");

    // 发送组合模块内部参数页打开命令。
    emit requestModuleCommand(ModuMotionType::GroupSet, params);
}

/**
 * @brief 打开图像源模块参数页示例。
 *
 * 输入数据：当前流程名和流程直属模块名。
 * 输出数据：requestModuleCommand(Set, params) 信号。
 */
void VmWindow::on_runPro_Button_6_clicked()
{
    // 构造流程直属模块参数包。
    QVariantMap params;
    params["processName"] = selectedProcessName();
    params["moduleName"] = QStringLiteral("图像源");

    // 发送流程直属模块参数页打开命令。
    emit requestModuleCommand(ModuMotionType::Set, params);
}

/**
 * @brief 显示多目标结果。
 * @param pack VMControl 返回的数据包。
 *
 * 输入数据：pack["processName"] 和 pack["TargetDataList"]。
 * 输出数据：向数据文本框追加流程名、目标数量和目标明细。
 */
void VmWindow::appendTargetData(const QVariantMap& pack)
{
    // 读取结果所属流程名。
    const QString processName = pack["processName"].toString();

    // 从 QVariant 中取出多目标列表。
    const QList<TargetData> dataList = pack["TargetDataList"].value<QList<TargetData>>();

    // 先显示流程名和目标数量。
    ui->textEdit_data->append(QStringLiteral("流程[%1]返回目标数据，数量：%2")
                                  .arg(processName)
                                  .arg(dataList.size()));

    // 向外部进程输出目标数据（stdout 管道，供 Robot_R06 捕获）
    {
        QString line = QStringLiteral("VISION_TARGET|%1|%2").arg(processName).arg(dataList.size());
        for (const TargetData &data : dataList) {
            line += QStringLiteral("|%1,%2,%3,%4,%5")
                        .arg(data.id).arg(data.status).arg(data.x).arg(data.y).arg(data.r);
        }
        outputVisionLine(line);
    }

    // 再逐条显示目标坐标。
    dataShow(dataList);
}

/**
 * @brief 显示目标列表明细。
 * @param dataList 多目标结果列表。
 *
 * 输入数据：TargetData 列表。
 * 输出数据：向数据文本框逐行追加目标编号、状态和坐标。
 */
void VmWindow::dataShow(const QList<TargetData>& dataList)
{
    // 遍历每一个目标点并追加显示。
    for (const TargetData& data : dataList) {
        ui->textEdit_data->append(QStringLiteral("编号[%1]: 状态:%2, 坐标:(%3, %4, %5)")
                                      .arg(data.id)
                                      .arg(data.status)
                                      .arg(data.x)
                                      .arg(data.y)
                                      .arg(data.r));
    }
}

/**
 * @brief 显示单点结果。
 * @param pack VMControl 返回的数据包。
 *
 * 输入数据：pack["processName"] 和 pack["PointData"]。
 * 输出数据：向数据文本框追加单点坐标。
 */
void VmWindow::appendPointData(const QVariantMap& pack)
{
    // 读取结果所属流程名。
    const QString processName = pack["processName"].toString();

    // 从 QVariant 中取出点位数据。
    const PointData point = pack["PointData"].value<PointData>();

    // 追加显示点位坐标。
    ui->textEdit_data->append(
        QStringLiteral("流程[%1]返回点位数据：(%2, %3, %4)")
            .arg(processName)
            .arg(point.x)
            .arg(point.y)
            .arg(point.r));

    // 向外部进程输出点位数据（stdout 管道，供 Robot_R06 捕获）
    outputVisionLine(QStringLiteral("VISION_POINT|%1|%2|%3|%4")
                         .arg(processName).arg(point.x).arg(point.y).arg(point.r));
}

/**
 * @brief 处理方案加载后的流程列表。
 * @param pack VMControl 返回的方案数据包。
 *
 * 输入数据：pack["number"] 和 pack["groupNameN"]。
 * 输出数据：刷新流程下拉框并输出加载日志。
 */
void VmWindow::handleSolData(const QVariantMap &pack)
{
    // 清空旧流程列表。
    ui->ProcessChoose_Box->clear();

    // 读取流程数量。
    const int number = pack["number"].toInt();

    // 逐个读取流程名并加入下拉框。
    for (int i = 0; i < number; ++i) {
        const QString groupName = QString("groupName%1").arg(i);
        const QString processName = pack[groupName].toString();
        ui->ProcessChoose_Box->addItem(processName);
        ui->textEdit_data->append(QStringLiteral("加载流程：%1").arg(processName));
    }

    // 向外部进程输出流程列表（stdout 管道，供 Robot_R06 捕获）
    {
        QStringList names;
        for (int i = 0; i < number; ++i) {
            names << pack[QString("groupName%1").arg(i)].toString();
        }
        outputVisionLine(QStringLiteral("VISION_PROCS|%1").arg(names.join(",")));
    }
}

/**
 * @brief 获取当前选择的流程名称。
 *
 * 输入数据：流程下拉框当前项。
 * 输出数据：当前流程名称字符串。
 */
QString VmWindow::selectedProcessName() const
{
    // 返回流程选择框当前文本。
    return ui->ProcessChoose_Box->currentText();
}
