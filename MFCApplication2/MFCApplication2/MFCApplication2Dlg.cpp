
// MFCApplication2Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char addr4_str[INET_ADDRSTRLEN];


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 대화 상자
CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent)
	, baseWidth(0)
	, baseHeight(0)
	, mAlarmlog(_T(""))
	, m_bWaitingLogged(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// 데이터 값
	DDX_Control(pDX, IDC_UPTIME, cUptime);
	DDX_Control(pDX, IDC_OCCUPIED, cOccupied);
	DDX_Control(pDX, IDC_DAILYUSAGE, cDailyusage);
	DDX_Control(pDX, IDC_AVGTIME, cAvgtime);
	DDX_Control(pDX, IDC_ALARM, cAlarm);
	DDX_Text(pDX, IDC_ALARMLOG, mAlarmlog);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DOOR, &CMFCApplication2Dlg::OnBnClickedDoor)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 메시지 처리기

BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// PLC 연결 확인
	ConnectionFlag = 1;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	InitServer();

	memset(m_startTick, 0, sizeof(m_startTick));
	memset(m_slotRunning, false, sizeof(m_slotRunning));
	memset(m_prevReg, 0xFF, sizeof(m_prevReg));
	SetTimer(1, 1000, NULL);

	SetFontSize();			// 폰트 설정

	// 모니터 크기 파악
	CRect Rect;
	GetClientRect(&Rect);
	baseWidth = Rect.Width();
	baseHeight = Rect.Height();

	SetTimer(2, 1000, nullptr);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCApplication2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		CDialogEx::OnPaint();
		DrawConnectionPoint(&dc);	// PLC 연결 상태 표시
		UpdateSlotStatus(&dc);		// 슬롯 상태 표시
	}
}

// 폰트 크기 설정
void CMFCApplication2Dlg::SetFontSize() {
	// Label 폰트 설정
	m_label.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));

	GetDlgItem(IDC_CONNSTATUS)->SetFont(&m_label);
	GetDlgItem(IDC_GUPTIME)->SetFont(&m_label);
	GetDlgItem(IDC_GUSAGE)->SetFont(&m_label);
	GetDlgItem(IDC_GTODAY)->SetFont(&m_label);
	GetDlgItem(IDC_GAVGTIME)->SetFont(&m_label);
	GetDlgItem(IDC_GALARM)->SetFont(&m_label);

	GetDlgItem(IDC_DATE)->SetFont(&m_label);
	GetDlgItem(IDC_GUSESTATUS)->SetFont(&m_label);
	GetDlgItem(IDC_LOG)->SetFont(&m_label);
	GetDlgItem(IDC_ALARMLOG)->SetFont(&m_label);

	GetDlgItem(IDC_DOOR)->SetFont(&m_label);


	// Static Text 폰트 설정
	m_fontSize.CreateFont(
		35, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));

	GetDlgItem(IDC_UPTIME)->SetFont(&m_fontSize);
	GetDlgItem(IDC_OCCUPIED)->SetFont(&m_fontSize);
	GetDlgItem(IDC_DAILYUSAGE)->SetFont(&m_fontSize);
	GetDlgItem(IDC_AVGTIME)->SetFont(&m_fontSize);
	GetDlgItem(IDC_ALARM)->SetFont(&m_fontSize);

	
	cUptime.SetWindowText(_T("0"));
	cOccupied.SetWindowText(_T("0/8"));
	cDailyusage.SetWindowText(_T("0"));
	cAvgtime.SetWindowText(_T("0.0h"));
	cAlarm.SetWindowText(_T("3"));
}

// PLC 연결 상태 표시 (연결 성공 - 초록, 연결 실패 - 빨강)
void CMFCApplication2Dlg::DrawConnectionPoint(CDC *pDC) 
{
	CRect clientRect;
	GetClientRect(&clientRect);

	int circleX = (int)(clientRect.Width() * 40.0 / baseWidth);
	int circleY = (int)(clientRect.Height() * 48.0 / baseHeight);
	int circleSize = (int)(clientRect.Width() * 12.0 / baseWidth);

	COLORREF circleColor = ConnectionFlag ? RGB(255, 0, 0) : RGB(29, 158, 117);

	CBrush brush(circleColor);
	CBrush* oldBrush = pDC->SelectObject(&brush);

	CPen pen(PS_NULL, 0, circleColor);
	CPen* oldPen = pDC->SelectObject(&pen);

	pDC->Ellipse(circleX, circleY, circleX + circleSize, circleY + circleSize);

	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}

