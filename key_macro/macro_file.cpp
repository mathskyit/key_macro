#include "stdafx.h"
#include "macro_def.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void LoadMacros (const char *fileName)
{
	FILE *fp = fopen (fileName, "rb");
	if (!fp) return;

	g_macros.clear ();

	long c = 0;
	fread (&c, sizeof(long), 1, fp);

	g_macros.resize(c);
	for (int i=0; i<c; ++i) {
		sMacro &m = g_macros[i];

		fread (m.name,       sizeof(char), 256, fp);
		fread (&m.start_key, sizeof(long), 1, fp);
		fread (&m.stop_key,  sizeof(long), 1, fp);
		fread (&m.options,   sizeof(long), 1, fp);
		
		long d = 0;
		fread (&d, sizeof(long), 1, fp);

		m._item.resize (d);
		for (int j=0; j<d; ++j) {
			fread (&m._item[j], sizeof(sMacroItem), 1, fp);
			
			if(m._item[j].type == sMacroItem::STRING) {
				int len = m._item[j].str.length;
				m._item[j].str.text = new char[len+1];
				
				fread (m._item[j].str.text, sizeof(char), len, fp);
				m._item[j].str.text[len] = 0;
			}
			else if(m._item[j].type == sMacroItem::MACRO) {
				m._item[j].mcall.name = new char[256];
				
				fread (m._item[j].mcall.name, sizeof(char), 256, fp);
			}
		}
	}
	fclose (fp);
}

void SaveMacro (const char *fileName)
{
	FILE *fp = fopen (fileName, "wb");
	if (!fp) return;

	long c = g_macros.size();
	fwrite (&c, sizeof(long), 1, fp);

	for (int i=0; i<c; ++i) {
		sMacro &m = g_macros[i];

		fwrite (m.name,       sizeof(char), 256, fp);
		fwrite (&m.start_key, sizeof(long), 1, fp);
		fwrite (&m.stop_key,  sizeof(long), 1, fp);
		fwrite (&m.options,   sizeof(long), 1, fp);
		
		long d = m._item.size();
		fwrite (&d, sizeof(long), 1, fp);

		for (int j=0; j<d; ++j) {
			fwrite (&m._item[j], sizeof(sMacroItem), 1, fp);
			
			if(m._item[j].type == sMacroItem::STRING) {
				int len = m._item[j].str.length;
				
				fwrite (m._item[j].str.text, sizeof(char), len, fp);
			}
			else if(m._item[j].type == sMacroItem::MACRO) {
				fwrite (m._item[j].mcall.name, sizeof(char), 256, fp);
			}
		}
	}
	fclose (fp);
}
