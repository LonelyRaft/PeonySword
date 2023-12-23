#include "clog.h"
#include "clog_data.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define strncpy(dest, src, size) strcpy_s(dest, size, src)
#define vsnprintf vsprintf_s
#define snprintf sprintf_s
#endif // _WIN32

static const char *stdfmt[] = {
        "\033[0m",     // clear format
        "\033[1;34m",  // bold and blue
        "\033[1;32m",  // bold and green
        "\033[1;33m",  // bold and yellow
        "\033[1;31m",  // bold and red
        "\033[1;35m",  // bold and purple
};

static int clog_level2str(char *_log_level, int _level) {
    if (_log_level == NULL) {
        return 0;
    }
    switch (_level) {
        case CLOG_LEVEL_INFO:
            strcpy(_log_level, "[Info] ");
            break;
        case CLOG_LEVEL_DEBUG:
            strcpy(_log_level, "[Debug] ");
            break;
        case CLOG_LEVEL_WARN:
            strcpy(_log_level, "[Warn] ");
            break;
        case CLOG_LEVEL_ERROR:
            strcpy(_log_level, "[Error] ");
            break;
        default:
            _log_level[0] = 0;
            break;
    }
    return (int) strlen(_log_level);
}

static int clog_write(
        clog_t *_log, const clog_head_t *_head,
        const char *_message, int _length) {
    int result = 0, idx = 0;
    char header[512] = {0};
    const clog_config_t *config =
            (clog_config_t *) _log;
    if (config->b_datetime) {
        int length = clog_datetime(header + idx);
        if (length > 0) {
            idx += length;
        }
    }
    if (config->b_level) {
        int length = clog_level2str(
                header + idx, _head->level);
        if (length > 0) {
            idx += length;
        }
    }
    if (config->b_name && config->name) {
        // name is not null
        header[idx++] = '[';
        int length = 0;
        while (config->name[length]) {
            header[idx++] =
                    config->name[length++];
        }
        header[idx++] = ']';
        header[idx++] = ' ';
    }
    if (config->b_function) {
        header[idx++] = '[';
        int length = 0;
        while (_head->func[length]) {
            header[idx++] =
                    _head->func[length++];
        }
        header[idx++] = ']';
        header[idx++] = ' ';
    }
    if (config->b_position) {
        int length = snprintf(
                header + idx, 511 - idx, "[%s:%zu]",
                _head->file, _head->line);
        if (length > 0) {
            idx += length;
        }
    }
    if (idx > 0) {
        header[idx++] = '\n';
        header[idx] = 0;
    }
    do {
        const char *path =
                clog_gen_path(_log);
        if (path == NULL) {
            break;
        }
        FILE *fd = fopen(path, "ab");
        if (fd == NULL) {
            break;
        }
        fwrite(header, 1, idx, fd);
        fwrite(_message, 1, _length, fd);
        fclose(fd);
        result = idx + _length;
    } while (0);
    if (config->b_stdout) {
        if (idx > 0) {
            fprintf(stderr, "%s%s%s%s",
                    stdfmt[_head->level],
                    header, stdfmt[0], _message);
        } else {
            fprintf(stderr, "%s%s%s",
                    stdfmt[_head->level],
                    _message, stdfmt[0]);
        }
    }
    return result;
}

