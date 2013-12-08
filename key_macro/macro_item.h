#pragma once

#define VK_KEY_DOWN		0x00000000
#define VK_KEY_UP		0x00010000
#define VK_KEY_MASK		0x0000FFFF

#define MOUSEEVENTF_SCREEN_INDEPENDENT_POS         0x0200


struct sKey {
	long vk;
	unsigned long flags;
	// 0x01 - 누르기
	// 0x02 - 떼기
	// 0x03 - 누르고 떼기

	void Step (bool key_pressed[256]);
};

struct sMouse {
	long x, y;
	unsigned long flags;

	void Step (bool key_pressed[256]);

private:
	BYTE GetMouseVK (unsigned long button);
};

struct sTime {
	long delay; // 단위: msec
	unsigned long flags; 
	// 0x00 - 시간지연(delay) 설정
	// 0x01 - 지연시각(time) 설정
	// 0x02 - 지연시각을 단위로 설정
	// 0x04 - 지연시각을 초/분 단위로 설정
	// 0x06 - 지연시각을 초/분/시간 단위로 설정

	bool Step (long &remain_time, int dt);

private:
	int GetTimeDelay (int flag, int delay);
	int GetRandomDelay (int delay, int random);
};

struct sString {
	long length;
	unsigned long flags;
	char *text;

	bool Step (long &str_index);

private:
	inline BYTE GetUpperRand () { return 'A' + rand()%26; }
	inline BYTE GetLowerRand () { return 'a' + rand()%26; }
	inline BYTE GetNumberRand() { return '0' + rand()%10; }
	
	BYTE GetVK (unsigned char ch, bool *shift_key);
};

struct sMacroCall {
	char *name;

	bool Step (long &str_index);
};

struct sLock {
	unsigned long flags;

	bool Step (bool &cs_enter);
};

struct sMacroItem {
	enum { NONE = 0, KEY, MOUSE, TIME, STRING, MACRO, CS } type;

	union {
		sKey       key;
		sMouse     mouse;
		sTime      time;
		sString    str;
		sMacroCall mcall;
		sLock      lock;
		long       data[3];
	};

	sMacroItem () : type(NONE) 
	{ 
		data[0] = data[1] = data[2] = 0;
	}
	
	~sMacroItem () 
	{
		Delete ();
	}

	sMacroItem (const sMacroItem &mi) 
	{
		Copy (mi);
	}
	
	sMacroItem &operator = (const sMacroItem &mi) 
	{
		if (this != &mi) {
			Delete ();
			Copy (mi);
		}
		return *this;
	}

private:
	void Delete (); 
	void Copy (const sMacroItem &mi); 
};
