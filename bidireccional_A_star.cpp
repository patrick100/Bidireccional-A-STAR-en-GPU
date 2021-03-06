#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime> // time_t
#include <chrono>  // for high_resolution_clock
#include <iomanip>


using namespace std;
using namespace std::chrono;

const int tam_map=100;


high_resolution_clock::time_point tstart;
high_resolution_clock::time_point tfinish;


class point {
public:
    int x, y;

    point( int a = 0, int b = 0 )
    { 
      x = a; 
      y = b; 
    }

    bool operator ==( const point& o ) { return o.x == x && o.y == y; }
    point operator +( const point& o ) { return point( o.x + x, o.y + y ); }
   
};
 
class map {
public:
    char m[tam_map][tam_map];
    int w, h;

    map() {
        /*
        char t[8][8] = {
            {0, 0, 0, 0, 0, 0, 0, 0}, 
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 1, 1, 0}, 
            {0, 0, 1, 0, 0, 0, 1, 0},
            {0, 0, 1, 0, 0, 0, 1, 0}, 
            {0, 0, 1, 1, 1, 1, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0}
        };*/
      
      string line;
      ifstream myfile ("mapa.csv");

      w = h = tam_map;  
       //vector<vector<char>> mapa(100,vector<char>(100,0));
      int x=0;
      int y=0;
      int value=0;

      if (myfile.is_open())
      {
        while ( getline (myfile,line) )
        {
          if(x<=tam_map-1){

            y=0;
            for (int i = 0; i < line.size(); ++i)
            {
                if(line[i]=='1' || line[i]=='0' ){
                  //cout<<line[i]<<";";
                  //mapa[x][y] = line[i];

                  value = (int)line[i] - 48;
                  m[x][y] = value;
                  y++;  
                }
                
            }
            //cout<<x<<endl;
            //cout <<'\n';
            x++;
          }

        }
        myfile.close();
      }

      else cout << "Unable to open file";

     /* 
    for (int i = 0; i < 100; ++i)
     {
        for (int j = 0; j < 100; ++j)
        {
          cout<<m[i][j]<<",";
        }
        cout<<endl;
     }*/

       
        /*
        w = h = 100;
        for( int r = 0; r < h; r++ )
            for( int s = 0; s < w; s++ )
                m[s][r] = t[r][s];*/

    }

    int operator() ( int x, int y ) { return m[x][y]; }
    
    
};
 
class node {
public:
    point pos, parent;
    int dist, cost;
    
    bool operator == (const node& o ) { return pos == o.pos; }
    bool operator == (const point& o ) { return pos == o; }
    bool operator < (const node& o ) { return dist + cost < o.dist + o.cost; }
    
};
 
class aStar {
public:
    aStar() {
        neighbours[0] = point( -1, -1 );//<-v
        neighbours[1] = point(  1, -1 );//^<-
        neighbours[2] = point( -1,  1 );//<- ^ 
        neighbours[3] = point(  1,  1 );//-> ^
        
        neighbours[4] = point(  0, -1 );//v 
        neighbours[5] = point( -1,  0 );//<
        neighbours[6] = point(  0,  1 );//^ 
        neighbours[7] = point(  1,  0 );//>
    }
 
    int calcDist( point& p ){

        // distancia de Manhattan
        int x = end.x - p.x, y = end.y - p.y;
        return( x * x + y * y );
    }


    int calcDist2( point& p ){

        // distancia de Manhattan
        int x = end2.x - p.x, y = end2.y - p.y;
        return( x * x + y * y );
    }
 
    bool isValid( point& p ) {
        //verificamos que el punto este dentro del mapa
        return ( p.x >-1 && p.y > -1 && p.x < m.w && p.y < m.h );
    }
 
    bool existPoint( point& p, int cost ) {
        std::list<node>::iterator i;
        i = std::find( closed.begin(), closed.end(), p );
        if( i != closed.end() ) {
            if( ( *i ).cost + ( *i ).dist < cost ) return true;
            else { closed.erase( i ); return false; }
        }
        i = std::find( open.begin(), open.end(), p );
        if( i != open.end() ) {
            if( ( *i ).cost + ( *i ).dist < cost ) return true;
            else { open.erase( i ); return false; }
        }
        return false;
    }



