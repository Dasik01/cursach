// BruteForceDlg.h: файл заголовка
//

#pragma once

#include "afxdialogex.h"
#include <string>
#include <vector>

struct UserInfo
{
    std::wstring name;
    std::wstring city;
    std::vector<std::wstring> hobbies;
};

struct GenOptions
{
    bool useCaseVariants = true;
    bool useNumbers = true;
    bool useSeparators = true;
    bool useLeet = false;
    bool useSpecialEnd = false;
    size_t maxCandidates = 5000;
};

struct CandidateRow
{
    std::wstring password;
    std::wstring reason;
    size_t index = 0;
    double secondsToReach = 0.0;
};

class CBruteForceDlg : public CDialogEx
{
public:
    CBruteForceDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_BRUTEFORCE_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

protected:
    HICON m_hIcon;

    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    // кнопки
    afx_msg void OnBnClickedGenerate();
    afx_msg void OnBnClickedEstimate();
    afx_msg void OnBnClickedClear();
    afx_msg void OnBnClickedLoad();
    afx_msg void OnBnClickedSave();

    DECLARE_MESSAGE_MAP()

private:
    CListCtrl m_list;
    std::vector<CandidateRow> m_candidates;

private:
    UserInfo ReadUserInfoFromControls() const;
    GenOptions ReadOptionsFromControls() const;
    double ReadRate() const;

    void FillList();
    void ClearAll();
    void SetResultText(const CString& text);

    static std::wstring Trim(const std::wstring& s);
    static std::vector<std::wstring> SplitHobbies(const std::wstring& s);
    static CString FormatTime(double seconds);
};