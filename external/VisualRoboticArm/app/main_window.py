# -*- coding: utf-8 -*-
from __future__ import annotations

import os
from datetime import datetime

from PySide6.QtCore import QTimer, Qt
from PySide6.QtGui import QAction, QColor, QFont, QImage, QPainter, QPen, QPixmap, QTextCursor
from PySide6.QtMultimedia import QCamera, QMediaCaptureSession, QMediaDevices, QVideoSink
from PySide6.QtWidgets import (
    QComboBox,
    QButtonGroup,
    QGridLayout,
    QGroupBox,
    QHBoxLayout,
    QLabel,
    QListWidget,
    QMainWindow,
    QMessageBox,
    QLineEdit,
    QPlainTextEdit,
    QPushButton,
    QSizePolicy,
    QSpinBox,
    QStatusBar,
    QTextEdit,
    QTabWidget,
    QVBoxLayout,
    QWidget,
)

from app.rs485 import RS485Config, RS485Manager
from app.pose_comm import PoseCommClient
from app.pose_filter import PoseMotionFilter
from app.worker import FrameProcessorThread


class MainWindow(QMainWindow):
    """Camera preview and joint-angle display window."""

    def __init__(self):
        super().__init__()
        self.preview_size = (760, 520)
        self.camera_map = {}
        self.camera_items = []
        self.connected_camera = None
        self.connected_camera_name = ""
        self.camera = None
        self.capture_session = None
        self.video_sink = None
        self.is_capturing = False
        self.frame_count = 0
        self.last_processed_image = None
        self.frame_worker = None
        self.rs485_manager = None
        self.rs485_ports = []
        self.pose_comm = None
        self.pose_filter = PoseMotionFilter(
            movement_threshold_deg=float(os.environ.get("VISUAL_ARM_POSE_MOVE_THRESHOLD_DEG", "5.0"))
        )

        self.preview_timer = QTimer(self)
        self.preview_timer.setInterval(400)

        self.setWindowTitle("VisualRoboticArm - 机器手臂视觉")
        self.resize(1100, 760)

        self._create_actions()
        self._create_menu_bar()
        self._create_status_bar()
        self._create_widgets()
        self._create_layout()
        self._connect_signals()
        self._apply_styles()
        self._ensure_frame_worker()
        self._init_rs485()
        self._init_pose_comm()

        self.on_enum_clicked()
        self._refresh_control_state()
        self._refresh_rs485_ports()
        self._update_preview("等待连接相机")

    def _create_actions(self):
        self.action_exit = QAction("退出", self)
        self.action_about = QAction("关于", self)

    def _create_menu_bar(self):
        menu_bar = self.menuBar()
        file_menu = menu_bar.addMenu("文件")
        file_menu.addAction(self.action_exit)
        help_menu = menu_bar.addMenu("帮助")
        help_menu.addAction(self.action_about)

    def _create_status_bar(self):
        status_bar = QStatusBar()
        status_bar.showMessage("准备就绪")
        self.setStatusBar(status_bar)

    def _create_widgets(self):
        self.central = QWidget()
        self.setCentralWidget(self.central)

        self.btn_camera_module = QPushButton("相机模块")
        self.btn_serial_module = QPushButton("串口模块")
        self.btn_log_window = QPushButton("打开日志窗口")
        self.btn_camera_module.setMinimumHeight(36)
        self.btn_serial_module.setMinimumHeight(36)
        self.btn_log_window.setMinimumHeight(36)
        self.btn_camera_module.setCheckable(True)
        self.btn_serial_module.setCheckable(True)
        self.btn_camera_module.setChecked(True)
        self.btn_serial_module.setChecked(False)
        self.module_button_group = QButtonGroup(self)
        self.module_button_group.setExclusive(True)
        self.module_button_group.addButton(self.btn_camera_module, 0)
        self.module_button_group.addButton(self.btn_serial_module, 1)

        self.preview_container = QWidget()
        self.preview_container.setObjectName("previewContainer")
        self.preview_container.setFixedSize(*self.preview_size)
        self.preview_container.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)

        self.preview_frame = QLabel()
        self.preview_frame.setObjectName("previewFrame")
        self.preview_frame.setAlignment(Qt.AlignCenter)
        self.preview_frame.setFixedSize(*self.preview_size)
        self.preview_frame.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)

        preview_layout = QVBoxLayout()
        preview_layout.setContentsMargins(0, 0, 0, 0)
        preview_layout.addWidget(self.preview_frame)
        self.preview_container.setLayout(preview_layout)

        self.image_info_label = QLabel("W:0,H:0 | X:-,Y:- | R:-,G:-,B:- | Gray:-")
        self.frame_info_label = QLabel("Frame: -")
        for label in (self.image_info_label, self.frame_info_label):
            label.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
            label.setFixedHeight(20)

        self.camera_panel = QGroupBox("设备模块")
        self.btn_enum = QPushButton("枚举")
        self.btn_connect = QPushButton("连接")
        self.btn_disconnect = QPushButton("断开")
        self.camera_list = QListWidget()
        self.camera_list.setMaximumHeight(160)
        self.camera_list.setMinimumHeight(80)
        self.btn_start = QPushButton("开始采集")
        self.btn_stop = QPushButton("停止")
        self.btn_params = QPushButton("参数")
        self.interval_spin = QSpinBox()
        self.interval_spin.setRange(10, 500)
        self.interval_spin.setValue(33)
        self.interval_spin.setSuffix(" ms")
        self.state_value = QLabel("等待枚举相机")
        self.action_value = QTextEdit()
        self.action_value.setReadOnly(True)
        self.action_value.setPlainText("暂无操作")
        self.action_value.setMinimumHeight(100)
        self.action_value.setMaximumHeight(400)
        self.action_value.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.action_value.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        self.action_value.setLineWrapMode(QTextEdit.WidgetWidth)
        self.action_value.setObjectName("actionText")

        self.runtime_group = QGroupBox("关节旋转角度")
        self.runtime_group.setMinimumHeight(180)
        self.runtime_text = QTextEdit()
        self.runtime_text.setObjectName("angleText")
        self.runtime_text.setReadOnly(True)
        self.runtime_text.setMinimumHeight(132)
        self.runtime_group.setMaximumHeight(180)
        self.runtime_group.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        self.runtime_text.setMaximumHeight(132)
        self.runtime_text.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        self.runtime_text.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.runtime_text.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.runtime_text.setLineWrapMode(QTextEdit.WidgetWidth)
        self.runtime_text.setHtml(_angle_placeholder_html())

        self.serial_port_combo = QComboBox()
        self.serial_baud_combo = QComboBox()
        self.serial_baud_combo.addItems(["9600", "19200", "38400", "57600", "115200", "230400"])
        self.serial_baud_combo.setCurrentText("115200")
        self.serial_refresh_button = QPushButton("刷新")
        self.serial_open_button = QPushButton("打开")
        self.serial_close_button = QPushButton("关闭")
        self.serial_send_button = QPushButton("发送")
        self.serial_hex_send_button = QPushButton("发HEX")
        self.serial_rx_view = QPlainTextEdit()
        self.serial_rx_view.setReadOnly(True)
        self.serial_rx_view.setPlaceholderText("接收区")
        self.serial_rx_view.setMaximumBlockCount(1000)
        self.serial_rx_view.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.serial_rx_view.setObjectName("serialLog")
        self.serial_rx_view.setMinimumHeight(220)
        self.serial_tx_edit = QLineEdit()
        self.serial_tx_edit.setPlaceholderText("输入要发送的文本")
        self.serial_tx_edit.setObjectName("serialTxEdit")
        self.serial_status_label = QLabel("未初始化")

    def _create_layout(self):
        main_layout = QVBoxLayout()
        main_layout.setContentsMargins(18, 12, 18, 12)
        main_layout.setSpacing(8)

        left_layout = QVBoxLayout()
        left_layout.setContentsMargins(0, 0, 0, 0)
        left_layout.setSpacing(6)
        left_layout.addWidget(self.preview_container, 0, Qt.AlignTop | Qt.AlignHCenter)
        left_layout.addWidget(self.image_info_label)
        left_layout.addWidget(self.frame_info_label)
        left_layout.addWidget(self.runtime_group, 0)
        left_layout.addStretch(1)

        runtime_layout = QVBoxLayout()
        runtime_layout.setContentsMargins(10, 18, 10, 10)
        runtime_layout.addWidget(self.runtime_text)
        self.runtime_group.setLayout(runtime_layout)

        self.module_tabs = QTabWidget()
        self.module_tabs.setDocumentMode(True)
        self.module_tabs.setTabPosition(QTabWidget.North)

        camera_page = QWidget()
        camera_layout = QVBoxLayout(camera_page)
        camera_layout.setContentsMargins(0, 0, 0, 0)
        camera_layout.setSpacing(8)
        top_control_layout = QHBoxLayout()
        top_control_layout.addWidget(self.btn_enum)
        top_control_layout.addWidget(self.btn_connect)
        top_control_layout.addWidget(self.btn_disconnect)
        camera_layout.addLayout(top_control_layout)
        camera_layout.addWidget(self.camera_list, 0)

        camera_bottom = QHBoxLayout()
        camera_bottom.addWidget(self.btn_start)
        camera_bottom.addWidget(self.btn_stop)
        camera_bottom.addWidget(self.btn_params)
        camera_layout.addLayout(camera_bottom)

        camera_info = QGridLayout()
        camera_info.addWidget(QLabel("采集间隔"), 0, 0)
        camera_info.addWidget(self.interval_spin, 0, 1)
        camera_info.addWidget(QLabel("状态"), 1, 0)
        camera_info.addWidget(self.state_value, 1, 1)
        camera_info.addWidget(QLabel("最近操作"), 2, 0)
        camera_info.addWidget(self.action_value, 2, 1)
        camera_info.setRowStretch(0, 0)
        camera_info.setRowStretch(1, 0)
        camera_info.setRowStretch(2, 1)
        camera_layout.addLayout(camera_info, 1)

        serial_page = QWidget()
        serial_layout = QVBoxLayout(serial_page)
        serial_layout.setContentsMargins(0, 0, 0, 0)
        serial_layout.setSpacing(8)
        serial_top = QHBoxLayout()
        serial_top.addWidget(QLabel("端口"))
        serial_top.addWidget(self.serial_port_combo, 2)
        serial_top.addWidget(self.serial_refresh_button)
        serial_top.addWidget(QLabel("波特率"))
        serial_top.addWidget(self.serial_baud_combo, 1)
        serial_layout.addLayout(serial_top)

        serial_open_bar = QHBoxLayout()
        serial_open_bar.addWidget(self.serial_open_button)
        serial_open_bar.addWidget(self.serial_close_button)
        serial_open_bar.addStretch(1)
        serial_layout.addLayout(serial_open_bar)

        serial_layout.addWidget(self.serial_rx_view, 1)

        serial_send_bar = QHBoxLayout()
        serial_send_bar.addWidget(self.serial_tx_edit, 1)
        serial_send_bar.addWidget(self.serial_send_button)
        serial_send_bar.addWidget(self.serial_hex_send_button)
        serial_layout.addLayout(serial_send_bar)
        serial_layout.addWidget(self.serial_status_label)

        self.module_tabs.addTab(camera_page, "相机模块")
        self.module_tabs.addTab(serial_page, "串口模块")

        panel_layout = QVBoxLayout()
        panel_layout.setSpacing(8)
        module_layout = QHBoxLayout()
        module_layout.setContentsMargins(0, 0, 0, 0)
        module_layout.setSpacing(8)
        module_layout.addWidget(self.btn_camera_module)
        module_layout.addWidget(self.btn_serial_module)
        module_layout.addWidget(self.btn_log_window)
        panel_layout.addLayout(module_layout)
        panel_layout.addWidget(self.module_tabs, 1)

        self.camera_panel.setLayout(panel_layout)
        self.camera_panel.setFixedWidth(610)
        self.camera_panel.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)

        body_layout = QHBoxLayout()
        body_layout.setContentsMargins(0, 0, 0, 0)
        body_layout.setSpacing(14)
        body_layout.addLayout(left_layout, 1)
        body_layout.addWidget(self.camera_panel, 0)

        main_layout.addLayout(body_layout, 1)
        self.central.setLayout(main_layout)

    def _connect_signals(self):
        self.action_exit.triggered.connect(self.close)
        self.action_about.triggered.connect(self.on_about)
        self.preview_timer.timeout.connect(self._update_capture_frame)
        self.btn_enum.clicked.connect(self.on_enum_clicked)
        self.btn_connect.clicked.connect(self.on_connect_clicked)
        self.btn_disconnect.clicked.connect(self.on_disconnect_clicked)
        self.btn_start.clicked.connect(self.on_start_clicked)
        self.btn_stop.clicked.connect(self.on_stop_clicked)
        self.btn_params.clicked.connect(self.on_params_clicked)
        self.btn_log_window.clicked.connect(self.on_show_log_clicked)
        self.interval_spin.valueChanged.connect(self.on_interval_changed)
        self.camera_list.itemSelectionChanged.connect(self._refresh_control_state)
        self.module_tabs.currentChanged.connect(self._on_module_tab_changed)
        self.btn_camera_module.clicked.connect(lambda: self.module_tabs.setCurrentIndex(0))
        self.btn_serial_module.clicked.connect(lambda: self.module_tabs.setCurrentIndex(1))
        self.serial_refresh_button.clicked.connect(self._refresh_rs485_ports)
        self.serial_open_button.clicked.connect(self.on_rs485_open_clicked)
        self.serial_close_button.clicked.connect(self.on_rs485_close_clicked)
        self.serial_send_button.clicked.connect(self.on_rs485_send_clicked)
        self.serial_hex_send_button.clicked.connect(self.on_rs485_send_hex_clicked)
        self.serial_tx_edit.returnPressed.connect(self.on_rs485_send_clicked)

    def _apply_styles(self):
        self.setStyleSheet(
            """
            QMainWindow, QWidget {
                background-color: #F4F4F4;
                color: #202124;
            }
            #titleLabel {
                font-size: 20px;
                font-weight: 600;
                padding: 6px 0;
            }
            QPushButton {
                min-height: 30px;
                padding: 4px 12px;
                border: 1px solid #CFCFCF;
                border-radius: 4px;
                background-color: #FAFAFA;
            }
            QPushButton:hover {
                background-color: #FFFFFF;
            }
            QPushButton:disabled {
                color: #9D9D9D;
                background-color: #F0F0F0;
            }
            QPushButton:checked {
                background-color: #FFFFFF;
                border-color: #AFCBFF;
            }
            QGroupBox {
                font-weight: 600;
                border: 1px solid #D6D6D6;
                border-radius: 6px;
                margin-top: 12px;
                padding-top: 12px;
                background-color: #F8F8F8;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 12px;
                padding: 0 4px;
            }
            QListWidget, QTextEdit, QSpinBox {
                background-color: #FFFFFF;
                border: 1px solid #D6D6D6;
                border-radius: 4px;
            }
            #serialLog {
                background-color: #FFFFFF;
                color: #202124;
                border: 1px solid #3D4045;
                font-family: Consolas, "Microsoft YaHei UI";
                font-size: 13px;
            }
            #serialTxEdit {
                font-family: Consolas, "Microsoft YaHei UI";
                font-size: 13px;
            }
            #angleText {
                background-color: #FFFFFF;
                color: #202124;
                border: 1px solid #3D4045;
                font-family: Consolas, "Microsoft YaHei UI";
                font-size: 14px;
            }
            #previewContainer, #previewFrame {
                border: 1px solid #C8C8C8;
                background-color: #1F1F1F;
            }
            """
        )

    def on_about(self):
        QMessageBox.information(
            self,
            "关于",
            "VisualRoboticArm 相机预览与机器手臂关节角度演示程序。",
        )

    def on_enum_clicked(self):
        self._teardown_camera()
        self.connected_camera = None
        self.connected_camera_name = ""
        self.is_capturing = False
        self.frame_count = 0
        self.frame_info_label.setText("Frame: -")
        self.camera_map = self._enumerate_cameras()
        self.camera_items = list(self.camera_map.keys())
        self.camera_list.clear()
        self.camera_list.addItems(self.camera_items)
        if self.camera_items:
            self.camera_list.setCurrentRow(0)
            message = f"枚举完成，共发现 {len(self.camera_items)} 台相机"
        else:
            message = "未发现可用相机"
        self.state_value.setText(message)
        self._set_action_text(message)
        self.statusBar().showMessage(message, 3000)
        self._update_preview("等待连接相机")
        self._refresh_control_state()

    def on_connect_clicked(self):
        current_item = self.camera_list.currentItem()
        if current_item is None:
            QMessageBox.warning(self, "提示", "请先选择相机设备")
            return
        selected_name = current_item.text()
        self.connected_camera = self.camera_map.get(selected_name)
        if self.connected_camera is None:
            QMessageBox.warning(self, "提示", "相机设备无效，请重新枚举")
            return
        self.connected_camera_name = selected_name
        self._setup_camera_pipeline()
        self.state_value.setText(f"已连接: {selected_name}")
        self.action_value.setPlainText(f"成功: 已连接 {selected_name}")
        self.statusBar().showMessage("相机已连接", 3000)
        self._update_preview("相机已连接\n等待开始采集")
        self._refresh_control_state()

    def on_disconnect_clicked(self):
        disconnected_name = self.connected_camera_name or "当前设备"
        self.preview_timer.stop()
        self._teardown_camera()
        self.connected_camera = None
        self.connected_camera_name = ""
        self.is_capturing = False
        self.frame_count = 0
        self.last_processed_image = None
        self.frame_info_label.setText("Frame: -")
        self.image_info_label.setText("W:0,H:0 | X:-,Y:- | R:-,G:-,B:- | Gray:-")
        self.runtime_text.setHtml(_angle_placeholder_html())
        self.state_value.setText("相机未连接")
        self.action_value.setPlainText(f"已断开: {disconnected_name}")
        self.statusBar().showMessage("相机已断开", 3000)
        self._update_preview("等待连接相机")
        self._refresh_control_state()

    def on_start_clicked(self):
        if self.connected_camera is None:
            QMessageBox.warning(self, "提示", "请先连接相机")
            return
        if self.camera is None:
            self._setup_camera_pipeline()
        if self.camera is None:
            QMessageBox.warning(self, "提示", "相机初始化失败，无法开始采集")
            return
        self._ensure_frame_worker()
        self.frame_count = 0
        self.pose_filter.reset()
        self.is_capturing = True
        self.preview_timer.setInterval(self.interval_spin.value())
        self.preview_timer.start()
        self.camera.start()
        self.state_value.setText("采集中")
        self.action_value.setPlainText(f"成功: 开始采集 {self.connected_camera_name}")
        self.statusBar().showMessage("采集已启动", 3000)
        self._refresh_control_state()

    def on_stop_clicked(self):
        self.preview_timer.stop()
        if self.camera is not None:
            self.camera.stop()
        self.is_capturing = False
        self.state_value.setText("采集已停止")
        self.action_value.setPlainText("成功: 已停止采集")
        self.statusBar().showMessage("采集已停止", 3000)
        self._update_preview("采集已停止")
        self._refresh_control_state()

    def on_params_clicked(self):
        selected_camera = self.connected_camera_name or "未连接设备"
        QMessageBox.information(
            self,
            "参数信息",
            f"设备: {selected_camera}\n采集间隔: {self.interval_spin.value()} ms\n预览区: 760 x 520",
        )

    def on_show_log_clicked(self):
        QMessageBox.information(self, "关节角度", "关节旋转角度显示在底部文本框中。")

    def on_interval_changed(self, value: int):
        self.preview_timer.setInterval(value)
        self.statusBar().showMessage(f"采集间隔已设置为 {value} ms", 2000)

    def on_video_frame_changed(self, frame):
        """接收 Qt 摄像头帧并提交到后台推理线程。

        输入：
        - frame：Qt Multimedia 推送的 QVideoFrame。

        输出：无。
        功能实现：
        - 将 QVideoFrame 转为 QImage；
        - 刷新帧计数与图像尺寸显示；
        - 如果后台推理线程可用，则提交给 `FrameProcessorThread.submit()`。
        """
        image = frame.toImage()
        if image.isNull():
            return
        # 这里计数的是摄像头输入帧，不是实际完成推理的帧。
        self.frame_count += 1
        self.frame_info_label.setText(f"Frame: {self.frame_count}")
        self.image_info_label.setText(
            f"W:{image.width()},H:{image.height()} | X:-,Y:- | R:-,G:-,B:- | Gray:-"
        )
        if self.frame_worker is None or not self.frame_worker.isRunning():
            # 推理线程不可用时仍显示原始画面，避免预览黑屏。
            self._display_image(image)
            return
        # 将帧交给后台线程；线程内部只保留最新帧以降低延迟。
        self.frame_worker.submit(image)

    def on_processed_frame(self, image: QImage, payload: dict):
        """接收后台推理结果并更新 UI/通信。

        输入：
        - image：推理线程返回的标注后 QImage；
        - payload：推理服务输出的角度、电机命令和识别状态字典。

        输出：无。
        功能实现：
        - 显示标注画面；
        - 通过 `PoseMotionFilter` 稳定角度和电机命令；
        - 更新右侧动作摘要和下方角度面板；
        - 将过滤后的 payload 交给 TCP 通信模块发送。
        """
        self.last_processed_image = image
        self._display_image(image)
        # 过滤层负责低幅抖动保持、无检测时沿用上一次有效值。
        payload = self.pose_filter.filter_payload(payload)
        angles = payload.get("angles", [])
        motor_6_summary = _format_motor_6_summary(payload.get("motor_6"), payload.get("angles", []))
        if angles:
            summary = _format_arm_rotation_summary(angles)
            if not payload.get("motion_allowed", True):
                summary = f"[保持] {summary}"
            # action_value 显示电机动作摘要，runtime_text 显示完整左右臂角度。
            self.action_value.setPlainText(motor_6_summary or summary.splitlines()[0])
            self.runtime_text.setHtml(_format_arm_rotation_html(angles))
        else:
            if motor_6_summary:
                self.action_value.setPlainText(motor_6_summary)
            self.runtime_text.setHtml(_angle_placeholder_html())

        if self.pose_comm is not None:
            # 通信 payload 附加当前输入帧编号，便于外部系统追踪。
            pose_payload = dict(payload)
            pose_payload["frame_id"] = self.frame_count
            self.pose_comm.send_pose(self.frame_count, pose_payload)

    def on_worker_error(self, message: str):
        self.statusBar().showMessage(message, 5000)
        QMessageBox.warning(self, "推理模块错误", message)
        self.action_value.setPlainText("推理模块不可用，已回退到原始画面")

    def _set_action_text(self, text: str):
        """设置最近操作文本并自动滚动到底部。"""
        self.action_value.setPlainText(text)
        self.action_value.moveCursor(QTextCursor.End)
        self.action_value.ensureCursorVisible()

    def _refresh_control_state(self):
        has_selection = self.camera_list.currentItem() is not None
        is_connected = self.connected_camera is not None
        self.btn_connect.setEnabled(has_selection and not is_connected)
        self.btn_disconnect.setEnabled(is_connected)
        self.btn_start.setEnabled(is_connected and not self.is_capturing)
        self.btn_stop.setEnabled(self.is_capturing)
        self.btn_params.setEnabled(is_connected)

    def _init_rs485(self):
        self.rs485_manager = RS485Manager(self)
        self.rs485_manager.port_opened.connect(self._on_rs485_opened)
        self.rs485_manager.port_closed.connect(self._on_rs485_closed)
        self.rs485_manager.bytes_received.connect(self._on_rs485_bytes_received)
        self.rs485_manager.frame_received.connect(self._on_rs485_frame_received)
        self.rs485_manager.text_received.connect(self._on_rs485_text_received)
        self.rs485_manager.error_occurred.connect(self._on_rs485_error)

    def _init_pose_comm(self):
        host = os.environ.get("VISUAL_ARM_COMM_HOST", "127.0.0.1").strip()
        try:
            port = int(os.environ.get("VISUAL_ARM_COMM_PORT", "5000"))
        except ValueError:
            port = 5000
        client_name = os.environ.get("VISUAL_ARM_COMM_CLIENT", "visual-arm").strip() or "visual-arm"

        self.pose_comm = PoseCommClient(self, host=host, port=port, client_name=client_name)
        self.pose_comm.statusChanged.connect(lambda text: self.statusBar().showMessage(text, 2500))
        self.pose_comm.errorOccurred.connect(lambda text: self.statusBar().showMessage(text, 4000))
        self.pose_comm.start()

    def _refresh_rs485_ports(self):
        self.serial_port_combo.clear()
        self.rs485_ports = RS485Manager.scan_ports()
        for port in self.rs485_ports:
            self.serial_port_combo.addItem(port.display_name, port.port_name)
        if not self.rs485_ports:
            self.serial_port_combo.addItem("未发现串口", "")
        self._set_rs485_status(f"已扫描 {len(self.rs485_ports)} 个串口")

    def _selected_rs485_port_name(self) -> str:
        return self.serial_port_combo.currentData() or ""

    def _selected_rs485_baud_rate(self) -> int:
        try:
            return int(self.serial_baud_combo.currentText())
        except ValueError:
            return 115200

    def _set_rs485_status(self, text: str):
        self.serial_status_label.setText(text)
        self.statusBar().showMessage(text, 3000)

    def _append_rs485_log(self, text: str):
        stamp = datetime.now().strftime("%H:%M:%S")
        self.serial_rx_view.appendPlainText(f"[{stamp}] {text}")

    def _on_module_tab_changed(self, index: int):
        self.btn_camera_module.setChecked(index == 0)
        self.btn_serial_module.setChecked(index == 1)
        if index == 0:
            self._refresh_control_state()
        elif index == 1:
            self._refresh_rs485_ports()

    def on_rs485_open_clicked(self):
        if self.rs485_manager is None:
            self._init_rs485()

        port_name = self._selected_rs485_port_name()
        if not port_name:
            QMessageBox.warning(self, "提示", "请先选择串口")
            return

        config = RS485Config(
            port_name=port_name,
            baud_rate=self._selected_rs485_baud_rate(),
        )
        if self.rs485_manager.open_port(config):
            self._set_rs485_status(f"已打开 {port_name}")

    def on_rs485_close_clicked(self):
        if self.rs485_manager is not None:
            self.rs485_manager.close_port()
        self._set_rs485_status("已关闭串口")

    def on_rs485_send_clicked(self):
        if self.rs485_manager is None or not self.rs485_manager.is_open:
            QMessageBox.warning(self, "提示", "请先打开串口")
            return
        text = self.serial_tx_edit.text().strip()
        if not text:
            return
        if self.rs485_manager.send_text(text):
            self._append_rs485_log(f"TX: {text}")
            self.serial_tx_edit.clear()

    def on_rs485_send_hex_clicked(self):
        if self.rs485_manager is None or not self.rs485_manager.is_open:
            QMessageBox.warning(self, "提示", "请先打开串口")
            return
        text = self.serial_tx_edit.text().strip()
        if not text:
            return
        if self.rs485_manager.send_hex(text):
            self._append_rs485_log(f"TX HEX: {text}")
            self.serial_tx_edit.clear()

    def _on_rs485_opened(self, port_name: str):
        self._set_rs485_status(f"串口已打开: {port_name}")

    def _on_rs485_closed(self):
        self._set_rs485_status("串口已关闭")

    def _on_rs485_bytes_received(self, payload: bytes):
        self._append_rs485_log(f"RX BYTES: {payload.hex(' ').upper()}")

    def _on_rs485_frame_received(self, payload: bytes):
        self._append_rs485_log(f"RX FRAME: {payload.hex(' ').upper()}")

    def _on_rs485_text_received(self, text: str):
        self._append_rs485_log(f"RX TEXT: {text}")

    def _on_rs485_error(self, message: str):
        self._set_rs485_status(message)
        self._append_rs485_log(f"ERR: {message}")

    def _ensure_frame_worker(self):
        """确保后台推理线程已创建并启动。

        输入：无。
        输出：无。
        功能：连接推理结果/错误信号，然后启动 `FrameProcessorThread`。
        """
        if self.frame_worker is not None and self.frame_worker.isRunning():
            return
        self.frame_worker = FrameProcessorThread(self)
        # 推理完成后回到主线程更新画面、角度文本和通信。
        self.frame_worker.frame_ready.connect(self.on_processed_frame)
        # 推理初始化或运行错误统一交给 UI 弹窗与状态栏显示。
        self.frame_worker.error.connect(self.on_worker_error)
        self.frame_worker.start()

    def _setup_camera_pipeline(self):
        """建立 Qt 摄像头采集管线。

        输入：无，使用当前选中的 `self.connected_camera`。
        输出：无。
        功能实现：
        - 清理旧摄像头管线；
        - 创建 QCamera/QMediaCaptureSession/QVideoSink；
        - 优先选择 1280x720 格式；
        - 将 `videoFrameChanged` 信号连接到实时推理入口。
        """
        if self.connected_camera is None:
            return
        self._teardown_camera(clear_preview=False)
        self.camera = QCamera(self.connected_camera)

        # 设置摄像头采集分辨率为 1280×720 以提升手部识别精度
        camera_device = self.connected_camera
        formats = camera_device.videoFormats()
        target_w, target_h = 1280, 720
        best_format = None
        for fmt in formats:
            res = fmt.resolution()
            if res.width() == target_w and res.height() == target_h:
                best_format = fmt
                break
        if best_format is not None:
            self.camera.setCameraFormat(best_format)

        self.capture_session = QMediaCaptureSession()
        self.video_sink = QVideoSink()
        # 每收到一帧就进入 on_video_frame_changed，再由后台线程处理。
        self.video_sink.videoFrameChanged.connect(self.on_video_frame_changed)
        self.capture_session.setCamera(self.camera)
        self.capture_session.setVideoSink(self.video_sink)

    def _teardown_camera(self, clear_preview: bool = True):
        if self.camera is not None:
            self.camera.stop()
            self.camera = None
        if self.capture_session is not None:
            self.capture_session.setCamera(None)
            self.capture_session.setVideoSink(None)
            self.capture_session = None
        if self.video_sink is not None:
            try:
                self.video_sink.videoFrameChanged.disconnect(self.on_video_frame_changed)
            except Exception:
                pass
            self.video_sink = None
        if clear_preview:
            self._update_preview("等待连接相机")

    def _enumerate_cameras(self):
        camera_map = {}
        for camera in QMediaDevices.videoInputs():
            description = camera.description().strip()
            if description:
                camera_map[description] = camera
        return camera_map

    def _display_image(self, image: QImage):
        pixmap = QPixmap.fromImage(image)
        scaled = pixmap.scaled(
            self.preview_frame.size(),
            Qt.KeepAspectRatio,
            Qt.SmoothTransformation,
        )
        self.preview_frame.setPixmap(scaled)

    def _update_capture_frame(self):
        if self.camera is None:
            self.frame_count += 1
            self.frame_info_label.setText(f"Frame: {self.frame_count}")
            self._update_preview(
                f"实时图像预览\n{self.connected_camera_name or '未连接设备'}\nFrame #{self.frame_count}"
            )

    def _update_preview(self, overlay_text: str):
        pixmap = QPixmap(*self.preview_size)
        pixmap.fill(QColor("#202225"))
        painter = QPainter(pixmap)
        checker_size = 28
        dark = QColor("#202225")
        light = QColor("#2B2D30")
        for x in range(0, pixmap.width(), checker_size):
            for y in range(0, pixmap.height(), checker_size):
                painter.fillRect(
                    x,
                    y,
                    checker_size,
                    checker_size,
                    light if (x // checker_size + y // checker_size) % 2 == 0 else dark,
                )
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setPen(QPen(QColor("#5CC8FF"), 2))
        painter.drawRect(12, 12, pixmap.width() - 24, pixmap.height() - 24)
        painter.setPen(QColor("#F5F7FA"))
        painter.setFont(QFont("Microsoft YaHei UI", 20, QFont.Bold))
        painter.drawText(pixmap.rect().adjusted(0, -30, 0, 0), Qt.AlignCenter, overlay_text)
        painter.setPen(QColor("#E8D36F"))
        painter.setFont(QFont("Consolas", 11))
        painter.drawText(
            pixmap.rect().adjusted(18, 0, -18, -16),
            Qt.AlignBottom | Qt.AlignLeft,
            "Preview Area 760 x 520",
        )
        painter.end()
        self.preview_frame.setPixmap(pixmap)

    def closeEvent(self, event):
        self.preview_timer.stop()
        self._teardown_camera(clear_preview=False)
        if self.pose_comm is not None:
            self.pose_comm.stop()
        if self.rs485_manager is not None:
            self.rs485_manager.close_port()
        if self.frame_worker is not None:
            self.frame_worker.requestInterruption()
            self.frame_worker.wait(1500)
        super().closeEvent(event)


def _angle_placeholder_html():
    return """
    <div style="height: 100%; color: #666666; font-size: 15px; background:#FFFFFF;">
        等待推理结果
    </div>
    """


def _normalize_enum_value(value):
    return getattr(value, "value", value)


def _format_arm_rotation_summary(angles):
    by_side = {_normalize_enum_value(arm.get("side")): arm for arm in angles}
    left = by_side.get("left")
    right = by_side.get("right")

    parts = []
    if left:
        parts.append(
            "左臂 "
            f"腕{_format_rotation(left.get('wrist_rotation'))} "
            f"肘{_fmt_whole(left.get('elbow_flex_deg'))}° "
            f"臂{_format_rotation(left.get('arm_rotation'))} "
            f"肩{_format_rotation(left.get('shoulder_rotation'))}"
        )
    if right:
        parts.append(
            "右臂 "
            f"腕{_format_rotation(right.get('wrist_rotation'))} "
            f"肘{_fmt_whole(right.get('elbow_flex_deg'))}° "
            f"臂{_format_rotation(right.get('arm_rotation'))} "
            f"肩{_format_rotation(right.get('shoulder_rotation'))}"
        )

    return " | ".join(parts) if parts else "等待推理结果"


def _format_motor_6_summary(command, angles=None):
    if not isinstance(command, dict):
        return ""

    reason = str(command.get("reason") or "-")
    direction = _direction_label(command.get("direction"), "horizontal")
    rotation_side = _rotation_side_label(command.get("rotation_side"))
    roll = _fmt_whole(command.get("roll_deg"))

    if not command.get("valid"):
        lines = [f"6号电机 保持 | 右手未有效识别 | reason={reason}"]
    elif not command.get("motion_allowed"):
        lines = [f"6号电机 保持 | {rotation_side}/{direction} {roll}° | reason={reason}"]
    else:
        lines = [f"6号电机 发送 | {rotation_side}/{direction} {roll}° | 掌面+拇指平面"]

    pitch_deg = command.get("wrist_pitch_deg")
    if pitch_deg is not None:
        motor_5_text = command.get("motor_5_direction_text", "保持")
        lines.append(f"5号电机 {motor_5_text} {_fmt_whole(pitch_deg)}°")

    # Motor 3 & Motor 4：肘部角度
    elbow_flex = None
    elbow_abduction = None
    if angles and isinstance(angles, list):
        for arm in angles:
            if isinstance(arm, dict) and arm.get("side") == "right":
                elbow_flex = arm.get("elbow_flex_deg")
                elbow_abduction = arm.get("elbow_abduction_deg")
                break

    # 1号电机：肩部上下（shoulder_flex_deg） & 2号电机：肩部左右（shoulder_abduction_deg）
    shoulder_flex = None
    shoulder_abduction = None
    if angles and isinstance(angles, list):
        for arm in angles:
            if isinstance(arm, dict) and arm.get("side") == "right":
                shoulder_flex = arm.get("shoulder_flex_deg")
                shoulder_abduction = arm.get("shoulder_abduction_deg")
                break

    # 1号电机：肩部上下翻转
    sflex_dir = "保持"
    if shoulder_flex is not None:
        if shoulder_flex > 3.0:
            sflex_dir = "上抬"
        elif shoulder_flex < -3.0:
            sflex_dir = "下垂"
        else:
            sflex_dir = "水平"
    lines.append(f"1号电机 {sflex_dir} | 俯仰 {_fmt_whole(shoulder_flex)}°")

    # 2号电机：肩部左右翻转
    sabd_dir = "保持"
    if shoulder_abduction is not None:
        if shoulder_abduction > 3.0:
            sabd_dir = "右偏"
        elif shoulder_abduction < -3.0:
            sabd_dir = "左偏"
        else:
            sabd_dir = "居中"
    lines.append(f"2号电机 {sabd_dir} | 偏转 {_fmt_whole(shoulder_abduction)}°")

    # 3号电机：肘部上下（elbow_flex_deg）
    flex_dir = "保持"
    if elbow_flex is not None:
        if elbow_flex > 3.0:
            flex_dir = "上抬"
        elif elbow_flex < -3.0:
            flex_dir = "下垂"
        else:
            flex_dir = "水平"
    lines.append(f"3号电机 {flex_dir} | 俯仰 {_fmt_whole(elbow_flex)}°")

    # 4号电机：肘部左右翻转（elbow_abduction_deg）
    abd_dir = "保持"
    if elbow_abduction is not None:
        if elbow_abduction > 3.0:
            abd_dir = "右偏"
        elif elbow_abduction < -3.0:
            abd_dir = "左偏"
        else:
            abd_dir = "中位"
    lines.append(f"4号电机 {abd_dir} | 偏转 {_fmt_whole(elbow_abduction)}°")

    return " | ".join(lines)


def _format_arm_rotation_html(angles):
    by_side = {_normalize_enum_value(arm.get("side")): arm for arm in angles}
    left = by_side.get("left", {})
    right = by_side.get("right", {})
    return f"""
    <table width="100%" cellspacing="0" cellpadding="0"
           style="color:#202124; font-size:15px;">
        <tr>
            {_arm_column_html("左臂关节旋转角度", left)}
            <td style="width:14px;"></td>
            {_arm_column_html("右臂关节旋转角度", right)}
        </tr>
    </table>
    """


def _arm_column_html(title, arm):
    return f"""
    <td width="50%" valign="top"
        style="padding:8px 10px; border:1px solid #D6D6D6; background:#FFFFFF;">
        <div style="font-weight:700; color:#202124; margin-bottom:8px;">{title}</div>
        {_joint_row_html("腕", arm.get("wrist_rotation"))}
        {_elbow_row_html(arm)}
        {_shoulder_row_html(arm)}
        {_joint_row_html("肩", arm.get("shoulder_rotation"))}
    </td>
    """


def _joint_row_html(label, rotation):
    return f"""
    <div style="white-space:nowrap; line-height:1.55;">
        <span style="display:inline-block; width:24px; color:#3F6FB8;">{label}：</span>
        <span>{_format_rotation(rotation)}</span>
    </div>
    """


def _elbow_row_html(arm):
    """渲染肘部角度行（elbow_flex_deg 上下 + elbow_abduction_deg 左右翻转）。"""
    flex = arm.get("elbow_flex_deg")
    abduction = arm.get("elbow_abduction_deg")
    flext = _fmt_whole(flex)
    abdt = _fmt_whole(abduction)
    abd_dir = ""
    if abduction is not None:
        if abduction > 3:
            abd_dir = "右偏"
        elif abduction < -3:
            abd_dir = "左偏"
        else:
            abd_dir = "中位"
    abd_text = f"{abd_dir} {abdt}°" if abd_dir else "-"
    return f"""
    <div style="white-space:nowrap; line-height:1.55;">
        <span style="display:inline-block; width:24px; color:#C26B2E;">肘：</span>
        <span>俯 {flext}°  偏 {abd_text}</span>
    </div>
    """


def _shoulder_row_html(arm):
    """渲染肩部角度行（shoulder_flex_deg 上下 + shoulder_abduction_deg 左右翻转）。"""
    flex = arm.get("shoulder_flex_deg")
    abduction = arm.get("shoulder_abduction_deg")
    flext = _fmt_whole(flex)
    abdt = _fmt_whole(abduction)
    flex_dir = ""
    if flex is not None:
        if flex > 3:
            flex_dir = "上抬"
        elif flex < -3:
            flex_dir = "下垂"
        else:
            flex_dir = "水平"
    flex_text = f"{flex_dir} {flext}°" if flex_dir else "-"
    abd_dir = ""
    if abduction is not None:
        if abduction > 3:
            abd_dir = "右偏"
        elif abduction < -3:
            abd_dir = "左偏"
        else:
            abd_dir = "居中"
    abd_text = f"{abd_dir} {abdt}°" if abd_dir else "-"
    return f"""
    <div style="white-space:nowrap; line-height:1.55;">
        <span style="display:inline-block; width:24px; color:#3F6FB8;">臂：</span>
        <span>俯 {flex_text}  偏 {abd_text}</span>
    </div>
    """


def _format_rotation(rotation):
    if not rotation:
        return "下 - 左 -"
    vertical = _direction_label(rotation.get("vertical"), "vertical")
    horizontal = _direction_label(rotation.get("horizontal"), "horizontal")
    return (
        f"{vertical} {_fmt_whole(rotation.get('vertical_deg'))} "
        f"{horizontal} {_fmt_whole(rotation.get('horizontal_deg'))}"
    )


def _fmt_whole(value):
    if value is None:
        return "-"
    try:
        return str(int(round(float(value))))
    except Exception:
        return "-"


def _direction_label(value, axis):
    value = getattr(value, "value", value)
    labels = {
        "up": "上",
        "down": "下",
        "left": "左",
        "right": "右",
    }
    if value in labels:
        return labels[value]
    return "下" if axis == "vertical" else "左"


def _rotation_side_label(value):
    value = getattr(value, "value", value)
    labels = {
        "inward": "向内",
        "outward": "向外",
        "neutral": "中位",
    }
    return labels.get(value, "未知")


def _side_label(value):
    value = getattr(value, "value", value)
    labels = {
        "left": "左臂",
        "right": "右臂",
    }
    return labels.get(value, "手臂")
