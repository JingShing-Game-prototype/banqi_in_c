#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define board_size_x 4
#define board_size_y 8

int board[board_size_x][board_size_y] = {0};
char* boardname[33]={"　","卒","卒","卒","卒","卒","包","包","馬","馬","車","車","象","象","士","士","將","\033[31m兵\033[m","\033[31m兵\033[m","\033[31m兵\033[m","\033[31m兵\033[m","\033[31m兵\033[m","\033[31m炮\033[m","\033[31m炮\033[m","\033[31m傌\033[m","\033[31m傌\033[m","\033[31m俥\033[m","\033[31m俥\033[m","\033[31m相\033[m","\033[31m相\033[m","\033[31m仕\033[m","\033[31m仕\033[m","\033[31m帥\033[m"};
int boardnum[32]={0};

char* boardrankname[9]={"　","\033[31m兵\033[m卒","　","\033[31m炮\033[m包","\033[31m傌\033[m馬","\033[31m俥\033[m車","\033[31m相\033[m象","\033[31m仕\033[m士","\033[31m帥\033[m將"};
int boardrank[9] = {0, 1, 5, 3, 4, 5, 6, 7, 8};//slot 2 for canon extra power
int gamerule[1] = {-1};//0 for normal, 1 for filped combo, 2 for dark combo.
int last_piece[1]= {0};
int last_position[2]={0};// 0 for x,1 for y
int combo[1]={0};
int mode2_has_flipped[1]={0};

int movingX[4] = {-1, 1, 0, 0};
int movingY[4] = {0, 0, -1, 1};
int save_info[3]={0};//0 is turn, 1 is game_mode, 2 is p1_color

void shuffle(int a[], int N);
void gameInit(int x);
void printBoard();
int actions(int x,int y,int color,int turn);

int rank(int n);
int colordefine(int n);
void winningdefine(int n,int turn);
void movement(int next_x,int next_y,int x, int y,int next_piece,int turn);
int basicmovementdefine(int next_x,int next_y,int x,int y,int next_piece);
int darkc_combo_movementdefine(int next_x,int next_y,int x,int y,int next_piece);

void editpiece();
void gamemodedefine();
int is_outwall_a_piece(int x, int y);
int rank_versus(int a, int b,int addition_a,int addition_b);

int read_game();
void save_game();

int main() {
    int n,i,x,y,turn=1,p1color=2,p2color=2;//0=black,1=red
    int open=1,edit=0,confirm;
    char* color[3]={"Black","\033[31mred\033[m","none"};
    srand((unsigned int)time(NULL));
    system("chcp 65001 & cls");
    if(read_game()==0){
        printf("Opened or Closed? (0 for closed, 1 for open) ");
        scanf("%d",&open);
        gameInit(open);
        printf("Edit piece power? (0 for no, 1 for yes) ");
        scanf("%d",&edit);
        while(gamerule[0]>2||gamerule[0]<0){
        printf("What mode you want to play? (0 for normal banqi,1 for flipped combo, 2 for dark combo) ");
        scanf("%d",&gamerule[0]);
        }
        if(edit)editpiece();
    }
    else{
        turn=save_info[0];
        gamerule[0]=save_info[1];
        p1color=save_info[2];
        p2color = 1-p1color;
    }
    printBoard();
    while(gamerule[0]>-1){
        if(turn>-1){
            printf("P1 is %s, P2 is %s\n",color[p1color],color[p2color]);
        }
        printf("Now it's turn for Player %d.\n",turn%2?1:2);
        printf("Turn: %d\n",turn);
        printf("Choose a cell (-1,-1 for give up): ");
        for(i=0;i<2;i++){
            if(combo[0]>0){
                x=last_position[0];
                y=last_position[1];
                break;
            }
            scanf("%d",&n);
            if(i==0)x=n;
            else y=n;
        }
        if (x==-2&&y==-2){
            printf("Do you confirm? (0 for no, 1 for yes) ");
            scanf("%d",&confirm);
            if (confirm){
                save_info[0]=turn;
                save_info[1]=gamerule[0];
                save_info[2]=p1color;
                save_game();
            }
            continue;
        }
        if (x==-3&&y==-3){
            printf("Do you confirm? (0 for no, 1 for yes) ");
            scanf("%d",&confirm);
            if (confirm){
                if(read_game()){
                    turn=save_info[0];
                    gamerule[0]=save_info[1];
                    p1color=save_info[2];
                    p2color = 1-p1color;
                    system("cls");
                    printBoard();
                }
            }
            continue;
        }
        if(x==9&&y==9&&combo[0]>0){
            combo[0]=0;
            turn++;
            mode2_has_flipped[0]=0;
            continue;
        }
        else if(x==-1&&y==-1){
            printf("Do you confirm? (0 for no, 1 for yes) ");
            scanf("%d",&confirm);
            if(confirm){
            printf("\nPlayer %d gave up.\n\nPlayer %d won!\n",turn%2?1:2,turn%2?2:1);
            break;
            }
            else continue;
        }
        else if(x>3||y>7){
            printf("\nInput is out of board limit.\n\n");
            continue;
        }
        if(combo[0]>0){
            if(board[x][y]<1&&board[x][y]!=last_piece[0]||(x!=last_position[0]||y!=last_position[1])){
                printf("\nNot your last piece!\n\n");
                continue;
            }
        }
        if(board[x][y]>0)printf("You select the %s\n",boardname[board[x][y]]);
        else if(combo[0]<1) printf("You flip the piece.Which is %s\n",boardname[board[x][y]*-1]);
        if(turn==1){
                if (board[x][y] < 0) p1color=colordefine((board[x][y]*-1));
                else p1color=colordefine(board[x][y]);
                p2color=1-p1color;
        }
        if(turn%2==1)turn=actions(x,y,p1color,turn);
        else turn=actions(x,y,p2color,turn);
        printBoard();
    }
    printf("\nThank you!! Bye~~\n\n");
    system("pause");
    return 0;
}

