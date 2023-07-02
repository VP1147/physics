// Simulador de Gás Ideal - Simula um conjunto de partículas 
// de massa definida contidas em um espaço (janela), dada uma
// certa temperatura, a partir da distribuição de velocidades
// de Boltzmann. Para visualização, foi incluido um fator de
// velocidade (menor = mais lento).

// Vinícius A. Pavão - Aluno de Graduação
// Instituto de Física - UFMS

// Para compilar no GNU/Linux:
// $ gcc boltz.c -o boltz -lSDL2 -lSDL2_ttf -lm
// Execução: $ ./boltz

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define RECT_SIZE 1
#define NUM_RECTS 10000
#define TEMPERATURE 273.0       // Temperatura absoluta (K)
#define MASS_IN_U 18.0          // Massa das partículas (u)
#define SPEED_FACTOR 1/1000     // Fator de velocidade
#define FONT_SIZE 14
#define FONT_PATH "FreeMono.ttf"
#define FPS 60                  // Frames por segundo

typedef struct {
    float x;
    float y;
    float dx;
    float dy;
} Rectangle;

float generateBoltzmannVelocity(float temperature, float mass) {
    const float k = 1.380649e-23;                           // Constante de Boltzmann em J/K
    const float uToKg = 1.66053906660e-27;                  // Conversão de unidades de massa atômica para quilogramas
    float boltzmannDistribution = sqrtf(-2.0f * k * temperature / (mass * uToKg) * logf(rand() / (float)RAND_MAX));
    return boltzmannDistribution;
}

void updateRectangle(Rectangle *rect) {
    rect->x += rect->dx * SPEED_FACTOR; // Variacao de posicao em X
    rect->y += rect->dy * SPEED_FACTOR; // Variacao de posicao em Y

    // Detecta colisao com as bordas da janela
    if (rect->x < 0 || rect->x + RECT_SIZE > WINDOW_WIDTH) {
        rect->dx *= -1;
    }

    if (rect->y < 0 || rect->y + RECT_SIZE > WINDOW_HEIGHT) {
        rect->dy *= -1;
    }
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, (SDL_Color){255, 255, 255, 255});
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char *argv[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;
    TTF_Font *font = NULL;
    int quit = 0;
    Rectangle rects[NUM_RECTS];

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // Caso SDL nao consiga inicializar
        printf("Falha ao inicializar a SDL: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Simulador de Gás Ideal", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        // Caso nao consiga criar a janela
        printf("Falha ao criar a janela: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        // Caso nao consiga criar o renderer
        printf("Falha ao criar o renderer: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        // Caso nao consiga inicializar a biblioteca SDL_ttf
        printf("Falha ao inicializar a biblioteca SDL_ttf: %s\n", TTF_GetError());
        return -1;
    }

    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (font == NULL) {
        // Caso nao consiga carregar a fonte
        printf("Falha ao carregar a fonte: %s\n", TTF_GetError());
        return -1;
    }
    // Recarrega o gerador de valores pseudoaleatorios
    srand(time(NULL));

    for (int i = 0; i < NUM_RECTS; i++) {
        // Atribui posicoes aleatorias para as particulas
        rects[i].x = rand() % (WINDOW_WIDTH - RECT_SIZE);
        rects[i].y = rand() % (WINDOW_HEIGHT - RECT_SIZE);

        // Gera velocidades aleatórias baseadas na distribuição de Boltzmann
        rects[i].dx = generateBoltzmannVelocity(TEMPERATURE, MASS_IN_U) * sqrtf(TEMPERATURE);
        rects[i].dy = generateBoltzmannVelocity(TEMPERATURE, MASS_IN_U) * sqrtf(TEMPERATURE);
    }

    while (!quit) {
        // Delay entre frames (ms)
        SDL_Delay(1000/FPS);
        // Caso janela ser fechada
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
        for (int i = 0; i < NUM_RECTS; i++) {
            updateRectangle(&rects[i]);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < NUM_RECTS; i++) {
            SDL_Rect sdlRect = {(int)rects[i].x, (int)rects[i].y, RECT_SIZE, RECT_SIZE};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &sdlRect);
        }

        // Renderizar indicadores
        char numParticlesText[50];
        sprintf(numParticlesText, "n = %d", NUM_RECTS);
        renderText(renderer, font, numParticlesText, 10, 20);

        char temperatureText[50];
        sprintf(temperatureText, "T = %.2f K", TEMPERATURE);
        renderText(renderer, font, temperatureText, 10, 40);

        // Calcular a velocidade média quadrática
        float averageVelocity = 0.0f;
        for (int i = 0; i < NUM_RECTS; i++) {
            averageVelocity += sqrtf(powf(rects[i].dx, 2) + powf(rects[i].dy, 2));
        }
        averageVelocity /= NUM_RECTS;

        char averageVelocityText[50];
        sprintf(averageVelocityText, "V_rms = %.2f m/s", averageVelocity);
        renderText(renderer, font, averageVelocityText, 10, 60);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}