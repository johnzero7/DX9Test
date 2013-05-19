// include the basic windows header files and the Direct3D header file
#define DIRECTINPUT_VERSION 0x0800
#define WINVER 0x0601
#include <string>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <Windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "includes/input.h"
#include <dinput.h>



//Oculus Rift integration
//#include "../X/OVR.h"

// include the Direct3D Library file only VB
//#pragma comment (lib, "d3d9.lib")
//#pragma comment (lib, "d3dx9.lib")

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
struct CUSTOMVERTEX {FLOAT X, Y, Z; D3DVECTOR NORMAL; FLOAT U, V;};

// function prototypes
void initD3D(HWND hWnd); // sets up and initializes Direct3D
void render_frame(void); // renders a single frame
void initDInput(HINSTANCE hInstance, HWND hWnd);    // sets up and initializes DirectInput
void detect_keys(void);    // gets the current keys being pressed
void detect_mousepos(void);    // gets the mouse movement and updates the static variables
void cleanDInput(void);    // closes DirectInput and releases memory
bool cleanD3D(void); // closes Direct3D and releases memory
void init_graphics(void); // creates the model

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXFONT dxfont;    // the pointer to the font object

LPDIRECTINPUT8 din;    // the pointer to our DirectInput interface
LPDIRECTINPUTDEVICE8 dinkeyboard;    // the pointer to the keyboard device
LPDIRECTINPUTDEVICE8 dinmouse;    // the pointer to the mouse

BYTE currKeyState[256];    // create a static storage for the current key-states
BYTE prevKeyState[256];    // create a static storage for the previous key-states
DIMOUSESTATE2 currMouseState;    // create a static storage for the current mouse-states
DIMOUSESTATE2 prevMouseState;    // create a static storage for the previous mouse-states

LPDIRECT3DVERTEXBUFFER9 t_buffer = NULL;    // the pointer to the vertex buffer
LPDIRECT3DTEXTURE9 texture_1;    // our first texture


//
    IDirect3DTexture9* mBkgdTex;
    IDirect3DTexture9* mShipTex;
    IDirect3DTexture9* mFrames;
    ID3DXSprite* mSprite;

    __int64 currTimeStamp = 0;
    __int64 prevTimeStamp = 0;
    __int64 cntsPerSec = 0;
    static float secsPerCnt = 1.0f / (float)cntsPerSec;
    static float numFrames = 0.0f;
    static float timeElapsed = 0.0f;
    static float dt;
    float mFPS;
    float mMilliSecPerFrame;

// Create Viewports
/*
D3DVIEWPORT9 q1 = {0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.0f, 1.0f};
D3DVIEWPORT9 q2 = {SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.0f, 1.0f};
D3DVIEWPORT9 q3 = {0, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.0f, 1.0f};
D3DVIEWPORT9 q4 = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.0f, 1.0f};
D3DVIEWPORT9 q5 = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 400, 100, 0.0f, 1.0f};

D3DVIEWPORT9 vps[5] = {q1, q2, q3, q4,q5};
*/

D3DVIEWPORT9 q0 = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f};
D3DVIEWPORT9 q1 = {0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, 0.0f, 1.0f};
D3DVIEWPORT9 q2 = {SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, 0.0f, 1.0f};

