// BruteForceDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "BruteForce.h"
#include "BruteForceDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include <unordered_set>
#include <cwctype>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Диалоговое окно CAboutDlg используется для описания сведений о приложении
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// Диалоговое окно CBruteForceDlg

CBruteForceDlg::CBruteForceDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_BRUTEFORCE_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBruteForceDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_CANDIDATES, m_list);
}

BEGIN_MESSAGE_MAP(CBruteForceDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()

    ON_BN_CLICKED(IDC_BTN_GENERATE, &CBruteForceDlg::OnBnClickedGenerate)
    ON_BN_CLICKED(IDC_BTN_ESTIMATE, &CBruteForceDlg::OnBnClickedEstimate)
    ON_BN_CLICKED(IDC_BTN_CLEAR, &CBruteForceDlg::OnBnClickedClear)
    ON_BN_CLICKED(IDC_BTN_LOAD, &CBruteForceDlg::OnBnClickedLoad)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CBruteForceDlg::OnBnClickedSave)
END_MESSAGE_MAP()

BOOL CBruteForceDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Добавление пункта "О программе..." в системное меню.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // ListCtrl должен быть в режиме Report в ресурсах!
    m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_list.InsertColumn(0, L"№", LVCFMT_LEFT, 50);
    m_list.InsertColumn(1, L"Кандидат", LVCFMT_LEFT, 220);
    m_list.InsertColumn(2, L"Причина", LVCFMT_LEFT, 260);
    m_list.InsertColumn(3, L"Время", LVCFMT_LEFT, 140);

    SetDlgItemText(IDC_EDIT_RATE, L"500000");
    SetDlgItemText(IDC_STATIC_RESULT, L"Итоговая оценка времени: -");

    return TRUE;
}

void CBruteForceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CBruteForceDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CBruteForceDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// ===== Helpers =====

std::wstring CBruteForceDlg::Trim(const std::wstring& s)
{
    size_t b = 0;
    while (b < s.size() && iswspace(s[b])) ++b;

    size_t e = s.size();
    while (e > b && iswspace(s[e - 1])) --e;

    return s.substr(b, e - b);
}

std::vector<std::wstring> CBruteForceDlg::SplitHobbies(const std::wstring& s)
{
    std::vector<std::wstring> out;
    std::wstring cur;

    auto push = [&]()
        {
            auto t = Trim(cur);
            if (!t.empty()) out.push_back(t);
            cur.clear();
        };

    for (wchar_t ch : s)
    {
        if (ch == L',' || ch == L';' || ch == L'\n' || ch == L'\r')
            push();
        else
            cur.push_back(ch);
    }
    push();
    return out;
}

CString CBruteForceDlg::FormatTime(double seconds)
{
    if (seconds <= 0.0) return L"-";

    long long s = (long long)(seconds + 0.5);

    const long long minute = 60;
    const long long hour = 3600;
    const long long day = 86400;

    CString out;
    if (s < minute) out.Format(L"%lld сек", s);
    else if (s < hour) out.Format(L"%lld мин %lld сек", s / minute, s % minute);
    else if (s < day) out.Format(L"%lld ч %lld мин", s / hour, (s % hour) / minute);
    else out.Format(L"%lld д %lld ч", s / day, (s % day) / hour);

    return out;
}

void CBruteForceDlg::SetResultText(const CString& text)
{
    SetDlgItemText(IDC_STATIC_RESULT, text);
}

double CBruteForceDlg::ReadRate() const
{
    CString cs;
    GetDlgItemText(IDC_EDIT_RATE, cs);
    cs.Trim();
    if (cs.IsEmpty()) return 0.0;

    wchar_t* end = nullptr;
    double rate = wcstod((LPCWSTR)cs, &end);
    if (rate < 0) rate = 0;
    return rate;
}

