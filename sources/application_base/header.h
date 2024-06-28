#pragma once

//
#define   IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include  "imgui_internal.h"

//
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <stdio.h>

extern ID3D11Device*            g_pd3dDevice;
extern ID3D11DeviceContext*     g_pd3dDeviceContext;
extern IDXGISwapChain*          g_pSwapChain;
extern ID3D11RenderTargetView*  g_mainRenderTargetView;
extern ID3D11Texture2D*         g_mainRenderDS;
extern ID3D11DepthStencilView*  g_mainRenderDSView;
extern HWND						g_hWnd;

int     Run_Run(unsigned int W = 1280, unsigned int H = 800);

bool    Run_Init();
void    Run_Free();
bool    Run_Draw();

void    Run_DrawDX11();

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

//
#include "include_cppheader.h"
