//Motor Functions
void setALow() {
    if (LOW_ACTIVE_LOW) {
        LOW_A = 0;
    } else {
        LOW_A = 1;
    }
}
void clrALow() {
    if (LOW_ACTIVE_LOW) {
        LOW_A = 1;
    } else {
        LOW_A = 0;
    }
}
void setAHigh() {
    if (HIGH_ACTIVE_LOW) {
        HIGH_A = 0;
    } else {
        HIGH_A = 1;
    }
}
void clrAHigh() {
    if (HIGH_ACTIVE_LOW) {
        HIGH_A = 1;
    } else {
        HIGH_A = 0;
    }
}
void setBLow() {
    if (LOW_ACTIVE_LOW) {
        LOW_B = 0;
    } else {
        LOW_B = 1;
    }
}

void clrBLow() {
    if (LOW_ACTIVE_LOW) {
        LOW_B = 1;
    } else {
        LOW_B = 0;
    }
}
void setBHigh() {
    if (HIGH_ACTIVE_LOW) {
        HIGH_B = 0;
    } else {
        HIGH_B = 1;
    }
}
void clrBHigh() {
    if (HIGH_ACTIVE_LOW) {
        HIGH_B = 1;
    } else {
        HIGH_B = 0;
    }
}

void setCLow() {
    if (LOW_ACTIVE_LOW) {
        LOW_C = 0;
    } else {
        LOW_C = 1;
    }
}
void clrCLow() {
    if (LOW_ACTIVE_LOW) {
        LOW_C = 1;
    } else {
        LOW_C = 0;
    }
}
void setCHigh() {
    if (HIGH_ACTIVE_LOW) {
        HIGH_C = 0;
    } else {
        HIGH_C = 1;
    }
}
void clrCHigh() {
    if (HIGH_ACTIVE_LOW) {
        HIGH_C = 1;
    } else {
        HIGH_C = 0;
    }
}