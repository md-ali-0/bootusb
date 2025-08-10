#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>
#include <QApplication>
#include <QThread>
#include <QLineEdit>
#include <QGroupBox>
#include <QFont>
#include <QFontMetrics>
#include <QScreen>
#include <QTabWidget>
#include <QTextEdit>
#include <QFile>
#include <QDateTime>
#include <QIcon>

#include "gui.h"
#include "usb_detect.h"
#include "format.h"
#include "write_iso.h"
#include "bootloader.h"

#include <functional>

void WorkerThread::run() {
    if (!job) return;
    job(progressCb);
}

MainWindow::MainWindow() : isRunning(false), progressValue(0) {
    setupUI();
    setupStyles();
    populateDeviceList();
    
    // Progress timer for simulation
    progressTimer = new QTimer(this);
    connect(progressTimer, &QTimer::timeout, this, &MainWindow::onProgressUpdate);
    
    updateStatus("Ready");
}

void MainWindow::setupUI() {
    setWindowTitle("BootUSB");
    setFixedSize(520, 720); // Increased size to prevent overlapping
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    
    // Set application icon
    setWindowIcon(QIcon::fromTheme("drive-removable-media", QIcon(":/icons/usb")));
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20); // Reduced spacing to fit more content./
    mainLayout->setContentsMargins(25, 25, 25, 25); // Adjusted margins
    
    // Tab Widget with modern styling
    tabWidget = new QTabWidget();
    mainTab = new QWidget();
    advancedTab = new QWidget();
    logsTab = new QWidget();
    
    // Setup tabs
    setupMainTab();
    setupAdvancedTab();
    setupLogsTab();
    
    // Add tabs with icons
    tabWidget->addTab(mainTab, QIcon::fromTheme("applications-system"), "Main");
    tabWidget->addTab(advancedTab, QIcon::fromTheme("preferences-system"), "Advanced");
    tabWidget->addTab(logsTab, QIcon::fromTheme("text-x-generic"), "Logs");
    
    mainLayout->addWidget(tabWidget);
    
    // Connect signals
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshDevices);
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowseISO);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStart);
}

