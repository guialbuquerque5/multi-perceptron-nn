#include <stdio.h>
#include <stdlib.h>  
#include <time.h>
#include <stdbool.h>
#include "nn.h"
#define MAP_SIZE 50
#define VISION_DIRECTIONS 7
#define VISION_LENGTH 80



typedef struct Body{
    float x;
    float y;
    struct Body* p_next;
}Body;

typedef struct snake{
  Body* head;

  float vx;
  float vy;
  
  clock_t birth;
  clock_t death;
  int score;

  int alive;
}Snake;


/*
FUNÇÕES NN
*/
int convertResult(int r,int velx,int vely);
int getSnakeDir(int x, int y);
float* vision(char** mapa, int headx, int heady, int snake_direction,float* vision_input);
char** buildMap(int fruit_position[2], int** snake, int snakeSize);
int** getSnakeXY(Snake* snake);
int getVelNumber(int velx,int vely);
int getSize(Snake* s);




const int n_players = 1;
const int HEIGHT = 50;
const int WIDTH = 50;
const float FPS_CAP = 50.0;
clock_t frame_begin;


int** fruits = NULL;
Snake** snakes = NULL;
//char*** game_boards = newBoards(n_players);

int allSnakesHaveDied();

void destructAllBodies(Snake* snk);
void destructAllFruits();
void destructAllSnakes();

void loop();

int** newFruits();
Snake** newSnakes();

void setInitialBody(Snake* snk);
void setup();

void resetAllSnakes();
void resetFruits(int begin, int end);

int updateBodies(Body* bdy, int haveAte);
void updateSnakes();

int snakeCollides(int i);
int collideWithOwnBody(int i);
int collideWithWall(int i);
int snakeHaveAte(int i);
void updateHead(Snake* snk);
Body* newBody(float x, float y);
int randomCoordinates(int XorY);
void getMovement();
void getMovementFromKeyboard(int m);
int itIsASnake(int x, int y);
int itIsAFruit(int x, int y);
void updateImage();


int main(int argc, char const *argv[]){
    setup();
    while(1){
        loop();
    }
    destructAllSnakes();
    destructAllFruits();
}
NN* nn;
void setup(){
    srand(time(NULL));
    snakes = newSnakes();
    fruits = newFruits();
    resetAllSnakes();
    resetFruits(0, n_players);
    //turnOnBrains(n_players);
    //turnOnDisplay(HEIGHT, WIDTH);
    printf("Setup finalizadso.\n");
    nn = makeNN(21, 18, 3);
}

void loop(){
    printf("loop iniciado.\n");
    frame_begin = clock();
    if(allSnakesHaveDied()){
        resetAllSnakes();
        //reset fruits from 0 -> n_players
        resetFruits(0, n_players);
    }
    else{
        //think
        getMovement();
        updateSnakes();
        updateImage();
    }
    printf("Esperando por tick cap.\n");
    while(clock() - frame_begin < 1.0 * CLOCKS_PER_SEC / FPS_CAP){
    }
    printf("loop finalizado.\n");
}

void updateImage(){
    system("cls");
    int i = 0;
    int j = 0;
    for(i = 0; i < HEIGHT+1; i++){
        for(j = 0; j < WIDTH+1; j++){
            if(itIsASnake(j, i)){
                printf(" O");
            }
            else if(itIsAFruit(j, i)){
                printf(" X");
            }
            else if(j == 0 || j == WIDTH){
                printf(" |");
            } 
            else if(i == 0 || i == HEIGHT){
                printf(" -");
            }
            else{
                printf("  ");
            }         
        }
        printf("\n");
    }
}

int itIsASnake(int x, int y){
    int i;
    Body* temp = NULL;
    for(i = 0; i < n_players; i++){
        temp = snakes[i]->head;
        while(temp){
            if((int) temp->x == x && (int) temp->y == y){
                return 1;
            }
            temp = temp->p_next;
        }
    }
    return 0;
}

