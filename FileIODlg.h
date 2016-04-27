
// FileIODlg.h : ͷ�ļ�
//

#include "IOs.h"
#pragma once

using namespace MyIO;

// CFileIODlg �Ի���
class CFileIODlg : public CDialogEx, public IIOFinishEventHandler
{
// ����
public:
	CFileIODlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILEIO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
    CDiskIO m_diskIO;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();


    void NotifyWriteFinish();
    void NotifyReadFinish();
};
