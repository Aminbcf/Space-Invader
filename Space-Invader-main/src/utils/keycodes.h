#ifndef KEYCODES_H
#define KEYCODES_H

// SDL3 keycodes for cross-platform compatibility
// These are the same values used by SDL3

// ASCII keys
#define SDLK_a 'a'
#define SDLK_b 'b'
#define SDLK_c 'c'
#define SDLK_d 'd'
#define SDLK_e 'e'
#define SDLK_f 'f'
#define SDLK_g 'g'
#define SDLK_h 'h'
#define SDLK_i 'i'
#define SDLK_j 'j'
#define SDLK_k 'k'
#define SDLK_l 'l'
#define SDLK_m 'm'
#define SDLK_n 'n'
#define SDLK_o 'o'
#define SDLK_p 'p'
#define SDLK_q 'q'
#define SDLK_r 'r'
#define SDLK_s 's'
#define SDLK_t 't'
#define SDLK_u 'u'
#define SDLK_v 'v'
#define SDLK_w 'w'
#define SDLK_x 'x'
#define SDLK_y 'y'
#define SDLK_z 'z'
#define SDLK_A 'A'
#define SDLK_B 'B'
#define SDLK_C 'C'
#define SDLK_D 'D'
#define SDLK_E 'E'
#define SDLK_F 'F'
#define SDLK_G 'G'
#define SDLK_H 'H'
#define SDLK_I 'I'
#define SDLK_J 'J'
#define SDLK_K 'K'
#define SDLK_L 'L'
#define SDLK_M 'M'
#define SDLK_N 'N'
#define SDLK_O 'O'
#define SDLK_P 'P'
#define SDLK_Q 'Q'
#define SDLK_R 'R'
#define SDLK_S 'S'
#define SDLK_T 'T'
#define SDLK_U 'U'
#define SDLK_V 'V'
#define SDLK_W 'W'
#define SDLK_X 'X'
#define SDLK_Y 'Y'
#define SDLK_Z 'Z'

// Number keys
#define SDLK_0 '0'
#define SDLK_1 '1'
#define SDLK_2 '2'
#define SDLK_3 '3'
#define SDLK_4 '4'
#define SDLK_5 '5'
#define SDLK_6 '6'
#define SDLK_7 '7'
#define SDLK_8 '8'
#define SDLK_9 '9'

// Special keys
#define SDLK_SPACE ' '
#define SDLK_RETURN '\r'
#define SDLK_ESCAPE 27
#define SDLK_BACKSPACE '\b'
#define SDLK_TAB '\t'

// Arrow keys (standard ASCII escape codes)
#define SDLK_UP 65
#define SDLK_DOWN 66
#define SDLK_RIGHT 67
#define SDLK_LEFT 68

// Function keys (F1-F12)
#define SDLK_F1 112
#define SDLK_F2 113
#define SDLK_F3 114
#define SDLK_F4 115
#define SDLK_F5 116
#define SDLK_F6 117
#define SDLK_F7 118
#define SDLK_F8 119
#define SDLK_F9 120
#define SDLK_F10 121
#define SDLK_F11 122
#define SDLK_F12 123

// Keypad keys
#define SDLK_KP_ENTER 13

// Modifier keys
#define SDL_KMOD_NONE 0x0000
#define SDL_KMOD_LSHIFT 0x0001
#define SDL_KMOD_RSHIFT 0x0002
#define SDL_KMOD_LCTRL 0x0040
#define SDL_KMOD_RCTRL 0x0080
#define SDL_KMOD_LALT 0x0100
#define SDL_KMOD_RALT 0x0200
#define SDL_KMOD_LGUI 0x0400
#define SDL_KMOD_RGUI 0x0800
#define SDL_KMOD_NUM 0x1000
#define SDL_KMOD_CAPS 0x2000
#define SDL_KMOD_MODE 0x4000
#define SDL_KMOD_SCROLL 0x8000

#define SDL_KMOD_CTRL (SDL_KMOD_LCTRL|SDL_KMOD_RCTRL)
#define SDL_KMOD_SHIFT (SDL_KMOD_LSHIFT|SDL_KMOD_RSHIFT)
#define SDL_KMOD_ALT (SDL_KMOD_LALT|SDL_KMOD_RALT)
#define SDL_KMOD_GUI (SDL_KMOD_LGUI|SDL_KMOD_RGUI)

#endif // KEYCODES_H