// PLC 연결
void CMFCApplication2Dlg::ErrQuit(wchar_t* msg) {
	LPVOID IpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);

	MessageBoxW((LPCWSTR)IpMsgBuf, (LPCWSTR)msg, MB_OK | MB_ICONERROR);
	LocalFree(IpMsgBuf);
	exit(-1);
}

void CMFCApplication2Dlg::ErrDisplay(wchar_t* msg) {
	LPVOID IpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	MessageBoxW((LPCWSTR)IpMsgBuf, (LPCWSTR)msg, MB_OK | MB_ICONERROR);
	LocalFree(IpMsgBuf);
}

int CMFCApplication2Dlg::InitServer() {
	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) {
		ErrDisplay(L"socket()");
		return -1;
	}

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(503);
	inet_pton(AF_INET, "192.168.0.66", &serveraddr.sin_addr);

	retval = connect(client_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		if (!m_bWaitingLogged) {
			mAlarmlog += L"[TCP 서버] 503 포트 연결 대기 중...\r\n";
			UpdateData(FALSE);
			m_bWaitingLogged = true;
		}
		ConnectionFlag = 1;
		return -1;
	}

	u_long mode = 1;
	ioctlsocket(client_sock, FIONBIO, &mode);

	ConnectionFlag = 0;
	m_bWaitingLogged = false;

	ReadHoldingRegs();

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &serveraddr.sin_addr, ip, sizeof(ip));
	CString ipStr(ip);
	
	CString logLine;
	logLine.Format(L"[TCP 서버] 접속 : IP 주소 = % s, 포트 = % d \r\n", ipStr, ntohs(serveraddr.sin_port));
	mAlarmlog += logLine;
	UpdateData(FALSE);
	Invalidate();
	return 0;
}

void CMFCApplication2Dlg::Connection() {
	if (ConnectionFlag == 1) {
		InitServer();
		return;
	}

	// 연결 상태 감지 - 끊겼을 때
	char buf[1];
	int ret = recv(client_sock, buf, 1, MSG_PEEK);
	if (ret == 0 || (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)) {
		closesocket(client_sock);
		client_sock = INVALID_SOCKET;
		ConnectionFlag = 1;
		m_bWaitingLogged = false;
		mAlarmlog += L"[TCP 서버] 연결 끊김. 재연결 대기 중...\r\n";
		UpdateData(FALSE);
		Invalidate();
	}
}