void MainWindow::setupMainTab() {
    auto *layout = new QVBoxLayout(mainTab);
    layout->setSpacing(16); // Reduced spacing to fit all elements
    layout->setContentsMargins(20, 20, 20, 20); // Adjusted margins
    
    // Device Selection - Clean and simple
    auto *deviceGroup = new QGroupBox("Device");
    deviceGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *deviceLayout = new QHBoxLayout(deviceGroup);
    deviceLayout->setSpacing(10);
    
    deviceCombo = new QComboBox();
    deviceCombo->addItem("Select a device");
    deviceCombo->setMinimumHeight(36); // Reduced height
    deviceCombo->setStyleSheet("QComboBox { font-size: 10pt; }");
    
    refreshBtn = new QPushButton();
    refreshBtn->setIcon(QIcon::fromTheme("view-refresh"));
    refreshBtn->setFixedSize(36, 36); // Reduced size
    refreshBtn->setToolTip("Refresh devices");
    refreshBtn->setStyleSheet("QPushButton { border-radius: 6px; }");
    
    deviceLayout->addWidget(deviceCombo);
    deviceLayout->addWidget(refreshBtn);
    layout->addWidget(deviceGroup);
    
    // ISO Selection - Clean and simple
    auto *isoGroup = new QGroupBox("Boot selection");
    isoGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *isoLayout = new QHBoxLayout(isoGroup);
    isoLayout->setSpacing(10);
    
    isoPathEdit = new QLineEdit();
    isoPathEdit->setText("No ISO selected");
    isoPathEdit->setReadOnly(true);
    isoPathEdit->setMinimumHeight(36); // Reduced height
    isoPathEdit->setStyleSheet("QLineEdit { font-size: 10pt; color: #666; }");
    
    browseBtn = new QPushButton("Browse");
    browseBtn->setIcon(QIcon::fromTheme("folder-open"));
    browseBtn->setMinimumHeight(36); // Reduced height
    browseBtn->setStyleSheet("QPushButton { font-size: 10pt; font-weight: bold; }");
    
    isoLayout->addWidget(isoPathEdit);
    isoLayout->addWidget(browseBtn);
    layout->addWidget(isoGroup);
    
    // File System & Target System - Side by side like in image
    auto *formatGroup = new QGroupBox("File system & Target system");
    formatGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *formatLayout = new QHBoxLayout(formatGroup);
    formatLayout->setSpacing(20); // Reduced spacing
    formatLayout->setContentsMargins(12, 16, 12, 16); // Reduced padding
    
    // Left side - File system
    auto *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(6); // Reduced spacing
    auto *fsLabel = new QLabel("File system:");
    fsLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 10pt; color: #333; }");
    leftLayout->addWidget(fsLabel);
    
    filesystemCombo = new QComboBox();
    filesystemCombo->addItems({"FAT32", "NTFS", "ext4"});
    filesystemCombo->setMinimumHeight(32); // Reduced height
    filesystemCombo->setMinimumWidth(130); // Reduced width
    filesystemCombo->setStyleSheet("QComboBox { font-size: 10pt; }");
    leftLayout->addWidget(filesystemCombo);
    leftLayout->addStretch();
    
    // Right side - Target system
    auto *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(6); // Reduced spacing
    auto *targetLabel = new QLabel("Target system:");
    targetLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 10pt; color: #333; }");
    rightLayout->addWidget(targetLabel);
    
    bootloaderCombo = new QComboBox();
    bootloaderCombo->addItems({"Syslinux", "GRUB", "None"});
    bootloaderCombo->setMinimumHeight(32); // Reduced height
    bootloaderCombo->setMinimumWidth(130); // Reduced width
    bootloaderCombo->setStyleSheet("QComboBox { font-size: 10pt; }");
    rightLayout->addWidget(bootloaderCombo);
    rightLayout->addStretch();
    
    formatLayout->addLayout(leftLayout);
    formatLayout->addLayout(rightLayout);
    layout->addWidget(formatGroup);
    
    // Partition Scheme & Volume Label - Side by side
    auto *partitionGroup = new QGroupBox("Partition Scheme & Volume Label");
    partitionGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *partitionLayout = new QHBoxLayout(partitionGroup);
    partitionLayout->setSpacing(20); // Reduced spacing
    partitionLayout->setContentsMargins(12, 16, 12, 16); // Reduced padding
    
    // Left side - Partition Scheme
    auto *partitionLeftLayout = new QVBoxLayout();
    partitionLeftLayout->setSpacing(6); // Reduced spacing
    auto *partitionLabel = new QLabel("Partition Scheme:");
    partitionLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 10pt; color: #333; }");
    partitionLeftLayout->addWidget(partitionLabel);
    
    partitionSchemeCombo = new QComboBox();
    partitionSchemeCombo->addItems({"MBR", "GPT"});
    partitionSchemeCombo->setMinimumHeight(32); // Reduced height
    partitionSchemeCombo->setMinimumWidth(130); // Reduced width
    partitionSchemeCombo->setStyleSheet("QComboBox { font-size: 10pt; }");
    partitionLeftLayout->addWidget(partitionSchemeCombo);
    partitionLeftLayout->addStretch();
    
    // Right side - Volume Label
    auto *partitionRightLayout = new QVBoxLayout();
    partitionRightLayout->setSpacing(6); // Reduced spacing
    auto *volumeLabel = new QLabel("Volume Label:");
    volumeLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 10pt; color: #333; }");
    partitionRightLayout->addWidget(volumeLabel);
    
    volumeLabelEdit = new QLineEdit();
    volumeLabelEdit->setPlaceholderText("Enter volume label");
    volumeLabelEdit->setMinimumHeight(32); // Reduced height
    volumeLabelEdit->setMinimumWidth(130); // Reduced width
    volumeLabelEdit->setStyleSheet("QLineEdit { font-size: 10pt; }");
    partitionRightLayout->addWidget(volumeLabelEdit);
    partitionRightLayout->addStretch();
    
    partitionLayout->addLayout(partitionLeftLayout);
    partitionLayout->addLayout(partitionRightLayout);
    layout->addWidget(partitionGroup);
    
    // Start Button - Prominent and clean
    startBtn = new QPushButton("START");
    startBtn->setIcon(QIcon::fromTheme("media-playback-start"));
    startBtn->setMinimumHeight(48); // Reduced height
    startBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #0078d4;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 13pt;
            font-weight: bold;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: #106ebe;
        }
        QPushButton:pressed {
            background-color: #005a9e;
        }
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
    )");
    layout->addWidget(startBtn);
    
    // Status Section - Clean and simple
    auto *statusGroup = new QGroupBox("Status");
    statusGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *statusLayout = new QVBoxLayout(statusGroup);
    statusLayout->setSpacing(8); // Reduced spacing
    
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setTextVisible(true);
    progressBar->setMinimumHeight(28); // Reduced height
    progressBar->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            text-align: center;
            background-color: #f8f8f8;
            font-size: 10pt;
        }
        QProgressBar::chunk {
            background-color: #0078d4;
            border-radius: 4px;
        }
    )");
    
    statusLabel = new QLabel("Ready");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("QLabel { font-size: 10pt; color: #666; }");
    
    statusLayout->addWidget(progressBar);
    statusLayout->addWidget(statusLabel);
    layout->addWidget(statusGroup);
}

