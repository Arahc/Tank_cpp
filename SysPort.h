/*
 * @brief Let the code portable from different platforms
 * @file SysPort.h
 * Main things to handle:
 *   - input: _kbhit(), _getch()
 *   - time: sleep(), LARGE_INTEGER, QueryPerformanceCounter()
 * Almost all of the code is copy from `Base.h`, `Terminal.h` (also TA's code in piazza)
 */

#pragma once
#include <stdint.h>
#include <time.h>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

// keyboard input

#ifndef _WIN32

int kbhit() {
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    struct termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int getch() {
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    struct termios newt = oldt;
    newt.c_lflag &= ~ICANON;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

#endif

// time control

void Daze() {
#if TK_ICC || TK_MSVC
    _mm_pause();
#elif __arm__ || __aarch64__
    __builtin_arm_yield();
#else
    __builtin_ia32_pause();
#endif
}

typedef struct {
    uint64_t freq;
    uint64_t cnt;
} sysTimer;

#define _1StoNS 1000000000

void timerFreqInit(sysTimer *timer) {
#ifdef _WIN32
    QueryPerformanceFrequency((LARGE_INTEGER *)&timer->freq);
#else
    timer->freq = _1StoNS;
#endif
}

void timerCntGet(sysTimer *timer) {
#ifdef _WIN32
    QueryPerformanceCounter((LARGE_INTEGER *)&timer->cnt);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    timer->cnt = (uint64_t)ts.tv_sec * _1StoNS + ts.tv_nsec;
#endif
}

double getTime(sysTimer *bg, sysTimer *ed) {
    return (double)(ed->cnt - bg->cnt) / (double)bg->freq;
}