UserInfo CBruteForceDlg::ReadUserInfoFromControls() const
{
    UserInfo u;
    CString cs;

    GetDlgItemText(IDC_EDIT_NAME, cs);
    u.name = (LPCWSTR)cs;

    GetDlgItemText(IDC_EDIT_CITY, cs);
    u.city = (LPCWSTR)cs;

    GetDlgItemText(IDC_EDIT_HOBBY, cs);
    u.hobbies = SplitHobbies((LPCWSTR)cs);

    u.name = Trim(u.name);
    u.city = Trim(u.city);
    for (auto& h : u.hobbies) h = Trim(h);

    return u;
}

GenOptions CBruteForceDlg::ReadOptionsFromControls() const
{
    GenOptions opt;
    opt.useCaseVariants = (IsDlgButtonChecked(IDC_CHK_CASE) == BST_CHECKED);
    opt.useNumbers = (IsDlgButtonChecked(IDC_CHK_NUMBERS) == BST_CHECKED);
    opt.useSeparators = (IsDlgButtonChecked(IDC_CHK_SEPARATORS) == BST_CHECKED);
    opt.useLeet = (IsDlgButtonChecked(IDC_CHK_LEET) == BST_CHECKED);
    opt.useSpecialEnd = (IsDlgButtonChecked(IDC_CHK_SPECIAL) == BST_CHECKED);
    opt.maxCandidates = 5000;
    return opt;
}

void CBruteForceDlg::FillList()
{
    m_list.SetRedraw(FALSE);
    m_list.DeleteAllItems();

    for (int i = 0; i < (int)m_candidates.size(); ++i)
    {
        const auto& r = m_candidates[i];

        CString idx; idx.Format(L"%d", i + 1);
        int item = m_list.InsertItem(i, idx);
        m_list.SetItemText(item, 1, r.password.c_str());
        m_list.SetItemText(item, 2, r.reason.c_str());
        m_list.SetItemText(item, 3, FormatTime(r.secondsToReach));
    }

    m_list.SetRedraw(TRUE);
    m_list.Invalidate();
}

void CBruteForceDlg::ClearAll()
{
    SetDlgItemText(IDC_EDIT_NAME, L"");
    SetDlgItemText(IDC_EDIT_CITY, L"");
    SetDlgItemText(IDC_EDIT_HOBBY, L"");

    m_candidates.clear();
    m_list.DeleteAllItems();
    SetResultText(L"Итоговая оценка времени: -");
}

// ===== Button handlers =====