void gameInit(int x) {
    int i,j,k,boardnum[32]={0};
    srand((unsigned int)time(NULL));
    system("chcp 65001 & cls");
    for(i=1;i<33;i++){
        boardnum[i-1]=i;
    }
    shuffle(boardnum,32);
    k=0;
    for(i=0;i<board_size_x;i++){
        for(j=0;j<board_size_y;j++){
            if (x%2==0)board[i][j]=boardnum[k]*-1;
            else board[i][j]=boardnum[k];
            k++;
        }
    }
}

void printBoard() {
    int i,j;
    for(i=0;i<board_size_x;i++){
            if (i==0)printf("x\\y 0  1  2  3  4  5  6  7\n");
            if(i==0)printf("  ┌──┬──┬──┬──┬──┬──┬──┬──┐\n");
            for(j=0;j<board_size_y;j++){
                if(j==0)printf("%d │",i);
                if (board[i][j]<0)printf("●");
                else {
                    printf("%s",boardname[board[i][j]]);}
                printf("│");
                if(j==7)printf("\n");
                }
            if(i<3)  printf("  ├──┼──┼──┼──┼──┼──┼──┼──┤\n");
            if(i==3)printf("  └──┴──┴──┴──┴──┴──┴──┴──┘\n");
    }
}

void shuffle(int a[], int N){
    int i, j, k, t;
    for(k=0;k<N;k++){
        i = rand() % N;
        j = rand() % N;
        t = a[i];
        a[i]=a[j];
        a[j]=t;
    }
}

int rank(int n){
    if(n<=0)return boardrank[0];
    if(n>16)n-=16;
    if(n<6&&n>0)return boardrank[1];
    else return boardrank[n/2];
}

