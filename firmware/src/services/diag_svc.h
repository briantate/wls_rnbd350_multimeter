/**
 * @file diag_svc.h
 * @brief Diagnostic logging to dedicated UART (M-06)
 */

#ifndef DIAG_SVC_H
#define DIAG_SVC_H

typedef enum {
    DIAG_DEBUG,
    DIAG_INFO,
    DIAG_WARN,
    DIAG_ERROR
} diag_level_t;

#ifndef DIAG_VERBOSITY
#define DIAG_VERBOSITY DIAG_INFO
#endif

#define DIAG_LOG(level, subsys, msg, ...) \
    do { if ((level) >= DIAG_VERBOSITY) diag_svc_log((level), (subsys), (msg), ##__VA_ARGS__); } while(0)

void diag_svc_init(void);
void diag_svc_log(diag_level_t level, const char* subsys, const char* fmt, ...);

#endif /* DIAG_SVC_H */
