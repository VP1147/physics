/*
Eletromagnetic oscilation simulator 
Copyright (C) 2023 Vinícius Pavão

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

// gcc -o eletromag eletromag.c -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lSDL2_ttf -lm
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <math.h>

// Tamanho da janela
#define WIDTH   600
#define HEIGHT  600
#define ZF      3

// Constantes
#define PI 3.14
#define MU0 (4 * PI * 1e-7)

// Variaveis
#define PERIOD 0.1             // rad/s 
#define MAGMOMENT 5e10

typedef struct {
    double x;
    double y;
    double magneticMoment;
    double radius; // Raio do objeto
    double B;
} MagneticObject;

void calculateMagneticField(MagneticObject object, double x, double y, double* Bx, double* By) {
    double dx = x - object.x;
    double dy = y - object.y;
    double r = sqrt(dx * dx + dy * dy);

    if (r < object.radius) {
        // Dentro do objeto, o campo magnético é zero
        *Bx = 0;
        *By = 0;
    } else {
        double theta = atan2(dy, dx);

        // Introduza uma variável de tempo para animação
        double time = SDL_GetTicks() / 1000.0; // Tempo em segundos

        // Modifique o campo magnético com base no tempo
        double angularFrequency = 2 * PI * PERIOD; // Frequência angular de PERIOD rad/s
        double phase = angularFrequency * time;

        object.B = (MU0 * object.magneticMoment * cos(phase)) / (2 * PI * r * r);

        *Bx = object.B * cos(theta);
        *By = object.B * sin(theta);
    }
}

void renderFillCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar o SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        printf("Erro ao inicializar o SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Campo Magnético", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Erro ao criar a janela: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Erro ao criar o renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("FreeMono.ttf", 12); // Escolha uma fonte e tamanho
    if (font == NULL) {
        printf("Erro ao carregar a fonte: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    MagneticObject object;
    object.x = WIDTH / 2.0;
    object.y = HEIGHT / 2.0;
    object.magneticMoment = MAGMOMENT; // Momento magnético do objeto
    object.radius = 25.0; // Raio do objeto

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        Uint32 start_time, frame_time;
        float fps;
        start_time = SDL_GetTicks();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Desenha o objeto magnético
        renderFillCircle(renderer, (int)object.x, (int)object.y, (int)object.radius);

        for (int x = 0; x < WIDTH*ZF; x+=(ZF)) {
            for (int y = 0; y < HEIGHT*ZF; y+=(ZF)) {
                double Bx, By;
                calculateMagneticField(object, x, y, &Bx, &By);

                // Mapeie os valores de Bx e By para uma cor
                Uint8 r = (Uint8)(fabs(Bx) * 255);
                Uint8 g = (Uint8)(fabs(By) * 255);
                Uint8 b = 0;
                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        // Renderiza o texto no canto inferior esquerdo
        char text[100];
        double time = SDL_GetTicks() / 1000.0; // Tempo em segundos
        snprintf(text, sizeof(text), "FPS: %.2f   Momento: %.2f", fps, object.magneticMoment);

        SDL_Color textColor = {255, 255, 255, 255}; // Cor do texto (branco)
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
        if (textSurface == NULL) {
            printf("Erro ao renderizar o texto: %s\n", TTF_GetError());
        } else {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture != NULL) {
                SDL_Rect textRect;
                textRect.x = 10;
                textRect.y = HEIGHT - 10;
                textRect.w = textSurface->w;
                textRect.h = textSurface->h;
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
        SDL_RenderPresent(renderer);
        frame_time = SDL_GetTicks()-start_time;
        fps = (frame_time > 0) ? 1000.0f / frame_time : 0.0f;
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
