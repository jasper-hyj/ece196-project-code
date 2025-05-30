#ifndef L298NPIN_H
#define L298NPIN_H

struct L298NPin {
    int enA;
    int enB;
    int in1;
    int in2;
    int in3;
    int in4;

    constexpr L298NPin(
        int enA, int enB, int in1, int in2, int in3, int in4)
        : enA(enA), enB(enB), in1(in1), in2(in2), in3(in3), in4(in4) {}
};

#endif