D3DVIEWPORT9 vps[2] = {q1,q2};

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	//Oculus initializartion
/*
	OVR::System::Init();
//	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
	OVR::Ptr<OVR::DeviceManager> pManager;
	OVR::Ptr<OVR::HMDDevice> pHMD;
	pManager = *OVR::DeviceManager::Create();
	pHMD = *pManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
*/

	HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "WindowClass1";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(NULL,
                          "WindowClass1",
                          "Our Direct3D Program",
                          WS_EX_TOPMOST | WS_POPUP,
                          0, 0,
                          SCREEN_WIDTH, SCREEN_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

    // set up and initialize Direct3D
    initD3D(hWnd);
    initDInput(hInstance, hWnd);


    //__int64 cntsPerSec = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
    secsPerCnt = 1.0f / (float)cntsPerSec;

    //__int64 prevTimeStamp = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);


    D3DXCreateTextureFromFile(d3ddev, "imgs/bkgd1.bmp", &mBkgdTex);
    D3DXCreateTextureFromFile(d3ddev, "imgs/alienship.png", &mShipTex);
    D3DXCreateTextureFromFile(d3ddev, "imgs/fireatlas.bmp", &mFrames);
    D3DXCreateSprite(d3ddev, &mSprite);

    // enter the main loop:
    MSG msg;
    msg.message = WM_NULL;

    float fLimit = 0;

//    *When rawinput is registered there is no directXinput
//    cInputDevices x = cInputDevices();
//    x.RegisterDevices();


    while(msg.message != WM_QUIT)
    {
        DWORD starting_point = GetTickCount();

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        detect_keys();
        detect_mousepos();

        //__int64 currTimeStamp = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
        //float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;
        dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;
        fLimit += dt;

        numFrames += 1.0f;

        // Accumulate how much time has passed.
        timeElapsed += dt;
        if( timeElapsed >= 1.0f )
        {
            // Frames Per Second = numFrames / timeElapsed,
            // but timeElapsed approx. equals 1.0, so
            // frames per second = numFrames.

            mFPS = numFrames;

            // Average time, in milliseconds, it took to render a
            // single frame.
            mMilliSecPerFrame = 1000.0f / mFPS;

            // Reset time counter and frame count to prepare
            // for computing the average stats over the next second.
            timeElapsed = 0.0f;
            numFrames   = 0.0f;
        }

        render_frame();
        // check the 'escape' key
        if(keyReleased(DIK_ESCAPE, prevKeyState, currKeyState))    // if the 'ESCAPE' key was pressed...
            PostMessage(hWnd, WM_DESTROY, 0, 0);

        //while (fLimit <= 1.0f);
        //while((GetTickCount() - starting_point) < 1.0f);

        // Prepare for next iteration: The current time
        // stamp becomes the previous time stamp for the
        // next iteration.
        prevTimeStamp = currTimeStamp;

    }

    // clean up DirectX and COM
    cleanD3D();
/*
    //Oculus Finalization
    OVR::System::Destroy();
*/
    return msg.wParam;
}

// PROCEDURES
//*********************************************
// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}



// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
    // Creates the interface
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    // create a device class using this information and the info from the d3dpp stuct

    // Set default settings
    UINT AdapterToUse=D3DADAPTER_DEFAULT;
    D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL;
    // Look for 'NVIDIA PerfHUD' adapter
    // If it is present, override default settings
    for (UINT Adapter=0;Adapter<d3d->GetAdapterCount();Adapter++)
    {
        D3DADAPTER_IDENTIFIER9 Identifier;
        HRESULT Res;
        Res = d3d->GetAdapterIdentifier(Adapter,0,&Identifier);
        if (strstr(Identifier.Description,"PerfHUD") != 0)
        {
            AdapterToUse=Adapter;
            DeviceType=D3DDEVTYPE_REF;
            break;
        }
    }

    D3DPRESENT_PARAMETERS d3dpp;

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed          = true;
    d3dpp.SwapEffect        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow     = hWnd;
    d3dpp.BackBufferFormat  = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferWidth   = SCREEN_WIDTH;
    d3dpp.BackBufferHeight  = SCREEN_HEIGHT;
    d3dpp.BackBufferCount   = 1;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;


    d3d->CreateDevice(
        AdapterToUse,
        DeviceType,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &d3ddev);

    init_graphics();

    //LIGHT fo 3D objects
    d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting

    //Enables alpha blending
    //d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);    // turn on the color blending
    //d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);    // set source factor
    //d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    // set dest factor
    //d3ddev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);    // set the operation

    D3DXCreateFont(d3ddev,    // the D3D Device
                   30,    // font height of 30
                   0,    // default font width
                   FW_NORMAL,    // font weight
                   1,    // not using MipLevels
                   true,    // italic font
                   DEFAULT_CHARSET,    // default character set
                   OUT_DEFAULT_PRECIS,    // default OutputPrecision,
                   DEFAULT_QUALITY,    // default Quality
                   DEFAULT_PITCH | FF_DONTCARE,    // default pitch and family
                   "Arial",    // use Facename Arial
                   &dxfont);    // the font object

    return;
}