void MainWindow::setupAdvancedTab() {
    auto *layout = new QVBoxLayout(advancedTab);
    layout->setSpacing(20); // Reduced spacing
    layout->setContentsMargins(20, 20, 20, 20); // Adjusted margins
    
    // Cluster Size
    auto *clusterGroup = new QGroupBox("Cluster Size");
    clusterGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *clusterLayout = new QHBoxLayout(clusterGroup);
    clusterLayout->setSpacing(10);
    
    clusterSizeCombo = new QComboBox();
    clusterSizeCombo->addItems({"Default", "512 bytes", "1 KB", "2 KB", "4 KB", "8 KB", "16 KB", "32 KB", "64 KB"});
    clusterSizeCombo->setMinimumHeight(36); // Reduced height
    clusterSizeCombo->setMinimumWidth(150); // Reduced width
    clusterSizeCombo->setStyleSheet("QComboBox { font-size: 10pt; }");
    clusterLayout->addWidget(clusterSizeCombo);
    clusterLayout->addStretch();
    layout->addWidget(clusterGroup);
    
    // Quick Format / Full Format
    auto *formatTypeGroup = new QGroupBox("Format Type");
    formatTypeGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *formatTypeLayout = new QHBoxLayout(formatTypeGroup);
    formatTypeLayout->setSpacing(20); // Reduced spacing
    
    quickFormatRadio = new QRadioButton("Quick Format");
    quickFormatRadio->setChecked(true);
    quickFormatRadio->setStyleSheet("QRadioButton { font-size: 10pt; }");
    
    fullFormatRadio = new QRadioButton("Full Format");
    fullFormatRadio->setStyleSheet("QRadioButton { font-size: 10pt; }");
    
    formatTypeLayout->addWidget(quickFormatRadio);
    formatTypeLayout->addWidget(fullFormatRadio);
    formatTypeLayout->addStretch();
    layout->addWidget(formatTypeGroup);
    
    // Check device for bad blocks
    auto *badBlocksGroup = new QGroupBox("Check device for bad blocks");
    badBlocksGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *badBlocksLayout = new QHBoxLayout(badBlocksGroup);
    badBlocksLayout->setSpacing(10);
    
    checkBadBlocksCheck = new QCheckBox("Enable bad block checking");
    checkBadBlocksCheck->setToolTip("Check for bad blocks before formatting (slower but safer)");
    checkBadBlocksCheck->setStyleSheet("QCheckBox { font-size: 10pt; }");
    badBlocksLayout->addWidget(checkBadBlocksCheck);
    badBlocksLayout->addStretch();
    layout->addWidget(badBlocksGroup);
    
    // Persistent Storage (Linux ISOs)
    auto *persistentGroup = new QGroupBox("Persistent Storage (Linux ISOs)");
    persistentGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    auto *persistentLayout = new QVBoxLayout(persistentGroup);
    persistentLayout->setSpacing(12); // Reduced spacing
    
    persistentCheck = new QCheckBox("Enable persistent storage");
    persistentCheck->setToolTip("Create persistent storage for Linux live USB (saves changes between boots)");
    persistentCheck->setStyleSheet("QCheckBox { font-size: 10pt; }");
    persistentLayout->addWidget(persistentCheck);
    
    auto *persistentSizeLayout = new QHBoxLayout();
    persistentSizeLayout->setSpacing(10);
    auto *sizeLabel = new QLabel("Persistent storage size:");
    sizeLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 10pt; color: #333; }");
    persistentSizeLayout->addWidget(sizeLabel);
    
    persistentSizeCombo = new QComboBox();
    persistentSizeCombo->addItems({"1 GB", "2 GB", "4 GB", "8 GB", "16 GB"});
    persistentSizeCombo->setMinimumHeight(32); // Reduced height
    persistentSizeCombo->setMinimumWidth(100); // Reduced width
    persistentSizeCombo->setStyleSheet("QComboBox { font-size: 10pt; }");
    persistentSizeLayout->addWidget(persistentSizeCombo);
    persistentSizeLayout->addStretch();
    persistentLayout->addLayout(persistentSizeLayout);
    
    layout->addWidget(persistentGroup);
    layout->addStretch();
}

