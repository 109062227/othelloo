    #include <iostream>
    #include <fstream>
    #include <array>
    #include <vector>
    #include <cstdlib>
    #include <ctime>

    /*struct Point {
        int x, y;
    };*/
    //debug
   // std::ifstream fin("a.txt");
    //std::ofstream fout("b.txt");
    const int SIZE = 8;
    //std::array<std::array<int, SIZE>, SIZE> board;
    int board[8][8];
    enum SPOT_STATE {
            EMPTY = 0,
            BLACK = 1,
            WHITE = 2
        };

    struct Point {
        int x, y;
        int currplayer = 0;
        //std::array<std::array<int, 8>, 8> ppboard;
        int ppboard[8][8] = {0};
        int discount[3] = {0};
        /*discount[EMPTY] = 8*8-4;
        discount[BLACK] = 2;
        discount[WHITE] = 2;*/
        Point() : Point(0, 0) {}
        Point(int x, int y) : x(x), y(y) {
        }
        bool operator==(const Point& rhs) const {
            return x == rhs.x && y == rhs.y;
        }
        bool operator!=(const Point& rhs) const {
            return !operator==(rhs);
        }
        Point operator+(const Point& rhs) const {
            return Point(x + rhs.x, y + rhs.y);
        }
        Point operator-(const Point& rhs) const {
            return Point(x - rhs.x, y - rhs.y);
        }
    };
    ////

    int player;


    std::vector<Point> next_valid_spots;

    //debug
    //std::array<std::array<int, SIZE>, SIZE> tmp_board;
    int setheuristic(Point p,int x,int y);
    int minimax(Point child,int depth,int player);
    bool is_spot_validd(Point center);
    void setboard(Point &a,Point &b);
    /////


        //static const int SIZE = 8;
        const std::array<Point, 8> directions{{//board
            Point(-1, -1), Point(-1, 0), Point(-1, 1),
            Point(0, -1), /*{0, 0}, */Point(0, 1),
            Point(1, -1), Point(1, 0), Point(1, 1)
        }};
        //std::array<std::array<int, SIZE>, SIZE> board;//二維陣列
       // std::vector<Point> next_valid_spots;
        std::array<int, 3> disc_count;
        int cur_player = 1;
        bool done;
        int winner;

        int get_next_player(int player) {//take off const...
            return 3 - player;//3-2,3-1
        }
        bool is_spot_on_board(Point p) {//in the boundary
            return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
        }
        int get_disc(Point p)  {
            //if(  p.ppboard[p.x][p.y] == 1 || p.ppboard[p.x][p.y] == 2)
                //fout << "board:" << p.ppboard[p.x][p.y] << std::endl;

            return p.ppboard[p.x][p.y];
        }
        void set_disc(Point p, int disc) {
            p.ppboard[p.x][p.y] = disc;

        }
        bool is_disc_at(Point p, int disc)  {//on board and is the same disc
            if (!is_spot_on_board(p))
                return false;
            if (get_disc(p) != disc)
            {
               // fout << "get" << get_disc(p) << std::endl;
                //fout << "disc: " << disc <<std::endl;
                //fout << "not at" <<std::endl;
                return false;
            }
            //fout << "some" <<std::endl;
            return true;
        }
        int is_spot_valid(Point center)  {//check if the tail has myself
            int cnt = 0;
            int ret = 0;
            if (get_disc(center) != EMPTY)
                return cnt;
            for (Point dir: directions) {
                // Move along the direction while testing.
                Point p = center + dir;
                if (!is_disc_at(p, get_next_player(p.currplayer)))
                    continue;
                p = p + dir;
                cnt = 2;
                while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                    if (is_disc_at(p, p.currplayer))
                    {
                        cnt++;
                    }
                    p = p + dir;
                    cnt++;
                }
                if(cnt > ret) ret = cnt;
                cnt = 0;
            }
            return ret;
        }
        void flip_discs(Point center) {
            for (Point dir: directions) {
                // Move along the direction while testing.
                Point p = center + dir;
                setboard(p,center);
                p.currplayer = center.ppboard[p.x][p.y];
                if (!is_disc_at(p, get_next_player(p.currplayer)))
                    continue;

                std::vector<Point> discs({p});//p elements???
                p = p + dir;
                setboard(p,center);
                p.currplayer = center.ppboard[p.x][p.y];
                while (is_spot_on_board(p) && get_disc(p) != EMPTY) {//if is b/w and on the board
                    if (is_disc_at(p, p.currplayer)) {//change to the same color
                        for (Point s: discs) {
                            set_disc(s, p.currplayer);
                        }
                        p.discount[p.currplayer] += discs.size();//my disc grow
                        p.discount[get_next_player(p.currplayer)] -= discs.size();//opposite's disc minus
                        break;
                    }
                    discs.push_back(p);
                    p = p + dir;
                    p.currplayer = p.ppboard[p.x][p.y];
                }
            }
        }

        std::vector<Point> get_valid_spots(Point P) {
        std::vector<Point> valid_spots;

        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                setboard(p,P);
               // p.currplayer = P.currplayer;
                //fout << "p.cur :" << p.currplayer << std::endl;
                if (P.ppboard[i][j] != EMPTY)//require empty spot
                {
                    continue;
                }

                if (is_spot_validd(p))
                {
                    //fout << "get valid" << std::endl;
                     valid_spots.push_back(p);
                }
               // else
                    //fout << "no get " << std::endl;

            }
        }
        return valid_spots;
    }
    /////////
    void read_board(std::ifstream& fin) {
        fin >> player;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                fin >> board[i][j];
               // tmp_board = board[i][j];
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
        int heuristic = 0;
        int ret_x,ret_y;
        Point p;

        //use setheuristic

        /*for(int i=0;i<n_valid_spots;i++)
        {
            p = next_valid_spots[i];
            //flip_discs(p);

            int cnt = is_spot_valid(p);
            disc_count[cur_player] += cnt;//my disc grow
            disc_count[get_next_player(cur_player)] -= cnt;//opposite's disc minus

            int tmp = setheuristic(p.x,p.y);
            if(tmp > heuristic)
            {
                heuristic = tmp;
                ret_x = p.x;
                ret_y = p.y;
            }
            disc_count[cur_player] -= cnt;//my disc grow
            disc_count[get_next_player(cur_player)] += cnt;//opposite's disc minus

        }
        if(heuristic == 0)
        {
            ret_x = p.x;
            ret_y = p.y;
        }*/

        //use minimax
        for(int i=0;i<n_valid_spots;i++)
        {
            p = next_valid_spots[i];
            p.currplayer = cur_player;
            //fout << "cur: "  << cur_player <<std::endl;
            for(int i=0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                {

                    p.ppboard[i][j] = board[i][j];
                    p.discount[p.ppboard[i][j]]++;
                }
            }

            int tmp = minimax(p,3,cur_player);

            if(tmp > heuristic)
            {
                heuristic = tmp;
                ret_x = p.x;
                ret_y = p.y;
            }


        }
        if(heuristic == 0)
        {
            ret_x = p.x;
            ret_y = p.y;
        }




        // Remember to flush the output to ensure the last action is written to file.
        fout << ret_x << " " << ret_y << std::endl;
        fout.flush();


    }

   /* int main() {
        //std::ifstream fin("a.txt");
        //std::ofstream fout("b.txt");
        read_board(fin);
        read_valid_spots(fin);
        write_valid_spot(fout);
        fin.close();
        fout.close();
        return 0;
    }*/
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

    int setheuristic(Point p,int x,int y)//return heuristic
    {
        int h;
        //original in 60 steps
        if((x==0&&y==0) || (x==0&&y==7)||(x==7&&y==0)||(x==7&&y==7))//4 corner
            h = 300;
        else if(x>=2 && x <= 5 && y >= 2 && y <=5)//4*4
            h = 290;
        else if(x == 0  || x== 7 || y == 0 || y == 7)//boundary
            h = 280;
        else //((x!=1&&y!=1) && (x!=6&&y!=1)&&(x!=1&&y!=6)&&(x!=6&&y!=6))
        {
            if((x==1&&y==1) || (x==6&&y==1)||(x==1&&y==6)||(x==6&&y==6))
                h = p.discount[1]-p.discount[2];
            else if((x==1&&y==0) || (x==0&&y==1)||(x==0&&y==6)||(x==1&&y==7)||(x==6&&y==0) || (x==7&&y==1)||(x==7&&y==6)||(x==6&&y==7))
                h = p.discount[1]-p.discount[2]+60;
            else{
                h = p.discount[1]-p.discount[2]+200;
            }
        }
        return h;
    }
    int minimax(Point child,int depth,int player)//write_spot_valid要記得呼叫
    {
        //記得下去減回來
        flip_discs(child);

        int value;
        //fout << "child: " <<child.currplayer << std::endl;
        std::vector<Point> v = get_valid_spots(child);

        //if(v.empty()) fout << "empty!" <<std::endl;
        //fout << "valid spot:" << v[0].x << v[0].y   <<std::endl;
        for(int i=0;i<v.size();i++)
        {
            Point p = v[i];
            p.currplayer = (player == 1?2:1);

            for(int i=0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                {
                    p.ppboard[i][j] = child.ppboard[i][j];
                    p.discount[p.ppboard[i][j]]++;
                }

            }
            //fout << "depth:" << depth <<std::endl;
            if(depth == 0 || i == v.size()-1)
            {
                return setheuristic(p,p.x,p.y);
            }
            if(player == 1)//maxplayer(my turn)
            {
                value = -10000000;
                value = std::max(value,minimax(p,depth-1,p.currplayer));

            }
            else
            {

                value = 10000000;
                value = std::min(value,minimax(p,depth-1,p.currplayer));

            }
        }

        return value;


    }
    bool is_spot_validd(Point center)  {//check if the tail has myself
        if (get_disc(center) != EMPTY)
        {
            //fout <<"first"<<std::endl;
            return false;
        }

        for (Point dir: directions) {

            // Move along the direction while testing.
            Point p = center + dir;

            setboard(p,center);
            p.currplayer = center.ppboard[p.x][p.y];


            if (!is_disc_at(p, get_next_player(p.currplayer)))
                continue;

            //fout << "heheheh" <<std::endl;
            p = p + dir;
            setboard(p,center);
            p.currplayer = p.ppboard[p.x][p.y];

           // if(get_disc(p) != EMPTY) fout << " get not_empty" <<std::endl;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {//都是empty
                //fout << "cur_player: " << p.currplayer << std::endl;
                if (is_disc_at(p, p.currplayer))
                {
                    //fout << "second" << std::endl;
                    return true;
                }

                p = p + dir;
                setboard(p,center);
                p.currplayer = p.ppboard[p.x][p.y];
            }
        }
        //fout << "third" <<std::endl;
        return false;
    }
    void setboard(Point &a,Point &b)
    {
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                a.ppboard[i][j] = b.ppboard[i][j];
                a.discount[a.ppboard[i][j]]++;
            }
        }
        a.currplayer = b.currplayer;
    }

