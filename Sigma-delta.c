int i = 0;
int Leitura = 0;

//----------------------------------------------------------------------------

void Delta_Sigma_Init() {
    VRCON = 0xEC;
    TRISA3_bit = 0;
    CMCON = 0x06;
}

//----------------------------------------------------------------------------

int Delta_Sigma_Leitura(int Valor) {
    int ADC_Valor = 0;
    T0CS_bit = 0;
    PSA_bit = 1;
    CMCON = 0x03;
    
    while (Valor) {
        TMR0 = 0xF7;
        T0IF_bit = 0;
        
        if (C1OUT_bit) {
            RA3_bit = 1;
        } else {
            RA3_bit = 0;
            ADC_Valor++;
        }
        
        Valor--;
        while (!T0IF_bit);
    }
    
    CMCON = 0x06;
    return (ADC_Valor);
}

//----------------------------------------------------------------------------

// Maximum number of servos
const int N_SERVOS = 8;

// Tick = 4 / clock(16MHz) * Prescaler
// Tick = 4 / 16 * 4 = 1us
const unsigned TicksFrame = 2500;   // 20 ms / 8 servos / Tick
const unsigned TicksMin = 1000;     // position 0º = 1 ms / Tick
const unsigned TicksCenter = 1500;  // position 90º = 1.5 ms / Tick
const unsigned TicksMax = 2000;     // position 180º = 2 ms / Tick

unsigned Timer1 at TMR1L;

// Configuration register for each servo
typedef struct {
    char Port;
    char Pin;
    unsigned Angle;
    union {
        char Enable:1;
    };
} Servos;

// Create the 8 servos
Servos myServos[N_SERVOS];
static char flags = 0;
sbit pulse at flags.B0;

// Initialize the servos
void Servo_Init() {
    char i;
    static char temp = 0;
    
    for (i = 0; i < N_SERVOS; i++) {
        myServos[i].Port = &temp;
        myServos[i].Pin = 0;
        myServos[i].Angle = TicksMin;
        myServos[i].Enable = 0;
    }
    
    Timer1 = 65535 - TicksFrame;
    T1CON = 0b00010001; // Clock 8 MHz / Prescaler 1:2
    TMR1IE_bit = 1;
    PEIE_Bit = 1;
    GIE_Bit = 1;
}

// Add a new servomotor
void Servo_Attach(char servo, char out, char pin) {
    Servos *ptr;
    
    if (servo > N_SERVOS)
        return;
    
    ptr = &myServos[servo];
    (*ptr).Port = out;
    (*ptr).Pin = 1 << pin;
    (*ptr).Enable = 1;
}

void Servo_Interrupt() {
    static char servo_idx = 0;
    char port, pin;
    unsigned an;
    
    // Pass the address value to the pointer
    FSR = (char)&myServos[servo_idx];
    port = INDF; // Get the port value, myServos[x].Port
    FSR++; // Increment the pointer
    pin = INDF; // Get the pin value, myServos[x].Pin
    FSR++; // Increment the pointer
    ((char*)&an)[0] = INDF;
    FSR++;
    ((char*)&an)[1] = INDF; // Get the angle, myServos[x].Angle
    FSR++;
    
    // Is the servo enabled?
    if (INDF.B0) { // myServos[x].enable
        FSR = port; // Pass the port address to the pointer
        if (!pulse) {
            Timer1 = 65535 - an + 29; // (29) - correction factor?
            INDF |= pin;
        } else {
            Timer1 = (65535 - TicksFrame) + an + 37;
            INDF &= ~pin;
            ++servo_idx &= 7;
        }
        pulse = ~pulse;
    } else {
        Timer1 = (65535 - TicksFrame);
        ++servo_idx &= 7;
    }
}

void interrupt() {
    if (TMR1IF_Bit) {
        Servo_Interrupt();
        TMR1IF_Bit = 0;
    }
}

void main() {
    // TRISA = 0b00100001;
    TRISB = 0; // Set PORTB pins as output
    // uart1_init(9600); // Initialize UART module with 9600bps
    Delta_Sigma_Init();
    Servo_Init();
    Delay_ms(200);
    // UART1_Write_Text(" UART Test Successful! "); // Character Message to be Sent
    
    Servo_Attach(0, (char)&PORTB, 0);
    Servo_Attach(1, (char)&PORTB, 1);
    Servo_Attach(2, (char)&PORTB, 2);
    Servo_Attach(3, (char)&PORTB, 3);
    
    while (1) {
        Leitura = Delta_Sigma_Leitura(1900);
        if (PORTA.F5 == 1) {
            Delay_ms(100);
            if (PORTA.F5 == 1) {
                i++;
                if (i > 3) i = 0;
            }
        }
        myServos[i].Angle = (500 + Leitura);
        Delay_ms(10);
    }
}