int actions(int x,int y,int color,int turn){
    int direction=0,next_piece,next_x,next_y;//0=black,1=red color = (code-1)/16
    int wall, target, canon_x, canon_y, i,canon_move_mode=0;
    if (board[x][y]<0){board[x][y]*=-1;return ++turn;}
    else if (board[x][y]==0){
            return turn;
    }
    else if(color == colordefine(board[x][y])) {
        if(combo==0) printf("Move to which direction (0, 1, 2, 3 for up, down, left, and right): ");
        else if(combo>0) printf("Move to which direction (0, 1, 2, 3 for up, down, left, and right;-1 to cancel): ");
        scanf("%d",&direction);
        if(combo>0&&direction<0){
            combo[0]=0;
            mode2_has_flipped[0]=0;
            return ++turn;
        }
        if(direction<0)return turn;
        next_x=x+movingX[direction];next_y=y+movingY[direction];
        next_piece = board[x+movingX[direction]][y+movingY[direction]];
        if(rank(board[x][y])==3){//define canon
            wall=0;target=0;
            printf("Please enter the canon's move mode: (1 for move 1 grid, 2 for fly across) ");
            scanf("%d",&canon_move_mode);
            if(canon_move_mode==1){
                if(combo[0]<1){
                    if(basicmovementdefine(next_x,next_y,x,y,next_piece)){
                        if(next_piece==0){//walk mode
                            movement(next_x,next_y,x,y,next_piece,turn);
                            return ++turn;
                        }
                    }
                }
                return turn;
            }
            else if(canon_move_mode==2){//fly mode
                for(i=1;i<board_size_y;i++){//find wall
                    if(board[x+movingX[direction]*i][y+movingY[direction]*i]!=0){
                        wall=board[x+movingX[direction]*i][y+movingY[direction]*i];
                        break;
                    }
                }
                for(i=1;i<board_size_y;i++){//find target
                    if(board[x+movingX[direction]*i][y+movingY[direction]*i]!=0&&board[x+movingX[direction]*i][y+movingY[direction]*i]!=wall){
                        target=board[x+movingX[direction]*i][y+movingY[direction]*i];
                        canon_x=x+movingX[direction]*i;canon_y=y+movingY[direction]*i;
                        break;
                    }
                }
                next_piece=target;
                printf("(%d,%d)\n",canon_x,canon_y);
                if(basicmovementdefine(canon_x,canon_y,x,y,next_piece)){
                    if(wall!=0&&target!=0&&rank_versus(board[x][y],target,boardrank[2],0)){
                        movement(canon_x,canon_y,x,y,next_piece,turn);
                        if(gamerule[0]==2)mode2_has_flipped[0]=0;
                        if(gamerule[0]==0)return ++turn;
                        else if(gamerule[0]==1||gamerule[0]==2){
                            if(is_outwall_a_piece(canon_x,canon_y)){
                                last_piece[0]=board[canon_x][canon_y];
                                last_position[0]=canon_x;
                                last_position[1]=canon_y;
                                combo[0]++;
                                return turn;
                                }
                            else{
                                last_piece[0]=0;
                                printf("\ndon't have next piece\n");
                                combo[0]=0;
                                return turn++;
                                }
                            }
                        }
                    }
                    else if(gamerule[0]==2&&mode2_has_flipped[0]==0) {
                        if(next_piece<0){
                            board[canon_x][canon_y]*=-1;
                            printf("Flipped out the %s\n\n",boardname[board[canon_x][canon_y]]);
                        }
                        else if(next_piece>0&&colordefine(board[x][y])==colordefine(next_piece))return turn;
                        if(colordefine(board[x][y])!=colordefine(board[canon_x][canon_y])){
                            mode2_has_flipped[0]=1;
                            last_piece[0]==board[x][y];
                            last_position[0]=x;
                            last_position[1]=y;
                            combo[0]++;
                            return turn;
                            }
                        else{
                            combo[0]=0;
                            return ++turn;
                        }
                    }
                }
                return turn;
        }
        else{
            if(basicmovementdefine(next_x,next_y,x,y,next_piece)){
                if(rank_versus(board[x][y],next_piece,0,0)){
                    if(combo[0]!=0&&next_piece==0)return turn;
                    if(next_piece==0){
                    movement(next_x,next_y,x,y,next_piece,turn);
                    return ++turn;
                    }
                    movement(next_x,next_y,x,y,next_piece,turn);
                    if(gamerule[0]==2)mode2_has_flipped[0]=0;
                    if(gamerule[0]==0)return ++turn;
                    else if(gamerule[0]==1||gamerule[0]==2){
                        if(is_outwall_a_piece(next_x,next_y)){
                            last_piece[0]=board[next_x][next_y];
                            last_position[0]=next_x;
                            last_position[1]=next_y;
                            combo[0]++;
                            return turn;
                        }
                        else{
                            last_piece[0]=0;
                            printf("\ndon't have next piece\n");
                            combo[0]=0;
                            return ++turn;
                        }
                    }
                }
            }
            else if(gamerule[0]==2&&mode2_has_flipped[0]==0) {
                board[next_x][next_y]*=-1;
                printf("Flipped out the %s\n\n",boardname[board[next_x][next_y]]);
                if(rank_versus(board[x][y],board[next_x][next_y],0,0)&&colordefine(board[x][y])!=colordefine(board[next_x][next_y])){
                    last_piece[0]==board[x][y];
                    last_position[0]=x;
                    last_position[1]=y;
                    combo[0]++;
                    mode2_has_flipped[0]=1;
                    return turn;
                }
                else{
                        combo[0]=0;
                        return ++turn;
                }
            }
        }
    }
    return turn;
}

