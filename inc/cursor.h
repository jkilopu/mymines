/**
 * @file cursor.h
 * @author jkilopu
 * @brief Show remote cursor on window.
 */
#ifndef __CURSOR_H
#define __CURSOR_H

#define REMOTE_CURSOR_IMG_PATH "res/remote_cursor.gif"
#define CURSOR_WIDTH 20
#define CURSOR_HEIGHT 25

void draw_remote_cursor(unsigned int y, unsigned int x);

#endif