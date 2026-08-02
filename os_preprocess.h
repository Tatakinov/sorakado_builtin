#ifndef OS_PREPROCESS_H_
#define OS_PREPROCESS_H_

#if defined(_WIN32) || defined(WIN32)
#define IS_WINDOWS
#endif // WIN32

#if defined(__unix__) && !defined(__APPLE__)
#define IS__NIX
#endif // Linux/Unix

#endif // OS_PREPROCESS_H_