int colordefine(int n){
    if (n<=0)return -1;
    return (n-1)/16;
}

void winningdefine(int n,int turn){
    if (rank(n)==boardrank[8]){
        printBoard();
        printf("\nPlayer %d win\n",(turn-1)%2+1);
        printf("\nThank you!! Bye~~\n");
        system("pause");
        exit(0);
    }
}

void movement(int next_x,int next_y,int x, int y,int next_piece,int turn){
    if(board[next_x][next_y]<0)printf("Your %s ate unflipped %s\n",boardname[board[x][y]],boardname[board[next_x][next_y]*-1]);
    else if(board[next_x][next_y]>0) printf("Your %s ate %s\n",boardname[board[x][y]],boardname[board[next_x][next_y]]);
    board[next_x][next_y]=board[x][y];
    board[x][y]=0;
    if(rank(next_piece)==boardrank[8])winningdefine(next_piece,turn);
}

int basicmovementdefine(int next_x,int next_y,int x,int y,int next_piece){
    if(next_x>-1&&next_x<board_size_x&&next_y>-1&&next_y<board_size_y){
                    if(next_piece>-1&&(colordefine(next_piece)!=colordefine(board[x][y]))){
                            return 1;
                    }
                    else if(gamerule[0]!=2)printf("\ncolor limit\n");
    }
    if(gamerule[0]!=2) printf("Wrong move\n");
    return 0;
}
int darkc_combo_movementdefine(int next_x,int next_y,int x,int y,int next_piece){
    if(next_x>-1&&next_x<board_size_x&&next_y>-1&&next_y<board_size_y){
                    if(colordefine(next_piece)!=colordefine(board[x][y])){
                            return 1;
                    }
    }
    return 0;
}

void editpiece(){
    int see=1,i,edit=1,extra_canon=0,confirm;
    while(edit>0){
    printf("see now piece rank? (0 for no, 1 for yes) ");
    scanf("%d",&see);
    if(see){
            for(i=1;i<9;i++){
                if(i!=2)printf("%s now is rank %d\n",boardrankname[i],boardrank[i]);
            }
            printf("%s's extrapower now is %d\n\n",boardrankname[3],boardrank[2]);
        }
        printf("Do you need extra power for canon? (0 for no, 1 for yes. canon become a glass canon) ");
        scanf("%d",&extra_canon);
        if(extra_canon){
            printf("%s's extrapower now is ",boardrankname[3]);
            scanf("%d",&boardrank[2]);
        }
        printf("edit or not?(1 or 0) ");
        scanf("%d",&edit);
        if(edit){
            for(i=1;i<9;i++){
                if(i!=2){
                    printf("%s now is rank ",boardrankname[i]);
                    scanf("%d",&boardrank[i]);
                }
            }
        }
        printf("Confirm or not? (1 or 0) ");
        scanf("%d",&confirm);
        if(confirm)break;
        else continue;
    }
    system("cls");
}

void gamemodedefine(){
    printf("What's game mode you would want? (0 for normal, 1 for fliped combo, 2 for dark combo) ");
    scanf("%d",&gamerule[0]);
}