void MainWindow::setupLogsTab() {
    auto *layout = new QVBoxLayout(logsTab);
    layout->setSpacing(20); // Reduced spacing
    layout->setContentsMargins(20, 20, 20, 20); // Adjusted margins
    
    logText = new QTextEdit();
    logText->setReadOnly(true);
    logText->setPlaceholderText("Operation logs will appear here...");
    logText->setMinimumHeight(350); // Reduced height
    logText->setStyleSheet(R"(
        QTextEdit {
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            background-color: white;
            color: #333;
            font-family: 'Consolas', 'Monaco', 'Courier New', monospace;
            font-size: 9pt;
            padding: 8px;
        }
    )");
    
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    auto *clearBtn = new QPushButton("Clear Logs");
    clearBtn->setIcon(QIcon::fromTheme("edit-clear"));
    clearBtn->setMinimumHeight(36); // Reduced height
    clearBtn->setStyleSheet("QPushButton { font-size: 10pt; font-weight: bold; }");
    
    auto *saveBtn = new QPushButton("Save Logs");
    saveBtn->setIcon(QIcon::fromTheme("document-save"));
    saveBtn->setMinimumHeight(36); // Reduced height
    saveBtn->setStyleSheet("QPushButton { font-size: 10pt; font-weight: bold; }");
    
    buttonLayout->addWidget(clearBtn);
    buttonLayout->addWidget(saveBtn);
    buttonLayout->addStretch();
    
    layout->addWidget(logText);
    layout->addLayout(buttonLayout);
    
    connect(clearBtn, &QPushButton::clicked, [this]() {
        logText->clear();
    });
    
    connect(saveBtn, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Logs", "", "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(logText->toPlainText().toUtf8());
                file.close();
            }
        }
    });
}

