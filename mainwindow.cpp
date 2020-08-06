#include "mainwindow.h"
#include "ui_mainwindow.h"

#define CONNECTION_TIMEOUT 2000 /* 2 sec */

QColor colorBasic(0,0,0), colorSuccess(0,0,255), colorWarning(255,156,0), colorError(255,0,0);

static const char userUrl[] = "https://jsonplaceholder.typicode.com/users/";
static const char postsUrl[] = "https://jsonplaceholder.typicode.com/posts/";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Fix the window size and hide maximize button */
    setFixedSize(this->geometry().width(),this->geometry().height());
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowTitle(tr("Post Viewer"));

    /* Set networking signal and slot */
    networkManager = new QNetworkAccessManager(this);
    networkManager->setTransferTimeout(CONNECTION_TIMEOUT);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    /* "Sync" Button */
    ui->pbSync->setEnabled(true);
    ui->pbSync->setToolTip(tr("Update the users field"));
    connect(ui->pbSync, SIGNAL (released()), this, SLOT (syncUsers()));

    /* "Previous" Button */
    ui->pbPrevious->setEnabled(true);
    ui->pbPrevious->setToolTip(tr("See the previous post"));
    connect(ui->pbPrevious, SIGNAL (released()), this, SLOT (checkPrevious()));

    /* "Next" Button */
    ui->pbNext->setEnabled(true);
    ui->pbNext->setToolTip(tr("See the next post"));
    connect(ui->pbNext, SIGNAL (released()), this, SLOT (checkNext()));

    /* "Users" Field */
    /* Unfortunately it was not possible to connect to a signal from List View */
    model = new QStringListModel();
    ui->lvUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkSelection()));
    timer->start(200);

    /* Set used variables */
    disableAllFields();
    connected = false;
    userId = -1; /* No user selected */
    userCounter = 0;
    postCounter = 0;
    runningState = _RS_IDLE;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::replyFinished(QNetworkReply * reply)
{
    if(reply->error() == QNetworkReply::NoError){
        if (connected == false){
            connected = true;
            ui->tbGeneralMessage->setTextColor(colorSuccess);
            ui->tbGeneralMessage->append("Connected!");
        }
        /* parse the reply JSON and display result in the UI */
        QJsonObject jsonObject = QJsonDocument::fromJson(reply->readAll()).object();
        std::string message;

        switch(runningState) {

        case _RS_SYNC_USERS :
            if (jsonObject.contains("name")) {
                list << jsonObject["name"].toString();
                model->setStringList(list);
                ui->lvUsers->setModel(model);
                userCounter++;
                syncUsers();
            }else{
                ui->tbGeneralMessage->setTextColor(colorWarning);
                ui->tbGeneralMessage->append("Error to parse JSON!");
            }
            break;

        case _RS_CHECK_NEXT :
            if (jsonObject.contains("userId")) {
                if( userId == jsonObject["userId"].toInt()){
                    ui->tbPostTitle->setText(jsonObject["title"].toString());
                    ui->tbPostBody->setText(jsonObject["body"].toString());
                }else{
                    checkNext();
                }
            }
            break;

        case _RS_CHECK_PREVIOUS :
            if (jsonObject.contains("userId")) {
                if( userId == jsonObject["userId"].toInt()){
                    ui->tbPostTitle->setText(jsonObject["title"].toString());
                    ui->tbPostBody->setText(jsonObject["body"].toString());
                }else{
                    checkPrevious();
                }
            }
            break;

        //default :
        }
    }else if(reply->error() == QNetworkReply::ContentNotFoundError){
        std::string message;

        switch(runningState) {

        case _RS_SYNC_USERS :
            ui->tbGeneralMessage->setTextColor(colorBasic);
            message = "There are a total of ";
            message += QString::number(userCounter).toStdString();
            message += " users.";
            ui->tbGeneralMessage->append(message.c_str());
            ui->lvUsers->setEnabled(true);
            userCounter = 0;
            break;

        case _RS_CHECK_NEXT :
        case _RS_CHECK_PREVIOUS :
            ui->tbGeneralMessage->setTextColor(colorWarning);
            ui->tbGeneralMessage->append("There is no more messages from this user!");
            break;

        //default :
        }
    }else{
        if (connected == true){
            connected = false;
            ui->tbGeneralMessage->setTextColor(colorError);
            ui->tbGeneralMessage->append("Error to connect!");
            disableAllFields();
        }
        networkManager->clearAccessCache();
    }
    reply->deleteLater();
    return;
}

void MainWindow::syncUsers(void)
{
    runningState = _RS_SYNC_USERS;
    std::string userFullPath = userUrl;
    userFullPath += QString::number(userCounter + 1).toStdString();
    sendNetworkRequest(userFullPath.c_str());
    return;
}

void MainWindow::checkNext(void)
{
    enablePostFields();
    runningState = _RS_CHECK_NEXT;
    postCounter++;
    std::string postFullPath;
    postFullPath = postsUrl;
    postFullPath += QString::number(postCounter + 1).toStdString();
    sendNetworkRequest(postFullPath.c_str());
    return;
}

void MainWindow::checkPrevious(void)
{
    if(postCounter == 0){
        ui->tbGeneralMessage->setTextColor(colorWarning);
        ui->tbGeneralMessage->append("This is the first message!");
        return;
    }
    runningState = _RS_CHECK_PREVIOUS;
    postCounter--;
    std::string postFullPath;
    postFullPath = postsUrl;
    postFullPath += QString::number(postCounter + 1).toStdString();
    sendNetworkRequest(postFullPath.c_str());
    return;
}

void MainWindow::checkSelection(void)
{
    if(list.isEmpty())return;
    QModelIndexList index = ui->lvUsers->selectionModel()->selectedIndexes();
    if(index.isEmpty())return; /* Any selected */
    if(userId != (index[0].row() + 1)){
        userId = index[0].row() + 1;
        postCounter = -1; /* Since we are checking the next, set to -1 to get the first post */
        checkNext();
    }
    return;
}

void MainWindow::enablePostFields(void)
{
    ui->pbPrevious->setEnabled(true);
    ui->pbNext->setEnabled(true);
    ui->tbPostTitle->setEnabled(true);
    ui->tbPostBody->setEnabled(true);
}

void MainWindow::disableAllFields(void)
{
    ui->lvUsers->setEnabled(false);
    ui->pbPrevious->setEnabled(false);
    ui->pbNext->setEnabled(false);
    ui->tbPostTitle->setEnabled(false);
    ui->tbPostBody->setEnabled(false);
}

void MainWindow::sendNetworkRequest(const char * urlString)
{
    url.setUrl(urlString);
    networkRequest.setUrl(url);
    networkManager->get(networkRequest);
}