int itIsAFruit(int x, int y){
    int i;
    for(i = 0; i < n_players; i++){
        if(fruits[i][0] == x && fruits[i][1] == y){
            return 1;
        }
    }
    return 0;
}

Snake** newSnakes(){
    Snake** snks = NULL;
    snks = (Snake**)malloc(sizeof(Snake*) * n_players);

    int i;
    for(i = 0; i < n_players; i++){
        snks[i] = (Snake*)malloc(sizeof(Snake));
    }

    return snks;
}

int** newFruits(){
    int** frts = NULL;
    frts = (int**)malloc(sizeof(int*)*n_players);

    int i;
    for(i = 0; i < n_players; i++){
        frts[i] = (int*)malloc(sizeof(int)*2);
    }

    return frts;
}

void destructAllSnakes(){
    if(snakes){
        int i;
        for(i = 0; i < n_players; i++){
            if(snakes[i]){
                destructAllBodies(snakes[i]);
                free(snakes[i]);
            }
        }
        free(snakes);
    }
}

void destructAllBodies(Snake* snk){
    Body* next = snk->head;
    Body* aux = NULL;
    while(next){
        aux = next;
        next = aux->p_next;
        free(aux);
    }
}

void destructAllFruits(){
    if(fruits){
        int i;
        for(i = 0; i < n_players; i++){
            if(fruits[i]){
                free(fruits[i]);
            }
        }
        free(fruits);
    }
}

void resetAllSnakes(){
    int i; 
    for(i = 0; i < n_players; i++){
        setInitialBody(snakes[i]);        
        snakes[i]->vx = -1;
        snakes[i]->vy = 0;
        snakes[i]->score = 0;
        snakes[i]->birth = clock();
        snakes[i]->death = -1;
        snakes[i]->alive = 1;
        setInitialBody(snakes[i]);
    }
}

void setInitialBody(Snake* snk){
    snk->head = newBody(10, 10);
    updateBodies(snk->head, 1);
    updateHead(snk);
    updateBodies(snk->head, 1);
    updateHead(snk);
    updateBodies(snk->head, 1);
    updateHead(snk);
}

int** getSnakeXY(Snake* snake){

    int size = getSize(snake);

    printf("size:  %d\n",size);
    int ** temp = (int**)malloc(sizeof(int*)*size);
    for(int i=0; i < size; i++){
        temp[i] = (int*)malloc(sizeof(int)*2);
    }
    Body* tmp = snakes[0]->head;

    int i = 0;
    while(tmp){
        temp[i][0] = tmp->x; 
        temp[i][1] = tmp->y;
        tmp = tmp->p_next;
        printf("i: %d | x: %d e y: %d\n",i,temp[i][0],temp[i][1]);
        i++;
    }
    return temp;
}


int updateBodies(Body* bdy, int haveAte){
    if(bdy->p_next){
        if(updateBodies(bdy->p_next, haveAte)){
            //reach the last body
            bdy->p_next->x = bdy->x;
            bdy->p_next->y = bdy->y;
            return 1;
        }
    }
    else if(haveAte){
        bdy->p_next = newBody(bdy->x, bdy->y);
    }
    return 1;
}

void updateSnakes(){
    //snakes[0]->
    //int** xyArray = getSnakeXY(snakes[0]);

    printf("atualizando snakes.\n");
    int i;
    for(i = 0; i < n_players; i++){
        if(snakes[i]->alive){
            printf("atulizando corpos da snake %d\n", i);
            updateBodies(snakes[i]->head, snakeHaveAte(i));
            printf("atulizando a cabeca da snake %d\n", i);
            updateHead(snakes[i]);
            if(snakeCollides(i)){
                printf("snake %d colidiu.\n", i);
                snakes[i]->alive = 0;
                snakes[i]->death = clock();
            }
        }
    }
    printf("snakes atualizadas.\n");
    //free(xyArray);
}

