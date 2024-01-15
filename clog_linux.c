#ifdef __linux__

#include "clog.h"
#include "clog_data.h"
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

static int clog_find_seq(clog_t *_log) {
    if (_log == NULL ||
        _log->config.directory == NULL ||
        _log->config.name == NULL) {
        return 0;
    }
    DIR *dir = opendir(_log->config.directory);
    if (dir == NULL) {
        return 0;
    }
    int seq_num = 0;
    struct dirent *d = NULL;
    while ((d = readdir(dir)) != NULL) {
        if (d->d_type != DT_REG) {
            continue;
        }
        if (strncmp(
                d->d_name,
                _log->config.name,
                _log->config.name_len) != 0) {
            continue;
        }
        size_t length = strlen(d->d_name);
        // backup log path
        size_t path_len = _log->config.dir_len;
        path_len += length;
        path_len += 4;
        char *path = (char *)malloc(path_len);
        if (path == NULL) {
            return 0;
        }
        memcpy(path, _log->config.directory,
            _log->config.dir_len);
        path[_log->config.dir_len] = 0;
        strcat(path, d->d_name);
        // check log path
        while (length > 0) {
            if (strcmp(d->d_name + length, ".log") == 0) {
                d->d_name[length] = 0;
                break;
            }
            --length;
        }
        size_t start = length;
        while (start > 0) {
            --start;
            if (!isdigit(d->d_name[start])) {
                ++start;
                break;
            }
        }
        if (start == 0) {
            continue;
        }
        length -= start;
        char seq_str[256];
        memcpy(seq_str, d->d_name + start, length);
        seq_str[length] = 0;
        char *seq_ret = NULL;
        long seq_val = strtol(
                seq_str, &seq_ret, 10);
        if (seq_ret != NULL && seq_val > seq_num &&
            seq_val <= LOG_SEQ_MAX) {
            int fd = open(path, O_RDONLY);
            free(path);
            path =  NULL;
            if (fd > 0) {
                struct stat f_state = {0};
                fstat(fd, &f_state);
                close(fd);
                if (f_state.st_size >= LOG_SIZE_MAX) {
                    continue;
                }
            }
            seq_num = (int) seq_val;
        }
        free(path);
    }
    closedir(dir);
    return seq_num;
}

static size_t clog_get_size(clog_t *_log) {
    if (_log->m_path == NULL) {
        return 0;
    }
    int fd = open(_log->m_path, O_RDONLY);
    if (fd < 0) {
        return 0;
    }
    struct stat f_state = {0};
    fstat(fd, &f_state);
    close(fd);
    return f_state.st_size;
}

char *clog_gen_path(clog_t *_log) {
    clog_config_t *config = (clog_config_t *) _log;
    if (config->name == NULL) {
        return NULL;
    }
    if (config->directory == NULL) {
        const char *dir = "./";
        config->dir_len = (int) strlen(dir);
        config->directory =
                (char *) malloc(config->dir_len + 4);
        if (config->directory == NULL) {
            config->dir_len = 0;
            return NULL;
        }
        strcpy(config->directory, dir);
    }
    if (config->sequence == 0) {
        config->sequence =
                clog_find_seq(_log);
        if (config->sequence == 0) {
            ++config->sequence;
        }
    }
    size_t size = clog_get_size(_log);
    if (size > LOG_SIZE_MAX) {
        ++config->sequence;
        if (config->sequence > LOG_SEQ_MAX) {
            config->sequence = 1;
        }
        free(_log->m_path);
        _log->m_path = NULL;
    }
    if (_log->m_path == NULL) {
        char *path = NULL;
        char suffix[16] = {0};
        int length = sprintf(
                suffix, "_%02d.log",
                config->sequence);
        length += config->dir_len;
        length += config->name_len;
        _log->m_path =
                (char *) malloc(length + 1);
        if (_log->m_path == NULL) {
            return NULL;
        }
        path = _log->m_path;
        strcpy(path, config->directory);
        path = path + config->dir_len;
        strcpy(path, config->name);
        path = path + config->name_len;
        strcpy(path, suffix);
        size = clog_get_size(_log);
        if (size > LOG_SIZE_MAX) {
            remove(_log->m_path);
        }
    }
    return _log->m_path;
}

void clog_lock(clog_t *_log) {
    pthread_mutex_lock(
            (pthread_mutex_t *) _log->m_lock);
}

void clog_unlock(clog_t *_log) {
    pthread_mutex_unlock(
            (pthread_mutex_t *) _log->m_lock);
}

int clog_datetime(char *_datetime) {
    int result = 0;
    time_t seconds = time(0);
    struct tm curr = {0};
    localtime_r(&seconds, &curr);
    result = snprintf(
            _datetime, 24,
            "[%04d-%02d-%02d %02d:%02d:%02d] ",
            curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday,
            curr.tm_hour, curr.tm_min, curr.tm_sec);
    return result;
}

clog_t *clog_create(int _msgsz_max) {
    char *addr = NULL;
    clog_t *log = NULL;
    if (_msgsz_max < 1024) {
        _msgsz_max = 1024;
    }
    int size = _msgsz_max + sizeof(clog_t) + 1;
    addr = (char *) malloc(size);
    if (addr == NULL) {
        return log;
    }
    memset(addr, 0, size);
    log = (clog_t *) addr;
    addr = addr + sizeof(clog_t);
    log->m_lock = (pthread_mutex_t *) addr;
    addr = addr + sizeof(pthread_mutex_t);
    pthread_mutex_init(log->m_lock, NULL);
    log->m_msgbuf = addr;
    log->config.level = CLOG_LEVEL_DEBUG;
    log->config.msgsize = _msgsz_max;
    return log;
}

void clog_desrtroy(clog_t *_log) {
    clog_config_t *config =
            (clog_config_t *) _log;
    if (_log == NULL) {
        return;
    }
    if (config->directory) {
        free(config->directory);
    }
    if (config->name) {
        free(config->name);
    }
    config->directory =
    config->name = NULL;
    if (_log->m_path) {
        free(_log->m_path);
    }
    _log->m_path = NULL;
    free(_log);
    _log = NULL;
}

#endif // __linux__
