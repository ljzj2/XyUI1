#pragma once

#include "resource.h"
#include<d2d1.h>
#include<d2d1_3.h>
#include<dwrite.h>
#include<Windows.h>
#include<Shlwapi.h>
#include<vector>
#include<functional>
#include<initializer_list>
#include<algorithm>


#include"xy_ctl.h"
#include"load_svgs.h"

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"shlwapi.lib")

ID2D1DeviceContext5* context = NULL;
ID2D1Factory* factory = NULL;

IDWriteFactory* pDWriteFactory = NULL;


HWND hwnd{};
RECT rect{};

std::vector<Control>controls;

std::vector<ID2D1SvgDocument*>svgs;

int id_ = 0;

void init_x(HWND h);
void update();
void get_clk_controls(UINT message, int x, int y);
void load_controls();


void init_x(HWND h) {
	hwnd = h;
	GetClientRect(hwnd, &rect);


	D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProperties{};
	hwndProperties.hwnd = hwnd;
	hwndProperties.pixelSize = D2D1::SizeU(rect.right, rect.bottom);
	hwndProperties.presentOptions = D2D1_PRESENT_OPTIONS_NONE;
	D2D1_RENDER_TARGET_PROPERTIES properties{};
	properties.dpiX = 96.0f;
	properties.dpiY = 96.0f;
	properties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	properties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
	properties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	properties.usage = D2D1_RENDER_TARGET_USAGE_NONE;

	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (FAILED(hr)) {
		return;
	}
	ID2D1HwndRenderTarget* rt = NULL;
	hr = factory->CreateHwndRenderTarget(&properties, &hwndProperties, &rt);
	if (FAILED(hr)) {
		return;
	}
	ID2D1DeviceContext* context1 = NULL;
	hr = rt->QueryInterface(__uuidof(ID2D1DeviceContext), (void**)&context1);
	if (FAILED(hr)) {
		return;
	}
	hr = context1->QueryInterface(__uuidof(ID2D1DeviceContext5), (void**)&context);
	if (FAILED(hr)) {
		return;
	}

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
	if (FAILED(hr)) {
		return;
	}


	load_controls();
	load_svg();
}

