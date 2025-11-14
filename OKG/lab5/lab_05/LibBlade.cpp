#include "stdafx.h"

CRectD::CRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}
//------------------------------------------------------------------------------
void CRectD::SetRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

//------------------------------------------------------------------------------
CSizeD CRectD::SizeD()
{
	CSizeD cz;
	cz.cx = fabs(right - left);	// Ширина прямоугольной области
	cz.cy = fabs(top - bottom);	// Высота прямоугольной области
	return cz;
}

//----------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy)
// Формирует матрицу для преобразования координат объекта при его смещении 
// на dx по оси X и на dy по оси Y в фиксированной системе координат
{
	CMatrix TM(3, 3);
	TM(0, 0) = 1; TM(0, 2) = dx;
	TM(1, 1) = 1;  TM(1, 2) = dy;
	TM(2, 2) = 1;
	return TM;
}

//------------------------------------------------------------------------------------
CMatrix CreateRotate2D(double fi)
// Формирует матрицу для преобразования координат объекта при его повороте
// на угол fi (при fi>0 против часовой стрелки)в фиксированной системе координат
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * pi; // Перевод в радианы
	CMatrix RM(3, 3);
	RM(0, 0) = cos(ff); RM(0, 1) = -sin(ff);
	RM(1, 0) = sin(ff);  RM(1, 1) = cos(ff);
	RM(2, 2) = 1;
	return RM;
}


//------------------------------------------------------------------------------

CMatrix SpaceToWindow(CRectD& RS, CRect& RW)
// Возвращает матрицу пересчета координат из мировых в оконные
// RS - область в мировых координатах - double
// RW - область в оконных координатах - int
{
	CMatrix M(3, 3);
	CSize sz = RW.Size();	 // Размер области в ОКНЕ
	int dwx = sz.cx;	     // Ширина
	int dwy = sz.cy;	     // Высота
	CSizeD szd = RS.SizeD(); // Размер области в МИРОВЫХ координатах

	double dsx = szd.cx;    // Ширина в мировых координатах
	double dsy = szd.cy;    // Высота в мировых координатах

	double kx = (double)dwx / dsx;   // Масштаб по x
	double ky = (double)dwy / dsy;   // Масштаб по y

	M(0, 0) = kx;  M(0, 1) = 0;    M(0, 2) = (double)RW.left - kx * RS.left;
	M(1, 0) = 0;   M(1, 1) = -ky;  M(1, 2) = (double)RW.bottom + ky * RS.bottom;
	M(2, 0) = 0;   M(2, 1) = 0;    M(2, 2) = 1;
	return M;
}

//------------------------------------------------------------------------------

void SetMyMode(CDC& dc, CRectD& RS, CRect& RW)
// Устанавливает режим отображения MM_ANISOTROPIC и его параметры
// dc - ссылка на класс CDC MFC
// RS -  область в мировых координатах - int
// RW -	 Область в оконных координатах - int  
{
	double dsx = RS.right - RS.left;
	double dsy = RS.top - RS.bottom;
	double xsL = RS.left;
	double ysL = RS.bottom;

	int dwx = RW.right - RW.left;
	int dwy = RW.bottom - RW.top;
	int xwL = RW.left;
	int ywH = RW.bottom;

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt((int)dsx, (int)dsy);
	dc.SetViewportExt(dwx, -dwy);
	dc.SetWindowOrg((int)xsL, (int)ysL);
	dc.SetViewportOrg(xwL, ywH);
}

