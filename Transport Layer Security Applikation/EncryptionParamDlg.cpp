#include "EncryptionParamDlg.h"
#include "TLSApplikation.h"

EncryptionParamDlg::EncryptionParamDlg(TLSApplikation& pApp, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::EncryptionParamDlgClass())
	, m_pApp(&pApp)
{
	ui->setupUi(this);

	if (m_pApp)
		m_pTLS = m_pApp->TakeTLS();

	connect(ui->ExitBtn, &QPushButton::clicked, this, &EncryptionParamDlg::OnExitBtnClicked);
	connect(ui->SearchCert, &QPushButton::clicked, this, &EncryptionParamDlg::OnCertSearchBtnClicked);
	connect(ui->SearchPK, &QPushButton::clicked, this, &EncryptionParamDlg::OnPKSearchBtnClicked);
	connect(ui->AdoptParamBtn, &QPushButton::clicked, this, &EncryptionParamDlg::OnAdoptParamBtnClicked);
	connect(ui->SSLv3_0, &QRadioButton::clicked, this, &EncryptionParamDlg::OnButtonClickedSSLv3_0);
	connect(ui->TLSv1_0, &QRadioButton::clicked, this, &EncryptionParamDlg::OnButtonClickedTLSv1_0);
	connect(ui->TLSv1_1, &QRadioButton::clicked, this, &EncryptionParamDlg::OnButtonClickedTLSv1_1);
	connect(ui->TLSv1_2, &QRadioButton::clicked, this, &EncryptionParamDlg::OnButtonClickedTLSv1_2);
	connect(ui->TLSv1_3, &QRadioButton::clicked, this, &EncryptionParamDlg::OnButtonClickedTLSv1_3);
}


EncryptionParamDlg::~EncryptionParamDlg()
{
	delete ui;
}

std::unique_ptr<TLS>  EncryptionParamDlg::TakeTLS()
{
	return std::move(m_pTLS);
}


void EncryptionParamDlg::OnCertSearchBtnClicked()
{
	strCertPath = QFileDialog::getOpenFileName(this, "Zertifikat wählen", "", "Zertifikate (*.pem)");

	if (!strCertPath.isEmpty())
	{
		ui->CertFilePath->setText(strCertPath);
	}
}


void EncryptionParamDlg::OnPKSearchBtnClicked()
{
	strPKPath = QFileDialog::getOpenFileName(this, "Private Key wählen", "", "Private Keys (*.pem *.key)");

	if (!strPKPath.isEmpty())
	{
		ui->PKFilePath->setText(strPKPath);
	}
}


void EncryptionParamDlg::OnAdoptParamBtnClicked()
{
	if (!m_pTLS) {
		QMessageBox::critical(this, "Error", "No TLS instance available!");
		return;
	}

	m_pTLS.get()->UseCertificate(strCertPath.toStdString());
	m_pTLS.get()->UsePrivateKey(strPKPath.toStdString());
	m_pTLS.get()->SetVersion(minVersion, maxVersion);

	this->accept();
}

void EncryptionParamDlg::OnButtonClickedSSLv3_0()
{
	minVersion = SSLv3_0;
	maxVersion = SSLv3_0;
}

void EncryptionParamDlg::OnButtonClickedTLSv1_0()
{
	minVersion = TLSv1_0;
	maxVersion = TLSv1_0;
}

void EncryptionParamDlg::OnButtonClickedTLSv1_1()
{
	minVersion = TLSv1_0;
	maxVersion = TLSv1_1;
}

void EncryptionParamDlg::OnButtonClickedTLSv1_2()
{
	minVersion = TLSv1_2;
	maxVersion = TLSv1_2;
}

void EncryptionParamDlg::OnButtonClickedTLSv1_3()
{
	minVersion = TLSv1_2;
	maxVersion = TLSv1_3;
}


void EncryptionParamDlg::OnExitBtnClicked()
{
	this->close();
}