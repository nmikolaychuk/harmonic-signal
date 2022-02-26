#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <math.h>

using namespace std;

struct Signal
{
	// Буфер для хранения куска сигнала.
	vector<double> signal;
	// Время последнего временного отсчёта.
	int t = 0;
};

// Диалоговое окно CharmonicsignalDlg
class CharmonicsignalDlg : public CDialogEx
{
// Создание
public:
	CharmonicsignalDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HARMONIC_SIGNAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Переменные области рисования.
	CWnd* PicWnd;
	CDC* PicDc;
	CRect Pic;

	// Инструменты рисования.
	CPen axes_pen;
	CPen grid_pen;
	CPen signal_pen;

	// Переменные, связанные с масштабом, отрисовкой.
	double Min, Max;
	double xx0, xxmax, yy0, yymax, xxi, yyi;
	char axes_values_buffer[1000];

	// Глобальные переменные.
	Signal sgn;

	// Функции.
	void DrawSignal(Signal sgn, CDC* WinDc, CRect WinPic, CPen* graphpen);
	void GenerateSignal(double ampl, double start_phase, double sampling_freq, int duration);

	// Обработчики событий.
	afx_msg void OnBnClickedButtonGetSignal();
	afx_msg void OnBnClickedButtonClear();

	// Переменные интерфейса.
	double e_amplitude;
	double e_start_phase;
	double e_sampling_freq;
	int e_duration;
	CEdit ctrl_e_amplitude;
	CEdit ctrl_e_start_phase;
	CEdit ctrl_e_sampling_freq;
};
