#pragma once
#include "pch.h"
#include "framework.h"
#include "harmonic_signal.h"
#include "harmonic_signalDlg.h"
#include "afxdialogex.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SCALE(x,y) (xp*((x)-xmin)),(yp*((y)-ymax))

CharmonicsignalDlg::CharmonicsignalDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HARMONIC_SIGNAL_DIALOG, pParent)
	, e_amplitude(1)
	, e_start_phase(2. * M_PI)
	, e_sampling_freq(44100)
	, e_duration(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CharmonicsignalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AMPL, e_amplitude);
	DDX_Text(pDX, IDC_EDIT_PHASE, e_start_phase);
	DDX_Text(pDX, IDC_EDIT_FREQ, e_sampling_freq);
	DDX_Text(pDX, IDC_EDIT_DURATION, e_duration);
	DDX_Control(pDX, IDC_EDIT_AMPL, ctrl_e_amplitude);
	DDX_Control(pDX, IDC_EDIT_PHASE, ctrl_e_start_phase);
	DDX_Control(pDX, IDC_EDIT_FREQ, ctrl_e_sampling_freq);
}

BEGIN_MESSAGE_MAP(CharmonicsignalDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GET_SIGNAL, &CharmonicsignalDlg::OnBnClickedButtonGetSignal)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CharmonicsignalDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()

BOOL CharmonicsignalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	PicWnd = GetDlgItem(IDC_SIGNAL_ZONE);
	PicDc = PicWnd->GetDC();
	PicWnd->GetClientRect(&Pic);

	grid_pen.CreatePen(PS_DOT, -1, RGB(0, 0, 0));
	axes_pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	signal_pen.CreatePen(PS_SOLID, -1, RGB(255, 0, 0));

	UpdateData(FALSE);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CharmonicsignalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CharmonicsignalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CharmonicsignalDlg::DrawSignal(Signal sgn, CDC* WinDc, CRect WinPic, CPen* graphpen)
{
	// отрисовка
	CBitmap bmp;
	CDC* MemDc;
	MemDc = new CDC;
	MemDc->CreateCompatibleDC(WinDc);
	bmp.CreateCompatibleBitmap(WinDc, WinPic.Width(), WinPic.Height());
	CBitmap* pBmp = (CBitmap*)MemDc->SelectObject(&bmp);
	// заливка фона графика белым цветом
	MemDc->FillSolidRect(WinPic, RGB(240, 240, 240));
	// отрисовка сетки координат
	MemDc->SelectObject(&grid_pen);

	int duration = sgn.signal.size();
	if (duration == 0) {
		WinDc->BitBlt(0, 0, WinPic.Width(), WinPic.Height(), MemDc, 0, 0, SRCCOPY);
		delete MemDc;
		return;
	}

	// поиск максимального и минимального значения
	Min = sgn.signal[0];
	Max = sgn.signal[0];
	for (int i = 1; i < duration; i++)
	{
		if (sgn.signal[i] < Min)
		{
			Min = sgn.signal[i];
		}
		if (sgn.signal[i] > Max)
		{
			Max = sgn.signal[i];
		}
	}

	// отрисовка осей
	MemDc->SelectObject(&axes_pen);
	// отрисовка оси X
	//создаём Ось Y
	MemDc->MoveTo(0, WinPic.Height() / 2);
	MemDc->LineTo(WinPic.Width(), WinPic.Height() / 2);
	// отрисовка оси Y
	MemDc->MoveTo(WinPic.Width() / 15, WinPic.Height());
	MemDc->LineTo(WinPic.Width() / 15, 0);
	// установка прозрачного фона текста
	MemDc->SetBkMode(TRANSPARENT);
	// установка шрифта
	CFont font;
	font.CreateFontW(14.5, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Courier"));
	MemDc->SelectObject(&font);
	// подпись оси X
	MemDc->TextOut(WinPic.Width() * 14 / 15, WinPic.Height() / 2, CString("t, с"));
	// подпись оси Y
	MemDc->TextOut(WinPic.Width() / 15 + 10, 0, CString("A"));
	// выбор области для рисования
	xx0 = WinPic.Width() * 1 / 15; xxmax = WinPic.Width();
	yy0 = WinPic.Height() / 10; yymax = WinPic.Height() * 9 / 10;
	// отрисовка
	MemDc->SelectObject(graphpen);
	MemDc->MoveTo(xx0, yymax + (sgn.signal[0] - Min) / (Max - Min) * (yy0 - yymax));
	for (int i = 0; i < duration; i++)
	{
		xxi = xx0 + (xxmax - xx0) * i / (duration - 1);
		yyi = yymax + (sgn.signal[i] - Min) / (Max - Min) * (yy0 - yymax);
		MemDc->LineTo(xxi, yyi);
	}
	// вывод на экран
	WinDc->BitBlt(0, 0, WinPic.Width(), WinPic.Height(), MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

void CharmonicsignalDlg::GenerateSignal(double ampl, double start_phase, double sampling_freq, int duration) {
	int length_of_signal = round(sampling_freq * (double)duration) + sgn.t;
	// Частота сигнала, Гц.
	double freq = 1.5;
	// Коэффициент увеличения частоты.
	double factor = 3.e-4;

	sgn.signal.clear();
	for (int t = sgn.t; t < length_of_signal; ++t) {
		// Вычисление частоты.
		double w = 2. * M_PI * freq;
		// Добавка, зависящая от времени.
		w += factor * t;

		sgn.signal.push_back(ampl * sin(w * t / sampling_freq + start_phase));
	}
	sgn.t = length_of_signal - 1;
}

void CharmonicsignalDlg::OnBnClickedButtonGetSignal()
{
	UpdateData(TRUE);
	sgn.signal.clear();

	// Генерация сигнала.
	GenerateSignal(e_amplitude, e_start_phase, e_sampling_freq, e_duration);
	// Отрисовка сигнала.
	DrawSignal(sgn, PicDc, Pic, &signal_pen);
	// Блокировка полей изменения параметров.
	// Т.к. новый сигнал - продолжение.
	ctrl_e_amplitude.EnableWindow(false);
	ctrl_e_start_phase.EnableWindow(false);
	ctrl_e_sampling_freq.EnableWindow(false);
}

void CharmonicsignalDlg::OnBnClickedButtonClear()
{
	sgn.signal.clear();
	sgn.t = 0;
	DrawSignal(sgn, PicDc, Pic, &signal_pen);
	// Разблокировка полей изменения параметров.
	// Т.к. после очистки рассматривается новый сигнал.
	ctrl_e_amplitude.EnableWindow(true);
	ctrl_e_start_phase.EnableWindow(true);
	ctrl_e_sampling_freq.EnableWindow(true);
}
