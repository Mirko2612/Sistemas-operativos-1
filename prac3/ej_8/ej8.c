#define B 2
#define K (2*B)

volatile int s, r, buf[B];

static inline int diff() { return (K + s - r) % K; }

void * prod(void *_arg) {
    int cur = 0;
    while (1) {
        while (diff() == B)
            ;
        buf[s % B] = cur++;
        s = (s+1) % K;
    }
}

void * cons(void *_arg) {
    int cur;
    while (1) {
        while (diff() == 0)
            ;
        cur = buf[r % B];
        r = (r+1) % K;
        printf("Lei %d\n", cur);
    }
}