void initDInput(HINSTANCE hInstance, HWND hWnd)
{
    // create the DirectInput interface
    DirectInput8Create(hInstance,    // the handle to the application
                       DIRECTINPUT_VERSION,    // the compatible version
                       IID_IDirectInput8,    // the DirectInput interface version
                       (void**)&din,    // the pointer to the interface
                       NULL);    // COM stuff, so we'll set it to NULL

    // create the keyboard device
    din->CreateDevice(GUID_SysKeyboard,    // the default keyboard ID being used
                      &dinkeyboard,    // the pointer to the device interface
                      NULL);    // COM stuff, so we'll set it to NULL

    // create the mouse device
    din->CreateDevice(GUID_SysMouse,    // the default mouse ID being used
                      &dinmouse,    // the pointer to the device interface
                      NULL);    // COM stuff, so we'll set it to NULL

    dinkeyboard->SetDataFormat(&c_dfDIKeyboard); // set the data format to keyboard format
    dinmouse->SetDataFormat(&c_dfDIMouse2);    // set the data format to mouse format

    // set the control you will have over the keyboard
    dinkeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    // set the control you will have over the mouse
    dinmouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    dinkeyboard->Acquire();    // Acquire the keyboard
    dinmouse->Acquire();    // Acquire the mouse only once

    ZeroMemory(&currKeyState, sizeof(currKeyState));
    ZeroMemory(&prevKeyState, sizeof(prevKeyState));
    ZeroMemory(&currMouseState, sizeof(currMouseState));
    ZeroMemory(&prevMouseState, sizeof(prevMouseState));

    return;    // return to WinMain()
}

// this is the function that detects keystrokes
void detect_keys(void)
{
    //static BYTE keystate[256];    // create a static storage for the key-states
    dinkeyboard->Acquire();    // get access if we don't have it already
    memcpy(prevKeyState, currKeyState, sizeof(currKeyState));	// save the previous state
    dinkeyboard->GetDeviceState(256, (LPVOID)&currKeyState);	// fill currKeyState with new values
/*
    if(keystate[DIK_A] & 0x80)    // if the 'A' key was pressed...
        // then inform the user of this very important message:
        MessageBox(NULL, "You pressed the 'A' key!", "IMPORTANT MESSAGE!", MB_OK);
*/
    if(currKeyState[DIK_A] & 0x80)
    	OutputDebugString( "Pressed A");
    return;
}

// this is the function that detets mouse movements and mouse buttons
void detect_mousepos(void)
{
    //static DIMOUSESTATE mousestate;    // create a static storage for the mouse-states
    dinmouse->Acquire();    // get access if we don't have it already
    memcpy(&prevMouseState, &currMouseState, sizeof(currMouseState));	// save the previous state
    dinmouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&currMouseState);// fill the mousestate with values
/*
    if(mousestate.rgbButtons[0] & 0x80)    // if the left mouse-button was clicked...
        // then inform the user of this very important message:
        MessageBox(NULL, L"You clicked the mouse button", L"IMPORTANT MESSAGE!", MB_OK);

*/
    return;
}

