#include "framework.h"
#include "Ghosts attack.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "ErrH.h"
#include "FCheck.h"
#include "D2BMPLOADER.h"
#include "gifresizer.h"
#include "platsrv.h"
#include <chrono>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "platsrv.lib")

constexpr wchar_t bWinClassName[]{ L"Ghosts" };
constexpr char tmp_file[]{ ".\\res\\data\\temp.dat" };
constexpr wchar_t Ltmp_file[]{ L".\\res\\data\\temp.dat" };
constexpr wchar_t snd_file[]{ L".\\res\\snd\\main.wav" };
constexpr wchar_t record_file[]{ L".\\res\\data\\record.dat" };
constexpr wchar_t help_file[]{ L".\\res\\data\\help.dat" };
constexpr wchar_t save_file[]{ L".\\res\\data\\save.dat" };

constexpr int mNew{ 1001 };
constexpr int mSpeed{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mLoad{ 1004 };
constexpr int mSave{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int no_record{ 2001 };
constexpr int first_record{ 2002 };
constexpr int record{ 2003 };

WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HICON bIcon{ nullptr };
HCURSOR bCursor{ nullptr };
HCURSOR outCursor{ nullptr };
HDC PaintDC{ nullptr };
PAINTSTRUCT bPaint{};
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
HWND bHwnd{ nullptr };
MSG bMsg{ nullptr };
BOOL bRet{ 0 };
UINT bTimer{ 0 };

POINT cur_pos{};

wchar_t current_player[16]{ L"TARLYO" };

D2D1_RECT_F FullScreen{ 0, 0, scr_width, scr_height };
D2D1_RECT_F GameScreen{ 0, 50.0f, sky, ground };

D2D1_RECT_F b1Rect{ 30.0f, 10.0f, scr_width / 3.0f - 30.0f, 40.0f };
D2D1_RECT_F b2Rect{ scr_width / 3.0f + 30.0f, 10.0f, scr_width * 2.0f / 3.0f - 30.0f, 40.0f };
D2D1_RECT_F b3Rect{ scr_width * 2.0f / 3.0f + 30.0f, 10.0f, scr_width - 30.0f, 40.0f };

D2D1_RECT_F b1TxtRect{ 50.0f, 15.0f, scr_width / 3.0f - 30.0f, 40.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3.0f + 50.0f, 15.0f, scr_width * 2.0f / 3.0f - 30.0f, 40.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2.0f / 3.0f + 40.0f, 15.0f, scr_width - 30.0f, 40.0f };

bool pause{ false };
bool sound{ true };
bool in_client{ true };
bool show_help{ false };
bool level_skipped{ false };
bool name_set{ false };
bool b1Hglt{ false };
bool b2Hglt{ false };
bool b3Hglt{ false };

float x_scale{ 0 };
float y_scale{ 0 };

float level = 1.0f;

int score = 0;
int mins = 0;
int secs = 0;

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

ID2D1SolidColorBrush* statBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmFormat{ nullptr };
IDWriteTextFormat* midFormat{ nullptr };
IDWriteTextFormat* bigFormat{ nullptr };

ID2D1Bitmap* bmpShot{ nullptr };
ID2D1Bitmap* bmpArmor{ nullptr };
ID2D1Bitmap* bmpChest{ nullptr };
ID2D1Bitmap* bmpLife{ nullptr };
ID2D1Bitmap* bmpGun{ nullptr };

ID2D1Bitmap* bmpLogoLevel{ nullptr };
ID2D1Bitmap* bmpLogoWin{ nullptr };
ID2D1Bitmap* bmpLogoLoose{ nullptr };
ID2D1Bitmap* bmpLogoRecord{ nullptr };

ID2D1Bitmap* bmpBoulder{ nullptr };
ID2D1Bitmap* bmpRock{ nullptr };
ID2D1Bitmap* bmpTree1{ nullptr };
ID2D1Bitmap* bmpTree2{ nullptr };
ID2D1Bitmap* bmpTree3{ nullptr };

ID2D1Bitmap* bmpDirt{ nullptr };
ID2D1Bitmap* bmpGrass{ nullptr };
ID2D1Bitmap* bmpGrassDirt{ nullptr };
ID2D1Bitmap* bmpGrassFB{ nullptr };
ID2D1Bitmap* bmpGrassFR{ nullptr };

ID2D1Bitmap* bmpIntro[106]{ nullptr };

ID2D1Bitmap* bmpHeroHor[2]{ nullptr };
ID2D1Bitmap* bmpHeroVerD[2]{ nullptr };
ID2D1Bitmap* bmpHeroVerU[2]{ nullptr };

ID2D1Bitmap* bmpBrain[202]{ nullptr };
ID2D1Bitmap* bmpDervish[68]{ nullptr };
ID2D1Bitmap* bmpGhost[223]{ nullptr };
ID2D1Bitmap* bmpSoul[120]{ nullptr };

//////////////////////////////////////////////////////

template<typename T>concept HasRelease = requires(T var)
{
	var.Release();
};
template<HasRelease T>bool FreeMem(T** var)
{
	if (*var)
	{
		(*var)->Release();
		(*var) = nullptr;
		return true;
	}

	return false;
};
void LogErr(const wchar_t* what_happened)
{
	std::wofstream err(L".\\res\\data\\error.log", std::ios::app);
	err << what_happened << L" Time stamp: " << std::chrono::system_clock::now() << std::endl;
	err.close();
}
void ClearResources()
{
	if (!FreeMem(&iFactory))LogErr(L"Error releasing D2D1 iFactory !");
	if (!FreeMem(&Draw))LogErr(L"Error releasing D2D1 HwndRenderTarget !");
	if (!FreeMem(&b1BckgBrush))LogErr(L"Error releasing D2D1 b1BckgBrush !");
	if (!FreeMem(&b2BckgBrush))LogErr(L"Error releasing D2D1 b2BckgBrush !");
	if (!FreeMem(&b3BckgBrush))LogErr(L"Error releasing D2D1 b3BckgBrush !");
	if (!FreeMem(&statBrush))LogErr(L"Error releasing D2D1 statBrush !");
	if (!FreeMem(&txtBrush))LogErr(L"Error releasing D2D1 txtBrush !");
	if (!FreeMem(&hgltBrush))LogErr(L"Error releasing D2D1 hgltBrush !");
	if (!FreeMem(&inactBrush))LogErr(L"Error releasing D2D1 inactBrush !");

	if (!FreeMem(&iWriteFactory))LogErr(L"Error releasing D2D1 iWriteFactory !");
	if (!FreeMem(&nrmFormat))LogErr(L"Error releasing D2D1 nrmFormat !");
	if (!FreeMem(&midFormat))LogErr(L"Error releasing D2D1 midFormat !");
	if (!FreeMem(&bigFormat))LogErr(L"Error releasing D2D1 bigFormat !");

	if (!FreeMem(&bmpShot))LogErr(L"Error releasing D2D1 bmpShot !");
	if (!FreeMem(&bmpArmor))LogErr(L"Error releasing D2D1 bmpArmor !");
	if (!FreeMem(&bmpChest))LogErr(L"Error releasing D2D1 bmpChest !");
	if (!FreeMem(&bmpLife))LogErr(L"Error releasing D2D1 bmpLife !");
	if (!FreeMem(&bmpGun))LogErr(L"Error releasing D2D1 bmpGun !");

	if (!FreeMem(&bmpLogoLevel))LogErr(L"Error releasing D2D1 bmpLogoLevel !");
	if (!FreeMem(&bmpLogoWin))LogErr(L"Error releasing D2D1 bmpLogoWin !");
	if (!FreeMem(&bmpLogoLoose))LogErr(L"Error releasing D2D1 bmpLogoLoose !");
	if (!FreeMem(&bmpLogoRecord))LogErr(L"Error releasing D2D1 bmpLogoRecord !");

	if (!FreeMem(&bmpBoulder))LogErr(L"Error releasing D2D1 bmpBoulder !");
	if (!FreeMem(&bmpRock))LogErr(L"Error releasing D2D1 bmpRock !");
	if (!FreeMem(&bmpTree1))LogErr(L"Error releasing D2D1 bmpTree1 !");
	if (!FreeMem(&bmpTree2))LogErr(L"Error releasing D2D1 bmpTree2 !");
	if (!FreeMem(&bmpTree3))LogErr(L"Error releasing D2D1 bmpTree3 !");

	if (!FreeMem(&bmpDirt))LogErr(L"Error releasing D2D1 bmpDirt !");
	if (!FreeMem(&bmpGrassDirt))LogErr(L"Error releasing D2D1 bmpGrassDirt !");
	if (!FreeMem(&bmpGrass))LogErr(L"Error releasing D2D1 bmpGrass !");
	if (!FreeMem(&bmpGrassFB))LogErr(L"Error releasing D2D1 bmpGrassFB !");
	if (!FreeMem(&bmpGrassFR))LogErr(L"Error releasing D2D1 bmpGrassFR !");

	for (int i = 0; i < 106; ++i)if (!FreeMem(&bmpIntro[i]))LogErr(L"Error releasing D2D1 bmpIntro !");

	for (int i = 0; i < 2; ++i)if (!FreeMem(&bmpHeroHor[i]))LogErr(L"Error releasing D2D1 bmpHeroHor !");
	for (int i = 0; i < 2; ++i)if (!FreeMem(&bmpHeroVerD[i]))LogErr(L"Error releasing D2D1 bmpHeroVerD !");
	for (int i = 0; i < 2; ++i)if (!FreeMem(&bmpHeroVerU[i]))LogErr(L"Error releasing D2D1 bmpHeroVerU !");

	for (int i = 0; i < 202; ++i)if (!FreeMem(&bmpBrain[i]))LogErr(L"Error releasing D2D1 bmpBrain !");
	for (int i = 0; i < 68; ++i)if (!FreeMem(&bmpDervish[i]))LogErr(L"Error releasing D2D1 bmpDervish !");
	for (int i = 0; i < 223; ++i)if (!FreeMem(&bmpGhost[i]))LogErr(L"Error releasing D2D1 bmpGhost !");
	for (int i = 0; i < 120; ++i)if (!FreeMem(&bmpSoul[i]))LogErr(L"Error releasing D2D1 bmpSoul !");
}










int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	bIns = hInstance;






    return (int) msg.wParam;
}