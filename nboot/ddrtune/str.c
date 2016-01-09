#include "str.h"

char *strcpy (char *s1, char *s2)
{
	char	*p = s1;

	while (*s2)
	{
		*s1 = *s2;
		s1++; s2++;
	}

	*s1 = '\0';

	return p;
}

int strlen (char *s)
{
	int i = 0;

	while (*s++)
		i++;

	return i;
}

char *long2str (char *s, unsigned long l)
{
	int i;
	char	hex[16];

	for (i = 0; i < 16; i++)
	{
		hex[i] = (i < 10) ? i + '0' : (i - 10 + 'a');
	}

	for (i = 0; i < 8; i++)
	{
		s[i] = hex[(l >> (7-i)*4) & 0x0fUL];
	}

	s[i] = '\0';

	return s;

}

char *strcat (char *s1, char *s2)
{
	char *p = s1;

	while (*s1)
		s1++;

	strcpy (s1, s2);
	return p;
}

/* end */
