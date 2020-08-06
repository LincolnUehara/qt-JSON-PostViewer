#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void replyFinished(QNetworkReply * reply);
    void syncUsers(void);
    void checkPrevious(void);
    void checkNext(void);
    void checkSelection(void);

private:
    bool connected;
    int userCounter;
    int userId;
    int postCounter;

    void enablePostFields(void);
    void disableAllFields(void);
    void sendNetworkRequest(const char * urlString);

    Ui::MainWindow *ui;
    QUrl url;
    QNetworkRequest networkRequest;
    QNetworkAccessManager * networkManager;
    QTimer * timer;
    QStringListModel * model;
    QStringList list;

    enum eRunningState {
        _RS_IDLE,
        _RS_SYNC_USERS,
        _RS_CHECK_PREVIOUS,
        _RS_CHECK_NEXT
    }runningState;
};

#endif // MAINWINDOW_H
