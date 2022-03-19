#ifndef __BRKSHELL__HELPER__
#define __BRKSHELL__HELPER__

static inline void eqstr_del(char* str1, char* str2, int len)
{
	int j;

	for (j = 0; j < len; j++) {
		str1[j] = str2[j]; // eqstr
		str2[j] = 0; // _del
	}
	
	str1[j] = '\0';
}


#endif