void CMFCApplication2Dlg::ReadHoldingRegs() {
	if (ConnectionFlag == 1) return;

	BYTE request[] = {
		0x00, 0x01,		// 요청 식별 번호
		0x00, 0x00,		// 프로토콜 ID (Modbus는 0x0000 고정)
		0x00, 0x06,		// 6 바이트 길이
		0x01,			// PLC 슬레이브 ID
		0x03,			// FC03 홀딩 레지스터 읽기	
		0x00, 0x01,		// 시작 주소 : HoldingRegs[0]
		0x00, 0x08		// HoldingRegs[0]부터 일곱 개
	};

	send(client_sock, (char*)request, sizeof(request), 0);

	BYTE response[128];
	int receivedData = recv(client_sock, (char*)response, sizeof(response), 0);

	if (receivedData == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) return;
		return;
	}
	if (receivedData <= 0) return;

	if (response[7] != 0x03) return;   // 응답 코드 확인

	WORD reg[8];
	reg[0] = (response[9] << 8) | response[10];			// HoldingRegs[0]
	reg[1] = (response[11] << 8) | response[12];		// HoldingRegs[1]
	reg[2] = (response[13] << 8) | response[14];		// HoldingRegs[2]
	reg[3] = (response[15] << 8) | response[16];		// HoldingRegs[3]
	reg[4] = (response[17] << 8) | response[18];		// HoldingRegs[4]
	reg[5] = (response[19] << 8) | response[20];		// HoldingRegs[5]
	reg[6] = (response[21] << 8) | response[22];		// HoldingRegs[6]
	reg[7] = (response[23] << 8) | response[24];		// HoldingRegs[7]

	bool changed = false;

	// 7, 8번 슬롯 고장 상태
	m_slotStates[6] = SLOT_FAULT;
	m_slotStates[7] = SLOT_FAULT;

	for (int i = 0; i < 6; i++) {
		// 슬롯이 고장 상태이면 무시
		if (m_slotStates[i] == SLOT_FAULT) continue;

		// 슬롯에 보관 시작할 때
		if (reg[i] == 1 && !m_slotRunning[i]) {
			m_slotRunning[i] = true;
			m_slotStates[i] = SLOT_USED;
			m_startTick[i] = GetTickCount();
			
			int occupiedCount = 0, usableslotCount = 0;			

			// 사용 중인 슬롯 수
			for (int j = 0; j < 8; j++) {
				if (m_slotStates[j] == SLOT_FAULT) continue;
				usableslotCount++;
				if (m_slotRunning[j]) occupiedCount++;
			}
			Occupied.Format(L"%d/8", occupiedCount);
			cOccupied.SetWindowText(Occupied);

			// 가동률
			Status.Format(L"%.1f%%", (double)occupiedCount / 6 * 100.0);
			cUptime.SetWindowText(Status);

			// 금일 이용량
			dailyUsage++;
			Usage.Format(L"%d", dailyUsage);
			cDailyusage.SetWindowText(Usage);

			// 평균 이용 시간
			if (m_completedCount > 0) {
				int avgSec = m_totalUsageSec / m_completedCount;
				int avgHours = avgSec / 3600;

				CString avgTime;
				avgTime.Format(_T("%.2fH"), (double)avgHours);
				cAvgtime.SetWindowText(avgTime);  // 평균 이용 시간 표시용 컨트롤
			}

			logLine.Format(L"슬롯 #%02d 보관 성공\r\n", i + 1);
			mAlarmlog += logLine;
			changed = true;
		}

		// 슬롯에서 우산이 회수되었을 때
		if (reg[i] == 0 && m_slotRunning[i]) {
			m_slotRunning[i] = false;
			m_slotStates[i] = SLOT_EMPTY;
			m_startTick[i] = 0;

			int occupiedCount = 0, usableslotCount = 0;

			// 사용 중인 슬롯 수
			for (int j = 0; j < 8; j++) {
				if (m_slotStates[j] == SLOT_FAULT) continue;
				usableslotCount++;
				if (m_slotRunning[j]) occupiedCount++;
			}
			Occupied.Format(L"%d/8", occupiedCount);
			cOccupied.SetWindowText(Occupied);

			// 가동률
			Status.Format(L"%.1f%%", (double)occupiedCount / 6 * 100.0);
			cUptime.SetWindowText(Status);

			logLine.Format(L"슬롯 #%02d 회수 처리 완료\r\n", i + 1);
			mAlarmlog += logLine;
			changed = true;
		}
	}

	if (changed) Invalidate();

	UpdateData(FALSE);
}

void CMFCApplication2Dlg::WriteRegs(WORD address, bool value) {
	if (ConnectionFlag == 1) return;

	WORD coilValue = value ? 0xFF00 : 0x0000;

	BYTE request[] = {
		0x00, 0x02,
		0x00, 0x00,
		0x00, 0x06,
		0x01,
		0x05,
		(BYTE)(address >> 8), (BYTE)(address & 0xFF),
		(BYTE)(coilValue >> 8), (BYTE)(coilValue & 0xFF)
	};

	send(client_sock, (char*)request, sizeof(request), 0);
}

/*
// PLC 데이터 값 읽기
void CMFCApplication2Dlg::DataFromPLC() {
	int occupiedCount = 0;

	// 가동률
	int totalSlots = 8;
	int errorSlots = 0;
	int usableSlots = 0;

	for (int i = 0; i < totalSlots; i++) {
		if (m_slotStates[i] == SLOT_FAULT) {
			errorSlots++;
			continue;
		}
		usableSlots++;
		if (m_slotRunning[i]) occupiedCount++;
	}

	double utilizationRate = usableSlots > 0 ? (double)occupiedCount / usableSlots * 100.0 : 0.0;
	Status.Format(L"%.1f%%", utilizationRate);

	// 사용 중인 슬롯 수
	for (int i = 0; i < 8; i++) if (m_slotRunning[i]) occupiedCount++;
	Occupied.Format(L"%d/8", occupiedCount);
}
*/

/*
// Static Text 배경 투명으로 설정
HBRUSH CMFCApplication2Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);  // 배경 투명 처리
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}
*/

