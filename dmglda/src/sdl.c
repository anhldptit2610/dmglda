#include "sdl.h"

struct sdl {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} sdl;

int sdl_init(gb_t *gb)
{
    // TODO: optimize this function to make it shorter and sweeter

    int error = 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        error = 1;
    }

    sdl.window = SDL_CreateWindow("dmglda", 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, SDL_WINDOW_SHOWN);
    if (!sdl.window) {
        SDL_Log("Unable to create window: %s\n", SDL_GetError());
        error = 1;
    }

    sdl.renderer = SDL_CreateRenderer(sdl.window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!sdl.renderer) {
        SDL_Log("Unable to create renderer: %s\n", SDL_GetError());
        error = 1;
    }
    SDL_RenderSetLogicalSize(sdl.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetIntegerScale(sdl.renderer, 1);

    sdl.texture = SDL_CreateTexture(sdl.renderer,
                    SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                    SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!sdl.texture) {
        SDL_Log("Unable to create texture: %s\n", SDL_GetError());
        error = 1;
    }

    gb->ppu.frame_buffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    if (!gb->ppu.frame_buffer) {
        GB_Error("[ERROR] sdl_init: malloc failed\n");
        error = 1;
    }

    return error;
}

void sdl_set_renderer_color(uint32_t color)
{
    uint8_t r = (color >> 24) & 0xff;
    uint8_t g = (color >> 16) & 0xff;
    uint8_t b = (color >> 8) & 0xff;
    uint8_t a = (color >> 0) & 0xff;

    SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
}

void sdl_drawframe(gb_t *gb)
{
    if (gb->ppu.first_frame_passed == false) {
        gb->ppu.first_frame_passed = true;
        return;
    }
    SDL_RenderClear(sdl.renderer);
    SDL_UpdateTexture(sdl.texture, NULL, gb->ppu.frame_buffer, SCREEN_WIDTH * 4);
    SDL_RenderCopy(sdl.renderer, sdl.texture, NULL, NULL);
    SDL_RenderPresent(sdl.renderer);
    gb->ppu.need_refresh = false;
}

void sdl_handle_input(gb_t *gb)
{
    SDL_Event event;
    const uint8_t *current_key_state = NULL;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            gb->quit = true;
            mbc_save_data(gb);
            break;
        default:
            break;
        }
    }
    current_key_state = SDL_GetKeyboardState(NULL);
    if (current_key_state[SDL_SCANCODE_Z] && gb->joypad.b)
        joypad_press_button(gb, JOYPAD_B);
    else if (current_key_state[SDL_SCANCODE_X] && gb->joypad.a)
        joypad_press_button(gb, JOYPAD_A);
    else if (current_key_state[SDL_SCANCODE_SPACE] && gb->joypad.select)
        joypad_press_button(gb, JOYPAD_SELECT);
    else if (current_key_state[SDL_SCANCODE_RETURN] && gb->joypad.start)
        joypad_press_button(gb, JOYPAD_START);
    else if (current_key_state[SDL_SCANCODE_UP] && gb->joypad.up)
        joypad_press_button(gb, JOYPAD_UP);
    else if (current_key_state[SDL_SCANCODE_DOWN] && gb->joypad.down)
        joypad_press_button(gb, JOYPAD_DOWN);
    else if (current_key_state[SDL_SCANCODE_LEFT] && gb->joypad.left)
        joypad_press_button(gb, JOYPAD_LEFT);
    else if (current_key_state[SDL_SCANCODE_RIGHT] && gb->joypad.right)
        joypad_press_button(gb, JOYPAD_RIGHT);
    else if (!current_key_state[SDL_SCANCODE_Z] && !gb->joypad.b)
        joypad_release_button(gb, JOYPAD_B);
    else if (!current_key_state[SDL_SCANCODE_X] && !gb->joypad.a)
        joypad_release_button(gb, JOYPAD_A);
    else if (!current_key_state[SDL_SCANCODE_SPACE] && !gb->joypad.select)
        joypad_release_button(gb, JOYPAD_SELECT);
    else if (!current_key_state[SDL_SCANCODE_RETURN] && !gb->joypad.start)
        joypad_release_button(gb, JOYPAD_START);
    else if (!current_key_state[SDL_SCANCODE_UP] && !gb->joypad.up)
        joypad_release_button(gb, JOYPAD_UP);
    else if (!current_key_state[SDL_SCANCODE_DOWN] && !gb->joypad.down)
        joypad_release_button(gb, JOYPAD_DOWN);
    else if (!current_key_state[SDL_SCANCODE_LEFT] && !gb->joypad.left)
        joypad_release_button(gb, JOYPAD_LEFT);
    else if (!current_key_state[SDL_SCANCODE_RIGHT] && !gb->joypad.right)
        joypad_release_button(gb, JOYPAD_RIGHT);

}

void sdl_cleanup(void)
{
    SDL_DestroyRenderer(sdl.renderer);
    sdl.renderer = NULL;
    SDL_DestroyWindow(sdl.window);
    sdl.window = NULL;
    SDL_DestroyTexture(sdl.texture);
    sdl.texture = NULL;
}