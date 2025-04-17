#include "TLSApplikation.h"

TLSApplikation::TLSApplikation(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TLSApplikationClass())
{
    ui->setupUi(this);

    m_pTLS = std::make_unique<TLS>(this, Method::selectClient);
    ui->CurrentStatus->setText("--> Current Status: No connection...");
    ui->IsClient->setChecked(TRUE);

    connect(    ui->ExitAppButton           ,   &QPushButton::clicked, this , &TLSApplikation::OnExitAppButtonClicked       );
    connect(    ui->Connect                 ,   &QPushButton::clicked, this , &TLSApplikation::OnConnectButtonClicked       );
    connect(    ui->EncryptionParam         ,   &QPushButton::clicked, this , &TLSApplikation::OnEncryptionParamClicked     );
    connect(    ui->IsServer                ,   &QCheckBox  ::clicked, this , &TLSApplikation::OnIsServerCheckBoxClicked    );
    connect(    ui->IsClient                ,   &QCheckBox  ::clicked, this , &TLSApplikation::OnIsClientCheckBoxClicked    );
    connect(    ui->SendEncryptedMessage    ,   &QPushButton::clicked, this , &TLSApplikation::OnSendMessageButtonClicked   );
}


TLSApplikation::~TLSApplikation()
{
    delete ui;
}


Ui::TLSApplikationClass* TLSApplikation::GetUI()
{
    return ui;
}


void TLSApplikation::SetStatus(std::string status_text)
{
    ui->CurrentStatus->setText(status_text.c_str());
}

std::unique_ptr<TLS> TLSApplikation::TakeTLS()
{
    return std::move(m_pTLS);
}


void TLSApplikation::InitSocket()
{
    if (!ui->IsEncrypted->isChecked()) {
        return;
    }

    if (ui->IsClient->isChecked()) {
        InitClientSocket();
    }
    else {
        InitServerSocket();
    }
}


void TLSApplikation::InitClientSocket()
{
    if (ui->Port->text().isEmpty() || ui->IPv4->text().isEmpty()) {
        return;
    }

    ui->CurrentStatus->setText("Setting up Client Socket");

    std::string strPort = ui->Port->text().toStdString();
    std::string strIP = ui->IPv4->text().toStdString();

    ClientSocket clientSocket(*this, strIP, strPort);

    ui->CurrentStatus->setText("Conneting to Server using Encryption!");

    SetUpEncryption(clientSocket.GetSocket());
}


void TLSApplikation::InitServerSocket()
{
    if (ui->Port->text().isEmpty())
    {
        return;
    }

    std::string strPort = ui->Port->text().toStdString();

    ServerSocket* pServerSocket = new ServerSocket(*this, strPort);

    SOCKET sock = pServerSocket->GetSocket();
    SetUpServer(sock);

    delete pServerSocket;
}


void TLSApplikation::SetUpEncryption(const SOCKET& nSocket)
{
    int nResult = m_pTLS->CreateSSL();
    if (nResult != 1) { return; }

    nResult = m_pTLS->SetEncryptedSocket(nSocket);
    if (nResult != 1) { return; }

    nResult = m_pTLS->EncryptedConnect();
    if (nResult != 1) { return; }

    ui->CurrentStatus->setText("Successfully connected!");

    ui->TLS_used_cipher ->setText(  m_pTLS->getTLScipher  ()    );
    ui->TLS_version     ->setText(  m_pTLS->getTLSVersion ()    );
}

void TLSApplikation::SetUpServer(const SOCKET& nSocket)
{
    int nResult = m_pTLS->UseAlgorithm();
    if (nResult != 1) { return; }

    nResult = m_pTLS->CreateSSL();
    if (nResult != 1) { return; }

    nResult = m_pTLS->SetEncryptedSocket(nSocket);
    if (nResult != 1) { return; }

    nResult = m_pTLS->AcceptEncryptedClient();
    if (nResult != 1) { return; }

    ui->CurrentStatus->setText("Successfully accepted Client!");

    ui->TLS_used_cipher ->setText(  m_pTLS->getTLScipher()  );
    ui->TLS_version     ->setText(  m_pTLS->getTLSVersion() );
}

void TLSApplikation::OnConnectButtonClicked()
{
    InitSocket();
}

void TLSApplikation::OnEncryptionParamClicked()
{
    EncryptionParamDlg dlg(*this);

    const int nRes = dlg.exec();

    if (nRes == QDialog::Accepted)
    {
        m_pTLS = dlg.TakeTLS();
    }
}

void TLSApplikation::OnIsServerCheckBoxClicked()
{
    if (ui->IsServer->isChecked())
    {
        if (m_pTLS.get() != NULL)
        {
            m_pTLS = std::make_unique<TLS>(this, Method::selectServer);

            ui->IsClient->setChecked(FALSE);
            ui->IPv4->setEnabled(FALSE);
            ui->IPv4->setStyleSheet("background-color: grey;");
            ui->EncryptionParam->setEnabled(TRUE);
            ui->Connect->setEnabled(TRUE);
        }
        else
        {
            m_pTLS = std::make_unique<TLS>(this, Method::selectServer);

            ui->IsClient->setChecked(FALSE);
            ui->IPv4->setEnabled(FALSE);
            ui->IPv4->setStyleSheet("background-color: grey;");
            ui->EncryptionParam->setEnabled(TRUE);
            ui->Connect->setEnabled(TRUE);
        }
    }
    else
    {
        ui->EncryptionParam->setEnabled(FALSE);
        ui->Connect->setEnabled(FALSE);
    }
}

void TLSApplikation::OnIsClientCheckBoxClicked()
{
    if (ui->IsClient->isChecked())
    {
        if (m_pTLS.get() != NULL)
        {
            m_pTLS = std::make_unique<TLS>(this, Method::selectClient);

            ui->IsServer->setChecked(FALSE);
            ui->IPv4->setEnabled(TRUE);
            ui->IPv4->setStyleSheet("background-color: white;");
            ui->EncryptionParam->setEnabled(TRUE);
            ui->Connect->setEnabled(TRUE);
        }
        else
        {
            m_pTLS = std::make_unique<TLS>(this, Method::selectClient);

            ui->IsServer->setChecked(FALSE);
            ui->IPv4->setEnabled(TRUE);
            ui->IPv4->setStyleSheet("background-color: white;");
            ui->EncryptionParam->setEnabled(TRUE);
            ui->Connect->setEnabled(TRUE);
        }
    }
    else
    {
        ui->EncryptionParam->setEnabled(FALSE);
        ui->Connect->setEnabled(FALSE);
    }
}

void TLSApplikation::OnSendMessageButtonClicked()
{
    QString strEncryptedMessage = ui->EnterEncryptedMessage->text();

    if (strEncryptedMessage.isEmpty()) {
        ui->CurrentStatus->setText("Error: Message is empty!");
        return;
    }

    int nRes = m_pTLS->SendEncryptedMessage(strEncryptedMessage.toStdString());

    switch (nRes) {
    case 1:
        ui->CurrentStatus->setText("Message sent successfully!");
        break;
    case SSL_ERROR_SYSCALL:
        ui->CurrentStatus->setText("Connection lost!");
        break;
    default:
        ui->CurrentStatus->setText(QString("Send error: %1")
            .arg(ERR_error_string(nRes, nullptr)));
    }

    ui->EnterEncryptedMessage->clear();
}


void TLSApplikation::OnExitAppButtonClicked()
{
    QCoreApplication::quit();
}