int is_outwall_a_piece(int x, int y){
    int i,j;
    int wall=0, target=0, canon_x=0, canon_y=0;
    if(gamerule[0]==1&&rank(board[x][y])!=3){
        for(i = 0;i<4;i++){
        if(board[x+movingX[i]][y+movingY[i]]>0&&colordefine(board[x][y])!=colordefine(board[x+movingX[i]][y+movingY[i]])&&rank_versus(board[x][y],board[x+movingX[i]][y+movingY[i]],0,0))return 1;
        }
    }
    else if(gamerule[0]==2&&rank(board[x][y])!=3){
         for(i = 0;i<4;i++){
         if(board[x+movingX[i]][y+movingY[i]]>0&&colordefine(board[x][y])!=colordefine(board[x+movingX[i]][y+movingY[i]])&&rank_versus(board[x][y],board[x+movingX[i]][y+movingY[i]],0,0)||board[x+movingX[i]][y+movingY[i]]<0)return 1;
        }
    }
    else if(gamerule[0]==1&&rank(board[x][y])==3){
        for(j=0;j<4;j++){// j for direction; 4 irection
            for(i=1;i<board_size_y;i++){//find wall
                if(board[x+movingX[movingX[j]*i]][y+movingY[movingY[j]]*i]!=0){
                    wall=board[x+movingX[movingX[j]]*i][y+movingY[movingY[j]]*i];
                    break;
                }
            }
            for(i=1;i<board_size_y;i++){//find target
                if(board[x+movingX[movingX[j]]*i][y+movingY[movingY[j]*i]]!=0&&board[x+movingX[movingX[j]]*i][y+movingY[movingY[j]]*i]!=wall){
                    target=board[x+movingX[movingX[j]]*i][y+movingY[movingY[j]]*i];
                    canon_x=x+movingX[movingX[j]]*i;canon_y=y+movingY[movingY[j]]*i;
                    break;
                }
            }
            if (target!=0&&colordefine(board[x][y])!=colordefine(target))return 1;
        }
    }
    else if(gamerule[0]==2&&rank(board[x][y])==3){
        for(j=0;j<4;j++){
            for(i=1;i<board_size_y;i++){//find wall
                if(board[x+movingX[movingX[j]*i]][y+movingY[movingY[j]]*i]!=0){
                    wall=board[x+movingX[movingX[j]]*i][y+movingY[movingY[j]]*i];
                    break;
                }
            }
            for(i=1;i<board_size_y;i++){//find target
                if(board[x+movingX[movingX[j]]*i][y+movingY[movingY[j]*i]]!=0&&board[x+movingX[movingX[j]]*i][y+movingY[movingY[j]]*i]!=wall){
                    target=board[x+movingX[movingX[j]]*i][y+movingY[movingY[j]]*i];
                    canon_x=x+movingX[movingX[j]]*i;canon_y=y+movingY[movingY[j]]*i;
                    break;
                }
            }
            if ((target!=0&&colordefine(board[x][y])!=colordefine(target))||target<0)return 1;
        }
    }
    return 0;
}

int rank_versus(int a, int b,int addition_a,int addition_b){
    a=rank(a);
    b=rank(b);
    if(a==boardrank[8]){
        if(b==boardrank[1])return 0;
        else if(a+addition_a>=b+addition_b)return 1;
    }
    else if(a+addition_a>=b+addition_b||(a==boardrank[1]&&b==boardrank[8]))return 1;
    return 0;
}

int read_game(){
    int read,i,j;
    FILE *gameFile;
    printf("Do you want to read the last game? (0 for no, 1 for yes) ");
    scanf("%d",&read);
    if(read){
        gameFile = fopen( "last_game_record.txt","r" );
        if( gameFile == NULL ){
        printf( "open failure\n\n" );
        fclose(gameFile);
        return 0;
        }
        else{
            fscanf(gameFile,"%d",&save_info[0]);// turn
            fscanf(gameFile,"\n");
            fscanf(gameFile,"%d",&save_info[1]);// game_mode
            fscanf(gameFile,"\n");
            fscanf(gameFile,"%d",&save_info[2]);// p1_color
            fscanf(gameFile,"\n");
            for(i=0;i<board_size_x;i++)
            {
                    for(j=0;j<board_size_y;j++)
                    {
                            fscanf(gameFile,"%d",&board[i][j]);
                            fscanf(gameFile,",");
                    }
                    fscanf(gameFile,"\n");
            }
            fclose(gameFile);
            return 1;
        }
    }
}

void save_game(){
    int read,i,j;
    FILE *gameFile;
    gameFile = fopen( "last_game_record.txt","w" );
    fprintf(gameFile, "%d\n",save_info[0]);//save_info[0] is turn
    fprintf(gameFile, "%d\n",save_info[1]);//save_info[0] is turn
    fprintf(gameFile, "%d\n",save_info[2]);//save_info[0] is turn
    for(i=0;i<board_size_x;i++){
            for(int j=0;j<board_size_y;j++){
                fprintf(gameFile, "%d,",board[i][j]);
            }
    fprintf(gameFile,"\n");
    }
    printf("\nHas been saving...\n");
    fclose(gameFile);
    printf("Saving is done!\n\n");
}
