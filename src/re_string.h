#ifndef _RE_STRING_H_INCLUDED_
#define _RE_STRING_H_INCLUDED_

typedef struct{
	size_t		len;
	u_char		*data;
} re_str_t;

#define	re_string(str)      		{ sizeof(str) -1, (u_char *) str }
#define	re_fix_string(str, len) 	{ len, (u_char *) str }
#define re_null_string				{ 0, NULL }
#define re_str_set(str, text)									\
	(str)->len = sizeof(text) -1;(str)->data = (u_char *) text
#define re_str_null(str)			(str)->len = 0; (str)->data = NULL


re_str_t nstring(const u_char *, ...);
re_str_t resetstr(re_str_t *, const u_char *);
u_char * re_cpystrn(u_char *, u_char *, size_t);
u_char * re_cpybuf(u_char *, u_char *, size_t);

#endif /* _RE_STRING_H_INCLUDED_ */