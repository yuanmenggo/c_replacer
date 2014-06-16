#include "replacer.h"

re_str_t 
nstring(const u_char *str, ...){
	u_char	buf[RE_STR_BUFF_SIZE], *p, *last, *value;
	va_list		args;
	last = buf + RE_STR_BUFF_SIZE;
	p = buf;
	while(*str && buf < last){
			*p++ = *str++;
	}
	va_start(args, str);
	while((value = va_arg(args, u_char *)) && *value != '\0'){
		while(*value && buf < last){
			*p++ = *value++;
		}
	}
	va_end(args);
	*p = '\0';
	return (re_str_t)re_string(buf);
}

re_str_t
resetstr(re_str_t *oldstr, const u_char *addstr){
	u_char *p, *ostr = oldstr->data;
	size_t t =0, len = strlen(ostr);
	p = ostr + len;
	while(p--){
		if (*p == '.'){
			break;
		} else if (*p == '\0'){
			continue;
		}
		len--;
	}
	u_char newstr[len];
	p = newstr;
	while(++t < len){
		*p++ = *ostr++;
	}
	return (re_str_t)nstring(newstr, addstr, "\0");
}

u_char *
re_cpystrn(u_char *dst, u_char *src, size_t n)
{
	if(n == 0){
		return dst;
	}
	while(--n){
		*dst = *src;
		if (*dst == '\0'){
			return dst;
		}
		dst++;
		src++;
	}
	*dst = '\0';
	return dst;
}

u_char *
re_cpybuf(u_char *dst, u_char *src, size_t len)
{
	if (len == 0){
		return dst;
	}
	while(--len >= 0 && *src){
		*dst++ = *src++;
	}
	return dst;
}
// int 
// main(int argc, char *const *argv)
// {
// 	re_str_t s;

// 	s= nstring("abcdefgaaaf", ".pt", "\0");
// 	printf("2----%s\n", s.data);
// 	printf("3---%d\n", (int)s.len);
// 	re_str_t t = resetstr(&s ,".hi");
// 	printf("8---%s\n", t.data);
// 	return 0;
// }