
// mouse

enum Mouse_buttons
{
	LBUTTON,
	RBUTTON,

	MOUSE_BUTTONS_COUNT
};


struct Buttons
{
	bool is_down{ false };
	bool changed{ false };
	bool pressed{ false };
};


struct Mouse_Input
{
	float pos_x, pos_y;
	Buttons buttons[MOUSE_BUTTONS_COUNT];
};



// keyboard

enum Keyboard_buttons
{
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,

	BUTTONS_COUNT
};


struct Key_Input
{
	Buttons buttons[BUTTONS_COUNT];

	Key_Input()
	{
		for (int i = 0; i < BUTTONS_COUNT; i++)
		{
			buttons[i].is_down = false;
			buttons[i].changed = false;
		}
	}

	void discard()
	{
		for (int i = 0; i < BUTTONS_COUNT; i++)
		{
			buttons[i].is_down = false;
			buttons[i].changed = false;
		}
	}
};


