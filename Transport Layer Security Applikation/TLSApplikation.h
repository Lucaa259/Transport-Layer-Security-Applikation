#pragma once
#include <QtWidgets/QDialog>
#include "ui_TLSApplikation.h"
#include <QPushButton>
#include <QCheckBox>
#include "EncryptionParamDlg.h"
#include <iostream>
#include "TLS.h"
#include "ClientSocket.h"
#include "ServerSocket.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TLSApplikationClass; };
QT_END_NAMESPACE

class TLSApplikation : public QDialog
{
    Q_OBJECT

public:
    explicit TLSApplikation(QWidget *parent = nullptr);
    ~TLSApplikation();

    Ui::TLSApplikationClass* GetUI();

    void SetStatus(std::string status_text);
    void SetReceived(std::string received_text);

    std::unique_ptr<TLS> TakeTLS(); // Take Ownership of m_pTLS

private:
    Ui::TLSApplikationClass* ui;
    std::unique_ptr<TLS> m_pTLS;

    std::unique_ptr<ServerSocket> m_pServerSocket;
    std::unique_ptr<ClientSocket> m_pClientSocket;

    void InitSocket();
    void InitClientSocket();
    void InitServerSocket();

private slots:
    void OnExitAppButtonClicked();
    void OnConnectButtonClicked();
    void OnEncryptionParamClicked();
    void OnIsServerCheckBoxClicked();
    void OnIsClientCheckBoxClicked();
    void OnSendMessageButtonClicked();
};
