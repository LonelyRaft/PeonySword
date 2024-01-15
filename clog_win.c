#ifdef _WIN32

#include "clog.h"
#include "clog_data.h"
#include <time.h>
#include <stdio.h>
#include <Windows.h>
#include <direct.h>


static int clog_find_seq(clog_t *_log) {
    if (_log == NULL ||
        _log->config.directory == NULL ||
        _log->config.name == NULL) {
        return 0;
    }
    size_t length =
            strlen(_log->config.directory) +
            strlen(_log->config.name) +
            strlen("_*.log") + 4;
    char *pattern = (char *) malloc(length);
    sprintf(pattern, "%s%s_*.log",
            _log->config.directory,
            _log->config.name);
    WIN32_FIND_DATA file;
    HANDLE hFind =
            FindFirstFileA(pattern, &file);
    free(pattern);
    pattern = NULL;
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }
    int seq_num = 0;
    do {
        length = strlen(file.cFileName);
        while (length > 0) {
            if (strcmp(file.cFileName + length, ".log") == 0) {
                file.cFileName[length] = 0;
                break;
            }
            --length;
        }
        while (length > 0) {
            --length;
            if (!isdigit(file.cFileName[length])) {
                ++length;
                break;
            }
        }
        char *seq_str = NULL;
        int seq_val = strtol(
                file.cFileName + length, &seq_str, 10);
        size_t fsize = file.nFileSizeHigh;
        fsize <<= 32;
        fsize += file.nFileSizeLow;
        if (seq_str != NULL && seq_val > seq_num &&
            seq_val <= LOG_SEQ_MAX && fsize < LOG_SIZE_MAX) {
            seq_num = seq_val;
        }
    } while (FindNextFileA(hFind, &file));
    FindClose(hFind);
    return seq_num;
}

static size_t clog_get_size(clog_t *_log) {
    if (_log->m_path == NULL) {
        return 0;
    }
    WIN32_FIND_DATA file;
    HANDLE hFind =
            FindFirstFileA(_log->m_path, &file);
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }
    size_t size = file.nFileSizeHigh;
    size <<= 32;
    size += file.nFileSizeLow;
    return size;
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
        int length = -1;
        char *path = NULL;
        char suffix[16] = {0};
        length = sprintf(
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
    WaitForSingleObject(_log->m_lock, INFINITE);
}

void clog_unlock(clog_t *_log) {
    ReleaseMutex(_log->m_lock);
}

int clog_datetime(char *_datetime) {
    int result = 0;
    time_t seconds = time(0);
    struct tm curr = {0};
    localtime_s(&curr, &seconds);
    result = sprintf_s(
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
    log->m_msgbuf = addr;
    log->config.level = CLOG_LEVEL_DEBUG;
    log->config.msgsize = _msgsz_max;
    log->m_lock = CreateMutexA(NULL, FALSE, NULL);
    if (log->m_lock == NULL) {
        free(log);
        addr = NULL;
        log = NULL;
    }
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
    CloseHandle(_log->m_lock);
    _log->m_lock = NULL;
    free(_log);
    _log = NULL;
}

#endif // _WIN32


