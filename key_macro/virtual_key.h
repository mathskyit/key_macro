#pragma once

struct VIRTUAL_KEY {
	int vk;
	const char *desc;
};

extern VIRTUAL_KEY g_vk_list[];

extern int getVkIndex (int vk);