void CBruteForceDlg::OnBnClickedGenerate()
{
    UserInfo u = ReadUserInfoFromControls();
    GenOptions opt = ReadOptionsFromControls();

    if (u.name.empty())
    {
        AfxMessageBox(L"Введите имя.");
        return;
    }

    m_candidates.clear();
    std::unordered_set<std::wstring> uniq;
    uniq.reserve(opt.maxCandidates * 2);

    auto add = [&](const std::wstring& pwd, const std::wstring& reason)
        {
            if (pwd.empty()) return;
            if (m_candidates.size() >= opt.maxCandidates) return;
            if (uniq.insert(pwd).second)
            {
                CandidateRow r;
                r.password = pwd;
                r.reason = reason;
                r.index = m_candidates.size() + 1;
                m_candidates.push_back(std::move(r));
            }
        };

    auto lower = [](std::wstring s) {
        for (auto& ch : s) ch = (wchar_t)towlower(ch);
        return s;
        };
    auto upper = [](std::wstring s) {
        for (auto& ch : s) ch = (wchar_t)towupper(ch);
        return s;
        };
    auto cap = [&](std::wstring s) {
        s = lower(s);
        if (!s.empty()) s[0] = (wchar_t)towupper(s[0]);
        return s;
        };

    std::wstring name = Trim(u.name);
    std::wstring city = Trim(u.city);

    std::vector<std::wstring> nameVariants;
    nameVariants.push_back(lower(name));
    if (opt.useCaseVariants)
    {
        nameVariants.push_back(cap(name));
        nameVariants.push_back(upper(name));
    }

    std::vector<std::wstring> hobbies = u.hobbies;
    if (hobbies.empty()) hobbies.push_back(L"");

    std::vector<std::wstring> numbers = opt.useNumbers
        ? std::vector<std::wstring>{ L"", L"1", L"12", L"123", L"2024", L"2025" }
    : std::vector<std::wstring>{ L"" };

    std::vector<std::wstring> seps = opt.useSeparators
        ? std::vector<std::wstring>{ L"", L"_", L".", L"-" }
    : std::vector<std::wstring>{ L"" };

    std::vector<std::wstring> specials = opt.useSpecialEnd
        ? std::vector<std::wstring>{ L"", L"!", L"@", L"#" }
    : std::vector<std::wstring>{ L"" };

    for (const auto& nv : nameVariants)
    {
        add(nv, L"Имя");

        if (!city.empty())
            for (const auto& sep : seps)
                add(nv + sep + lower(city), L"Имя + город");

        for (const auto& h : hobbies)
            if (!h.empty())
                for (const auto& sep : seps)
                    add(nv + sep + lower(h), L"Имя + хобби");

        for (const auto& num : numbers)
            if (!num.empty())
                add(nv + num, L"Имя + число");

        for (const auto& h : hobbies)
        {
            if (h.empty()) continue;
            for (const auto& num : numbers)
            {
                if (num.empty()) continue;
                for (const auto& sep : seps)
                    add(nv + sep + lower(h) + num, L"Имя + хобби + число");
            }
        }

        if (opt.useSpecialEnd)
            for (const auto& sp : specials)
                if (!sp.empty())
                    add(nv + sp, L"Имя + спецсимвол");
    }

    if (opt.useLeet && !m_candidates.empty())
    {
        auto leetOne = [](std::wstring s) {
            for (auto& ch : s)
            {
                switch (towlower(ch))
                {
                case L'a': ch = L'@'; break;
                case L'o': ch = L'0'; break;
                case L'e': ch = L'3'; break;
                case L'i': ch = L'1'; break;
                case L's': ch = L'5'; break;
                default: break;
                }
            }
            return s;
            };

        size_t limit = (std::min)((size_t)200, m_candidates.size());
        for (size_t i = 0; i < limit && m_candidates.size() < opt.maxCandidates; ++i)
        {
            auto leetPwd = leetOne(m_candidates[i].password);
            if (leetPwd != m_candidates[i].password)
                add(leetPwd, L"Лит-замены от: " + m_candidates[i].password);
        }
    }

    double rate = ReadRate();
    if (rate > 0)
        for (size_t i = 0; i < m_candidates.size(); ++i)
            m_candidates[i].secondsToReach = (double)(i + 1) / rate;

    FillList();
    SetResultText(L"Словарь сгенерирован. Нажмите «Оценить».");
}

void CBruteForceDlg::OnBnClickedEstimate()
{
    if (m_candidates.empty())
    {
        AfxMessageBox(L"Сначала нажмите «Сгенерировать».");
        return;
    }

    double rate = ReadRate();
    if (rate <= 0)
    {
        AfxMessageBox(L"Введите скорость попыток/сек (например 500000).");
        return;
    }

    double K = (double)m_candidates.size();
    double best = 1.0 / rate;
    double avg = (K / 2.0) / rate;
    double worst = K / rate;

    for (size_t i = 0; i < m_candidates.size(); ++i)
        m_candidates[i].secondsToReach = (double)(i + 1) / rate;

    FillList();

    CString result;
    result.Format(L"Итоговая оценка времени: худший %s / средний %s / лучший %s (кандидатов: %d)",
        FormatTime(worst).GetString(),
        FormatTime(avg).GetString(),
        FormatTime(best).GetString(),
        (int)m_candidates.size());

    SetResultText(result);
}

void CBruteForceDlg::OnBnClickedClear()
{
    ClearAll();
}

