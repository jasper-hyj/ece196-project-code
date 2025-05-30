#ifndef TMC2209PIN_H
#define TMC2209PIN_H

struct TMC2209Pin {
    int en;
    int step;
    int dir;
    int clk;
    int uart_rx;
    int uart_tx;
    int ms1;
    int ms2;

    constexpr TMC2209Pin(
        int en, int step, int dir, int clk,
        int uart_rx, int uart_tx, int ms1, int ms2
    ) : en(en), step(step), dir(dir), clk(clk),
        uart_rx(uart_rx), uart_tx(uart_tx), ms1(ms1), ms2(ms2) {}
};

#endif