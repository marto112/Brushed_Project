//Setup 2 ports which we are using for H bridge. 

#define FORWARD_LOW  LOW_C

#define FORWARD_HIGH HIGH_A

#define BACKWARD_LOW  LOW_A

#define BACKWARD_HIGH HIGH_C
#define SPARE_HIGH HIGH_B
#define SPARE_LOW LOW_B


#define FORWARD_LOW_DIR DIR_C
#defina FORWARD_LOW_ENABLE ENABLE_C

#define FORWARD_HIGH_DIR DIR_A
#defina FORWARD_HIGH_ENABLE ENABLE_A

#define BACKWARD_HIGH_DIR DIR_C
#defina BACKWARD_HIGH_ENABLE ENABLE_C

#define BACKWARD_LOW_DIR DIR_A
#defina BACKWARD_LOW_ENABLE ENABLE_A

#define SPARE_DIR DIR_B
#defina SPARE_ENABLE ENABLE_B

//Motor Functions
void setForwardLow() {
    if (ENABLE){
        FORWARD_LOW_DIR = 0;
        FORWARD_LOW_ENABLE = 1;
    }
    else {
        if (LOW_ACTIVE_LOW) {
            FORWARD_LOW = 0;
        } else {
            FORWARD_LOW = 1;
        }
    }
}



void clrForwardLow() {
    if (ENABLE){
        FORWARD_ENABLE = 0;
    }
    else {
        if (LOW_ACTIVE_LOW) {
            FORWARD_LOW = 1;
        } else {
            FORWARD_LOW = 0;
        }
    }
}



void setForwardHigh() {
    if (ENABLE){
        FORWARD_DIR = 1;
        FORWARD_ENABLE = 1;
    }
    else {
        if (HIGH_ACTIVE_LOW) {
            FORWARD_HIGH = 0;
        } else {
            FORWARD_HIGH = 1;
        }

    }
}



void clrForwardHigh() {
    if (ENABLE){
        FORWARD_ENABLE = 0;
    }
    else {
        if (HIGH_ACTIVE_LOW) {
            FORWARD_HIGH = 1;
        } else {
            FORWARD_HIGH = 0;
        }
    }
}



void setBackwardLow() {
    if (ENABLE){
        BACKWARD_DIR = 0;
        BACKWARD_ENABLE = 1;
    }
    else {
        if (LOW_ACTIVE_LOW) {
            BACKWARD_LOW = 0;
        } else {
            BACKWARD_LOW = 1;
        }
    }
}



void clrBackwardLow() {
    if (ENABLE){
        BACKWARD_ENABLE = 0;
    }
    else {
        if (LOW_ACTIVE_LOW) {
            BACKWARD_LOW = 1;
        } else {
            BACKWARD_LOW = 0;
        }
    }
}



void setBackwardHigh() {
    if (ENABLE){
        BACKWARD_DIR = 1;
        BACKWARD_ENABLE = 1;
    }
    else {
        if (HIGH_ACTIVE_LOW) {
            BACKWARD_HIGH = 0;
        } else {
            BACKWARD_HIGH = 1;
        }
    }
}



void clrBackwardHigh() {
    if (ENABLE){
        BACKWARD_ENABLE = 0;
    }
    else {
        if (HIGH_ACTIVE_LOW) {
            BACKWARD_HIGH = 1;
        } else {
            BACKWARD_HIGH = 0;
        }
    }
}


void setSpareLow() {
    if (ENABLE){
        SPARE_DIR = 0;
        SPARE_ENABLE = 1;
    }
    else {
        if (LOW_ACTIVE_LOW) {
            SPARE_LOW = 0;
        } else {
            SPARE_LOW = 1;
        }
    }
}



void clrSpareLow() {
    if (ENABLE){
        SPARE_ENABLE = 0;       
    }
    else {
        if (LOW_ACTIVE_LOW) {
            SPARE_LOW = 1;
        } else {
            SPARE_LOW = 0;  
        }
    }
}



void setSpareHigh() {
    if (ENABLE){
        SPARE_DIR = 1;
        SPARE_ENABLE = 1;
    }
    else {
        if (HIGH_ACTIVE_LOW) {
            SPARE_HIGH = 0;
    } else {

        SPARE_HIGH = 1;

    }
    }
}



void clrSpareHigh() {
    if (ENABLE){
        SPARE_ENABLE = 0;
    }
    else {
        if (HIGH_ACTIVE_LOW) {
            SPARE_HIGH = 1;
        } else {
            SPARE_HIGH = 0;
        }
    }
}
