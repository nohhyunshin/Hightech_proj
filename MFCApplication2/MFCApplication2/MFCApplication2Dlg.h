
// MFCApplication2Dlg.h: 헤더 파일
//

#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 512

// 슬롯 상태 정의
enum SlotState
{
	SLOT_EMPTY = 0,		// 비어있음
	SLOT_USED = 1,		// 사용중
	SLOT_FAULT = 2		// 고장
};

// CMFCApplication2Dlg 대화 상자
class CMFCApplication2Dlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCApplication2Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void SetFontSize();		// 폰트 크기 설정
	CFont m_label;			// 그룹 박스 헤더용
	CFont m_fontSize;		// 실제 데이터 값용

	int baseWidth;
	int baseHeight;


	// PLC 연결 상태
	void DrawConnectionPoint(CDC* pDC);

	int ConnectionFlag;
	int retval;
	SOCKET client_sock;
	WSADATA wsa;

	void ErrQuit(wchar_t* msg);
	void ErrDisplay(wchar_t* msg);
	void Connection();
	int InitServer();
	void ReadHoldingRegs();
	int addrlen;
	char buf[BUFSIZE + 1];


	// Static Text - 실제 데이터 값
	CStatic cUptime;
	CStatic cOccupied;
	CStatic cDailyusage;
	CStatic cAvgtime;
	CStatic cAlarm;

	
	// PLC 데이터 값
	int dailyUsage;
	CString logLine;
	CString Status;					// 가동률
	CString Occupied;				// 사용 중인 슬롯 개수
	CString Usage;					// 금일 사용량		
	void WriteRegs(WORD address, bool value);
	//void DataFromPLC();

	// List Box - 슬롯 상태 표시 (사용 중인지 아닌지)
	void UpdateSlotStatus(CDC* pDC);
	SlotState m_slotStates[10];		// 슬롯 상태 배열

	CString mAlarmlog;				// 로그 데이터
	bool m_bWaitingLogged;			// 로그 대기 상태 플래그
	ULONGLONG m_startTick[8];		// 슬롯별 시작 시간
	bool m_slotRunning[8];			// 슬롯별 측정 중 여부
	WORD m_prevReg[5];				// 이전 레지스터 값 저장 (로그 알림용)
	DWORD m_totalUsageSec = 0;		// 누적 이용 시간
	int m_completedCount = 0;


	// 관리자 권한
	bool WriteHoldingRegister(UINT16 regAddr, UINT16 value);
	afx_msg void OnBnClickedDoor();

private :
	UINT16 m_transactionId = 0;
};
