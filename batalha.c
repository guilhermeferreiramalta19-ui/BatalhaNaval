#include <stdio.h>
#include <stdlib.h> // Para system("cls")/"clear" e rand()
#include <time.h>   // Para srand()
#include <ctype.h>  // Para toupper()

// --- Constantes do Jogo ---
#define TAMANHO_TABULEIRO 10
#define NUM_NAVIOS 5

// --- Protótipos das Funções ---
void limparTela();
void pausar();
void inicializarTabuleiro(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
void imprimirTabuleiros(char tabuleiroJogador[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], char tabuleiroRastreio[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
void imprimirTabuleiroUnico(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
void posicionarNaviosJogador(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
void posicionarNaviosCPU(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
int lugarValido(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], int tam, int lin, int col, char ori);
void obterTiroJogador(int* lin, int* col, char tabuleiroRastreio[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
void obterTiroCPU(int* lin, int* col, char tabuleiroJogador[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);

// --- Função Principal ---
int main() {
    // Define os navios (tamanhos)
    int navios[NUM_NAVIOS] = {5, 4, 3, 3, 2}; // Porta-aviões, Navio-tanque, Cruzador, Submarino, Destróier
    int totalAcertosNecessarios = 0;
    for(int i = 0; i < NUM_NAVIOS; i++) {
        totalAcertosNecessarios += navios[i];
    }

    int acertosJogador = 0;
    int acertosCPU = 0;
    int turno = 0; // 0 = Jogador, 1 = CPU

    // Tabuleiros
    char tabuleiroJogador[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    char tabuleiroCPU[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];         // Oculto do jogador
    char tabuleiroRastreio[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]; // Tiros do jogador

    // Inicializa a semente para números aleatórios
    srand(time(NULL));

    // 1. Inicializar todos os tabuleiros
    inicializarTabuleiro(tabuleiroJogador);
    inicializarTabuleiro(tabuleiroCPU);
    inicializarTabuleiro(tabuleiroRastreio);

    // 2. Posicionar Navios
    posicionarNaviosJogador(tabuleiroJogador);
    posicionarNaviosCPU(tabuleiroCPU);

    // 3. Loop Principal do Jogo
    limparTela();
    printf("--- BATALHA NAVAL INICIADA! ---\n\n");
    pausar();

    while (acertosJogador < totalAcertosNecessarios && acertosCPU < totalAcertosNecessarios) {
        limparTela();
        imprimirTabuleiros(tabuleiroJogador, tabuleiroRastreio);

        if (turno == 0) {
            // --- Turno do Jogador ---
            printf("\n>>> SEU TURNO <<<\n");
            int lin, col;
            obterTiroJogador(&lin, &col, tabuleiroRastreio);

            // Processar o tiro
            if (tabuleiroCPU[lin][col] == 'N') {
                printf("\nACERTO! Você atingiu um navio inimigo!\n");
                tabuleiroRastreio[lin][col] = 'X'; // Marca o acerto no tabuleiro de rastreio
                tabuleiroCPU[lin][col] = 'X';      // Marca o acerto no tabuleiro real do CPU
                acertosJogador++;
            } else {
                printf("\nÁGUA! Você atirou na água.\n");
                tabuleiroRastreio[lin][col] = 'O';
            }
            turno = 1; // Passa o turno para o CPU
        } else {
            // --- Turno do CPU ---
            printf("\n>>> TURNO DO COMPUTADOR <<<\n");
            int lin, col;
            obterTiroCPU(&lin, &col, tabuleiroJogador);

            printf("O computador atira em %c%d...\n", 'A' + lin, col);

            // Processar o tiro
            if (tabuleiroJogador[lin][col] == 'N') {
                printf("ACERTO! O computador atingiu um de seus navios!\n");
                tabuleiroJogador[lin][col] = 'X'; // Marca o acerto no tabuleiro do jogador
                acertosCPU++;
            } else {
                printf("ÁGUA! O computador atirou na água.\n");
                tabuleiroJogador[lin][col] = 'O';
            }
            turno = 0; // Passa o turno para o Jogador
        }

        // Checar condição de vitória
        if (acertosJogador == totalAcertosNecessarios) {
            printf("\n\n========================================\n");
            printf("  PARABÉNS! VOCÊ VENCEU A BATALHA!\n");
            printf("========================================\n");
        } else if (acertosCPU == totalAcertosNecessarios) {
            printf("\n\n========================================\n");
            printf("  FIM DE JOGO! O COMPUTADOR VENCEU.\n");
            printf("========================================\n");
        } else {
            pausar();
        }
    }

    // Opcional: Mostrar o tabuleiro do CPU no final
    printf("\n--- Tabuleiro Final do CPU ---\n");
    imprimirTabuleiroUnico(tabuleiroCPU);

    return 0;
}


// --- Implementação das Funções ---

/**
 * @brief Limpa o console (multi-plataforma).
 */
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/**
 * @brief Pausa a execução esperando que o usuário pressione Enter.
 */
void pausar() {
    printf("\nPressione Enter para continuar...");
    // Limpa o buffer de entrada antes de esperar
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar(); // Espera pelo Enter
}

/**
 * @brief Preenche o tabuleiro com o caractere de 'água' (~).
 * @param tabuleiro O tabuleiro 2D a ser inicializado.
 */
void inicializarTabuleiro(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
            tabuleiro[i][j] = '~';
        }
    }
}

/**
 * @brief Imprime o tabuleiro do jogador e o de rastreio lado a lado.
 * @param tabuleiroJogador O tabuleiro 10x10 do jogador (seus navios).
 * @param tabuleiroRastreio O tabuleiro 10x10 de rastreio (seus tiros).
 */
void imprimirTabuleiros(char tabuleiroJogador[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], char tabuleiroRastreio[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    
    printf("        SEU TABULEIRO (Seus Navios) \t\t   TABULEIRO DE TIROS (Inimigo)\n");
    printf("    0  1  2  3  4  5  6  7  8  9 \t\t    0  1  2  3  4  5  6  7  8  9\n");
    printf("   --------------------------------\t\t   --------------------------------\n");

    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        // Imprime linha do Tabuleiro do Jogador
        printf(" %c |", 'A' + i);
        for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
            printf(" %c ", tabuleiroJogador[i][j]);
        }
        printf("|\t\t");

        // Imprime linha do Tabuleiro de Rastreio
        printf(" %c |", 'A' + i);
        for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
            printf(" %c ", tabuleiroRastreio[i][j]);
        }
        printf("|\n");
    }
    printf("   --------------------------------\t\t   --------------------------------\n");
    printf("Legenda: ~ Agua | N Navio | X Acerto | O Tiro na Agua\n");
}

/**
 * @brief Imprime um único tabuleiro (usado para mostrar o gabarito no final).
 */
void imprimirTabuleiroUnico(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    printf("    0  1  2  3  4  5  6  7  8  9\n");
    printf("   --------------------------------\n");
    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        printf(" %c |", 'A' + i);
        for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
            // No final, mostra navios não atingidos como 'N'
            if(tabuleiro[i][j] == 'N') printf(" N ");
            else printf(" %c ", tabuleiro[i][j]);
        }
        printf("|\n");
    }
    printf("   --------------------------------\n");
}


/**
 * @brief Verifica se um navio pode ser colocado na posição e orientação dadas.
 * @return 1 se for válido, 0 se for inválido (fora dos limites ou sobreposição).
 */
int lugarValido(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], int tam, int lin, int col, char ori) {
    for (int i = 0; i < tam; i++) {
        if (ori == 'H' || ori == 'h') {
            // 1. Verifica se está dentro dos limites horizontais
            if (col + i >= TAMANHO_TABULEIRO) return 0; 
            // 2. Verifica se sobrepõe outro navio
            if (tabuleiro[lin][col + i] == 'N') return 0;
        } else { // Vertical
            // 1. Verifica se está dentro dos limites verticais
            if (lin + i >= TAMANHO_TABULEIRO) return 0;
            // 2. Verifica se sobrepõe outro navio
            if (tabuleiro[lin + i][col] == 'N') return 0;
        }
    }
    return 1; // Se passou por todas as verificações, é válido
}

/**
 * @brief Pergunta ao jogador onde posicionar seus 5 navios.
 */
void posicionarNaviosJogador(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    int navios[NUM_NAVIOS] = {5, 4, 3, 3, 2};
    const char* nomesNavios[NUM_NAVIOS] = {"Porta-aviões", "Navio-tanque", "Cruzador", "Submarino", "Destróier"};

    char coord[10];
    int lin, col;
    char ori;

    for (int i = 0; i < NUM_NAVIOS; i++) {
        int valido = 0;
        while (!valido) {
            limparTela();
            printf("--- Posicione seus Navios ---\n");
            imprimirTabuleiroUnico(tabuleiro);
            printf("\nPosicionando: %s (Tamanho: %d)\n", nomesNavios[i], navios[i]);
            
            // 1. Obter Coordenada (ex: A5)
            printf("Digite a coordenada inicial (ex: A5): ");
            scanf("%s", coord);
            lin = toupper(coord[0]) - 'A';
            col = coord[1] - '0'; // Converte '5' para 5

            // 2. Obter Orientação (H/V)
            printf("Digite a orientação (H para Horizontal, V para Vertical): ");
            scanf(" %c", &ori);
            ori = toupper(ori);

            // 3. Validar
            if (lin < 0 || lin >= TAMANHO_TABULEIRO || col < 0 || col >= TAMANHO_TABULEIRO) {
                printf("Coordenada inválida! Fora do tabuleiro. Tente novamente.\n");
                pausar();
            } else if (ori != 'H' && ori != 'V') {
                printf("Orientação inválida! Use H ou V. Tente novamente.\n");
                pausar();
            } else if (!lugarValido(tabuleiro, navios[i], lin, col, ori)) {
                printf("Posição inválida! O navio sai do tabuleiro ou sobrepõe outro. Tente novamente.\n");
                pausar();
            } else {
                valido = 1; // Posição é válida!
            }
        }

        // 4. Posicionar o navio no tabuleiro
        for (int j = 0; j < navios[i]; j++) {
            if (ori == 'H') {
                tabuleiro[lin][col + j] = 'N';
            } else { // 'V'
                tabuleiro[lin + j][col] = 'N';
            }
        }
    }
}

/**
 * @brief Posiciona aleatoriamente os navios do CPU.
 */
void posicionarNaviosCPU(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    int navios[NUM_NAVIOS] = {5, 4, 3, 3, 2};
    int lin, col;
    char ori;

    for (int i = 0; i < NUM_NAVIOS; i++) {
        int valido = 0;
        while (!valido) {
            // Gera posição e orientação aleatórias
            lin = rand() % TAMANHO_TABULEIRO;
            col = rand() % TAMANHO_TABULEIRO;
            ori = (rand() % 2 == 0) ? 'H' : 'V';

            // Verifica se é válido
            if (lugarValido(tabuleiro, navios[i], lin, col, ori)) {
                valido = 1;
                // Posiciona o navio
                for (int j = 0; j < navios[i]; j++) {
                    if (ori == 'H') {
                        tabuleiro[lin][col + j] = 'N';
                    } else { // 'V'
                        tabuleiro[lin + j][col] = 'N';
                    }
                }
            }
        }
    }
}

/**
 * @brief Pede ao jogador uma coordenada para atirar.
 * Valida a entrada e verifica se o local já foi atingido.
 */
void obterTiroJogador(int* lin, int* col, char tabuleiroRastreio[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    char coord[10];
    int valido = 0;

    while (!valido) {
        printf("Digite a coordenada do seu tiro (ex: A5): ");
        scanf("%s", coord);

        // Converte (ex: 'A' -> 0, '5' -> 5)
        *lin = toupper(coord[0]) - 'A';
        *col = coord[1] - '0';

        // Validação
        if (*lin < 0 || *lin >= TAMANHO_TABULEIRO || *col < 0 || *col >= TAMANHO_TABULEIRO) {
            printf("Coordenada inválida! Fora do tabuleiro. Tente novamente.\n");
        } else if (tabuleiroRastreio[*lin][*col] != '~') {
            printf("Você já atirou nesse local! Tente novamente.\n");
        } else {
            valido = 1;
        }
    }
}

/**
 * @brief Gera um tiro aleatório para o CPU.
 * Garante que o CPU não atire no mesmo local duas vezes.
 */
void obterTiroCPU(int* lin, int* col, char tabuleiroJogador[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    int valido = 0;
    while (!valido) {
        *lin = rand() % TAMANHO_TABULEIRO;
        *col = rand() % TAMANHO_TABULEIRO;

        // Verifica se o local *não* foi atingido (não é 'X' nem 'O')
        if (tabuleiroJogador[*lin][*col] != 'X' && tabuleiroJogador[*lin][*col] != 'O') {
            valido = 1;
        }
    }
}