#ifndef _RE_REPLACE_H_INCLUDED_
#define _RE_REPLACE_H_INCLUDED_
#include <string.h>
#include <inttypes.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <stdio.h>
#include <stdarg.h>


#include "re_palloc.h"
#include "re_string.h"
#include "re_queue.h"


#define  RE_OK          0
#define  RE_ERROR      -1
#define  RE_AGAIN      -2
#define  RE_BUSY       -3
#define  RE_DONE       -4
#define  RE_DECLINED   -5
#define  RE_ABORT      -6

#ifndef RE_POOL_SIZE
#define RE_POOL_SIZE            4096
#endif

#ifndef RE_BUFF_SIZE
#define RE_BUFF_SIZE            4096
#endif

#ifndef RE_STR_BUFF_SIZE
#define RE_STR_BUFF_SIZE        512
#endif

#ifndef RE_ALIGNMENT
#define RE_ALIGNMENT            sizeof(unsigned long)    /* platform word */
#endif

#define RE_POOL_ALIGNMENT       16

typedef struct stat file_info_t;

typedef struct {
      int         fd;
      re_str_t    *name;
      re_queue_t  *buffs;
      off_t       offset;

} file_fd_t;

typedef struct
{
  u_char        *start;
  size_t        len;
  size_t        index;
  size_t        type;
} buff_data_t;

typedef struct {
        file_fd_t   fd_file;
        re_buf_t    *buff; 
        file_fd_t   fd_tmp_file;
        file_fd_t   fd_inc_file;
        file_info_t info;

} re_file_t;


typedef struct {
        re_pool_t   *pool;
        re_queue_t  queue;
        u_char      **paths;
} re_cycle_t;
    
volatile re_cycle_t *re_cycle;

re_file_t * build_rep_file(re_cycle_t *, re_str_t *);
static size_t re_save_argv(re_cycle_t *, int, char *const *);
void process_dir(re_cycle_t *cycle, u_char *path);
void * init_buf(re_cycle_t *, re_buf_t *);
size_t process_file(re_cycle_t *, re_str_t *);
size_t loop_replace(re_cycle_t *, re_file_t  *);
size_t write_buf(size_t, re_buf_t *, u_char *, size_t);
void * write_last(size_t, re_buf_t *);

void add_index_queue(re_cycle_t *, re_queue_t *, size_t);
void add_buff_queue(re_cycle_t *, re_queue_t *, u_char *, size_t);
void * write_queue(size_t, re_queue_t *);
size_t close_file(re_file_t *);
size_t flush_file(re_file_t *);

u_char * re_str(u_char *, u_char *, u_char *);
size_t open_read_file(u_char *);
size_t open_write_file(u_char *);
size_t exit_with_error(u_char *);
size_t exit_with_ok(u_char *);

size_t include_chinese(char *, size_t);
size_t is_chinese(char c1, char c2);



#endif /* _RE_REPLACE_H_INCLUDED_ */