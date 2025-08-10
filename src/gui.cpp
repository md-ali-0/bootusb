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
    setFixedSize(400, 520);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    auto *titleLabel = new QLabel("BootUSB");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // Tab Widget
    tabWidget = new QTabWidget();
    mainTab = new QWidget();
    advancedTab = new QWidget();
    logsTab = new QWidget();
    
    // Setup tabs
    setupMainTab();
    setupAdvancedTab();
    setupLogsTab();
    
    // Add tabs
    tabWidget->addTab(mainTab, "Main");
    tabWidget->addTab(advancedTab, "Advanced");
    tabWidget->addTab(logsTab, "Logs");
    
    mainLayout->addWidget(tabWidget);
    
    // Connect signals
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshDevices);
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowseISO);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStart);
}

void MainWindow::setupMainTab() {
    auto *layout = new QVBoxLayout(mainTab);
    layout->setSpacing(16);
    layout->setContentsMargins(16, 16, 16, 16);
    
    // Device Selection
    auto *deviceGroup = new QGroupBox("Device");
    auto *deviceLayout = new QHBoxLayout(deviceGroup);
    deviceLayout->setSpacing(8);
    
    deviceCombo = new QComboBox();
    deviceCombo->setMinimumHeight(36);
    refreshBtn = new QPushButton("🔄");
    refreshBtn->setFixedSize(36, 36);
    refreshBtn->setToolTip("Refresh devices");
    
    deviceLayout->addWidget(deviceCombo);
    deviceLayout->addWidget(refreshBtn);
    layout->addWidget(deviceGroup);
    
    // ISO Selection
    auto *isoGroup = new QGroupBox("Boot selection");
    auto *isoLayout = new QHBoxLayout(isoGroup);
    isoLayout->setSpacing(8);
    
    isoPathEdit = new QLineEdit();
    isoPathEdit->setPlaceholderText("Select an ISO file");
    isoPathEdit->setReadOnly(true);
    isoPathEdit->setMinimumHeight(36);
    browseBtn = new QPushButton("Browse");
    browseBtn->setMinimumHeight(36);
    
    isoLayout->addWidget(isoPathEdit);
    isoLayout->addWidget(browseBtn);
    layout->addWidget(isoGroup);
    
    // Format Options - Side by side
    auto *formatGroup = new QGroupBox("Format options");
    auto *formatLayout = new QHBoxLayout(formatGroup);
    formatLayout->setSpacing(16);
    
    // Left side - Filesystem
    auto *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("File system:"));
    filesystemCombo = new QComboBox();
    filesystemCombo->addItems({"FAT32", "NTFS", "ext4"});
    filesystemCombo->setMinimumHeight(32);
    leftLayout->addWidget(filesystemCombo);
    leftLayout->addStretch();
    
    // Right side - Bootloader
    auto *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel("Bootloader:"));
    bootloaderCombo = new QComboBox();
    bootloaderCombo->addItems({"Syslinux", "GRUB", "None"});
    bootloaderCombo->setMinimumHeight(32);
    rightLayout->addWidget(bootloaderCombo);
    rightLayout->addStretch();
    
    formatLayout->addLayout(leftLayout);
    formatLayout->addLayout(rightLayout);
    layout->addWidget(formatGroup);
    
    // Start Button
    startBtn = new QPushButton("START");
    startBtn->setMinimumHeight(48);
    QFont startFont = startBtn->font();
    startFont.setPointSize(12);
    startFont.setBold(true);
    startBtn->setFont(startFont);
    layout->addWidget(startBtn);
    
    // Progress Section
    auto *progressGroup = new QGroupBox("Progress");
    auto *progressLayout = new QVBoxLayout(progressGroup);
    progressLayout->setSpacing(8);
    
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setTextVisible(true);
    progressBar->setMinimumHeight(28);
    
    statusLabel = new QLabel("Ready");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    progressLayout->addWidget(progressBar);
    progressLayout->addWidget(statusLabel);
    layout->addWidget(progressGroup);
}

void MainWindow::setupAdvancedTab() {
    auto *layout = new QVBoxLayout(advancedTab);
    layout->setSpacing(16);
    layout->setContentsMargins(16, 16, 16, 16);
    
    // Advanced options will be added here
    auto *infoLabel = new QLabel("Advanced options coming soon...");
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);
    layout->addStretch();
}

void MainWindow::setupLogsTab() {
    auto *layout = new QVBoxLayout(logsTab);
    layout->setSpacing(16);
    layout->setContentsMargins(16, 16, 16, 16);
    
    logText = new QTextEdit();
    logText->setReadOnly(true);
    logText->setPlaceholderText("Operation logs will appear here...");
    
    auto *buttonLayout = new QHBoxLayout();
    auto *clearBtn = new QPushButton("Clear Logs");
    auto *saveBtn = new QPushButton("Save Logs");
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
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 9pt;
        }
        
        QTabWidget::pane {
            border: 1px solid #cccccc;
            background-color: white;
        }
        
        QTabBar::tab {
            background-color: #f0f0f0;
            border: 1px solid #cccccc;
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        
        QTabBar::tab:selected {
            background-color: white;
            border-bottom-color: white;
        }
        
        QTabBar::tab:hover {
            background-color: #e0e0e0;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 1px solid #cccccc;
            border-radius: 4px;
            margin-top: 8px;
            padding-top: 8px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px 0 4px;
        }
        
        QPushButton {
            background-color: #f0f0f0;
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #e0e0e0;
        }
        
        QPushButton:pressed {
            background-color: #d0d0d0;
        }
        
        QPushButton:disabled {
            background-color: #f8f8f8;
            color: #888888;
        }
        
        QComboBox {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 4px 8px;
            background-color: white;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 4px solid #666666;
        }
        
        QLineEdit {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 4px 8px;
            background-color: white;
        }
        
        QProgressBar {
            border: 1px solid #cccccc;
            border-radius: 4px;
            text-align: center;
            background-color: #f8f8f8;
        }
        
        QProgressBar::chunk {
            background-color: #0078d4;
            border-radius: 2px;
        }
        
        QLabel {
            color: #333333;
        }
        
        QTextEdit {
            border: 1px solid #cccccc;
            border-radius: 4px;
            background-color: white;
            color: #333333;
            font-family: 'Consolas', 'Monaco', monospace;
            font-size: 8pt;
        }
    )");
}

void MainWindow::populateDeviceList() {
    deviceCombo->clear();
    
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
    if (deviceCombo->currentData().toString().isEmpty()) {
        QMessageBox::warning(this, "No Device", "Please select a USB device first.");
        return;
    }
    
    if (selectedIsoPath.isEmpty()) {
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
                        bootloader = bootloaderCombo->currentText().toStdString()](std::function<void(size_t,size_t)> progressFunc) {
        
        updateStatus("Formatting device...");
        
        // Format device
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