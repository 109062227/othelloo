#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Point {
    int x, y;
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
   /* srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = next_valid_spots[index];*/

    //debug
    for(int i=0;i<n_valid_spots;i++)
    {

    }

    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
int setheuristic(int x,int y)//return heuristic
{
    //original in 60 steps
    if((x==0&&y==0) || (x==0&&y==7)||(x==7&&y==0)||(x==7&&y==7))//4 corner
        return 200;
    else if(x>=2 && x <= 5 && y >= 2 && y <=5)//4*4
        return 190;
    else if(x == 0  || x== 7 || y == 0 || y == 7)//ÃäÃä
        return 180;
    else if((x!=1&&y!=1) || (x!=6&&y!=1)||(x!=1&&y!=6)||(x!=6&&y!=6))
    {
        //dis_count+70
    }
    else
    {
        //dis_count
    }


}