// 슬롯 상태 표시 리스트 박스 디자인
void CMFCApplication2Dlg::UpdateSlotStatus(CDC* pDC)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	int startX = (int)(clientRect.Width() * 105.0 / baseWidth);
	int startY = (int)(clientRect.Height() * 350.0 / baseHeight);
	int cellSize = (int)(clientRect.Width() * 100.0 / baseWidth);
	int gap = 30;
	int cols = 4;

	CFont fontSlot;
	fontSlot.CreateFont(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));
	CFont* oldFont = pDC->SelectObject(&fontSlot);

	CFont fontTime;
	fontTime.CreateFont(25, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));
	

	for (int i = 0; i < 8; i++)
	{
		int row = i / cols;
		int col = i % cols;

		int x = startX + col * (cellSize + gap);
		int y = startY + row * (cellSize + gap);

		CRect cellRect(x, y, x + cellSize, y + cellSize);

		// 상태에 따른 색상 (지금은 테스트로 임의 지정)
		COLORREF bgColor;
		if (m_slotStates[i] == SLOT_USED)
			bgColor = RGB(245, 229, 176);
		else if (m_slotStates[i] == SLOT_FAULT)
			bgColor = RGB(247, 193, 193);
		else
			bgColor = RGB(180, 224, 190);

		CBrush brush(bgColor);
		CBrush* oldBrush = pDC->SelectObject(&brush);

		CPen pen(PS_SOLID, 1, RGB(200, 200, 200));
		CPen* oldPen = pDC->SelectObject(&pen);
		pDC->RoundRect(x, y, x + cellSize, y + cellSize, 8, 8);

		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);

		// 슬롯 번호 텍스트
		CString text;
		text.Format(_T("%02d"), i + 1);

		CRect slotNumber = cellRect;
		slotNumber.bottom = cellRect.top + (cellRect.Height() * 6 / 10);

		pDC->SelectObject(&fontSlot);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(text, slotNumber, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		// 경과 시간 텍스트
		CString elapsedTime;
		if (m_slotRunning[i]) {
			DWORD elapsed = GetTickCount() - m_startTick[i];
			int totalSec = elapsed / 1000;

			m_totalUsageSec += totalSec;
			m_completedCount++;

			int hours = totalSec / 3600;
			int minutes = (totalSec % 3600) / 60;
			elapsedTime.Format(_T("%02d:%02d"), hours, minutes);
		}
		else if (m_slotStates[i] == SLOT_FAULT) {
			elapsedTime = _T("고장");
		}
		else {
			elapsedTime = _T("--:--");
		}

		CRect timeRect = cellRect;
		timeRect.top = slotNumber.bottom;
		timeRect.DeflateRect(0, 8);

		pDC->SelectObject(&fontTime);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(elapsedTime, timeRect, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
	}

	pDC->SelectObject(oldFont);
}

// 강제 개방
bool CMFCApplication2Dlg::WriteHoldingRegister(UINT16 regAddr, UINT16 value)
{
	if (ConnectionFlag == 1 || client_sock == INVALID_SOCKET)
	{
		mAlarmlog += L"[TCP 서버] 연결 끊김. 재연결 대기 중...\r\n";
		UpdateData(FALSE);
		return false;
	}

	m_transactionId++;

	BYTE req[12] = {};
	req[0] = HIBYTE(m_transactionId);
	req[1] = LOBYTE(m_transactionId);
	req[2] = 0x00; req[3] = 0x00;
	req[4] = 0x00; req[5] = 0x06;
	req[6] = 0x01;
	req[7] = 0x06;
	req[8] = HIBYTE(regAddr);
	req[9] = LOBYTE(regAddr);
	req[10] = HIBYTE(value);
	req[11] = LOBYTE(value);

	int sent = send(client_sock, (char*)req, sizeof(req), 0);
	if (sent == SOCKET_ERROR)
	{
		ErrDisplay(L"send()");
		return false;
	}

	return true;
}

void CMFCApplication2Dlg::OnBnClickedDoor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (WriteHoldingRegister(9, 0x0001))
	{
		mAlarmlog += L"[관리자 권한] 도어 강제 개방\r\n";
		UpdateData(FALSE);
		Sleep(2000);
		WriteHoldingRegister(9, 0x0000);
		UpdateData(FALSE);
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCApplication2Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1) {
		Connection();
		ReadHoldingRegs();
		Invalidate();
	}
	else if (nIDEvent == 2)
	{
		CTime now = CTime::GetCurrentTime();
		CString strNow = now.Format(_T("%Y-%m-%d %H:%M:%S"));

		SetDlgItemText(IDC_DATE, strNow);
	} else if (nIDEvent == 3) {
		WriteRegs(144, false);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CMFCApplication2Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	closesocket(client_sock);
	WSACleanup();
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
}


