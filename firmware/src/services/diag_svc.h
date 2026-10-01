/**
 * @file diag_svc.h
 * @brief Diagnostic logging to dedicated UART (M-06)
 */

#ifndef BTOHM_SERVICES_DIAG_SVC_H_
#define BTOHM_SERVICES_DIAG_SVC_H_

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
    do { if ((level) >= DIAG_VERBOSITY) \
        diag_svc_log((level), (subsys), (msg), ##__VA_ARGS__); \
    } while (0)

/**
 * @brief Initialize the diagnostic service.
 *
 * Configures UART for diagnostic output.
 */
void diag_svc_init(void);

/**
 * @brief Log a diagnostic message.
 *
 * Formats and transmits a timestamped message.
 *
 * @param level   Severity level.
 * @param subsys  Subsystem identifier string.
 * @param fmt     printf-style format string.
 * @param ...     Format arguments.
 */
void diag_svc_log(diag_level_t level, const char* subsys, const char* fmt, ...);

#endif  /* BTOHM_SERVICES_DIAG_SVC_H_ */