int snakeCollides(int i){
    printf("checando colisao da snake .\n");
    if(collideWithOwnBody(i) || collideWithWall(i)){
        return 1;
    }
    return 0;
}

int collideWithOwnBody(int i){
    printf("checando colisao da snake %d com seu proprio corpo.\n", i);
    Body* tmp = snakes[i]->head->p_next;
    while(tmp){
        if(tmp->x == snakes[i]->head->x &&
           tmp->y == snakes[i]->head->y ){
            return 1;
        }
        tmp = tmp->p_next;
    }
    return 0;
}

int collideWithWall(int i){
    printf("checando colisao da snake %d com as paredes.\n", i);
    if(snakes[i]->head->x <= 0 || snakes[i]->head->x >= WIDTH || 
       snakes[i]->head->y <= 0 || snakes[i]->head->y >= HEIGHT){
       return 1;
    }
    return 0;
}

int snakeHaveAte(int i){
    if(snakes[i]->head->x + snakes[i]->vx == fruits[i][0] &&
       snakes[i]->head->y + snakes[i]->vy == fruits[i][1] ){
        resetFruits(i, i+1);
        return 1;
    }
    return 0;
}

void updateHead(Snake* snk){
    snk->head->x += snk->vx;
    snk->head->y += snk->vy;
}

Body* newBody(float x, float y){
    Body* bdy = (Body*)malloc(sizeof (Body));
    bdy->x = x;
    bdy->y = y;
    bdy->p_next = NULL;
    return bdy;
}

void resetFruits(int begin, int end){
    int i;
    for(i = begin; i < end; i++){
        fruits[i][0] = randomCoordinates(0);
        fruits[i][1] = randomCoordinates(1);
        
    }
}

int randomCoordinates(int XorY){
    int coordinate = 0;
    if(XorY == 0){
        coordinate = 1 + (rand() % (WIDTH - 1));
    }
    else if(XorY == 1){
        coordinate = 1 + (rand() % (HEIGHT-1));
    }
    return coordinate;
}

int getSize(Snake* s){
    int size = 1;
    Body* tmp = snakes[0]->head;

    while(tmp){
        tmp = tmp->p_next;
        size++;
    }
    return size;
}
void getMovement(){
    //the real version communicate with Tiva C for snakes commands
    //tiva-c

    printf("GETTING MOVEMENT\n");
    float vision_input[21]; 
    Snake* s = snakes[0];
    int snakeSize = s->score+4;
    int x = (int)s->head->x;
    int y = (int)s->head->y;

    printf("GETTING DIRECTION\n");
    int direction = getSnakeDir(s->vx,s->vy);
    int fruits_pos[2] = {(int)fruits[0][0],(int)fruits[0][1]};
    printf("BUILDING MAP\n");
    int** snk_array = getSnakeXY(s);
    printf("EXIT MAP\n");


    char** map = buildMap(fruits_pos,snk_array,snakeSize);

    printf("GETTING VISION\n");

    float* vision_result = vision(map,x,y,direction,(float*)vision_input);

    for(int i = 0; i < 21; i++){
        printf("i: %d -- v: %f\n",i,vision_result[i]);
    }


    printf("PREDICT VISION\n");
    int result = predict( vision_result,nn);
    printf("CONVERTING RESULT\n");
    int key = convertResult(result,s->vx,s->vy);
    printf("Result predict: %d -- converting: %d -- direction: x: %f, y: %f\n",result,key,s->vx,s->vy);
    getMovementFromKeyboard(key);
    free(snk_array);
    free(map);

}
int convertResult(int r,int velx,int vely) {
    int temp = getVelNumber(velx,vely);
    switch (r) {
      case 1: //FRENTE
        return temp;
        break;
      case 0: //ESQUERDA
        temp -= 1;
        if (temp == -1) temp = 3;
        return temp;
        break;
      case 2: //DIREITA
        temp += 1;
        temp %= 4;
        return temp;
        break;
    }
}
int getVelNumber(int velx,int vely) {
    int temp[2] = {velx,vely};
    if (temp[0] == -1 && temp[1] == 0) {
      return 1;
    } else if (temp[0] == 0 && temp[1] == 1) {
      return 0;
    } else if (temp[0] == 1 && temp[1] == 0) {
      return 3;
    } else if (temp[0] == 0 && temp[1] == -1) {
      return 2;
    }
}  
void getMovementFromKeyboard(int m){
    
//	if(kbhit()) {
        printf("pressionamento de tecla detectado.\n");
        //char m = getch();
        switch (m)
        {
            case 1:
                snakes[0]->vx = -1;
                snakes[0]->vy = 0;
                break;
            
            case 2:
                snakes[0]->vx = 0;
                snakes[0]->vy = -1;
                break;
            
            case 0:
                snakes[0]->vx = 0;
                snakes[0]->vy = 1;
                break;
            
            case 3:
                snakes[0]->vx = 1;
                snakes[0]->vy = 0;
                break;
            
            default:
                break;
        }
  //  }
}