clog_t *clog_read_cfg(const char *_cfgpath) {
    if (_cfgpath == NULL ||
        _cfgpath[0] == 0) {
        return NULL;
    }
    FILE *f_config = fopen(_cfgpath, "r");
    if (f_config == NULL) {
        return NULL;
    }
    char rowbuf[1024];
    clog_config_t config = {
            1024, 0, 0, 0, 0, 0, 0,
            CLOG_LEVEL_DEBUG,
            0, 0, 0, NULL, NULL
    };
    while (fgets(rowbuf, 1024, f_config)) {
        int start = 0;
        while (rowbuf[start] != 0 &&
               isspace(rowbuf[start])) {
            ++start;
        }
        if (rowbuf[start] == '#') {
            continue;
        }
        char *val = strstr(rowbuf + start, "=");
        if (val == NULL) {
            continue;
        }
        *val = 0;
        char *p = rowbuf + start;
        while (p < val && !isspace(*p)) {
            ++p;
        }
        (*p) = 0;
        if (strcmp(rowbuf + start, "use_datetime") == 0) {
            p = strstr(++val, "true");
            if (p != NULL) {
                config.b_datetime = 1;
            }
        } else if (strcmp(rowbuf + start, "use_level") == 0) {
            p = strstr(++val, "true");
            if (p != NULL) {
                config.b_level = 1;
            }
        } else if (strcmp(rowbuf + start, "use_function") == 0) {
            p = strstr(++val, "true");
            if (p != NULL) {
                config.b_function = 1;
            }
        } else if (strcmp(rowbuf + start, "use_position") == 0) {
            p = strstr(++val, "true");
            if (p != NULL) {
                config.b_position = 1;
            }
        } else if (strcmp(rowbuf + start, "use_stdout") == 0) {
            p = strstr(++val, "true");
            if (p != NULL) {
                config.b_stdout = 1;
            }
        } else if (strcmp(rowbuf + start, "use_name") == 0) {
            p = strstr(++val, "true");
            if (p != NULL) {
                config.b_name = 1;
            }
        } else if (strcmp(rowbuf + start, "msgsize") == 0) {
            p = ++val;
            while ((*p) && !isdigit(*p)) {
                ++p;
            }
            val = p;
            while ((*val) && isdigit(*val)) {
                ++val;
            }
            *val = 0;
            config.msgsize = (int) strtol(p, NULL, 10);
        } else if (strcmp(rowbuf + start, "level") == 0) {
            p = strstr(++val, "DEBUG");
            if (p != NULL) {
                config.level = CLOG_LEVEL_DEBUG;
                continue;
            }
            p = strstr(val, "INFO");
            if (p != NULL) {
                config.level = CLOG_LEVEL_INFO;
                continue;
            }
            p = strstr(val, "WARN");
            if (p != NULL) {
                config.level = CLOG_LEVEL_WARN;
                continue;
            }
            p = strstr(val, "ERROR");
            if (p != NULL) {
                config.level = CLOG_LEVEL_ERROR;
                continue;
            }
            p = strstr(val, "CLOSE");
            if (p != NULL) {
                config.level = CLOG_LEVEL_CLOSE;
            }
        } else {
            continue;
        }
    }
    fclose(f_config);
    if (config.msgsize < 1024) {
        config.msgsize = 1024;
    }
    clog_t *log = (clog_t *) clog_create(config.msgsize);
    if (log == NULL) {
        return log;
    }
    log->config = config;
    return log;
}

int clog_use_datetime(clog_t *_log, int _show) {
    if (_log == NULL) {
        return -1;
    }
    clog_lock(_log);
    _log->config.b_datetime = _show;
    clog_unlock(_log);
    return 0;
}

int clog_use_level(clog_t *_log, int _show) {
    if (_log == NULL) {
        return -1;
    }
    clog_lock(_log);
    _log->config.b_level = _show;
    clog_unlock(_log);
    return 0;
}

int clog_use_position(clog_t *_log, int _show) {
    if (_log == NULL) {
        return -1;
    }
    clog_lock(_log);
    _log->config.b_position = _show;
    clog_unlock(_log);
    return 0;
}

int clog_use_function(clog_t *_log, int _show) {
    if (_log == NULL) {
        return -1;
    }
    clog_lock(_log);
    _log->config.b_function = _show;
    clog_unlock(_log);
    return 0;
}

int clog_use_stdout(clog_t *_log, int _show) {
    if (_log == NULL) {
        return -1;
    }
    clog_lock(_log);
    _log->config.b_stdout = _show;
    clog_unlock(_log);
    return 0;
}

int clog_use_name(clog_t *_log, int _show) {
    if (_log == NULL) {
        return -1;
    }
    clog_lock(_log);
    _log->config.b_name = _show;
    clog_unlock(_log);
    return 0;
}

int clog_set_name(clog_t *_log, const char *_name) {
    char *name_buf = NULL;
    if (_log == NULL ||
        _name == NULL ||
        _name[0] == 0) {
        return -1;
    }
    size_t length = strlen(_name);
    name_buf = (char *) malloc(length + 1);
    if (name_buf == NULL) {
        return -2;
    }
    strncpy(name_buf, _name, length + 1);
    clog_lock(_log);
    if (_log->config.name) {
        free(_log->config.name);
    }
    _log->config.name = name_buf;
    _log->config.name_len = (int) length;
    if (_log->m_path) {
        free(_log->m_path);
    }
    _log->m_path = NULL;
    _log->config.sequence = 0;
    clog_unlock(_log);
    return 0;
}

int clog_set_dir(clog_t *_log, const char *_dir) {
    char *dir_buf = NULL;
    if (_log == NULL ||
        _dir == NULL || _dir[0] == 0) {
        return -1;
    }
    size_t length = (int) strlen(_dir);
    dir_buf = (char *) malloc(length + 4);
    if (dir_buf == NULL) {
        return -2;
    }
    strcpy(dir_buf, _dir);
    if (dir_buf[length - 1] != '/' &&
        dir_buf[length - 1] != '\\') {
        dir_buf[length++] = '/';
        dir_buf[length] = 0;
    }
    clog_lock(_log);
    if (_log->config.directory) {
        free(_log->config.directory);
    }
    _log->config.directory = dir_buf;
    _log->config.dir_len = (int) length;
    if (_log->m_path) {
        free(_log->m_path);
    }
    _log->m_path = NULL;
    _log->config.sequence = 0;
    clog_unlock(_log);
    return 0;
}

int clog_set_level(clog_t *_log, int _level) {
    if (_log == NULL) {
        return -1;
    }
    if (_level < CLOG_LEVEL_DEBUG ||
        _level > CLOG_LEVEL_ERROR) {
        return -2;
    }
    clog_lock(_log);
    _log->config.level = _level;
    clog_unlock(_log);
    return 0;
}

