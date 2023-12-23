
#ifndef CLOG_CLOG_DATA_H
#define CLOG_CLOG_DATA_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define LOG_SIZE_MAX (0x10000) // 1M bytes
#define LOG_SEQ_MAX (99)

typedef struct clog_config_t {
    int msgsize;
    unsigned char b_datetime;
    unsigned char b_level;
    unsigned char b_function;
    unsigned char b_position;
    unsigned char b_stdout;
    unsigned char b_name;
    unsigned char level;
    unsigned char sequence;
    int name_len;
    int dir_len;
    char *directory;
    char *name;
} clog_config_t;

typedef struct clog_t {
    clog_config_t config;
    char *m_msgbuf;
    char *m_path;
    void *m_lock;
} clog_t;

typedef struct clog_head_t {
    const char *file;
    const char *func;
    size_t line;
    unsigned char level;
} clog_head_t;

char *clog_gen_path(clog_t *_log);

void clog_lock(clog_t *);

void clog_unlock(clog_t *);

int clog_datetime(char *_datetime);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //CLOG_CLOG_DATA_H
