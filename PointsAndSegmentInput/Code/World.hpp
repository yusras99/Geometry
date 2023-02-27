#ifndef COMMON_H
#define COMMON_H

struct World {

    static float X_MIN;
    static float X_MAX;
    static float Y_MIN;
    static float Y_MAX;
    static float WORLD_WIDTH;
    static float WORLD_HEIGHT;

    static void setWorldBounds(float xmin, float xmax, float ymin, float ymax){
        if ((xmax <= xmin) || (ymax <= xmin)){
            exit(5);
        }
        X_MIN = xmin;
        X_MAX = xmax;
        Y_MIN = ymin;
        Y_MAX = ymax;
        WORLD_WIDTH = xmax - xmin;
        WORLD_HEIGHT = ymax - ymin;
    }
};


#endif  //  WORLD_H

