#pragma once
#include <QtWidgets/QDialog>
#include "ui_TLSApplikation.h"
#include <QPushButton>
#include <QCheckBox>
#include "EncryptionParamDlg.h"

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

    std::unique_ptr<TLS> TakeTLS(); // Take Ownership of m_pTLS

private:
    Ui::TLSApplikationClass* ui;
    std::unique_ptr<TLS> m_pTLS;

    void InitSocket();
    void InitClientSocket();
    void InitServerSocket();
    void SetUpEncryption(const SOCKET& nSocket);
    void SetUpServer(const SOCKET& nSocket);

private slots:
    void OnExitAppButtonClicked();
    void OnConnectButtonClicked();
    void OnEncryptionParamClicked();
    void OnIsServerCheckBoxClicked();
    void OnIsClientCheckBoxClicked();
    void OnSendMessageButtonClicked();
};