inline void update()
{
	context->BeginDraw();
	context->Clear(D2D1::ColorF(1, 1, 1));

	for (auto& c : controls) {
		if (c.mode == 1) {
			if (c.color != NULL) {
				context->DrawLine(D2D1::Point2F(c.x, c.y), D2D1::Point2F(c.x + c.width, c.y + c.height), c.color);
			}
		}
		else if (c.mode == 3) {
			D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(c.x, c.y, c.x + c.width, c.y + c.height), c.radius, c.radius);
			if (c.backgroundColor != NULL) {
				context->FillRoundedRectangle(roundedRect, c.backgroundColor);
			}
			if (c.color != NULL) {
				context->DrawRoundedRectangle(roundedRect, c.color);
			}
		}
		else if (c.mode == 5) {
			D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(c.x, c.y, c.x + c.width, c.y + c.height), c.radius, c.radius);
			if (c.backgroundColor != NULL) {
				context->FillRoundedRectangle(roundedRect, c.backgroundColor);
			}
			if (c.color != NULL) {
				context->DrawRoundedRectangle(roundedRect, c.color);
			}
		}
		else if (c.mode == 6) {
			if (c.text != NULL && c.format != NULL && c.color != NULL) {
				D2D1_RECT_F layoutRect = D2D1::RectF(c.x, c.y, c.x + c.width, c.y + c.height);
				context->DrawTextW(c.text, static_cast<UINT32>(wcslen(c.text)), c.format, &layoutRect, c.color);
			}
		}
		else if (c.mode == 7) {
			D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(c.x + c.width, c.y + c.height), c.width, c.height);
			if (c.backgroundColor != NULL) {
				context->FillEllipse(ellipse, c.backgroundColor);
			}
			if (c.color != NULL) {
				context->DrawEllipse(ellipse, c.color);
			}
		}
		else if (c.mode == 8) {
			D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(c.x + c.width, c.y + c.width), c.width, c.width);
			if (c.backgroundColor != NULL) {
				context->FillEllipse(ellipse, c.backgroundColor);
			}
			if (c.color != NULL) {
				context->DrawEllipse(ellipse, c.color);
			}
		}
		else if (c.mode == 9) {
			D2D1_RECT_F rect1 = D2D1::RectF(c.x, c.y, c.width + c.x, c.y + c.height);
			if (c.backgroundColor != NULL) {

				//context->FillRectangle(rect1, c.backgroundColor);

				if (c.radius > 0) {
					context->FillRoundedRectangle(D2D1::RoundedRect(rect1, c.radius, c.radius), c.backgroundColor);
				}
				else {
					context->FillRectangle(rect1, c.backgroundColor);
				}
			}
			if (c.color != NULL) {
				//context->DrawRectangle(rect1, c.color);
				if (c.radius > 0) {
					context->DrawRoundedRectangle(D2D1::RoundedRect(rect1, c.radius, c.radius), c.color);
				}
				else {
					context->DrawRectangle(rect1, c.color);
				}
			}
		}
		else if (c.mode == 10) {
			//menu
			if (c.svgs != NULL) {
				int o = 0;
				while (c.svgs[o] >= 0) {
					o++;
				}
				for (int i = 0;i < o;i++) {
					if (i == 0) {
						D2D1_RECT_F backgroundRect{};
						float x = c.x + c.width / 2;
						float y = c.y + c.height / o / 2;
						backgroundRect = D2D1::RectF(x - 16, y - 16, x + 16, y + 16);
						context->FillRoundedRectangle(D2D1::RoundedRect(backgroundRect, 6, 6), c.backgroundColor);
					}
					D2D1_MATRIX_3X2_F matrix{};
					context->GetTransform(&matrix);
					matrix.dx = c.x + c.width / 2 - 8;
					matrix.dy = c.y + c.height / o * (i + 1) / 2 - 8;
					context->SetTransform(matrix);
					context->DrawSvgDocument(svgs.at(c.svgs[i]));
					matrix.dx = 0;
					matrix.dy = 0;
					context->SetTransform(matrix);

				}
			}

		}
	}

	context->EndDraw();
}

inline void get_clk_controls(UINT message, int x, int y)
{
}
bool cmp(Control const* c, Control const* c1) {
	return c1->id > c->id;
}

inline void load_controls()
{
	Grid({ -1 }, { 80,-1,-21.6,-1,-10 })
		.addViews({
		Grid({80,-1,80},{-1})
		.addView(Menu({ICON_HOME,ICON_CH,ICON_FLOW,ICON_GEAR,ICON_CA}).setRow(1)),
			VStack().setColumn(1).setBackgroundColor(239,239,239),
			VStack().setBackgroundColor(239,239,239).setColumn(2),
			Text(L"Dashboard").setColor(0,0,0).setFontSize(30).setColumn(2).withMargin(0,15,0,0),
			Grid({-1}, { -1.9,15,-1.9,15,-1 })
			.addViews({
				XyRectangle(190,65).setBackgroundColor(255,255,255).withRadius(7),
				Text(L"$1032.43").setFontSize(19).setColor(0,0,0).setWidth(150).withMargin(15,5,0,0),
				Text(L"Daily revenue").setFontSize(13).setColor(190,190,190).setWidth(150).withMargin(15,35,0,0),
				XyRectangle(35,35).setBackgroundColor(236,252,238).withRadius(7).withMargin(150,15,0,0),
				Text(L"+11%").setColor(93,150,93).setFontSize(9).setWidth(35).withMargin(155,25,0,0)
}).withMargin(0,80,0,0).setColumn(2)
			});




	std::sort(controls.begin(), controls.end());
}