// this is the function used to render a single frame
void render_frame(void)
{
    // First config Wordl, View and Projection Matris
    static float indexX = 0.0f; indexX += ((float)currMouseState.lX/500);
    static float indexY = 0.0f; indexY += ((float)currMouseState.lY/500);
    static float indexZ = 0.0f; indexZ += ((float)currMouseState.lZ/200);

    static bool viewMode = false;
    static bool renderMode = false;
    if (keyPressed(DIK_W, prevKeyState, currKeyState)){
    	renderMode = !renderMode;
    }
    if (renderMode){
    	d3ddev->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
     }else{
    	 d3ddev->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
    }

    if (mousePressed(1, prevMouseState, currMouseState)){
    	viewMode = !viewMode;
    }


    d3ddev->BeginScene();    // begins the 3D scene
    // SET UP THE PIPELINE
    //WORLD TRANSFORMATION
    ///////////////////////////
    D3DXMATRIX matRotateX;
    D3DXMATRIX matRotateY;
    D3DXMATRIX matTrans;
    // build a matrix to rotate the model based on the increasing float value
    D3DXMatrixRotationY(&matRotateX, indexX);
    D3DXMatrixRotationX(&matRotateY, indexY);
    D3DXMatrixTranslation(&matTrans,0,0,0);
    // tell Direct3D about our matrix
    D3DXMATRIX xxx = (matTrans * matRotateY * matRotateX);
    d3ddev->SetTransform(D3DTS_WORLD, &xxx);

    // time for pageflip
    static float timeElapsed2 = 0.0f;
    timeElapsed2 += dt;

    bool jitter = false;
    if (jitter) {

    }else{

    }

	for(int nViewport = 0; nViewport < int(sizeof(vps)/sizeof(vps[0])); ++nViewport){


		D3DVIEWPORT9 viewport;
		if (viewMode){
			viewport = vps[nViewport];
		}else{
			viewport = q0;
		}
		//VIEW TRANSFORMATION
		///////////////////////////
		d3ddev->SetViewport(&viewport);
		D3DXMATRIX matView;
		static int frameLoop = 0;

		float cameraXPos = indexZ+(nViewport*(-indexZ*2));
		D3DXVECTOR3 camPos = D3DXVECTOR3 (cameraXPos , 0.0f, -50.0f);
		D3DXVECTOR3 camLookAt = D3DXVECTOR3 (cameraXPos, 0.0f, 0.0f);
		D3DXVECTOR3 camUpVec = D3DXVECTOR3 (0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&matView,
						   &camPos,    // the camera position
						   &camLookAt,    // the look-at position
						   &camUpVec);    // the up direction

		d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

		//VIEW TRANSFORMATION
		///////////////////////////
		D3DXMATRIX matProjection;// the projection transform matrix
		D3DXMatrixPerspectiveFovLH(&matProjection,
								   D3DXToRadian(45),    // the horizontal field of view
								   //(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
								   (FLOAT)viewport.Width/(FLOAT)viewport.Height,
								   1.0f,    // the near view-plane
								   5000.0f);    // the far view-plane

		d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection
		// clear each viewport BACKBUFFER
		d3ddev->Clear(0, NULL, D3DCLEAR_TARGET , D3DCOLOR_XRGB(100, 0, 0), 1.0f, 0);

		//texture del fondo
		mSprite->Begin(D3DXSPRITE_OBJECTSPACE | D3DXSPRITE_DONOTMODIFY_RENDERSTATE | D3DXSPRITE_ALPHABLEND);
		//mSprite->Begin(D3DXSPRITE_ALPHABLEND);
		//mSprite->Begin(0);
		//d3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		//d3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	/*
		// The following code specifies an alpha test and
		// reference value.
		d3ddev->SetRenderState(D3DRS_ALPHAREF, 200);
		d3ddev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		// The following code is used to set up alpha blending.
		d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	*/
		d3ddev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		D3DXMATRIX texScaling;
		D3DXMatrixScaling(&texScaling, 2.0f, 2.0f, 0.0f);
		d3ddev->SetTransform(D3DTS_TEXTURE0, &texScaling);

		D3DXMATRIX R,S,T,mIdentity,ST,SRT;
		D3DXMatrixRotationZ(&R, indexX);
		D3DXMatrixScaling(&S, 2.0f, 2.0f, 0.0f);
		D3DXMatrixTranslation(&T, 0,0,100);
		ST=S*T;
		mSprite->SetTransform(&ST);

		//&D3DXVECTOR3 (indexX*50, indexY*50, 0)
		mSprite->Draw(mBkgdTex, 0, 0, 0, D3DCOLOR_XRGB(255, 255, 255));
		D3DXMatrixTranslation(&T, 0,0,100);
		D3DXMatrixScaling(&S, 0.5f, 0.5f, 0.0f);
		SRT=S*R*T;
		mSprite->SetTransform(&SRT);
		D3DXVECTOR3 drawVec = D3DXVECTOR3 (64, 64, 0);
		mSprite->Draw(mShipTex, 0, &drawVec, 0, D3DCOLOR_XRGB(255, 255, 255));
		mSprite->Flush();
		D3DXMatrixScaling(&texScaling, 1.0f, 1.0f, 0.0f);
		d3ddev->SetTransform(D3DTS_TEXTURE0, &texScaling);



		// Flip Textures
		//////////////////
		static int mCurrFrame = 0;
		if (mCurrFrame > 29)
			mCurrFrame = 0;
		if (timeElapsed2 >= .03)
		{
			mCurrFrame += 1;
			timeElapsed2 = 0;
		}
		int i = mCurrFrame / 6;   // Row
		int j = mCurrFrame % 6;   // Column
		RECT Rect = {j*64, i*64, (j+1)*64, (i+1)*64};
		mSprite->SetTransform(D3DXMatrixIdentity(&mIdentity));
		mSprite->Draw(mFrames, &Rect, 0, 0, D3DCOLOR_XRGB(255, 255, 255));
		mSprite->Flush();

		// End spites
		//mSprite->End();
		// Reset to Normal
		D3DXMatrixScaling(&texScaling, 1.0f, 1.0f, 0.0f);
		d3ddev->SetTransform(D3DTS_TEXTURE0, &texScaling);

		// create a RECT to contain the text
		static RECT textbox;
		static RECT textbox2;
		static RECT textbox3;
		static RECT textbox4;
		SetRect(&textbox, 0, 0, 640, 50);
		SetRect(&textbox2, 0, 50, 640, 100);
		SetRect(&textbox3, 0, 100, 640, 150);
		SetRect(&textbox4, 0, 150, 640, 200);
		// draw the Hello World text

		////////////
		unsigned int mButton = 22;
		string s="12";
		stringstream out;

		for(mButton=0;mButton<sizeof(currMouseState.rgbButtons);mButton++){
			int j = currMouseState.rgbButtons[mButton]?1:0;
			out<<j;
		}
		s=out.str();
	/*
		i=keystate[DIK_B];
		out << i;
		s = out.str();
		out.str("");
		i= keystate[DIK_A];
		out << i;
		s = out.str();
	 */
		////////////
		float fOut;
		fOut = dt;
		out.str("");
		out << fOut;
		string xx = out.str();


		fOut = mFPS;
		out.str("");
		out << fOut;
		string yy = out.str();

		fOut = mMilliSecPerFrame ;
		out.str("");
		out << fOut;
		string zz = out.str();
		D3DXMatrixTranslation(&T,50,0,0);
		mSprite->SetTransform(&(T));


				dxfont->DrawText(mSprite,
								  s.c_str(),
								  -1,
								  0,
								  DT_WORDBREAK,
								  D3DCOLOR_ARGB(100, 255, 255, 255));
		D3DXMatrixIdentity(&T);
		mSprite->SetTransform(&(T));

				dxfont->DrawText(mSprite,
								  xx.c_str(),
								  -1,
								  &textbox2,
								  DT_WORDBREAK,
								  D3DCOLOR_ARGB(255, 255, 255, 255));
				dxfont->DrawText(mSprite,
								  yy.c_str(),
								  -1,
								  &textbox3,
								  DT_WORDBREAK,
								  D3DCOLOR_ARGB(255, 255, 255, 255));
				dxfont->DrawText(NULL,
								  zz.c_str(),
								  -1,
								  &textbox4,
								  DT_WORDBREAK,
								  D3DCOLOR_ARGB(255, 255, 255, 255));
		mSprite->Flush();
		mSprite->End();

		// select which vertex format we are using
		d3ddev->SetFVF(CUSTOMFVF);

		// select the vertex buffer to display
		d3ddev->SetStreamSource(0, t_buffer, 0, sizeof(CUSTOMVERTEX));

		d3ddev->SetTexture(0, texture_1);
		// copy the vertex buffer to the back buffer
		d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
		d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
		d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);
		d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
		d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);
	}
    d3ddev->EndScene();    // ends the 3D scene
    d3ddev->Present(NULL, NULL, NULL, NULL);
    return;
}

