
#ifndef CLOG_H
#define CLOG_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define CLOG_LEVEL_CLOSE 0x05
#define CLOG_LEVEL_ERROR 0x04
#define CLOG_LEVEL_WARN 0x03
#define CLOG_LEVEL_INFO 0x02
#define CLOG_LEVEL_DEBUG 0x01

typedef struct clog_t clog_t;

#ifdef _MSC_VER
#ifdef CLOG_EXPORT
#define CLOG_API __declspec(dllexport)
#else
#define CLOG_API __declspec(dllimport)
#endif
#else
#define CLOG_API
#endif

/************************************
 * # create a CLog object
 * @_msgsz_max: max length of a piece of message
 * @return: a CLog object pointer or NULL
 ************************************/
CLOG_API
clog_t *clog_create(int _msgsz_max);

/************************************
 * # create a CLog object by config file
 * @_cfgpath: config file path
 * @return: a CLog object pointer or NULL
 ************************************/
CLOG_API
clog_t *clog_read_cfg(
    const char *_cfgpath);

/************************************
 * # free a CLog object
 * @_log: a CLog object pointer created by clog_create
 ************************************/
CLOG_API
void clog_desrtroy(clog_t *_log);

/************************************
 * # show date and time in log
 * @_log: a CLog object pointer
 * @_show:  0 is not
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_use_datetime(
    clog_t *_log, int _show);

/************************************
 * # show log level in log
 * @_log: a CLog object pointer
 * @_show:  0 is not
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_use_level(
    clog_t *_log, int _show);

/************************************
 * # show position in log
 * @_log: a CLog object pointer
 * @_show:  0 is not
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_use_position(
    clog_t *_log, int _show);

/************************************
 * # show function in log
 * @_log: a CLog object pointer
 * @_show:  0 is not
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_use_function(
    clog_t *_log, int _show);

/************************************
 * # print logs on stdout at the same time
 * @_log: a CLog object pointer
 * @_show:  0 is not
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_use_stdout(
    clog_t *_log, int _show);

/************************************
 * # show log name in log
 * @_log: a CLog object pointer
 * @_show:  0 is not
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_use_name(
    clog_t *_log, int _show);

/************************************
 * # set name for log file
 * @_log: a CLog object pointer
 * @_name:  new file name
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_set_name(
    clog_t *_log, const char *_name);

/************************************
 * # set path for log file
 * @_log: a CLog object pointer
 * @_dire:  new file directory
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_set_dir(
    clog_t *_log, const char *_dire);

/************************************
 * # set level for log
 * @_log: a CLog object pointer
 * @_level:  log level
 * @return: 0 is success
 ************************************/
CLOG_API
int clog_set_level(
    clog_t *_log, int _level);

CLOG_API
int clog_error_(
    clog_t *_log, const char *_file, size_t _line,
    const char *_func, const char *_fmt, ...);

CLOG_API
int clog_warn_(
    clog_t *_log, const char *_file, size_t _line,
    const char *_func, const char *_fmt, ...);

CLOG_API
int clog_info_(
    clog_t *_log, const char *_file, size_t _line,
    const char *_func, const char *_fmt, ...);

CLOG_API
int clog_debug_(
    clog_t *_log, const char *_file, size_t _line,
    const char *_func, const char *_fmt, ...);

CLOG_API
int clog_status_(
    clog_t *_log, const char *_fmt, ...);

/************************************
 * # output error message
 * @log:    CLog object pointer
 * @fmt:    message format
 * @...:    message data
 * @return:   message length
 *************************************/
#define clog_error(log, fmt, ...) clog_error_( \
    log, __FILE__, __LINE__, __FUNCTION__,     \
    fmt, ##__VA_ARGS__)

/************************************
 * # output warning message
 * @log:    CLog object pointer
 * @fmt:    message format
 * @...:    message data
 * @return:  message length
 *************************************/
#define clog_warn(log, fmt, ...) clog_warn_( \
    log, __FILE__, __LINE__, __FUNCTION__,   \
    fmt, ##__VA_ARGS__)

/************************************
 * # output information message
 * @log:    CLog object pointer
 * @fmt:    message format
 * @...:    message data
 * @return:  message length
 *************************************/
#define clog_info(log, fmt, ...) clog_info_( \
    log, __FILE__, __LINE__, __FUNCTION__,   \
    fmt, ##__VA_ARGS__)

/************************************
 * # output debug message
 * @log:    CLog object pointer
 * @fmt:    message format
 * @...:    message data
 * @return:  message length
 *************************************/
#define clog_debug(log, fmt, ...) clog_debug_( \
    log, __FILE__, __LINE__, __FUNCTION__,     \
    fmt, ##__VA_ARGS__)

/************************************
 * # output status message
 * @log:    CLog object pointer
 * @fmt:    message format
 * @...:    message data
 * @return:  message length
 *************************************/
#define clog_status(log, fmt, ...) clog_status_( \
    log, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CLOG_H
