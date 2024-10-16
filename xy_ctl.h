#pragma once

extern ID2D1DeviceContext5* context;


extern IDWriteFactory* pDWriteFactory;

extern int id_;
int get_id() {
	id_++;
	return id_;
}

struct XySize {
	float x = 0;
	float y = 0;

	int mode = 0;

	float* rows = NULL;
	float* columns = NULL;

	XySize(float x1, float y1) {
		x = x1;
		y = y1;
	}
	XySize(float x1, float y1, int mode1, float* rows1, float* columns1) {
		x = x1;
		y = y1;
		mode = mode1;
		rows = rows1;
		columns = columns1;
	}
};

class Control {
public:
	float x = 0;
	float y = 0;
	float width = -1;
	float height = -1;

	int mode = 0;

	const WCHAR* text = NULL;
	int fontSize = 16;

	IDWriteTextFormat* format = NULL;

	ID2D1SolidColorBrush* backgroundColor = NULL;
	ID2D1SolidColorBrush* color = NULL;
	ID2D1SolidColorBrush* borderColor = NULL;
	float radius = 0;

	int id = get_id();

	float border_width = 0;

	HWND hwnd{};

	//ID2D1SvgDocument* svg = NULL;
	//ID2D1SvgDocument* svg1 = NULL;
	int* svgs = NULL;

	std::function<void(int)>click = NULL;
	std::function<void(int)>over = NULL;

	float child_x = -1;
	float child_y = -1;

	float margin_top = 0;
	float margin_left = 0;
	float margin_bottom = 0;
	float margin_right = 0;

	//计算时需要根据id读取
	int ref_id = 0;

	int row = -1;
	int column = -1;

	float* rows = NULL;
	float* columns = NULL;

	float row_value = 0;
	float column_value = 0;

	bool check_child = false;

	bool operator<(const Control& o)const {
		return o.id > id;
	}

	bool status = true;

	Control withRadius(float rad);
	Control setBackgroundColor(int r, int g, int b);
	Control setColor(int r, int g, int b);
	Control addView(Control c);
	Control addViews(std::initializer_list<Control>c);
	Control setWidth(float w);
	Control setHeight(float h);
	Control setSize(float w, float h);
	XySize updateSize(float x, float y, float width, float height);
	Control setColumn(int i);
	Control setRow(int i);
	float get_row_width(float w, float v, float width);
	float get_column_width(float h, float v, float height);
	Control setFontSize(float fontSize);
	Control withMargin(float left, float top, float right, float bottom);
};

extern std::vector<Control>controls;

extern RECT rect;

bool firstControl = false;

inline Control Control::withRadius(float rad)
{
	this->radius = rad;
	return *this;
}

inline Control Control::setBackgroundColor(int r, int g, int b)
{
	if (context != NULL) {
		if (backgroundColor != NULL) {
			backgroundColor->Release();
			backgroundColor = NULL;
		}
		context->CreateSolidColorBrush(D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f), &backgroundColor);
	}
	return *this;
}

inline Control Control::setColor(int r, int g, int b)
{
	if (context != NULL) {
		if (color != NULL) {
			color->Release();
			color = NULL;
		}
		context->CreateSolidColorBrush(D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f), &color);
	}
	return *this;
}

inline Control Control::addView(Control c)
{
	if (child_x == -1) {
		child_x = x;
	}
	if (child_y = -1) {
		child_y = y;
	}
	if (this->mode == 3) {
		//VStack
		auto size = c.updateSize(child_x, child_y, width, 0);
		child_x += size.x;
		child_y += size.y;
	}
	else if (this->mode == 5) {
		//HStack
		auto size = c.updateSize(child_x, child_y, 0, height);
		child_x += size.x;
		child_y += size.y;

	}
	else if (this->mode == 17) {
		float x1 = x, y1 = y;
		for (int i = 0;i < c.row;i++) {
			y1 += get_row_width(*(rows + i), row_value, height);
		}
		for (int i = 0;i < c.column;i++) {
			x1 += get_column_width(*(columns + i), column_value, width);
		}
		auto size = c.updateSize(x1, y1, get_column_width(*(columns + (c.column < 0 ? 0 : c.column)), column_value, width), get_row_width(*(rows + (c.row < 0 ? 0 : c.row)), row_value, height));
	}

	c.ref_id = this->id;

	controls.push_back(c);

	return *this;
}

bool get_has_control() {
	bool has_control = false;
	for (auto& c : controls) {
		if (c.width == -1 || c.height == -1 || c.x < 0 || c.status == false) {
			has_control = true;
		}
	}
	return has_control;
}

inline Control Control::addViews(std::initializer_list<Control>c)
{
	for (auto& c1 : c) {
		this->addView(c1);
	}
	if (check_child) {
		this->check_child = false;
		while (get_has_control()) {

			for (auto& c : controls) {
				if (c.width == -1 || c.height == -1 || c.x < 0 || c.status == false) {
					for (auto& c1 : controls) {
						if (c1.id == c.ref_id) {
							if (c1.mode == 3) {
								auto size = c.updateSize(c1.child_x, c1.child_y, c1.width, 0);
								c1.child_x += size.x;
								c1.child_y += size.y;
								break;
							}
							else if (c1.mode == 3) {
								auto size = c.updateSize(c1.child_x, c1.child_y, 0, c1.height);
								c1.child_x += size.x;
								c1.child_y += size.y;
							}
							else if (c1.mode == 17) {
								float x1 = c1.x;
								float y1 = c1.y;
								for (int i = 0;i < c.row;i++) {
									y1 += get_row_width(*(c1.rows + i), c1.row_value, c1.height);
								}
								for (int i = 0;i < c.column;i++) {
									x1 += get_column_width(*(c1.columns + i), c1.column_value, c1.width);
								}
								c.updateSize(x1, y1, get_column_width(*(c1.columns + (c.column < 0 ? 0 : c.column)), c1.column_value, c1.width), get_row_width(*(c1.rows + (c.row < 0 ? 0 : c.row)), c1.row_value, c1.height));
							}
							break;
						}
					}
					if (c.status == false) {
						c.status = true;
					}
				}
			}
		}
	}
	return *this;
}