void init_graphics(void)
{

    // load the texture we will use
    D3DXCreateTextureFromFile(d3ddev,"imgs/wood1.png",&texture_1);

    // create the vertices using the CUSTOMVERTEX struct
    CUSTOMVERTEX t_vert[] =
    {
        // side 1
        { -3.0f, 3.0f, -3.0f, {0, 0, -1},     0, 0, },
        { 3.0f, 3.0f, -3.0f, {0, 0, -1},      1, 0, },
        { -3.0f, -3.0f, -3.0f, {0, 0, -1},    0, 1, },
        { 3.0f, -3.0f, -3.0f, {0, 0, -1},     1, 1, },

        // side 2
        { -3.0f, 3.0f, 3.0f, {0, 0, 1},       1, 0, },
        { -3.0f, -3.0f, 3.0f, {0, 0, 1},      1, 1, },
        { 3.0f, 3.0f, 3.0f, {0, 0, 1},        0, 0, },
        { 3.0f, -3.0f, 3.0f, {0, 0, 1},       0, 1, },

        // side 3
        { -3.0f, 3.0f, 3.0f, {0, 1, 0},       0, 0, },
        { 3.0f, 3.0f, 3.0f, {0, 1, 0},        1, 0, },
        { -3.0f, 3.0f, -3.0f, {0, 1, 0},      0, 1, },
        { 3.0f, 3.0f, -3.0f, {0, 1, 0},       1, 1, },

        // side 4
        { -3.0f, -3.0f, 3.0f, {0, -1, 0},     0, 1, },
        { -3.0f, -3.0f, -3.0f, {0, -1, 0},    0, 0, },
        { 3.0f, -3.0f, 3.0f, {0, -1, 0},      1, 1, },
        { 3.0f, -3.0f, -3.0f, {0, -1, 0},     1, 0, },

        // side 5
        { 3.0f, 3.0f, -3.0f, {1, 0, 0},       0, 0, },
        { 3.0f, 3.0f, 3.0f, {1, 0, 0},        1, 0, },
        { 3.0f, -3.0f, -3.0f, {1, 0, 0},      0, 1, },
        { 3.0f, -3.0f, 3.0f, {1, 0, 0},       1, 1, },

        // side 6
        { -3.0f, 3.0f, -3.0f, {-1, 0, 0},     1, 0, },
        { -3.0f, -3.0f, -3.0f, {-1, 0, 0},    1, 1, },
        { -3.0f, 3.0f, 3.0f, {-1, 0, 0},      0, 0, },
        { -3.0f, -3.0f, 3.0f, {-1, 0, 0},     0, 1, },
    };    // that reminds me of programming in binary!

    // create a vertex buffer interface called t_buffer
    d3ddev->CreateVertexBuffer(24*sizeof(CUSTOMVERTEX),
                               0,
                               CUSTOMFVF,
                               D3DPOOL_MANAGED,
                               &t_buffer,
                               NULL);

    VOID* pVoid;    // a void pointer

    // lock t_buffer and load the vertices into it
    t_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, t_vert, sizeof(t_vert));
    t_buffer->Unlock();

    return;
}

