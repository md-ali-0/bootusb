#ifndef GUI_H
#define GUI_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QThread>
#include <QTimer>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <functional>

class WorkerThread : public QThread {
    Q_OBJECT
public:
    std::function<void(std::function<void(size_t,size_t)>)> job;
    std::function<void(size_t,size_t)> progressCb;

    void run() override;
};

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow();

private slots:
    void onRefreshDevices();
    void onBrowseISO();
    void onStart();
    void onProgressUpdate();
    void onOperationComplete();

private:
    void setupUI();
    void setupStyles();
    void setupMainTab();
    void setupAdvancedTab();
    void setupLogsTab();
    void populateDeviceList();
    void updateStatus(const QString &message);
    void simulateProgress();
    
    // Tab Widget
    QTabWidget *tabWidget;
    QWidget *mainTab;
    QWidget *advancedTab;
    QWidget *logsTab;
    
    // UI Components - Main Tab
    QComboBox *deviceCombo;
    QPushButton *refreshBtn;
    
    QLineEdit *isoPathEdit;
    QPushButton *browseBtn;
    
    QComboBox *filesystemCombo;
    QComboBox *bootloaderCombo;
    
    // New components for Partition Scheme & Volume Label
    QComboBox *partitionSchemeCombo;
    QLineEdit *volumeLabelEdit;
    
    QPushButton *startBtn;
    QProgressBar *progressBar;
    QLabel *statusLabel;
    
    // UI Components - Advanced Tab
    QComboBox *clusterSizeCombo;
    QRadioButton *quickFormatRadio;
    QRadioButton *fullFormatRadio;
    QCheckBox *checkBadBlocksCheck;
    QCheckBox *persistentCheck;
    QComboBox *persistentSizeCombo;
    
    // Logs
    QTextEdit *logText;
    
    // State
    QString selectedIsoPath;
    bool isRunning;
    QTimer *progressTimer;
    int progressValue;
    
    // Worker
    WorkerThread *workerThread;
};

#endif // GUI_H 