monitor fileConcurrency {
        enum {NOT_REQUESTED, REQUESTED, READING} state[N];
        condition self[N];
        int processSum;

        void open(int i) {
                state[i] = REQUESTED;
                if (i + processSum >= N){
                        self[i].wait();
                }
                state[i] = READING;
                processSum += i;
        }

        void close(int i) {
                state[i] = NOT_REQUESTED;
                processSum -= i;

                for (int x = N - processSum - 1; x >= 0; x--) {
                        if (state[x] == REQUESTED) {
                                self[x].signal(); 
                                break;
                        }
                }
        }

        initialization.code() {
                for (int i = 0; i < N; i++) {
                        state[i] = NOT_REQUESTED;
                }
                processSum = 0;
        }
}