#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // Para funções como write, read, close
#include <fcntl.h>      // Para funções como open
#include <termios.h>    // Para configuração da porta serial
// --- CONSTANTES E VARIÁVEIS DE ESTADO ---
#define SERIAL_PORT "/dev/ttyUSB0" // Verifique sua porta! Pode ser /dev/ttyUSB0 ou outra.
#define BAUDRATE B9600             // Deve ser o mesmo do Arduino
#define MAX_POINTS 100             // Limite de pontos para o mesh
// Estrutura para armazenar um ponto 3D
struct Point3D {
    int x;
    int y;
    int z;
};
struct Point3D mesh_data[MAX_POINTS];
int mesh_index = 0;
int current_x = 0; // Posição atual X (calculada)
int current_y = 0; // Posição atual Y (calculada)
// --- FUNÇÃO DE CONFIGURAÇÃO DA PORTA SERIAL ---
int serial_open(const char *port) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Erro ao abrir a porta serial");
        return -1;
    }
    struct termios options;
    tcgetattr(fd, &options);
    // Configura o Baud rate de entrada e saída
    cfsetispeed(&options, BAUDRATE);
    cfsetospeed(&options, BAUDRATE);
    // Configuração do Frame (8 bits, sem paridade, 1 stop bit)
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    
    // Desabilita controle de fluxo de software e hardware
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_cflag &= ~CRTSCTS;
    // Modo Canônico desabilitado (raw data) e desabilita eco
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    // Aplica as configurações
    tcsetattr(fd, TCSANOW, &options);
    
    // Limpa o buffer de entrada e saída
    tcflush(fd, TCIOFLUSH);
    return fd;
}
// --- FUNÇÃO PRINCIPAL DE PROCESSAMENTO ---
void process_data(const char *data_string) {
    int deltaX, deltaY, distanceZ;
    
    // Tenta decodificar a string no formato "<%d,%d,%d>"
    // Usamos strncmp para garantir que a string comece com '<'
    if (data_string[0] == '<' && sscanf(data_string, "<%d,%d,%d>", &deltaX, &deltaY, &distanceZ) == 3) {
        
        // 1. Cálculo da Nova Posição (Odometria Simulada)
        current_x += deltaX;
        current_y += deltaY;
        
        // 2. Armazenamento no Array (Mesh)
        if (mesh_index < MAX_POINTS) {
            mesh_data[mesh_index].x = current_x;
            mesh_data[mesh_index].y = current_y;
            mesh_data[mesh_index].z = distanceZ;
            
            printf("Ponto %d: (X:%d, Y:%d, Z:%d) SALVO\n", mesh_index, current_x, current_y, distanceZ);
            mesh_index++;
        } else {
            printf("AVISO: Mesh cheio. Dados apenas exibidos: (X:%d, Y:%d, Z:%d)\n", current_x, current_y, distanceZ);
        }
        
    } else {
        // Ignora dados corrompidos
        // printf("DEBUG: Pacote ignorado: %s\n", data_string);
    }
}
// --- FUNÇÃO PRINCIPAL ---
int main() {
    int fd = serial_open(SERIAL_PORT);
    if (fd == -1) {
        fprintf(stderr, "Verifique se o Arduino está conectado e se a porta (%s) está correta.\n", SERIAL_PORT);
        return 1;
    }
    printf("Conexão Serial aberta com sucesso em %s.\n", SERIAL_PORT);
    printf("Aguardando dados do Arduino... Pressione Ctrl+C para sair.\n");
    
    // Buffer para armazenar os bytes lidos e a linha completa
    char buffer[256];
    char line[256] = "";
    int bytes_read;
    int line_len = 0;
    // Loop de leitura
    while (1) {
        // Tenta ler até 255 bytes
        bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = 0; // Termina a string com nulo
            for (int i = 0; i < bytes_read; i++) {
                char current_char = buffer[i];
                if (current_char == '<') {
                    // Novo pacote iniciando. Limpa a linha anterior.
                    line_len = 0;
                    line[line_len++] = current_char;
                } else if (current_char == '>') {
                    // Fim do pacote. Adiciona e processa a string.
                    if (line_len > 0) {
                        line[line_len++] = current_char;
                        line[line_len] = 0; // Termina a string
                        process_data(line);
                        line_len = 0; // Prepara para o próximo pacote
                    }
                } else if (line_len > 0 && line_len < sizeof(line) - 1) {
                    // Está no meio de um pacote. Adiciona o caractere.
                    line[line_len++] = current_char;
                }
            }
        }
        // sleep(0); // Não durma para ler o mais rápido possível (uso de 0 é otimização)
    }
    close(fd);
    return 0;
}