CBlade::CBlade()            
{
	double rS = 30;       
	double RoE = 10 * rS;	 
	double d = RoE;		    
	// все что рисуем будет переводиться из RS в RW через пересчет SpaceToWindow
	RS.SetRectD(-d, d, d, -d); // Область системы в мировых координатах
	RW.SetRect(0, 0, 690, 640); // Область в окне. то что видит пользователь
	MainPoint.SetRect(-rS, rS, rS, -rS);
	FirstTop.SetRect(-5, 5, 5, -5);
	SecondTop.SetRect(-5, 5, 5, -5);
	FirstBootom.SetRect(-5, 5, 5, -5);
	SecondBootom.SetRect(-5, 5, 5, -5);
	WayRotation.SetRect(-RoE, RoE, RoE, -RoE);
	// fiFT, fiST, и т.д. — углы поворота
	fiSB = 215;
	fiFB = 205;			
	fiST = 25;			
	fiFT = 35; //угол положение для FT
	fiHSB = 5;
	fiHFB = 355;
	fiHST = 175;
	fiHFT = 185;
	wPoint = -8;
	dt = 0.1;
	// FTCoords, STCoords — координаты вершин треугольников
	FTCoords.RedimMatrix(3);
	STCoords.RedimMatrix(3);
	FBCoords.RedimMatrix(3);
	SBCoords.RedimMatrix(3);
	FTHCoords.RedimMatrix(3);
	STHCoords.RedimMatrix(3);
	FBHCoords.RedimMatrix(3);
	SBHCoords.RedimMatrix(3);
}

