
// FileIODlg.h : 头文件
//

#include "IOs.h"
#pragma once

using namespace MyIO;

// CFileIODlg 对话框
class CFileIODlg : public CDialogEx, public IIOFinishEventHandler
{
// 构造
public:
	CFileIODlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILEIO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
    CDiskIO m_diskIO;

	// 生成的消息映射函数
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