// this is the function that cleans up Direct3D and COM
bool cleanD3D(void)
{

    // Release Input
    dinmouse->Unacquire(); // make sure the mouse is unacquired
    dinmouse->Release();
    dinmouse=NULL;
    dinkeyboard->Unacquire(); // make sure the keyboard is unacquired
    dinkeyboard->Release();
    din->Release(); // close DirectInput before exiting
    din=NULL;

    //Release Fonts
    dxfont->Release(); // Close the font
    dxfont=NULL;

    //Release Textures
    if (mBkgdTex!=NULL){
        mBkgdTex->Release();
        mBkgdTex=NULL;
    }
    if (mShipTex!=NULL){
    	mShipTex->Release();
    	mShipTex=NULL;
    }
    if (mFrames!=NULL){
		mFrames->Release();
		mFrames=NULL;
    }
    if (texture_1!=NULL){
    	texture_1->Release();
    	texture_1=NULL;
    }

    //Release Sprite
    if (mSprite!=NULL){
		mSprite->Release();
		mSprite=NULL;
    }

    // Release Device
    if (d3ddev!=NULL){
		d3ddev->Release();
		d3ddev=NULL;
    }
    // Release Interface
    if (d3d!=NULL){
		d3d->Release();
		d3d=NULL;
    }
    return true;

}

