/*************************************************************************
	> File Name: replacer.c
	> Author: thomas
	> Mail: yuanmenggo@hotmail.com 
	> Created Time: 2014年06月08日 星期日 13时19分07秒
 ************************************************************************/

#include "replacer.h"

int 
main(int argc, char *const *argv)
{
    u_char *p;
    size_t i = 0;
    struct stat stat_buf;
    re_cycle_t cycle;

    re_memzero(&cycle, sizeof(re_cycle_t));
    cycle.pool = re_create_pool(RE_POOL_SIZE);
    if (cycle.pool == NULL){
        return 1;
    }

    re_cycle = &cycle;
    if(re_save_argv(&cycle, argc, argv) != RE_OK){
        return 1;
    }

    i++;
    while(i < argc){
        p = (u_char *) argv[i];
        stat(p, &stat_buf);
        if(S_ISDIR(stat_buf.st_mode)){
            process_dir(&cycle, p);
        }else{
            re_str_t f;
            f = (re_str_t)re_string(p);
            process_file(&cycle, &f);
        }
        i++;
    }
    return 0;
}

static size_t
re_save_argv(re_cycle_t *cycle, int argc, char *const *argv)
{
    size_t len, i=0;
    u_char **paths;
    paths =  re_alloc((argc + 1) * sizeof(char *));
    if (paths == NULL){
        return RE_ERROR;
    }
    for (i = 1; i < argc; i++){
        len = strlen(argv[i]) + 1;
        paths[i] = re_alloc(len);
        if (paths[i] == NULL){
            return RE_ERROR;
        }
        (void)re_cpystrn((u_char *)paths[i], (u_char *)argv[i], len);
    }

    paths[i] = NULL;
    cycle->paths = paths;
    return RE_OK;
}

void 
process_dir(re_cycle_t *cycle, u_char *path)
{
    DIR *dir;
    struct dirent *ptr;
    struct stat stat_buf;
    re_str_t p;
    chdir(path);
    dir = opendir(path);
    while((ptr = readdir(dir)) != NULL){
        if (stat(ptr->d_name, &stat_buf) == -1){
            continue;
        }
        if ((stat_buf.st_mode & S_IFDIR) && strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") !=0){
            p = nstring(path, "/", ptr->d_name, "\0");
            process_file(cycle, &p);
        }
        if (stat_buf.st_mode & S_IFREG){
            p = (re_str_t)re_string(ptr->d_name);
            process_file(cycle, &p);
        }
        chdir(path);
    }
    closedir(dir);
}

re_file_t *
build_rep_file(re_cycle_t *cycle, re_str_t *name)
{
    re_str_t  tmp_name, inc_name;
    re_buf_t  *buf, *tmp_buf, *inc_buf;
    re_file_t *file;

    if (name->len == 0) {
       exit_with_error("open file error with empty name");
    }

    file = re_pcalloc(cycle->pool, sizeof(re_file_t));

    file->fd_file.fd = open_read_file(name->data);
    if( file->fd_file.fd == RE_ERROR){
        exit_with_error("open file error");
    }

    buf = re_pcalloc(cycle->pool, sizeof(re_buf_t));
    file->fd_file.name = name;
    file->fd_file.offset = 0;
    file->fd_file.buff = buf;
    init_buf(cycle, buf);


    tmp_name = resetstr(name ,".tmp");
    file->fd_tmp_file.name = &tmp_name;

    file->fd_tmp_file.fd = open_write_file(tmp_name.data);
    if( file->fd_tmp_file.fd == RE_ERROR){
        exit_with_error("open tmp file error");
    }
    tmp_buf = re_pcalloc(cycle->pool, sizeof(re_buf_t));
    file->fd_tmp_file.offset = 0;
    file->fd_tmp_file.buff = tmp_buf;
    init_buf(cycle, tmp_buf);



    inc_name = resetstr(name ,".h");
    file->fd_inc_file.name = &inc_name;

    file->fd_inc_file.fd = open_write_file(inc_name.data);
    if( file->fd_inc_file.fd == RE_ERROR){
        exit_with_error("open include file error");
    }
    inc_buf = re_pcalloc(cycle->pool, sizeof(re_buf_t));
    file->fd_inc_file.offset = 0;
    file->fd_inc_file.buff = inc_buf;
    init_buf(cycle, inc_buf);

    return file;
}