int clog_error_(
        clog_t *_log, const char *_file, size_t _line,
        const char *_func, const char *_fmt, ...) {
    int result = 0;
    if (_log == NULL || _file == NULL ||
        _func == NULL || _fmt == NULL) {
        return result;
    }
    do {
        va_list args;
        clog_head_t header = {0};
        clog_lock(_log);
        if (_log->config.level > CLOG_LEVEL_ERROR) {
            break;
        }
        va_start(args, _fmt);
        result = vsnprintf(
                _log->m_msgbuf,
                _log->config.msgsize,
                _fmt, args);
        va_end(args);
        if (result < 0) {
            break;
        }
        _log->m_msgbuf[result++] = '\n';
        _log->m_msgbuf[result] = 0;
        header.file = _file;
        header.line = _line;
        header.func = _func;
        header.level = CLOG_LEVEL_ERROR;
        result = clog_write(
                _log, &header,
                _log->m_msgbuf, result);
    } while (0);
    clog_unlock(_log);
    return result;
}

int clog_warn_(
        clog_t *_log, const char *_file, size_t _line,
        const char *_func, const char *_fmt, ...) {
    int result = 0;
    if (_log == NULL || _file == NULL ||
        _func == NULL || _fmt == NULL) {
        return result;
    }
    do {
        va_list args;
        clog_head_t header = {0};
        clog_lock(_log);
        if (_log->config.level > CLOG_LEVEL_WARN) {
            break;
        }
        va_start(args, _fmt);
        result = vsnprintf(
                _log->m_msgbuf,
                _log->config.msgsize,
                _fmt, args);
        va_end(args);
        if (result < 0) {
            break;
        }
        _log->m_msgbuf[result++] = '\n';
        _log->m_msgbuf[result] = 0;
        header.file = _file;
        header.line = _line;
        header.func = _func;
        header.level = CLOG_LEVEL_WARN;
        result = clog_write(
                _log, &header,
                _log->m_msgbuf, result);
    } while (0);
    clog_unlock(_log);
    return result;
}

int clog_info_(
        clog_t *_log, const char *_file, size_t _line,
        const char *_func, const char *_fmt, ...) {
    int result = 0;
    if (_log == NULL || _file == NULL ||
        _func == NULL || _fmt == NULL) {
        return result;
    }
    do {
        va_list args;
        clog_head_t header = {0};
        clog_lock(_log);
        if (_log->config.level > CLOG_LEVEL_INFO) {
            break;
        }
        va_start(args, _fmt);
        result = vsnprintf(
                _log->m_msgbuf,
                _log->config.msgsize,
                _fmt, args);
        va_end(args);
        if (result < 0) {
            break;
        }
        _log->m_msgbuf[result++] = '\n';
        _log->m_msgbuf[result] = 0;
        header.file = _file;
        header.line = _line;
        header.func = _func;
        header.level = CLOG_LEVEL_INFO;
        result = clog_write(
                _log, &header,
                _log->m_msgbuf, result);
    } while (0);
    clog_unlock(_log);
    return result;
}

int clog_debug_(
        clog_t *_log, const char *_file, size_t _line,
        const char *_func, const char *_fmt, ...) {
    int result = 0;
    if (_log == NULL || _file == NULL ||
        _func == NULL || _fmt == NULL) {
        return result;
    }
    do {
        va_list args;
        clog_head_t header = {0};
        clog_lock(_log);
        if (_log->config.level > CLOG_LEVEL_DEBUG) {
            break;
        }
        va_start(args, _fmt);
        result = vsnprintf(
                _log->m_msgbuf,
                _log->config.msgsize,
                _fmt, args);
        va_end(args);
        if (result < 0) {
            break;
        }
        _log->m_msgbuf[result++] = '\n';
        _log->m_msgbuf[result] = 0;
        header.file = _file;
        header.line = _line;
        header.func = _func;
        header.level = CLOG_LEVEL_DEBUG;
        result = clog_write(
                _log, &header,
                _log->m_msgbuf, result);
    } while (0);
    clog_unlock(_log);
    return result;
}

int clog_status_(clog_t *_log, const char *_fmt, ...) {
    int result = 0;
    if (_log == NULL || _fmt == NULL) {
        return 0;
    }
    do {
        va_list args;
        clog_lock(_log);
        va_start(args, _fmt);
        result = vsnprintf(
                _log->m_msgbuf,
                _log->config.msgsize,
                _fmt, args);
        va_end(args);
        if (result < 0) {
            break;
        }
        _log->m_msgbuf[result++] = '\n';
        _log->m_msgbuf[result] = 0;
        fprintf(
                stderr, "%s%s%s",
                stdfmt[CLOG_LEVEL_CLOSE],
                _log->m_msgbuf, stdfmt[0]);
    } while (0);
    clog_unlock(_log);
    return result;
}


