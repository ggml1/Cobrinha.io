#include "libSocket/server.h"
#include "libAllegro/AllegroCore.h"

DADOS packet_server[maxPlayers];
direc pack_server;
sync syncy;
int l, alguem;

int worldWidth = 2000;
int worldHeight = 2000;
enum directions { UP, DOWN, LEFT, RIGHT };
int *orientation;
float *orientation_rad;
int moveSpeed = 2;
const float FPS = 60.0;

int count, z, idAtual, quantPlayers;

int main()
{
    //Inicialização básica do Allegro, como vista no Client
    if(!coreInit())
        return -1;

    //Criação de uma janela 150px por 100px com o titulo "Server"
    if (!windowInit(150, 100, "Server"))
        return -1;

    struct msg_ret_t retorno;
    bool sair = false;

    orientation = (int *) malloc(1 * sizeof(int));
    orientation_rad = (float *) malloc(1 * sizeof(float));
    syncy.numPlayers = 0;
    *orientation = 0;
    *orientation_rad = 0;

    serverInit(maxPlayers);

    while(!sair)
    {
        startTimer();
        int id = acceptConnection();

        if(id != NO_CONNECTION)
        {
            printf("Alguem se Conectou com ID : %d\n", id);
            quantPlayers = id;
            if(quantPlayers == 0)
            {
                alguem = 1;

                packet_server[id].x = 1000;
                packet_server[id].y = 1000;

                for(l=0; l<25; l++)
                {
                    packet_server[id].orientacao[l] = -1;
                }

                for (count = 0; count < (score / 2); count++)
                {
                    packet_server[id].orientacao[count] = 0;
                }

            } else if(quantPlayers > 0){

                    orientation = (int *) realloc(orientation, (quantPlayers+1) * sizeof(int));
                    orientation_rad = (float *) realloc(orientation_rad, (quantPlayers+1) * sizeof(float));

                    orientation[quantPlayers] = 0;
                    orientation_rad[quantPlayers] = 0;

                    packet_server[id].x = 1000;
                    packet_server[id].y = 1000;

                    syncy.numPlayers = quantPlayers;

                    for(l=0; l<25; l++)
                    {
                        packet_server[id].orientacao[l] = -1;
                    }

                    for (count = 0; count < (score / 2); count++)
                    {
                        packet_server[id].orientacao[count] = 0;
                    }
                }
            
            sendMsgToClient(&id, sizeof(int), id);
        }
        
        if(alguem)
        {
            retorno = recvMsg(&pack_server);
            if(retorno.status != NO_MESSAGE)
            {   
                id = retorno.client_id;

                if (pack_server.pressed)
                {
                    switch (pack_server.dir)
                    {
                        case RIGHT:
                            orientation[id]--;
                            break;
                        case LEFT:
                            orientation[id]++;
                            break;
                    }

                    if (orientation[id] == 360)
                        orientation[id] = 0;
                    else if (orientation[id] == -1)
                        orientation[id] = 359;
                }

                orientation_rad[id] = orientation[id] * 3.1415926 / 180.0;
                for (count = (score / 2) - 1; count > 0; count--)
                    packet_server[id].orientacao[count] = packet_server[id].orientacao[count-1];

                packet_server[id].orientacao[0] = orientation_rad[id];

                packet_server[id].x += cos(orientation_rad[id]) * moveSpeed;
                packet_server[id].y -= sin(orientation_rad[id]) * moveSpeed;

                if (packet_server[id].x > worldWidth)
                        packet_server[id].x -= worldWidth;
                    else if (packet_server[id].x < 0)
                        packet_server[id].x += worldWidth;

                if (packet_server[id].y > worldHeight)
                        packet_server[id].y -= worldHeight;
                    else if (packet_server[id].y < 0)
                        packet_server[id].y += worldHeight;

                packet_server[id].r = 249;
                packet_server[id].g = 38;
                packet_server[id].b = 114;

                packet_server[id].pontos = score;

                for(idAtual = 0; idAtual <= quantPlayers; idAtual++)
                {
                    if(idAtual != id)
                    {
                        for (count = (score / 2) - 1; count > 0; count--)
                            packet_server[idAtual].orientacao[count] = packet_server[idAtual].orientacao[count-1];

                        packet_server[idAtual].orientacao[0] = orientation_rad[idAtual];

                        (packet_server[idAtual].x) += cos(orientation_rad[idAtual]) * moveSpeed;
                        (packet_server[idAtual].y) -= sin(orientation_rad[idAtual]) * moveSpeed;

                        if (packet_server[idAtual].x > worldWidth)
                                packet_server[idAtual].x -= worldWidth;
                            else if (packet_server[idAtual].x < 0)
                                packet_server[idAtual].x += worldWidth;

                        if (packet_server[idAtual].y > worldHeight)
                                packet_server[idAtual].y -= worldHeight;
                            else if (packet_server[idAtual].y < 0)
                                packet_server[idAtual].y += worldHeight;

                        packet_server[idAtual].r = 249;
                        packet_server[idAtual].g = 38;
                        packet_server[idAtual].b = 114;

                        packet_server[idAtual].pontos = score;
                    }
                }

            } else {

                for(idAtual = 0; idAtual <= quantPlayers; idAtual++)
                {

                    for (count = (score / 2) - 1; count > 0; count--)
                        packet_server[idAtual].orientacao[count] = packet_server[idAtual].orientacao[count-1];

                    packet_server[idAtual].orientacao[0] = orientation_rad[idAtual];

                    (packet_server[idAtual].x) += cos(orientation_rad[idAtual]) * moveSpeed;
                    (packet_server[idAtual].y) -= sin(orientation_rad[idAtual]) * moveSpeed;

                    if (packet_server[idAtual].x > worldWidth)
                            packet_server[idAtual].x -= worldWidth;
                        else if (packet_server[idAtual].x < 0)
                            packet_server[idAtual].x += worldWidth;

                    if (packet_server[idAtual].y > worldHeight)
                            packet_server[idAtual].y -= worldHeight;
                        else if (packet_server[idAtual].y < 0)
                            packet_server[idAtual].y += worldHeight;

                    packet_server[idAtual].r = 249;
                    packet_server[idAtual].g = 38;
                    packet_server[idAtual].b = 114;

                    packet_server[idAtual].pontos = score;
                }
            }

            broadcast(&syncy, sizeof(sync));

            for(z = 0; z <= quantPlayers; z++)
            {
                broadcast(&packet_server[z], sizeof(DADOS));
            }
            
        }
        
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));
        FPSLimit();
    }

    free(orientation);
    free(orientation_rad);

    allegroEnd();

    return 0;
}