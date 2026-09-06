//
//  water_line.h
//  firstSketch
//
//  Created by 민동현 on 26/09/2019.
//


#ifndef water_line_h
#define water_line_h

#include "ofMain.h"

#define EPSILON 0.01


class LineSegment
{
public:
    float x1;
    float x2;
    float y1;
    float y2;
    double slope;
    float x_coef;
    float y_coef;
    float constant;
};
class Dot
{
public:
    float x1;
    float y1;
};

class WaterLine{
protected:

public:



    WaterLine(int num_of_line);
    ~WaterLine();
    
    void reset();
    void update();
    void draw();
    void calculate_path(LineSegment *lineseg, int num_of_line);
    
    Dot* path;

    float uniqueColor_r, uniqueColor_g, uniqueColor_b;
    float scale;
    int hexcolor;
    
    int path_idx;
    int draw_complete;
    int calc_complete;
    ofPoint start_dot;
    float dot_radius;
    
};

#endif /* water_line_h */