// раз в кадр пересчитываешь, где должны находиться вершины всех лопастей, 
// исходя из текущих углов поворота
void CBlade::SetNewCoords()
{
	double RoV = (WayRotation.right - WayRotation.left) / 2;
	double ff = (fiFT / 90.0) * pi;
	double x = RoV * cos(ff);
	double y = RoV * sin(ff);
	FTCoords(0) = x;
	FTCoords(1) = y;
	FTCoords(2) = 1;
	// wPoint, dt — скорость и шаг анимации
	fiFT += wPoint * dt;
	//CMatrix P = CreateRotate2D(fiFT); // на fi по окружности
	//FTCoords = P * FTCoords;

	// аффинное вращение вокруг произвольной точки
	// сдвигаем, крутим, сдвигаем обратно
	CMatrix T1 = CreateTranslate2D(-20, -10);
	CMatrix R = CreateRotate2D(fiFT);
	CMatrix T2 = CreateTranslate2D(20, 10);

	// === STCoords ===
	CMatrix P = T2 * R * T1;
	FTCoords = P * FTCoords;
	RoV = (WayRotation.right - WayRotation.left) / 2;// WayRotation — круг, по которому всё крутится
	ff = (fiST / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	STCoords(0) = x;
	STCoords(1) = y;
	STCoords(2) = 1;

	fiST += wPoint * dt;
	T1 = CreateTranslate2D(-20, -10);
	R = CreateRotate2D(fiST);
	T2 = CreateTranslate2D(20, 10);
	P = T2 * R * T1;
	STCoords = P * STCoords;

	// === FBCoords ===
	RoV = (WayRotation.right - WayRotation.left) / 2;
	ff = (fiFB / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	FBCoords(0) = x;
	FBCoords(1) = y;
	FBCoords(2) = 1;

	fiFB += wPoint * dt;
	T1 = CreateTranslate2D(-20, -10);
	R = CreateRotate2D(fiFB);
	T2 = CreateTranslate2D(20, 10);
	P = T2 * R * T1;
	FBCoords = P * FBCoords;

	// === SBCoords ===
	RoV = (WayRotation.right - WayRotation.left) / 2;
	ff = (fiSB / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	SBCoords(0) = x;
	SBCoords(1) = y;
	SBCoords(2) = 1;

	fiSB += wPoint * dt;
	T1 = CreateTranslate2D(-20, -10);
	R = CreateRotate2D(fiSB);
	T2 = CreateTranslate2D(20, 10);
	P = T2 * R * T1;
	SBCoords = P * SBCoords;

	// === FTHCoords ===
	// для двух синих треугольников
	double horizontalFF = (fiHFT / 90.0) * pi;
	double horizontalX = RoV * cos(horizontalFF);
	double horizontalY = RoV * sin(horizontalFF);
	FTHCoords(0) = horizontalX;
	FTHCoords(1) = horizontalY;
	FTHCoords(2) = 1;

	fiHFT += wPoint * dt;
	T1 = CreateTranslate2D(-20, -10);
	CMatrix horizontalP = CreateRotate2D(fiHFT);
	T2 = CreateTranslate2D(20, 10);
	FTHCoords = horizontalP * FTHCoords;

	// === STHCoords ===
	double horizontalSF = (fiHST / 90.0) * pi;
	horizontalX = RoV * cos(horizontalSF);
	horizontalY = RoV * sin(horizontalSF);
	STHCoords(0) = horizontalX;
	STHCoords(1) = horizontalY;
	STHCoords(2) = 1;

	fiHST += wPoint * dt;
	T1 = CreateTranslate2D(-20, -10);
	horizontalP = CreateRotate2D(fiHST);
	T2 = CreateTranslate2D(20, 10);
	STHCoords = horizontalP * STHCoords;

	double horizontalBF = (fiHFB / 90.0) * pi;
	horizontalX = RoV * cos(horizontalBF);
	horizontalY = RoV * sin(horizontalBF);
	FBHCoords(0) = horizontalX;
	FBHCoords(1) = horizontalY;
	FBHCoords(2) = 1;

	fiHFB += wPoint * dt;
	horizontalP = CreateRotate2D(fiHFB);
	FBHCoords = horizontalP * FBHCoords;

	double horizontalSBF = (fiHSB / 90.0) * pi;
	horizontalX = RoV * cos(horizontalSBF);
	horizontalY = RoV * sin(horizontalSBF);
	SBHCoords(0) = horizontalX;
	SBHCoords(1) = horizontalY;
	SBHCoords(2) = 1;

	fiHSB += wPoint * dt;
	horizontalP = CreateRotate2D(fiHSB);
	SBHCoords = horizontalP * SBHCoords;
}


void CBlade::Draw(CDC& dc)
{
	CBrush SBrush, EBrush, MBrush, VBrush, * pOldBrush;
	CRect R;

	SBrush.CreateSolidBrush(RGB(0, 0, 255));
	EBrush.CreateSolidBrush(RGB(0, 0, 255));
	MBrush.CreateSolidBrush(RGB(0, 255, 0));
	VBrush.CreateSolidBrush(RGB(0, 128, 0));


	dc.MoveTo(0, 0);
	dc.LineTo(FTCoords(0), FTCoords(1));
	DrawTriangle(FTCoords, STCoords, dc, true);
	DrawTriangle(FBCoords, SBCoords, dc, true);
	DrawTriangle(FTHCoords, STHCoords, dc, false);
	DrawTriangle(FBHCoords, SBHCoords, dc, false);
	dc.SelectObject(&VBrush);
	dc.Ellipse(MainPoint);  

	pOldBrush = dc.SelectObject(&SBrush);	// Цвет красный

	dc.SelectObject(pOldBrush);				//Восстанавливаем контекст по pOldBrush 
}
void CBlade::DrawTriangle(CMatrix FTCoords, CMatrix STCoords, CDC& dc, bool color) 
{

	if (color) {
		CPen Pen(PS_SOLID, 1, RGB(255, 0, 0));
		dc.SelectObject(&Pen);

		CBrush Brush(RGB(255, 0, 0));
		dc.SelectObject(&Brush);

		CPoint points[3];
		points[0] = CPoint(0, 0);
		points[1] = CPoint(FTCoords(0), FTCoords(1));
		points[2] = CPoint(STCoords(0), STCoords(1));

		dc.Polygon(points, 3);
	}
	else {
		CPen Pen(PS_SOLID, 1, RGB(0, 0, 255));
		dc.SelectObject(&Pen);

		CBrush Brush(RGB(0, 0, 255));
		dc.SelectObject(&Brush);

		CPoint points[3];
		points[0] = CPoint(0, 0);
		points[1] = CPoint(FTCoords(0), FTCoords(1));
		points[2] = CPoint(STCoords(0), STCoords(1));

		dc.Polygon(points, 3);
	}

}


void CBlade::GetRS(CRectD& RSX)
// RS - структура, куда записываются параметры области графика
{
	RSX.left = RS.left;
	RSX.top = RS.top;
	RSX.right = RS.right;
	RSX.bottom = RS.bottom;
}







