#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#define MAX(a, b) (a>b) ? a : b
#define MIN(a, b) (a<b) ? a : b

//debug
//std::ifstream fin("a.txt");
//std::ofstream fout("b.txt");
const int SIZE = 8;

int board[8][8];
enum SPOT_STATE
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

struct Point
{
    int x, y;
    int currplayer = 0;
    //std::array<std::array<int, 8>, 8> ppboard;
    int ppboard[8][8] = {0};
    int discount[3] = {0};
    int my_valid_spots;
    /*discount[EMPTY] = 8*8-4;
    discount[BLACK] = 2;
    discount[WHITE] = 2;*/
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y)
    {
    }
    bool operator==(const Point& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const
    {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const
    {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const
    {
        return Point(x - rhs.x, y - rhs.y);
    }
};
////

int player;


std::vector<Point> next_valid_spots;

//debug
//std::array<std::array<int, SIZE>, SIZE> tmp_board;
int setheuristic(Point child);
int minimax(Point &child,int depth,int pplayer);
bool is_spot_validd(Point center);
void setboard(Point &a,Point &b);
int cnt_corner(Point p);
int three_dead_pt(Point p);
int num_of_valid_spots(int cur_steps,int next_steps);
int in_middle(Point p);
int boundary(Point p);
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
int cur_player;
bool done;
int winner;

int get_next_player(int player)  //take off const...
{
    return 3 - player;//3-2,3-1
}
bool is_spot_on_board(Point p)  //in the boundary
{
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
int get_disc(Point &p)
{
    return p.ppboard[p.x][p.y];
}
void set_disc(Point &center,Point s, int disc)
{
    center.ppboard[s.x][s.y] = disc;

}
bool is_disc_at(Point &p, int disc)   //on board and is the same disc
{
    if (!is_spot_on_board(p))
        return false;
    if (get_disc(p) != disc)
    {
        return false;
    }
    //fout << "some" <<std::endl;
    return true;
}

void flip_discs(Point &center)
{
    for (Point dir: directions)
    {
        // Move along the direction while testing.
        Point p = center + dir;
        setboard(p,center);
        //p.currplayer = center.ppboard[p.x][p.y];
        if (!is_disc_at(p, get_next_player(p.currplayer)))
            continue;

        std::vector<Point> discs({p});//p elements???
        p = p + dir;
        setboard(p,center);

        //p.currplayer = center.ppboard[p.x][p.y];
        while (is_spot_on_board(p) && get_disc(p) != EMPTY)  //if is b/w and on the board
        {
            if (is_disc_at(p, p.currplayer))  //change to the same color
            {

                for (Point s: discs)
                {
                    set_disc(center,s, center.currplayer);
                    setboard(s,center);
                }
                //p.discount[p.currplayer] += discs.size();//my disc grow
                //p.discount[get_next_player(p.currplayer)] -= discs.size();//opposite's disc minus
                break;
            }
            discs.push_back(p);
            p = p + dir;
            setboard(p,center);
            //p.currplayer = p.ppboard[p.x][p.y];
        }
    }
}

std::vector<Point> get_valid_spots(Point &P)
{
    std::vector<Point> valid_spots;

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            Point p = Point(i, j);
            setboard(p,P);
            // p.currplayer = P.currplayer;
            //fout << "p.cur :" << p.currplayer << std::endl;
            if (p.ppboard[i][j] != EMPTY)//require empty spot
            {
                continue;
            }

            if (is_spot_validd(p))
            {
                //fout << "get valid" << std::endl;
                valid_spots.push_back(p);
            }
            //else
            //  fout << "no get " << std::endl;

        }
    }
    return valid_spots;
}
/////////
void read_board(std::ifstream& fin)
{
    fin >> player;
    cur_player = player;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fin >> board[i][j];
            // tmp_board = board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin)
{
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++)
    {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout)
{
    int n_valid_spots = next_valid_spots.size();
    //debug

    Point p;
    int ret_x,ret_y;
    //use minimax
    int cnt = 0;
    //fout << "hello" << std::endl;
    while(cnt < 4)
    {

        //fout << "cnt: " << cnt << std::endl;
        int heuristic = 0;
        for(int i=0; i<n_valid_spots; i++)
        {
            p = next_valid_spots[i];
            p.currplayer = cur_player;
            p.my_valid_spots = n_valid_spots;
            for(int i=0; i<8; i++)
            {
                for(int j=0; j<8; j++)
                {
                    p.ppboard[i][j] = board[i][j];
                    p.discount[p.ppboard[i][j]]++;
                }
            }
            set_disc(p,p,p.currplayer);//remember!!!
            flip_discs(p);
            /*std::vector<Point> v = get_valid_spots(p);
            if(!v.empty())
                p.oppo_valid_spots = v.size();
            else p.oppo_valid_spots = 0;*/


            int tmp = minimax(p,cnt,p.currplayer);

            //fout << "tmp: " << tmp <<std::endl;
            if(tmp > heuristic)
            {
                heuristic = tmp;
                ret_x = p.x;
                ret_y = p.y;
            }
            if(heuristic == 0)
            {
                ret_x = p.x;
                ret_y = p.y;
            }
            fout << ret_x << " " << ret_y << std::endl;
            //fout << "tmp: " << heuristic << std::endl;
        }
        cnt++;

    }
    //fout << "i: " <<i << std::endl;

    // Remember to flush the output to ensure the last action is written to file.
    // fout << ret_x << " " << ret_y << std::endl;
    fout.flush();
}

/*int main()
{
    //std::ifstream fin("a.txt");
    //std::ofstream fout("b.txt");
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}*/
int main(int, char** argv)
{
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

int setheuristic(Point child)//return heuristic
{
    int corner = cnt_corner(child);
    int dead_pt = three_dead_pt(child);
    std::vector<Point> v = get_valid_spots(child);
    int spots = num_of_valid_spots(child.my_valid_spots,v.size());
    int mid = in_middle(child);
    int bound =  boundary(child);
    return (corner+dead_pt+spots+mid+bound);
    //return spots;
}
int minimax(Point &child,int depth,int pplayer)//write_spot_valid要記得呼叫
{

    //fout << "Point: " << child.x << " " << child.y << "depth: " <<depth<<" player: " << pplayer << std::endl;
    //debug
    /*for(int i=0; i<8; i++)
    {
        for(int j=0; j<8; j++)
            fout << child.ppboard[i][j];
        fout << std::endl;
    }
    fout << std::endl;*/

    int value;
    //fout << "child: " <<pplayer << std::endl;
    if(depth == 0)
    {
        int h = setheuristic(child);
        //fout <<"h: " <<h <<std::endl;
        //fout << "Point: " << child.x << " " << child.y << "depth: " <<depth<<" player: " << pplayer << std::endl;
        return h;
    }
    std::vector<Point> v = get_valid_spots(child);


    for(int i=0; i<v.size(); i++)
    {
        Point p = v[i];
        p.my_valid_spots = v.size();
        setboard(p,child);
        p.currplayer = get_next_player(pplayer);
        set_disc(p,p,p.currplayer);
        flip_discs(p);
        // fout << "x: " << p.x << "  "<< "y: " << p.y << std::endl;

        if(pplayer == cur_player)//maxplayer(my turn)
        {
            value = -10000000;
            //fout << "p.x: " << p.x << "p.y: " << p.y <<std::endl;
            value = MAX(value,minimax(p,depth-1,p.currplayer));
            //fout << "value_black: " << value <<std::endl;
        }
        else
        {
            value = 10000000;
            value = MIN(value,minimax(p,depth-1,p.currplayer));
            //fout << "value_white: " << value <<std::endl;
        }
    }
    //fout << "p.x: " << child.x << "p.y: " << child.y <<std::endl;
    //fout << "value: " << value <<std::endl;
    return value;


}
bool is_spot_validd(Point center)   //check if the tail has myself
{
    if (get_disc(center) != EMPTY)
    {
        return false;
    }

    for (Point dir: directions)
    {

        // Move along the direction while testing.
        Point p = center + dir;

        setboard(p,center);
        p.currplayer = 3-center.currplayer;

        if (!is_disc_at(p, get_next_player(p.currplayer)))
            continue;


        p = p + dir;
        setboard(p,center);
        p.currplayer = 3-center.currplayer;

        while (is_spot_on_board(p) && get_disc(p) != EMPTY)  //都是empty
        {
            if (is_disc_at(p, p.currplayer))
            {
                return true;
            }

            p = p + dir;
            setboard(p,center);
            p.currplayer = 3-center.currplayer;
        }
    }
    return false;
}
void setboard(Point &a,Point &b)
{
    for(int i=0; i<8; i++)
    {
        for(int j=0; j<8; j++)
        {
            a.ppboard[i][j] = b.ppboard[i][j];
            a.discount[a.ppboard[i][j]]++;
            /*std::vector<Point> v  = get_valid_spots(b);
            if(!v.empty())
                a.oppo_valid_spots = v.size();
            else a.oppo_valid_spots = 0;*/
        }
    }
    a.currplayer = b.currplayer;
}
int cnt_corner(Point p)
{
    int pl=0,op=0;
    if(p.ppboard[0][0] == cur_player) pl++;
    else if(p.ppboard[0][0] == (3-cur_player)) op++;
    if(p.ppboard[0][7] == cur_player) pl++;
    else if(p.ppboard[0][7] == (3-cur_player)) op++;
    if(p.ppboard[7][0] == cur_player) pl++;
    else if(p.ppboard[7][0] == (3-cur_player)) op++;
    if(p.ppboard[7][7] == cur_player) pl++;
    else if(p.ppboard[7][7] == (3-cur_player)) op++;
    return 20000*(pl-op);//有角很棒，要乘多一點

}
int three_dead_pt(Point p)//頂點是空，但是三角有我，不利
{
    int me=0,op=0;
    if(p.ppboard[0][0] == 0)
    {
        if(p.ppboard[0][1] == cur_player) me++;
        else if(p.ppboard[0][1]  == (3-cur_player)) op++;
        if(p.ppboard[1][0] == cur_player) me++;
        else if(p.ppboard[1][0]  == (3-cur_player)) op++;
        if(p.ppboard[1][1] == cur_player) me++;
        else if(p.ppboard[1][1]  == (3-cur_player)) op++;
    }
    if(p.ppboard[0][7] == 0)
    {
        if(p.ppboard[0][6] == cur_player) me++;
        else if(p.ppboard[0][6]  == (3-cur_player)) op++;
        if(p.ppboard[1][6] == cur_player) me++;
        else if(p.ppboard[1][6]  == (3-cur_player)) op++;
        if(p.ppboard[1][7] == cur_player) me++;
        else if(p.ppboard[1][7]  == (3-cur_player)) op++;
    }
    if(p.ppboard[7][0] == 0)
    {
        if(p.ppboard[6][1] == cur_player) me++;
        else if(p.ppboard[6][1]  == (3-cur_player)) op++;
        if(p.ppboard[6][0] == cur_player) me++;
        else if(p.ppboard[6][0]  == (3-cur_player)) op++;
        if(p.ppboard[7][1] == cur_player) me++;
        else if(p.ppboard[7][1]  == (3-cur_player)) op++;
    }
    if(p.ppboard[7][7] == 0)
    {
        if(p.ppboard[7][6] == cur_player) me++;
        else if(p.ppboard[7][6]  == (3-cur_player)) op++;
        if(p.ppboard[6][6] == cur_player) me++;
        else if(p.ppboard[6][6]  == (3-cur_player)) op++;
        if(p.ppboard[6][7] == cur_player) me++;
        else if(p.ppboard[6][7]  == (3-cur_player)) op++;
    }
    return (-5000)*(me-op);
}
int num_of_valid_spots(int cur_steps,int next_steps)//敵人越少選擇越好
{
    /*std::vector<Point> v = get_valid_spots(p);

    int ret = v.size();
    if(p.currplayer == cur_player)
        return (-0.1)*ret;//(-2000)*(ret);
    else if(p.currplayer == (3-cur_player))
        return (0.1)*ret;//(2000)*(ret);
    else
        return 0;*/

    return cur_steps-next_steps;
}
int in_middle(Point p)//點比較多，數字不要乘太大
{
    int me=0,op=0;
    for(int i=2; i<=5; i++)
    {
        for(int j=2; j<=5; j++)
        {
            if(p.ppboard[i][j] == cur_player)
                me++;
            else if(p.ppboard[i][j] == (3-cur_player))
                op++;
        }
    }
    return 500*(me-op);
}
int boundary(Point p)
{
    int me = 0,op = 0;
    /*for(int i=0; i<8; i++)
    {
        if(((p.ppboard[0][7] == cur_player)||(p.ppboard[0][0] == cur_player)))
            me++;
        if(((p.ppboard[0][7] == (3-cur_player))||(p.ppboard[0][0] == (3-cur_player))))
            op++;
        if(((p.ppboard[7][0] == cur_player)||(p.ppboard[7][7] == cur_player)))
            me++;
        if(((p.ppboard[7][0] == (3-cur_player))||(p.ppboard[7][7] == (3-cur_player))))
            op++;
    }*/
    for(int i=1;i<=6;i++)
    {
        if(p.ppboard[i][0] == cur_player) me++;
        else if(p.ppboard[i][0] == (3-cur_player)) op++;
        if(p.ppboard[i][7] == cur_player) me++;
        else if(p.ppboard[i][7] == (3-cur_player)) op++;
    }
     for(int j=1;j<=6;j++)
    {
        if(p.ppboard[0][j] == cur_player) me++;
        else if(p.ppboard[0][j] == (3-cur_player)) op++;
        if(p.ppboard[7][j] == cur_player) me++;
        else if(p.ppboard[7][j] == (3-cur_player)) op++;
    }
    return 500*(me-op);
}