void CBruteForceDlg::OnBnClickedLoad()
{
    CFileDialog dlg(TRUE, L"txt", nullptr,
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        L"Text files (*.txt)|*.txt|All files (*.*)|*.*||", this);

    if (dlg.DoModal() != IDOK) return;

    CString path = dlg.GetPathName();

    CStdioFile file;
    if (!file.Open(path, CFile::modeRead | CFile::typeText))
    {
        AfxMessageBox(L"Не удалось открыть файл.");
        return;
    }

    ClearAll();
    SetDlgItemText(IDC_EDIT_RATE, L"500000");

    CString line;
    while (file.ReadString(line))
    {
        line.Trim();
        if (line.IsEmpty()) continue;

        int pos = line.Find(L'=');
        if (pos < 0) continue;

        CString key = line.Left(pos); key.Trim();
        CString val = line.Mid(pos + 1); val.Trim();

        if (key.CompareNoCase(L"name") == 0) SetDlgItemText(IDC_EDIT_NAME, val);
        else if (key.CompareNoCase(L"city") == 0) SetDlgItemText(IDC_EDIT_CITY, val);
        else if (key.CompareNoCase(L"hobbies") == 0) SetDlgItemText(IDC_EDIT_HOBBY, val);
        else if (key.CompareNoCase(L"rate") == 0) SetDlgItemText(IDC_EDIT_RATE, val);
        else if (key.CompareNoCase(L"case") == 0) CheckDlgButton(IDC_CHK_CASE, (val == L"1") ? BST_CHECKED : BST_UNCHECKED);
        else if (key.CompareNoCase(L"numbers") == 0) CheckDlgButton(IDC_CHK_NUMBERS, (val == L"1") ? BST_CHECKED : BST_UNCHECKED);
        else if (key.CompareNoCase(L"separators") == 0) CheckDlgButton(IDC_CHK_SEPARATORS, (val == L"1") ? BST_CHECKED : BST_UNCHECKED);
        else if (key.CompareNoCase(L"leet") == 0) CheckDlgButton(IDC_CHK_LEET, (val == L"1") ? BST_CHECKED : BST_UNCHECKED);
        else if (key.CompareNoCase(L"special") == 0) CheckDlgButton(IDC_CHK_SPECIAL, (val == L"1") ? BST_CHECKED : BST_UNCHECKED);
    }

    file.Close();
    SetResultText(L"Данные загружены. Нажмите «Сгенерировать».");
}

void CBruteForceDlg::OnBnClickedSave()
{
    CFileDialog dlg(FALSE, L"txt", L"result.txt",
        OFN_OVERWRITEPROMPT,
        L"Text files (*.txt)|*.txt|All files (*.*)|*.*||", this);

    if (dlg.DoModal() != IDOK) return;

    CString path = dlg.GetPathName();

    CStdioFile file;
    if (!file.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        AfxMessageBox(L"Не удалось сохранить файл.");
        return;
    }

    CString name, city, hobby, rate;
    GetDlgItemText(IDC_EDIT_NAME, name);
    GetDlgItemText(IDC_EDIT_CITY, city);
    GetDlgItemText(IDC_EDIT_HOBBY, hobby);
    GetDlgItemText(IDC_EDIT_RATE, rate);

    file.WriteString(L"name=" + name + L"\n");
    file.WriteString(L"city=" + city + L"\n");
    file.WriteString(L"hobbies=" + hobby + L"\n");
    file.WriteString(L"rate=" + rate + L"\n");

    auto chk = [&](int id)->CString {
        return (IsDlgButtonChecked(id) == BST_CHECKED) ? L"1" : L"0";
        };

    file.WriteString(L"case=" + chk(IDC_CHK_CASE) + L"\n");
    file.WriteString(L"numbers=" + chk(IDC_CHK_NUMBERS) + L"\n");
    file.WriteString(L"separators=" + chk(IDC_CHK_SEPARATORS) + L"\n");
    file.WriteString(L"leet=" + chk(IDC_CHK_LEET) + L"\n");
    file.WriteString(L"special=" + chk(IDC_CHK_SPECIAL) + L"\n\n");

    file.WriteString(L"--- candidates ---\n");
    for (const auto& r : m_candidates)
    {
        CString line;
        line.Format(L"%zu\t%s\t%s\t%s\n",
            r.index,
            r.password.c_str(),
            r.reason.c_str(),
            FormatTime(r.secondsToReach).GetString());
        file.WriteString(line);
    }

    file.Close();
    AfxMessageBox(L"Сохранено.");
}