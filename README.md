# PIC16F628A using Sigma delta
 
![image](https://github.com/user-attachments/assets/fff0582b-9d29-4417-9db3-6f0fdef833a2)

Servo Motor Control With PIC16F628A Using Sigma-delta Mode As Analog Converter.

Hello everyone, I present to you a circuit/code that can be very helpful for people who have difficulty controlling a servo motor using PIC microcontrollers. In this project, I use the 16F628A due to its powerful yet simple nature, although it lacks an A/D converter.

The highlight of this circuit is the employability of the delta-sigma mode in emulating an A/D converter of up to 10 bits, which can have countless applications depending on your creativity. Another great advantage is using the PIC to control up to 8 servo motors through port B. With this combination of advantages, we have a versatile circuit for creating robotic arms, mobile robots, intelligent automation, etc.

Things that are noticeable when using the circuit in delta-sigma mode are that it sometimes shows instability in the response to the input signal. Therefore, it is extremely necessary to use resistors with a 1% tolerance and good grounding to avoid spikes.
This circuit was the first step in a larger project involving the use of other sensors in the control of the servo motor.

The code is set for the microcontroller to operate at 8MHz, but it can be easily modified by changing the predefined prescaler.

The delta-sigma value "1900" was defined this way because, although each servo theoretically has a pulse of 2 ms = 2000us for +90°, there are variations. Therefore, I needed to compensate by starting with the code line:

"myServos[i].Angle = (500 + Reading);", where 500 corresponds to the 500us that makes the servo start at -90°.

The code lines:

if(porta.f5 == 1){
    delay_ms(100);
    if(porta.f5 == 1){
        i++;
        if (i > 3) i = 0;
    }
}

function to increment the variable "i", which will change the servo motor control. The use of two identical conditionals with the delay serves to avoid bounce when pressing the contact switch.