int allSnakesHaveDied(){
    int i = 0;
    for(i = 0; i < n_players; i++){
        if(snakes[i]->alive){
            return 0;
        }
    }
    return 1;
}

char** buildMap(int fruit_position[2], int** snake, int snakeSize) {
  char** m = (char**)malloc(sizeof(char*)*MAP_SIZE);
  for (int i = 0; i < MAP_SIZE; i++) {
    m[i] = (char*)calloc(sizeof(char), MAP_SIZE);
  }
  m[fruit_position[0]][fruit_position[1]] = 'f';

  //head position
  m[snake[0][0]][snake[0][1]] = 'h';
  //body position
  for (int i = 1; i < snakeSize; i++) {
    m[snake[i][0]][snake[i][1]] = 'b';
  }
  return m;
  }

int directions[VISION_DIRECTIONS+1][2] = {{0, 1}, { -1, 1}, { -1, 0}, { -1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}};

float* vision(char** mapa, int headx, int heady, int snake_direction,float* vision_input) {
  int pos_init = (snake_direction + 5)%8;
  int temp_pos = pos_init;
  char FOOD = 'f';
  char BODY = 'b';
  //{fruta, corpo, parede}*vision_directions
  //Para cada direção da visao:
  for (int i = 0; i < VISION_DIRECTIONS; i++) {
    int x = headx + directions[temp_pos][0];
    int y = heady + directions[temp_pos][1];
    
    vision_input[i * 3] = -1;
    vision_input[i * 3+1] = -1;
    vision_input[i * 3+2] = -1;

    //para profundidade
    bool found = false;
    for (float k = 2; k <= VISION_LENGTH + 1 && !found; k++) {
   
      if ((x >= 0 && x < MAP_SIZE) && (y >= 0 && y < MAP_SIZE)) {
        if (mapa[x][y] == FOOD) {
          vision_input[i * 3] = 3.0 / k;
          found = true;
        }
        else if (mapa[x][y] == BODY) {
          
          vision_input[i * 3 + 1] = (2.0 / (k));
          found = true;
        }
      }
      else {
        vision_input[i * 3 + 2] =(2.0 / (k));
        /*
        Serial.print("WALL DETEC ");
        Serial.println(vision_input[i*3+2]);
        Serial.println(k);
        */
        // wall
        found = true;
      }

      //positio ++
      x += directions[temp_pos][0];
      y += directions[temp_pos][1];
    }    temp_pos = (temp_pos+1)%8; 
  }

  return vision_input;
}

int getSnakeDir(int x, int y){
    int dir[2] = {x,y};
    if(dir[0] == 0 && dir[1] == 1){
      return 0;
    }
    else if(dir[0] == -1 && dir[1] == 0){
      return 2;
    }
    else if(dir[0] == 0 && dir[1] == -1){
      return 4;
    }
    else if(dir[0] == 1 && dir[1] == 0){
      return 6;
    }
}