void MainWindow::setupStyles() {
    setStyleSheet(R"(
        QWidget {
            font-family: 'Segoe UI', 'Helvetica Neue', Arial, sans-serif;
            font-size: 9pt;
            background-color: #ffffff;
        }
        
        QTabWidget::pane {
            border: 1px solid #e0e0e0;
            background-color: white;
            border-radius: 8px;
        }
        
        QTabBar::tab {
            background-color: #f8f8f8;
            border: 1px solid #e0e0e0;
            padding: 10px 20px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            font-weight: bold;
            font-size: 10pt;
        }
        
        QTabBar::tab:selected {
            background-color: white;
            border-bottom-color: white;
            color: #0078d4;
        }
        
        QTabBar::tab:hover {
            background-color: #f0f0f0;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 12px;
            background-color: #fafafa;
            font-size: 11pt;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 8px 0 8px;
            color: #333333;
        }
        
        QPushButton {
            background-color: #f0f0f0;
            border: 2px solid #d0d0d0;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            font-size: 10pt;
        }
        
        QPushButton:hover {
            background-color: #e0e0e0;
            border-color: #0078d4;
        }
        
        QPushButton:pressed {
            background-color: #d0d0d0;
        }
        
        QPushButton:disabled {
            background-color: #f8f8f8;
            color: #888888;
            border-color: #e0e0e0;
        }
        
        QComboBox {
            border: 2px solid #d0d0d0;
            border-radius: 6px;
            padding: 8px 12px;
            background-color: white;
            min-height: 20px;
            font-size: 10pt;
        }
        
        QComboBox:hover {
            border-color: #0078d4;
        }
        
        QComboBox:focus {
            border-color: #0078d4;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #666666;
        }
        
        QLineEdit {
            border: 2px solid #d0d0d0;
            border-radius: 6px;
            padding: 8px 12px;
            background-color: white;
            font-size: 10pt;
        }
        
        QLineEdit:focus {
            border-color: #0078d4;
        }
        
        QProgressBar {
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            text-align: center;
            background-color: #f8f8f8;
            font-size: 10pt;
        }
        
        QProgressBar::chunk {
            background-color: #0078d4;
            border-radius: 4px;
        }
        
        QLabel {
            color: #333333;
        }
        
        QTextEdit {
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            background-color: white;
            color: #333333;
            font-family: 'Consolas', 'Monaco', 'Courier New', monospace;
            font-size: 9pt;
            padding: 8px;
        }
        
        QRadioButton {
            spacing: 8px;
            color: #333333;
            font-size: 10pt;
        }
        
        QRadioButton::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #d0d0d0;
            border-radius: 9px;
            background-color: white;
        }
        
        QRadioButton::indicator:checked {
            border-color: #0078d4;
            background-color: #0078d4;
        }
        
        QRadioButton::indicator:checked::after {
            width: 6px;
            height: 6px;
            border-radius: 3px;
            background-color: white;
            margin: 4px;
        }
        
        QCheckBox {
            spacing: 8px;
            color: #333333;
            font-size: 10pt;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #d0d0d0;
            border-radius: 4px;
            background-color: white;
        }
        
        QCheckBox::indicator:checked {
            border-color: #0078d4;
            background-color: #0078d4;
        }
        
        QCheckBox::indicator:checked::after {
            color: white;
            font-weight: bold;
            font-size: 12pt;
        }
    )");
}

void MainWindow::populateDeviceList() {
    deviceCombo->clear();
    deviceCombo->addItem("Select a device");
    
    auto devices = list_usb_devices();
    for (const auto &d : devices) {
        QString label = QString::fromStdString(d.devnode + " - " + d.model + " (" + d.size + ")");
        deviceCombo->addItem(label, QString::fromStdString(d.devnode));
    }
    
    if (devices.empty()) {
        deviceCombo->addItem("No USB devices found", "");
    }
}

void MainWindow::onRefreshDevices() {
    populateDeviceList();
    updateStatus("Device list refreshed");
}

void MainWindow::onBrowseISO() {
    QString file = QFileDialog::getOpenFileName(this, "Select ISO File", QString(), 
                                               "ISO Files (*.iso);;All Files (*)");
    if (!file.isEmpty()) {
        selectedIsoPath = file;
        isoPathEdit->setText(file);
        updateStatus("ISO file selected");
    }
}