     bool existPoint2( point& p, int cost ) {
        std::list<node>::iterator i;
        i = std::find( closed2.begin(), closed2.end(), p );
        if( i != closed2.end() ) {
            if( ( *i ).cost + ( *i ).dist < cost ) return true;
            else { closed2.erase( i ); return false; }
        }
        i = std::find( open2.begin(), open2.end(), p );
        if( i != open2.end() ) {
            if( ( *i ).cost + ( *i ).dist < cost ) return true;
            else { open2.erase( i ); return false; }
        }
        return false;
    }


 
    void fillOpen( node& n ) {
        
                    
        int stepCost, nc, dist;
        point neighbour;
        //bool rpta = false;

        //omp_set_dynamic(0);     // Explicitly disable dynamic teams
        //omp_set_num_threads(4); // Use 4 threads for all consecutive parallel regions

        
        //#pragma omp parallel for 
        for( int x = 0; x < 8; x++ ) {
            // one can make diagonals have different cost
            stepCost = x < 4 ? 1 : 1;
            neighbour = n.pos + neighbours[x];
            
            //if( neighbour == end ) return true;  //rpta=true;

            //cout<<"vecino pos "<<neighbour.x<<" "<< neighbour.y <<endl;
 
            if( isValid( neighbour ) && m( neighbour.x, neighbour.y ) != 1 ) {
                nc = stepCost + n.cost;
                dist = calcDist( neighbour );
                if( !existPoint( neighbour, nc + dist ) ) {
                    node m;
                    m.cost = nc; m.dist = dist;
                    m.pos = neighbour; 
                    m.parent = n.pos;

                    //omp_set_lock(&lock);
                    open.push_back( m );
                    //omp_unset_lock(&lock);
                }
            }
        }

        //return false;

        //return false;
    }
 
    
    void fillOpen2( node& n ) {
        
                    
        int stepCost, nc, dist;
        point neighbour;
        //bool rpta = false;

        //omp_set_dynamic(0);     // Explicitly disable dynamic teams
        //omp_set_num_threads(4); // Use 4 threads for all consecutive parallel regions

        
        //#pragma omp parallel for 
        for( int x = 0; x < 8; x++ ) {
            // one can make diagonals have different cost
            stepCost = x < 4 ? 1 : 1;
            neighbour = n.pos + neighbours[x];
            //if( neighbour == end2 ) return true;  //rpta=true;

            //cout<<"vecino pos "<<neighbour.x<<" "<< neighbour.y <<endl;
 
            if( isValid( neighbour ) && m( neighbour.x, neighbour.y ) != 1 ) {
                nc = stepCost + n.cost;
                dist = calcDist2( neighbour );
                if( !existPoint2( neighbour, nc + dist ) ) {
                    node m;
                    m.cost = nc; m.dist = dist;
                    m.pos = neighbour; 
                    m.parent = n.pos;

                    //omp_set_lock(&lock);
                    open2.push_back( m );
                    //omp_unset_lock(&lock);
                }
            }
        }

        //return false;

        //return false;
    }




    /*
    int path( std::list<point>& path ) {
        path.push_front( end );
        int cost = 1 + closed.back().cost; 
        path.push_front( closed.back().pos );
        point parent = closed.back().parent;
 
        for( std::list<node>::reverse_iterator i = closed.rbegin(); i != closed.rend(); i++ ) {
            if( ( *i ).pos == parent && !( ( *i ).pos == start ) ) {
                path.push_front( ( *i ).pos );
                parent = ( *i ).parent;
            }
        }
        path.push_front( start );
        return cost;
    }*/


    bool interseccion(list<node> &closed1, list<node> &closed2){

        std::list<node>::iterator i;
        for (std::list<node>::iterator it = closed1.begin(); it != closed1.end(); ++it){
            i = std::find( closed2.begin(), closed2.end(), *it );

            if( i != closed2.end() ) {

                cout<<"Punto de Interseccion :"<<(*i).pos.x<<","<<(*i).pos.y<<"   "<<(*it).pos.x<<","<<(*it).pos.y<<endl;

                return true;
            }     
        }
        return false;
    }
 
    map m; point end, start;
    point end2, start2;


    point neighbours[8];
    std::list<node> open;
    std::list<node> closed;

    std::list<node> open2;
    std::list<node> closed2;


