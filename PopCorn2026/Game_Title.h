#pragma once

#include "Final_Letter.h"

//------------------------------------------------------------------------------------------------------------
enum class EGame_Title_State : unsigned char
{
	Idle,

	Game_Over_Descent,
	Game_Over_Show,
	Game_Over_Destroy,

	Game_Won_Descent,
	Game_Won_Animate,

	Finished
};
//------------------------------------------------------------------------------------------------------------
class AsGame_Title : public AGraphics_Object
{
public:
	~AsGame_Title();
	AsGame_Title();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT& paint_area);
	virtual void Draw(HDC hdc, RECT& paint_area);
	virtual bool Is_Finished();

	void Show(bool game_over);
	bool Is_Visible();

private:
	void Destroy_Letters(int current_tick);
	void Animate_Game_Won();

	EGame_Title_State Game_Title_State;
	int Start_Tick;
	int Destroy_Index;
	RECT Prev_Title_Rect, Title_Rect;

	std::vector<AFinal_Letter*> Title_Letters;

	static const int Descent_Timeout = AsConfig::FPS * 4;
	static const int Game_Over_Showing_Timeout = AsConfig::FPS * 2;
	static const int Game_Won_Animate_Timeout = AsConfig::FPS * 30;
	static const int Destroy_Delay = AsConfig::FPS / 2;
	static const int Height = 32;
	static const double Low_Y_Pos;
};
//------------------------------------------------------------------------------------------------------------
enum class ETutorial_State : unsigned char
{
	Missing,

	Draw,
	Clear
};
//------------------------------------------------------------------------------------------------------------
class AsTutorial : public AGame_Object
{
public:
	AsTutorial();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT& paint_area);
	virtual void Draw(HDC hdc, RECT& paint_area);
	virtual bool Is_Finished();

	void Check_Name(bool got_name);

	ETutorial_State Tutorial_State;

private:
	AFont Font;
	RECT Tutorial_Rect;

	static const int Width = 180;
	static const int Height = 60;
	static const int X_Pos = 15;
	static const int Y_Pos = 30;
};
//------------------------------------------------------------------------------------------------------------
