#include "Game_Title.h"

// AsGame_Title
const double AsGame_Title::Low_Y_Pos = 135.0;
//------------------------------------------------------------------------------------------------------------
AsGame_Title::~AsGame_Title()
{
	for (auto* letter : Title_Letters)
		delete letter;

	Title_Letters.erase(Title_Letters.begin(), Title_Letters.end());
}
//------------------------------------------------------------------------------------------------------------
AsGame_Title::AsGame_Title()
	: Game_Title_State(EGame_Title_State::Idle), Start_Tick(0), Destroy_Index(0), Prev_Title_Rect{}, Title_Rect{}
{
}
//------------------------------------------------------------------------------------------------------------
void AsGame_Title::Act()
{
	int current_tick;
	double y_pos;
	double ratio;

	if (Game_Title_State == EGame_Title_State::Idle || Game_Title_State == EGame_Title_State::Finished)
		return;

	current_tick = AsConfig::Current_Timer_Tick - Start_Tick;

	switch (Game_Title_State)
	{
	case EGame_Title_State::Idle:
		break;


	case EGame_Title_State::Game_Over_Descent:
	case EGame_Title_State::Game_Won_Descent:
		if (current_tick < Descent_Timeout)
			ratio = (double)current_tick / (double)Descent_Timeout;
		else
		{
			ratio = 1.0;

			if (Game_Title_State == EGame_Title_State::Game_Over_Descent)
				Game_Title_State = EGame_Title_State::Game_Over_Show;
			else
				Game_Title_State = EGame_Title_State::Game_Won_Animate;

			Start_Tick = AsConfig::Current_Timer_Tick;
		}

		y_pos = Low_Y_Pos * ratio;

		for (auto* letter : Title_Letters)
			letter->Y_Pos = y_pos;

		Prev_Title_Rect = Title_Rect;

		Title_Rect.top = (int)(y_pos * AsConfig::D_Global_Scale);
		Title_Rect.bottom = Title_Rect.top + Height * AsConfig::Global_Scale;

		AsTools::Invalidate_Rect(Title_Rect);
		AsTools::Invalidate_Rect(Prev_Title_Rect);
		break;


	case EGame_Title_State::Game_Over_Show:
		if (current_tick > Game_Over_Showing_Timeout)
		{
			Game_Title_State = EGame_Title_State::Game_Over_Destroy;
			Start_Tick = AsConfig::Current_Timer_Tick;
		}
		break;


	case EGame_Title_State::Game_Over_Destroy:
		Destroy_Letters(current_tick);
		break;


	case EGame_Title_State::Game_Won_Animate:
		if (current_tick < Game_Won_Animate_Timeout)
			Animate_Game_Won();
		else
			Game_Title_State = EGame_Title_State::Finished;
		break;


	case EGame_Title_State::Finished:
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsGame_Title::Clear(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (Game_Title_State == EGame_Title_State::Idle || Game_Title_State == EGame_Title_State::Finished)
		return;

	if (!IntersectRect(&intersection_rect, &paint_area, &Prev_Title_Rect))
		return;

	AsTools::Rect(hdc, Prev_Title_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AsGame_Title::Draw(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (Game_Title_State == EGame_Title_State::Idle || Game_Title_State == EGame_Title_State::Finished)
		return;

	if (!IntersectRect(&intersection_rect, &paint_area, &Title_Rect))
		return;

	for (auto* letter : Title_Letters)
		letter->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsGame_Title::Is_Finished()
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsGame_Title::Show(bool game_over)
{
	const double d_scale = AsConfig::D_Global_Scale;
	double title_x, title_end_x;
	double title_y = 0.0;

	if (game_over)
	{
		title_x = 33.0;

		Title_Letters.push_back(new AFinal_Letter(title_x, title_y, L'К'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 13.0, title_y, L'О'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 29.0, title_y, L'Н'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 45.0, title_y, L'Е'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 59.0, title_y, L'Ц'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 80.0, title_y, L'И'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 96.0, title_y, L'Г'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 108.0, title_y, L'Р'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 120.0, title_y, L'Ы'));

		Game_Title_State = EGame_Title_State::Game_Over_Descent;
	}
	else
	{
		title_x = 57.0;

		Title_Letters.push_back(new AFinal_Letter(title_x, title_y, L'П'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 16.0, title_y, L'О'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 33.0, title_y, L'Б'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 46.0, title_y, L'Е'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 59.0, title_y, L'Д'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 75.0, title_y, L'А'));
		Title_Letters.push_back(new AFinal_Letter(title_x + 91.0, title_y, L'!'));

		Game_Title_State = EGame_Title_State::Game_Won_Descent;
	}

	title_end_x = Title_Letters[Title_Letters.size() - 1]->X_Pos + 16;

	Title_Rect.left = (int)(title_x * d_scale);
	Title_Rect.top = (int)(title_y * d_scale);
	Title_Rect.right = Title_Rect.left + (int)(title_end_x * d_scale);
	Title_Rect.bottom = Title_Rect.top + Height * AsConfig::Global_Scale;

	Start_Tick = AsConfig::Current_Timer_Tick;
	Destroy_Index = -1;

	AsTools::Invalidate_Rect(Title_Rect);
}
//------------------------------------------------------------------------------------------------------------
bool AsGame_Title::Is_Visible()
{
	if (Game_Title_State != EGame_Title_State::Idle)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AsGame_Title::Destroy_Letters(int current_tick)
{
	bool all_letters_are_finished = true;
	bool can_finish = false;

	if (Destroy_Index == -1 || current_tick % Destroy_Delay == 0)
	{// Условие сработает каждый Destroy_Delay тик

		++Destroy_Index;

		if (Destroy_Index >= 0 && Destroy_Index < (int)Title_Letters.size())
			Title_Letters[Destroy_Index]->Destroy();
		else
			can_finish = true;
	}

	for (auto* letter : Title_Letters)
	{
		letter->Act();

		all_letters_are_finished &= letter->Is_Finished();
	}

	if (can_finish && all_letters_are_finished)
		Game_Title_State = EGame_Title_State::Finished;

}
//------------------------------------------------------------------------------------------------------------
void AsGame_Title::Animate_Game_Won()
{
	unsigned char r, g, b;
	int letter_index;
	AFinal_Letter* letter;

	letter_index = AsTools::Rand((int)Title_Letters.size());

	letter = Title_Letters[letter_index];

	r = AsTools::Rand(256);
	g = AsTools::Rand(256);
	b = AsTools::Rand(256);

	letter->Set_Color(r, g, b);
}
//------------------------------------------------------------------------------------------------------------




// AsTutorial
//------------------------------------------------------------------------------------------------------------
AsTutorial::AsTutorial()
	: Tutorial_State(ETutorial_State::Missing), Font(AsConfig::Tutorial_Font)
{
	const int scale = AsConfig::Global_Scale;

	Tutorial_Rect.left = X_Pos * scale;
	Tutorial_Rect.top = Y_Pos * scale;
	Tutorial_Rect.right = Tutorial_Rect.left + Width * scale;
	Tutorial_Rect.bottom = Tutorial_Rect.top + Height * scale;
}
//------------------------------------------------------------------------------------------------------------
void AsTutorial::Begin_Movement()
{
}
//------------------------------------------------------------------------------------------------------------
void AsTutorial::Finish_Movement()
{
}
//------------------------------------------------------------------------------------------------------------
void AsTutorial::Advance(double max_speed)
{
}
//------------------------------------------------------------------------------------------------------------
double AsTutorial::Get_Speed()
{
	return 0;
}
//------------------------------------------------------------------------------------------------------------
void AsTutorial::Act()
{
	// Заглушка. Не используется в данном классе
}
//------------------------------------------------------------------------------------------------------------
void AsTutorial::Clear(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (Tutorial_State != ETutorial_State::Clear)
		return;

	if (!IntersectRect(&intersection_rect, &paint_area, &Tutorial_Rect))
		return;

	AsTools::Rect(hdc, Tutorial_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AsTutorial::Draw(HDC hdc, RECT& paint_area)
{
	int y_offset = 35;
	const int scale = AsConfig::Global_Scale;
	RECT intersection_rect;

	if (Tutorial_State != ETutorial_State::Draw)
		return;

	if (!IntersectRect(&intersection_rect, &paint_area, &Tutorial_Rect))
		return;

	const wchar_t* str_1 = L"1. Для начала игры введите имя и нажмите Enter.";
	const wchar_t* str_2 = L"2. Нажмите пробел, чтобы отпустить мячик с платформы.";
	const wchar_t* str_3 = L"3. Используйте стрелки влево и вправо на клавиатуре";
	const wchar_t* str_4 = L"для перемещения платформы.";

	SetBkMode(hdc, TRANSPARENT);
	Font.Select(hdc);
	SetTextColor(hdc, AsConfig::White_Color.Get_RGB());

	TextOut(hdc, X_Pos * scale, Y_Pos * scale, str_1, 47);
	TextOut(hdc, X_Pos * scale, Y_Pos * scale + y_offset, str_2, 53);
	TextOut(hdc, X_Pos * scale, Y_Pos * scale + y_offset * 2, str_3, 51);
	TextOut(hdc, X_Pos * scale, Y_Pos * scale + y_offset * 3, str_4, 26);
}
//------------------------------------------------------------------------------------------------------------
bool AsTutorial::Is_Finished()
{
	return false;  // заглушка. Не используется
}
//------------------------------------------------------------------------------------------------------------
void AsTutorial::Check_Name(bool got_name)
{
	if (got_name)
		Tutorial_State = ETutorial_State::Clear;
	else
		Tutorial_State = ETutorial_State::Draw;

	AsTools::Invalidate_Rect(Tutorial_Rect);
}
//------------------------------------------------------------------------------------------------------------