void MainWindow::onStart() {
    if (deviceCombo->currentText() == "Select a device" || deviceCombo->currentData().toString().isEmpty()) {
        QMessageBox::warning(this, "No Device", "Please select a USB device first.");
        return;
    }
    
    if (selectedIsoPath.isEmpty() || isoPathEdit->text() == "No ISO selected") {
        QMessageBox::warning(this, "No ISO", "Please select an ISO file first.");
        return;
    }
    
    QString devicePath = deviceCombo->currentData().toString();
    auto reply = QMessageBox::question(this, "Confirm Operation", 
                                      QString("This will overwrite %1.\n\nAre you sure?").arg(devicePath),
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;
    
    // Start operation
    startBtn->setEnabled(false);
    isRunning = true;
    progressValue = 0;
    progressBar->setValue(0);
    
    updateStatus("Starting operation...");
    
    // Create worker thread
    workerThread = new WorkerThread();
    workerThread->job = [this, devicePath, isoPath = selectedIsoPath.toStdString(), 
                        fs = filesystemCombo->currentText().toStdString(),
                        bootloader = bootloaderCombo->currentText().toStdString(),
                        partitionScheme = partitionSchemeCombo->currentText().toStdString(),
                        volumeLabel = volumeLabelEdit->text().toStdString(),
                        clusterSize = clusterSizeCombo->currentText().toStdString(),
                        quickFormat = quickFormatRadio->isChecked(),
                        checkBadBlocks = checkBadBlocksCheck->isChecked(),
                        persistent = persistentCheck->isChecked(),
                        persistentSize = persistentSizeCombo->currentText().toStdString()](std::function<void(size_t,size_t)> progressFunc) {
        
        updateStatus("Formatting device...");
        
        // Format device with new options
        bool formatted = format_usb(devicePath.toStdString(), fs);
        if (!formatted) {
            updateStatus("Format failed, attempting raw write");
        } else {
            updateStatus("Format completed, writing ISO...");
        }
        
        // Write ISO
        bool writeOk = write_iso_to_usb(isoPath, devicePath.toStdString(), progressFunc);
        if (!writeOk) {
            updateStatus("Write operation failed");
            return;
        }
        
        updateStatus("Write completed, installing bootloader...");
        
        // Install bootloader
        if (bootloader == "Syslinux") {
            install_syslinux(devicePath.toStdString());
        } else if (bootloader == "GRUB") {
            // Mount and install GRUB
            std::string mountPoint = "/tmp/bootusb_mount";
            std::string cmd = "sudo mkdir -p " + mountPoint + " && sudo mount " + devicePath.toStdString() + "1 " + mountPoint;
            system(cmd.c_str());
            
            cmd = "sudo grub-install --target=i386-pc --boot-directory=" + mountPoint + "/boot " + devicePath.toStdString();
            system(cmd.c_str());
            
            system(("sudo umount " + mountPoint).c_str());
        }
        
        updateStatus("Operation completed successfully");
    };
    
    workerThread->progressCb = [this](size_t written, size_t total) {
        int percentage = total > 0 ? int((100.0 * written) / total) : 0;
        progressValue = percentage;
        progressBar->setValue(percentage);
        
        if (percentage == 100) {
            onOperationComplete();
        }
    };
    
    connect(workerThread, &QThread::finished, [this]() {
        workerThread->deleteLater();
    });
    
    workerThread->start();
    
    // Start progress simulation timer
    progressTimer->start(100);
}

void MainWindow::onProgressUpdate() {
    if (isRunning && progressValue < 100) {
        // Simulate progress for better UX
        int currentProgress = progressBar->value();
        if (currentProgress < progressValue) {
            progressBar->setValue(currentProgress + 1);
        }
    }
}

void MainWindow::onOperationComplete() {
    isRunning = false;
    startBtn->setEnabled(true);
    progressTimer->stop();
    progressBar->setValue(100);
    
    updateStatus("Operation completed successfully!");
    QMessageBox::information(this, "Success", "Bootable USB created successfully!");
}

void MainWindow::updateStatus(const QString &message) {
    statusLabel->setText(message);
    
    // Add to logs
    if (logText) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        QString logEntry = QString("[%1] %2").arg(timestamp, message);
        logText->append(logEntry);
    }
    
    qApp->processEvents(); // Update UI immediately
}

void MainWindow::simulateProgress() {
    // This function is kept for compatibility but not used in the new design
}