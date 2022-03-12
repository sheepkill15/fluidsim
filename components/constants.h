#ifndef CONSTANTS_H
#define CONSTANTS_H

class Constants {
    public:
    Constants() = delete;

    const static int population = 5000;

    const static struct
    {
        float x = 100.f;
        float y = 100.f;
    } velocity;
};

#endif