    bool search( point& s, point& e, map& mp ) {


    //source and target
    m = mp;

    // agente 1
    node n; 
    end = e; 
    start = s; 
    
    n.cost = 0; 
    n.pos = s; 
    n.parent = 0; 
    n.dist = calcDist( s );


    // agente 2
    node n2;
    end2 = s;
    start = e;

    n2.cost =0;
    n2.pos = e;
    n2.parent =0;
    n2.dist = calcDist2(e);


    open.push_back( n );

    open2.push_back( n2 ); 

    // Record start time
    //start = std::chrono::high_resolution_clock::now();

    tstart = high_resolution_clock::now();

    int num_nodos_expandidos=0;
    while( !open.empty() && !open2.empty() ) {
        //open.sort();
        //#pragma omp parallel  
        node n = open.front();

        node n2 = open2.front();

        end = n2.pos;
        end2 = n.pos;

        open.pop_front();
        closed.push_back(n);
        fillOpen(n);


        

        /*
        if(n==n2){

            cout<<"Punto de Interseccion :"<<n.pos.x<<","<<n.pos.y<<"   "<<n2.pos.x<<","<<n2.pos.y<<endl;
            
            cout<<"Numero de nodos expandidos "<<num_nodos_expandidos<<endl;
            return true;

            //cout<<"Punto de Interseccion :"<<n.pos.x<<","<<n.pos.y<<"   "<<n2.pos.x<<","<<n2.pos.y<<endl;
        }*/


        open2.pop_front();
        closed2.push_back(n2);
        fillOpen2(n2);


        

        //if( fillOpen( n ) ){
            //tfinish = high_resolution_clock::now(); 
        //    return true;
        //}

        if(interseccion(open,open2)==true){

            tfinish = high_resolution_clock::now(); 
            cout<<"Numero de nodos expandidos "<<num_nodos_expandidos<<endl;


            std::chrono::duration<double> elapsed = tfinish - tstart;
            //std::cout << "Elapsed time: " <<std::setprecision(3) << elapsed.count() << " s\n";

            printf(" Elapsed time %.2f", elapsed.count());


            return true;
        }

        //cout<<"Punto Agente 1:"<<n.pos.x<<","<<n.pos.y<<endl;
        //cout<<"Punto Agente 2:"<<n2.pos.x<<","<<n2.pos.y<<endl;

        num_nodos_expandidos++;

    }

    // Record end time
    //finish = std::chrono::high_resolution_clock::now();

    

    //elapsed = finish - start;

    //double tiempo = difftime (end,begin);


    return false;
    }

 
};
 
int main( int argc, char* argv[] ) {


        
    map m;
    point s, e( tam_map-1, tam_map-1 );
    //point s, e( 20, 10 );
    aStar as;

    bool rpta;

    rpta = as.search( s, e, m ); 

    if(rpta){

        //cout<<"Encontro camino"<<endl;
    }else{
        //cout<<"No encontro camino"<<endl;
    }




    /* 
    if( as.search( s, e, m ) ) {

        
        std::list<point> path;
        int c = as.path( path );

        
        
        for( int y = -1; y <= tam_map; y++ ) {
            for( int x = -1; x <= tam_map; x++ ) {
                //x < 0 || y < 0 || x > 7 || y > 7 || 
                if( x < 0 || y < 0 || x > tam_map-1 || y > tam_map-1 ||  m( x, y ) == 1 ){
                    //std::cout << char(0xdb);
                    //cout << "\u25A0";
                    //cout << "\uxdb";
                    cout<<"\u2302";
                    //std::cout << (char)254u;
                }        
                else {
                    if( std::find( path.begin(), path.end(), point( x, y ) )!= path.end() )
                        std::cout << "x";
                    else std::cout << ".";
                }
            }
            std::cout << "\n";
        }
 
        std::cout << "\nPath cost " << c << ": ";
        for( std::list<point>::iterator i = path.begin(); i != path.end(); i++ ) {
            std::cout<< "(" << ( *i ).x << ", " << ( *i ).y << ") ";
        }
        
    
    }
    */





    
    cout<<endl;



    

    //std::chrono::duration<double> elapsed = tfinish - tstart;
    //std::cout << "Elapsed time: " <<std::setprecision(3) << elapsed.count() << " s\n";

    //printf(" Elapsed time %.2f", elapsed.count());

    //cout<<"TIEMPO "<<tiempo<<endl;
    std::cout << "\n\n";
    return 0;
}
 