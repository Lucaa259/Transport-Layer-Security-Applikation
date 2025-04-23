#include "TLSApplikation.h"

TLSApplikation::TLSApplikation(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TLSApplikationClass())
{
    ui->setupUi(this);

    m_pTLS = std::make_unique<TLS>(this, Method::selectClient);
    ui->CurrentStatus->setText("No connection...");
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

void TLSApplikation::SetReceived(std::string received_text)
{
    ui->ReceivedMessage->setText(received_text.c_str());
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
        ui->CurrentStatus->setText("Fehler: IP oder Port fehlt!");
        return;
    }

    std::string strPort = ui->Port->text().toStdString();
    std::string strIP = ui->IPv4->text().toStdString();

    ui->CurrentStatus->setText("Starte TLS-Client-Verbindung...");



    std::thread clientThread([this, strIP, strPort]() {
        try
        {
            m_pClientSocket = std::make_unique<ClientSocket>(*this, strIP, strPort);

            SOCKET socket = m_pClientSocket->GetSocket();

            int nResult = m_pTLS->CreateSSL();
            if (nResult != 1) throw std::runtime_error("CreateSSL fehlgeschlagen");

            nResult = m_pTLS->SetEncryptedSocket(socket);
            if (nResult != 1) throw std::runtime_error("SetEncryptedSocket fehlgeschlagen");

            nResult = m_pTLS->EncryptedConnect();
            if (nResult != 1) throw std::runtime_error("SSL_connect fehlgeschlagen");

            QMetaObject::invokeMethod(this, [this]() {
                ui->CurrentStatus->setText("Erfolgreich verbunden (Client)");
                ui->TLS_used_cipher->setText(m_pTLS->getTLScipher());
                ui->TLS_version->setText(m_pTLS->getTLSVersion());
                }, Qt::QueuedConnection);

            std::thread receiveThread([this]() {
                while (true) {
                    int res = m_pTLS->ReceiveEncryptedMessage();
                    if (res <= 0)
                        break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                });

            receiveThread.detach();
        }
        catch (const std::exception& ex)
        {
            QMetaObject::invokeMethod(this, [this, ex]() {
                ui->CurrentStatus->setText("Client-Verbindungsfehler: " + QString::fromStdString(ex.what()));
                }, Qt::QueuedConnection);
        }
        });

    clientThread.detach();
}



void TLSApplikation::InitServerSocket()
{
    if (ui->Port->text().isEmpty()) 
        return;

    std::string strPort = ui->Port->text().toStdString();

    m_pServerSocket = std::make_unique<ServerSocket>(*this, strPort);

    std::thread serverThread([=]() {
        try
        {
            QMetaObject::invokeMethod(this, [=]() {
                this->SetStatus("Setze Server auf...");
                }, Qt::QueuedConnection);

            SOCKET clientSocket = m_pServerSocket->StartAcceptConnections();  // Blockierender accept()

            QMetaObject::invokeMethod(this, [=]() {
                this->SetStatus("Client verbunden – Starte TLS...");
                }, Qt::QueuedConnection);

            m_pTLS->CreateSSL();

            m_pTLS->SetEncryptedSocket(clientSocket);
            int tlsResult = m_pTLS->AcceptEncryptedClient();

            if (tlsResult == 1)
            {
                QMetaObject::invokeMethod(this, [=]() {
                    this->SetStatus("TLS-Verbindung erfolgreich!");
                    ui->TLS_used_cipher->setText(m_pTLS->getTLScipher());
                    ui->TLS_version->setText(m_pTLS->getTLSVersion());
                    }, Qt::QueuedConnection);

                std::thread receiveThread([this]() {
                    while (true) {
                        int res = m_pTLS->ReceiveEncryptedMessage();

                        if (res <= 0) {
                            QMetaObject::invokeMethod(this, [=]() {
                                this->SetStatus("Verbindung wurde geschlossen oder Fehler beim Empfang.");
                                }, Qt::QueuedConnection);
                            break;
                        }

                        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // CPU-schonend
                    }
                    });

                receiveThread.detach();  // Thread selbstständig weiterlaufen lassen
            }
        }
        catch (const std::exception& error)
        {
            QMetaObject::invokeMethod(this, [=]() {
                this->SetStatus("Fehler: " + std::string(error.what()));
                }, Qt::QueuedConnection);
        }
        });

    serverThread.detach();  // GUI nicht blockieren
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

    if (nRes > 0) {
        ui->CurrentStatus->setText("Message sent successfully!");
    }
    else if (nRes == SSL_ERROR_SYSCALL) {
        ui->CurrentStatus->setText("Error: Connection lost!");
    }

    ui->EnterEncryptedMessage->setText("");
}

void TLSApplikation::OnExitAppButtonClicked()
{
    QCoreApplication::quit();
}