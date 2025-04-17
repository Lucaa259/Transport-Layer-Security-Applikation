#pragma once

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_EncryptionParamDlg.h"
#include "TLS.h"

#include <iostream>

class TLSApplikation;

QT_BEGIN_NAMESPACE
namespace Ui { class EncryptionParamDlgClass; };
QT_END_NAMESPACE

class EncryptionParamDlg : public QDialog
{
	Q_OBJECT

public:
	EncryptionParamDlg(TLSApplikation& pApp, QWidget *parent = nullptr);
	~EncryptionParamDlg();

	std::unique_ptr<TLS> TakeTLS();
private:
	TLSApplikation* m_pApp;
	Ui::EncryptionParamDlgClass *ui;
	std::unique_ptr<TLS> m_pTLS;

	QString strCertPath;
	QString strPKPath;

	Version minVersion;
	Version maxVersion;

private slots:
	void OnExitBtnClicked();
	void OnCertSearchBtnClicked();
	void OnPKSearchBtnClicked();
	void OnAdoptParamBtnClicked();
	void OnButtonClickedSSLv3_0();
	void OnButtonClickedTLSv1_0();
	void OnButtonClickedTLSv1_1();
	void OnButtonClickedTLSv1_2();
	void OnButtonClickedTLSv1_3();
};
