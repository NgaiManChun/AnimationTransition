// =======================================================
// main.cpp
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG 
int AllocBreakPoint() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(208458);
	
	return 0;
}
static int _AllocBreakPoint = AllocBreakPoint();
#endif

#include "keyboard.h"
#include "CSVResource.h"
#include "MGCommon.h"
#include "rendererDX.h"
#include "resourceToolDX.h"
#include "drawToolDX.h"
#include "audioToolDX.h"
#include "scene.h"
#include "sceneTransitaion.h"
#include "input.h"
using namespace MG;

// =======================================================
// マクロ定義
// =======================================================
#define CLASS_NAME "AnimationTransition"
#define WINDOW_CAPTION "AnimationTransition"
#define INIT_SCENE "test"
#define SCREEN_WIDTH (1920.0f)
#define SCREEN_HEIGHT (1080.0f)
#define SCREEN_CENTER_X (SCREEN_WIDTH * 0.5f)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT * 0.5f)
#define INIT_SCREEN_WIDTH (960.0f)
#define INIT_SCREEN_HEIGHT (540.0f)

// =======================================================
// プロトタイプ宣言
// =======================================================
LRESULT	CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT	Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
HRESULT InitDirectInput(HINSTANCE hInstance);
HRESULT InitGamepad(HWND hWnd);
BOOL CALLBACK EnumGamepadsCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
void UpdateGamepad();


// =======================================================
// グローバル変数
// =======================================================
static RendererDX* renderer;
static ResourceToolDX* resourceTool;
static DrawToolDX* drawTool;
static AudioToolDX* audioTool;
static int joyDevices;

// =======================================================
// メイン関数
// =======================================================
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmd, int nCmdShow)
{
	// フレームレート計測用変数
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	//COMコンポーネントの準備（機能を部品化して外部のプログラムから共有利用する仕組み）
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	//ウィンドウクラスの登録（ウィンドウの仕様的な物を決めてWindowsへセットする）
	WNDCLASS	wc;	//構造体を準備
	ZeroMemory(&wc, sizeof(WNDCLASS));//内容を０で初期化
	wc.lpfnWndProc = WndProc;	//コールバック関数のポインター
	wc.lpszClassName = CLASS_NAME;	//この仕様書の名前
	wc.hInstance = hInstance;	//このアプリケーションのこと
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソルの種類
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);//ウィンドウの背景色
	RegisterClass(&wc);	//構造体をWindowsへセット

	//ウィンドウサイズの調整
	RECT rc = { 0, 0, INIT_SCREEN_WIDTH, INIT_SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZE ^ WS_THICKFRAME, FALSE);

	//ウィンドウの作成
	HWND	hWnd = CreateWindow(
		CLASS_NAME,	//作りたいウィンドウ
		WINDOW_CAPTION,	//ウィンドウに表示するタイトル
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZE ^ WS_THICKFRAME,	//標準的な形状のウィンドウ サイズ変更禁止
		CW_USEDEFAULT,		//デフォルト設定でおまかせ
		CW_USEDEFAULT,
		rc.right - rc.left,//CW_USEDEFAULT,//ウィンドウの幅
		rc.bottom - rc.top,//CW_USEDEFAULT,//ウィンドウの高さ
		NULL,
		NULL,
		hInstance,		//アプリケーションのハンドル
		NULL
	);

	//初期化処理
	if (FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;//初期化失敗
	}

	//作成したウィンドウを表示する
	ShowWindow(hWnd, nCmdShow);//引数に従って表示、または非表示
	//ウィンドウの内容を強制表示
	UpdateWindow(hWnd);

	//メッセージループ
	MSG	msg;
	ZeroMemory(&msg, sizeof(MSG));//メッセージ構造体を作成して初期化

	//joyDevices = JslConnectDevices();

	// フレームレート計測初期化
	timeBeginPeriod(1); // タイマーの分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;

	//終了メッセージが来るまでループする
	//ゲームループ
	while (1)
	{	//メッセージの有無をチェック
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ //Windowsからメッセージが来た！
			if (msg.message == WM_QUIT)//完全終了しましたメッセージ
			{
				break;	//whileループからぬける
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);	//WndProcが呼び出される
			}
	
		}
		dwCurrentTime = timeGetTime();
		if (dwCurrentTime - dwExecLastTime >= 1000.0f / 60.0f)
		{
			MG::SetDeltaTime(dwCurrentTime - dwExecLastTime);

			Update();	//更新処理

			Draw();		//描画処理

			keycopy();
			dwExecLastTime = dwCurrentTime;
		}

	}//while

	//終了処理
	Uninit();

	//終了する
	return (int)msg.wParam;

}

// =======================================================
// ウィンドウプロシージャ
// メッセージループ内で呼び出される
// =======================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATEAPP:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:

	case WM_SYSKEYUP:
	case WM_KEYDOWN:	//キーが押された
		if (wParam == VK_MENU) {
			return 0;  // 処理済みにする
		}
		else if (wParam == VK_ESCAPE)//押されたのはESCキー
		{
			//ウィンドウを閉じたいリクエストをWindowsに送る
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		
		Keyboard_ProcessMessage(uMsg, wParam, lParam);
		break;
	case WM_CLOSE:	//ウィンドウを閉じなさい命令				
		if (
			MessageBox(hWnd, "本当に終了してよろしいですか？",
				"確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK
			)
		{//OKが押されたとき
			DestroyWindow(hWnd);//終了する手続きをWindowsへリクエスト
		}
		else
		{
			return 0;	//やっぱり終わらない
		}

		break;
	case WM_DESTROY:	//終了してOKですよ
		PostQuitMessage(0);		//自分のメッセージに０を送る
		break;

	}

	//必用の無いメッセージは適当に処理させて終了
	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}

// =======================================================
// 初期化
// =======================================================
HRESULT	Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{

	SetScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y);

	renderer = new RendererDX(SCREEN_WIDTH, SCREEN_HEIGHT);
	renderer->Init(hInstance, hWnd, bWindow);
	resourceTool = new ResourceToolDX(renderer);
	drawTool = new DrawToolDX(renderer);
	audioTool = new AudioToolDX();

	SetRenderer(renderer);
	SetResourceTool(resourceTool);
	SetDrawTool(drawTool);
	SetAudioTool(audioTool);
	
	InitInput();

	// バックグラウンドで稼働
	StartScene("bgm");

	// 最初のシーン
	StartScene(INIT_SCENE);

	return	S_OK;
}

// =======================================================
// 終了処理
// =======================================================
void Uninit(void)
{
	UnitInput();
	UninitScene();
	UnregisterAllTransition();
	resourceTool->ReleaseAllResource();
	renderer->Uninit();
	delete renderer;
	delete resourceTool;
	delete drawTool;
	delete audioTool;
}

// =======================================================
// 更新処理
// =======================================================
void Update(void)
{
	UpdateInput();
	//UpdateGamepad();
	UpdateScene();
}

// =======================================================
// 描画処理
// =======================================================
void	Draw(void)
{
	renderer->SetRenderTarget(nullptr);
	renderer->ClearRenderTargetView();
	renderer->SetViewport(SCREEN_CENTER_X, SCREEN_CENTER_Y, SCREEN_WIDTH, SCREEN_HEIGHT);


	DrawScene();

	renderer->Present();
}
