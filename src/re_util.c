/*************************************************************************
	> File Name: replacer.c
	> Author: thomas
	> Mail: yuanmenggo@hotmail.com 
	> Created Time: 2014年06月08日 星期日 13时19分07秒
 ************************************************************************/

#include "replacer.h"

u_char *
re_str(u_char *str, u_char *str1, u_char *str2){
     strcpy(str, str1);
     strcat(str, str2);
     return str;
}

size_t 
open_read_file(u_char *name){
	open((const u_char *)name, O_RDWR, 0);
}

size_t 
open_write_file(u_char *name){
	 open((const char *)name, O_CREAT|O_RDWR);
}

size_t 
exit_with_error(u_char *str){
	printf("%s, errno:%d, errstr:%s \n", str, errno, strerror(errno));
    return 1;
}

size_t
exit_with_ok(u_char *str){
	printf("%s\n", str);
    return 0;
}


size_t
write_buf(size_t fd, re_buf_t *buf, u_char *src, size_t len)
{
    size_t r;
    if (buf->end - buf->last < len)
    {
        r = write(fd, buf->start, buf->last - buf->start);
        if(r == RE_ERROR){
            return exit_with_error("write fd error");
        }
        buf->last = buf->start;
    }

    re_cpybuf(buf->last, src, len);
    buf->last +=len;
    return RE_OK;
}

void *
write_last(size_t fd, re_buf_t *buf)
{
    size_t r;
    if (buf->last > buf->start){
        r = write(fd, buf->start, buf->last - buf->start);
        if(r == RE_ERROR){
            exit_with_error("write fd error");
        }
        buf->last = buf->start;
    }
}

size_t
is_chinese(char c1, char c2)
{
	if(c1 == 0) return 0;
	if(c1 & 0x80){
		if(c2 & 0x80){
			return 1;
		}
	}
	return 0;
}

size_t
include_chinese(char *str, size_t len)
{
	char c;
	while(--len >= 0 ){
		c=*str++;
		if (c==0) break;
		if(c & 0x80){
			if(*str & 0x80){
				return 1;
			}
		}
	}
	return 0;
}