void *
init_buf(re_cycle_t *cycle, re_buf_t *buf)
{
    buf->start = re_pnalloc(cycle->pool, RE_BUFF_SIZE);
    if(buf->start == NULL){
        exit_with_error("buf malloc error");
    }
    buf->pos = buf->start;
    buf->last = buf->start;
    buf->end =buf->last +  RE_BUFF_SIZE;
}

size_t 
process_file(re_cycle_t *cycle, re_str_t *name)
{
    size_t nlen; 
    re_file_t   *cur_file;        
    cur_file = build_rep_file(cycle, name);

    fstat(cur_file->fd_file.fd, &cur_file->info);
    loop_replace(cycle, cur_file);
}

size_t
loop_replace(re_cycle_t *cycle, re_file_t  *rep_file){
    off_t   file_size;
    u_char  str[10], lang[]="LAN", *start, ch, *s, *src, *dst;
    size_t  len,size, n, r, offset, fd, tmp_fd, inc_fd;
    uintptr_t   commented, quoted, t_quoted, line;
    int order = 0;
    re_buf_t   *b, *tmp_buf, *inc_buf;

    fd = rep_file->fd_file.fd;
    tmp_fd = rep_file->fd_tmp_file.fd;
    inc_fd = rep_file->fd_inc_file.fd;

    b = rep_file->fd_file.buff;
    tmp_buf = rep_file->fd_tmp_file.buff;
    inc_buf = rep_file->fd_inc_file.buff;

    start = b->pos;
    file_size = rep_file->info.st_size;
    quoted = 0;
    t_quoted = 0;

    for( ;; ){
        len = b->pos - start;
        offset = rep_file->fd_file.offset;
        size =(size_t)(file_size - offset);
        if(size <=0)
        {
            close_file(rep_file);
            return exit_with_ok("finished");
        }
        if(size > b->end - (b->start + len))
        {
            size = b->end -(b->start + len);
        }

        n = pread(fd, b->start, size, offset);
        if(n == -1)
        {
            return exit_with_error("pread error");
        }
        rep_file->fd_file.offset +=n;
        offset += n;
        b->last = b->pos + n;
        start = b->start;

        for(; b->pos < b->last;){
            ch = *b->pos++;
            if(ch=='"'){
                if(t_quoted){
                    t_quoted = 0;
                    continue;
                }
                if(quoted){
                    quoted = 0;
                    int len=b->pos - b->start +4;
                                    
                    s = re_pnalloc(cycle->pool, len);
                    re_memzero(s, len);
                    dst = s;
                    *dst='=';
                    *++dst='"';
                    for(dst++, src = b->start;  src < b->pos;){
                        *dst++ = *src++;
                    }
                    *dst++ = '\n';
                    *dst++= '\0';
                    if (include_chinese(s, len)){
                        order++;
                        sprintf(str, "%s_%d", lang, order);
                        write_buf(tmp_fd, tmp_buf, str, (size_t)strlen(str));
                        write_buf(inc_fd, inc_buf, str, (size_t)strlen(str));
                        write_buf(inc_fd, inc_buf, s, (size_t)(strlen(s)));
                    }else{
                        write_buf(tmp_fd, tmp_buf, s+1, (size_t)len-3);
                    }

                    b->start = b->pos;
                }else{
                    write_buf(tmp_fd, tmp_buf, b->start, (size_t)(b->pos - b->start-1));
                    b->start = b->pos;
                    quoted = 1;
                }
            }
            if(ch=='\\'){
                if(quoted) 
                    t_quoted=1;     
            }

        }
    }
    return 0;
}

size_t
close_file(re_file_t *file)
{
    if(close(file->fd_file.fd) ==RE_ERROR){
        return exit_with_error("close fd error");
    }
    write_last(file->fd_tmp_file.fd, file->fd_tmp_file.buff);
    if(close(file->fd_tmp_file.fd) ==RE_ERROR){
        return exit_with_error("close tmp_fd error");
    }
    write_last(file->fd_inc_file.fd, file->fd_inc_file.buff);
    if(close(file->fd_inc_file.fd) ==RE_ERROR){
        return exit_with_error("close h_fd error");
    }
    return RE_OK;
}