inline Control Control::setWidth(float w)
{
	width = w;

	return *this;
}

inline Control Control::setHeight(float h)
{
	height = h;

	return *this;
}

inline Control Control::setSize(float w, float h)
{
	width = w;
	height = h;

	return *this;
}

inline XySize Control::updateSize(float x, float y, float width, float height)
{
	this->x = x;
	this->y = y;
	if (this->width == -1) {
		this->width = width;
	}
	if (this->height <= -1) {
		this->height = height;
	}
	if (this->margin_top > 0) {
		this->y += this->margin_top;
	}
	if (this->margin_left > 0) {
		this->x += this->margin_left;
	}
	if (this->margin_right > 0) {
		//this->x -= this->margin_right;
	}
	if (this->margin_bottom > 0) {
	}
	return XySize(width, height);
}

inline Control Control::setColumn(int i)
{
	column = i;
	return *this;
}

inline Control Control::setRow(int i)
{
	row = i;
	return *this;
}

inline float Control::get_row_width(float w, float h, float height1)
{
	if (w < 0) {

		//return (height - h) * abs(w);
		return (height1 - h) * abs(w);
	}
	return w;
}

inline float Control::get_column_width(float h, float w, float width1)
{
	if (h < 0) {
		//return (width - w) * abs(h);
		return (width1 - w) * abs(h);
	}
	return h;
}

inline Control Control::setFontSize(float fontSize)
{
	this->fontSize = fontSize;
	if (this->format != NULL) {
		this->format->Release();
		this->format = NULL;

		pDWriteFactory->CreateTextFormat(L"微软雅黑", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, this->fontSize, L"zh_CN", &this->format);
	}
	return *this;
}

inline Control Control::withMargin(float left, float top, float right, float bottom)
{
	this->margin_left = left;
	this->margin_top = top;
	this->margin_right = right;
	this->margin_bottom = bottom;
	return *this;
}

class VStack :public Control {
public:
	VStack() {
		this->mode = 3;
	}
};

class HStack :public Control {
public:
	HStack() {
		this->mode = 5;
	}
};

class Text :public Control {
public:
	Text(const WCHAR* text) {
		this->mode = 6;
		this->text = text;

		pDWriteFactory->CreateTextFormat(L"微软雅黑", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"zh_CN", &this->format);
	}
};

class XyEllipse :public Control {
public:
	XyEllipse(float width, float height) {
		this->mode = 7;
		this->width = width;
		this->height = height;
	}
};
class Circle :public Control {
public:
	Circle(float width) {
		this->mode = 8;
	}
};

class XyRectangle :public Control {
public:
	XyRectangle(float width, float height) {
		this->mode = 9;
		this->width = width;
		this->height = height;
		this->x = -1;
		this->status = false;
	}
};

class Menu :public Control {
public:
	Menu(std::initializer_list<int>svgs1) {
		this->mode = 10;

		svgs = new int[svgs1.size()];
		int ind = 0;
		for (auto& s : svgs1) {
			svgs[ind] = s;
			ind++;
		}

		context->CreateSolidColorBrush(D2D1::ColorF(190.0f / 255.0f, 190.0f / 255.0f, 190.0f / 255.0f), &backgroundColor);

	}
	//Menu(int svg[], char* names) {
	//	this->mode = 10;
	//}
};

class Image :public Control {
public:
	Image(const char* path) {
		this->mode = 11;
	}
};
class Chart :public Control {
public:
	Chart(float x[], float y[]) {
		this->mode = 13;
	}
};
class Pie :public Control {
public:
	Pie(float value[]) {
		this->mode = 15;
	}
};
class LineChart :public Control {
public:
	LineChart(float value[]) {
		this->mode = 16;
	}
};
class Grid :public Control {
public:
	Grid(float rows[], float columns[]) {
		this->mode = 17;

		if (!firstControl) {
			firstControl = true;

			this->width = rect.right;
			this->height = rect.bottom;
		}

	}
	Grid(float rows[]) {
		this->mode = 17;
	}

	Grid(std::initializer_list<float>rows1, std::initializer_list<float>columns1) {
		this->mode = 17;

		if (!firstControl) {
			this->width = rect.right;
			this->height = rect.bottom;

			firstControl = true;
			this->check_child = true;
		}
		else {

		}

		this->rows = new float[rows1.size()];
		this->columns = new float[columns1.size()];

		float r_ = 0;
		for (auto& r : rows1) {
			if (r < 0) {
				r_ += abs(r);
			}
			else {
				row_value += r;
			}
		}
		float c_ = 0;
		for (auto& c : columns1) {
			if (c < 0) {
				c_ += abs(c);
			}
			else {
				column_value += c;
			}
		}

		int ind = 0;
		for (auto& r : rows1) {
			if (ind < rows1.size()) {
				this->rows[ind] = r < 0 ? r / r_ : r;
				ind++;
			}
		}
		ind = 0;
		for (auto& c : columns1) {
			this->columns[ind] = c < 0 ? c / c_ : c;
			ind++;
		}
	}
};