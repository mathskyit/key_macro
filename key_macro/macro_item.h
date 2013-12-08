#pragma once

#define VK_KEY_DOWN		0x00000000
#define VK_KEY_UP		0x00010000
#define VK_KEY_MASK		0x0000FFFF

#define MOUSEEVENTF_SCREEN_INDEPENDENT_POS         0x0200


struct sKey {
	long vk;
	unsigned long flags;
	// 0x01 - ������
	// 0x02 - ����
	// 0x03 - ������ ����

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
	long delay; // ����: msec
	unsigned long flags; 
	// 0x00 - �ð�����(delay) ����
	// 0x01 - �����ð�(time) ����
	// 0x02 - �����ð��� ������ ����
	// 0x04 - �����ð��� ��/�� ������ ����
	// 0x06 - �����ð��� ��/��/�ð� ������ ����

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
