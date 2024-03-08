#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <time.h>
#include <SDL2\SDL.h>

  
  
typedef enum { false, true } bool;

struct chip8 {
    unsigned short opcode; // 35 opcode so need 2bytes in hex
    unsigned char memoryC8[4096]; // memoryC8chip8 locations of 1bytes * 2
    // 512 first bytesare occupied 0x200 
    //Up to 1FFF in our case with 8192 bytes
    //Raise it to a var holding 8 bytes ?
    // (0xF00-0xFFF)  Display refresh
    // the 96 bytes below that (0xEA0-0xEFF) were reserved for callstackC8
    /*

     These machines had 4096 (0x1000) memoryC8locations, all of which are 8 bits (a byte) which is where the term CHIP-8 originated. 
    First 512 bytes occupied.  From (0x200) and do not access any of the memoryC8below the location 512 (0x200) 0100 0000 0000.
    The uppermost 256 bytes (0xF00-0xFFF) are reserved for display refresh, and the 96 bytes below that (0xEA0-0xEFF) were reserved for the call stackC8, internal use, and other variables.

    0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
    0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
    0x200-0xFFF - Program ROM and work RAM*/
    unsigned char Vreg[16]; //Data register V0to VF *2 //31 is honorary drawflag
    unsigned short I; // Index
    unsigned short pc; // Programm counter
    unsigned short stackC8[16]; // stackC8
    unsigned short sp; // stackC8pointer 

    unsigned char gfx[64 * 32]; // Graphic chips, array double
    unsigned char gfx2[64][32]; // Graphic chips, array double

    unsigned char delay_timer; // Dekat timer
    unsigned char sound_timer; // Sound timer

    unsigned char keystate[16];
    bool draw ;

 };

 
unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};


typedef struct chip8 Chip8;

void initialize(Chip8 * emu) {
    emu -> pc = 0x200; // Program counter starts at 0x200
    emu -> opcode = 0; // Reset current opcode	
    emu -> I = 0; // Reset index register
    emu -> sp = 0; // Reset stackC8 pointer

    // Clear display	
    memset(emu -> gfx, 0, sizeof(emu -> gfx));
    //Cler key
        memset(emu -> keystate, 0, sizeof(emu ->keystate));

    // Clear stackC8
    memset(emu -> stackC8, 0, sizeof(emu -> stackC8));
    // Clear registers V0-VF
    memset(emu -> Vreg, 0, sizeof(emu -> Vreg));
    // Clear memoryC8   
    memset(emu -> memoryC8, 0, sizeof(emu -> memoryC8));

    // Load fontset
    for (int i = 0; i < 80; ++i) {
        // (0x000to 0x04F)  79 adresslocations
        emu -> memoryC8[i + 0x050] = chip8_fontset[i];
    }

    // Reset timers
	emu->delay_timer = 0;
	emu->sound_timer = 0;
    // Clear screen once
	emu->draw  = true;
    srand (time(NULL));

}

// Function CCCCCCCCCCCCCCCCCCCCCC

void OP_CodeFunctions0NNN(Chip8 * emu) {
  //    emu -> pc += 2;  
         //printf("OP_CodeFunctions0NNN\n");

};
void OP_CodeFunctions00E0(Chip8 * emu) {
     memset(emu -> gfx, 0, sizeof(emu -> gfx));
        //printg("OP_CodeFunctions00E0\n");

    emu -> draw = true;
    emu -> pc += 2;
 };
void OP_CodeFunctions00EE(Chip8 * emu) {
     //Tutorial code, overlooked this 
        //printg("OP_CodeFunctions00EE\n");

    --emu->sp;			 
	emu->pc = emu->stackC8[emu->sp];
     emu -> pc += 2; //Useless only here for the count
 };

void OP_CodeFunctions1NNN(Chip8 * emu) {
    emu -> pc = emu -> opcode & 0x0FFF;
       //printg("OP_CodeFunctions1NNN\n");

};
void OP_CodeFunctions2NNN(Chip8 * emu) {
    //Parallel to return subroutine  
       //printg("OP_CodeFunctions2NNN\n");

    emu -> stackC8[emu -> sp] = emu -> pc;
    ++emu -> sp;
    emu -> pc = emu -> opcode & 0x0FFF;
};
void OP_CodeFunctions3XNN(Chip8 * emu) {
       //printg("OP_CodeFunctions3XNN\n");

    //Get only X an then Shift 
    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    if (emu -> Vreg[Xval] == (emu -> opcode & 0x00FF)) {
        emu -> pc += 2;
    }
    emu -> pc += 2;
};
void OP_CodeFunctions4XNN(Chip8 * emu) {
       //printg("OP_CodeFunctions4XNN\n");

    //Get only X an then Shift 
    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    if (emu -> Vreg[Xval] != emu -> opcode & 0x00FF) {
        emu -> pc += 2;
    }
    emu -> pc += 2;
};

void OP_CodeFunctions5XY0(Chip8 * emu) {
       //printg("OP_CodeFunctions5XY0\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned short Yval = (emu -> opcode & 0x00F0) >> 4;
    if (emu -> Vreg[Xval] == emu -> Vreg[Yval]) {
        emu -> pc += 2;
    }
    emu -> pc += 2;
};

void OP_CodeFunctions6XNN(Chip8 * emu) {
       //printg("OP_CodeFunctions6XNN\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    emu -> Vreg[Xval] = (emu -> opcode & 0x00FF);
    emu -> pc += 2;
};

void OP_CodeFunctions7XNN(Chip8 * emu) {
       //printg("OP_CodeFunctions7XNN\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    emu -> Vreg[Xval] += (emu -> opcode & 0x00FF);
    emu -> pc += 2;
};

void OP_CodeFunctions8XY0(Chip8 * emu) {
       //printg("OP_CodeFunctions8XY0\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned short Yval = (emu -> opcode & 0x00F0) >> 4;
    emu -> Vreg[Xval] = emu -> Vreg[Yval];
    emu -> pc += 2;
};

void OP_CodeFunctions8XY1(Chip8 * emu) {
           //printg("OP_CodeFunctions8XY1\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned short Yval = (emu -> opcode & 0x00F0) >> 4;
    emu -> Vreg[Xval] |= emu -> Vreg[Yval];
    emu -> pc += 2;
};

void OP_CodeFunctions8XY2(Chip8 * emu) {
           //printg("OP_CodeFunctions8XY2\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned short Yval = (emu -> opcode & 0x00F0) >> 4;
    emu -> Vreg[Xval] &= emu -> Vreg[Yval];
    emu -> pc += 2;
};

void OP_CodeFunctions8XY3(Chip8 * emu) {
           //printg("OP_CodeFunctions8XY3\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned short Yval = (emu -> opcode & 0x00F0) >> 4;
    emu -> Vreg[Xval] ^= emu -> Vreg[Yval];
    emu -> pc += 2;
};

void OP_CodeFunctions8XY4(Chip8 * emu) {
           //printg("OP_CodeFunctions8XY4\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned short Yval = (emu -> opcode & 0x00F0) >> 4;
    //VF setting for overflow  emu->Vreg[15]   done
    if (emu -> Vreg[Yval] > (0xFF - emu -> Vreg[Xval])) {
        emu -> Vreg[15] = 1; 
    } else {
        emu -> Vreg[15] = 0;
    }
    emu -> Vreg[Xval] += emu -> Vreg[Yval];
    emu -> pc += 2;
};

void OP_CodeFunctions8XY5(Chip8 * emu) {
           //printg("OP_CodeFunctions8XY5\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned int Yval = (emu -> opcode & 0x00F0) >> 4;
    //VF setting for underflow  emu->Vreg[15] not done
    if (emu->Vreg[Yval] <= (0xFF - emu->Vreg[Xval])) {
    emu->Vreg[15] = 1; 
} else {
    emu->Vreg[15] = 0; 
}
    emu -> Vreg[Xval] -= emu -> Vreg[Yval];
    emu -> pc += 2;
};

void OP_CodeFunctions8XY6(Chip8 * emu) {
           //printg("OP_CodeFunctions8XY6\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned int Yval = (emu -> opcode & 0x00F0) >> 4;
    //VF   not done
    emu -> Vreg[Xval] >>= 1;
    emu -> pc += 2;
};

void OP_CodeFunctions8XY7(Chip8 * emu) {
           //printg("OP_CodeFunctions8XY7\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned int Yval = (emu -> opcode & 0x00F0) >> 4;
    //VF   not done
    emu -> Vreg[Xval] = emu -> Vreg[Yval] - emu -> Vreg[Xval];
    emu -> pc += 2;
};

void OP_CodeFunctions8XYE(Chip8 * emu) {
           //printg("OP_CodeFunctions8XYE\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned int Yval = (emu -> opcode & 0x00F0) >> 4;
    //VF   not done
    emu -> Vreg[Xval] <<= 1;
    emu -> pc += 2;

};

void OP_CodeFunctions9XY0(Chip8 * emu) {
           //printg("OP_CodeFunctions9XY0\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    unsigned int Yval = (emu -> opcode & 0x00F0) >> 4;
    if (emu -> Vreg[Xval] != emu -> Vreg[Yval]) {
        emu -> pc += 2;
    }
    emu -> pc += 2;

};

void OP_CodeFunctionsANNN(Chip8 * emu) {
       //printg("OP_CodeFunctionsANNN\n");

    emu -> I = emu -> opcode & 0x0FFF;
    emu -> pc += 2;

};
void OP_CodeFunctionsBNNN(Chip8 * emu) {
       //printg("OP_CodeFunctionsBNNN\n");
    
    emu -> pc = emu -> Vreg[(emu ->opcode & 0x0F00)>>8] + (emu ->opcode & 0x0FFF);

   // emu -> pc = emu -> Vreg[0] + (emu ->opcode & 0x0FFF);
};
void OP_CodeFunctionsCXNN(Chip8 * emu) {
       //printg("OP_CodeFunctionsCNNN\n");

    //Get only X an then Shift 
    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    emu -> Vreg[Xval] = (rand()) & 0x00FF;
    emu -> pc += 2;

};

//Logic script
void OP_CodeFunctionsDXYN(Chip8 * emu) {
       //printg("OP_CodeFunctionsDNNN\n");

    //Get only X an then Shift 
    unsigned short Xval = emu->Vreg[(emu -> opcode & 0x0F00) >> 8] %64;
    //Get only Y an then Shift 
    unsigned short Yval = emu->Vreg[(emu -> opcode & 0x00F0) >> 4] %32;
    unsigned short Nval = (emu -> opcode & 0x000F);
    unsigned short pixel;

    emu -> Vreg[15] = 0;

    for (int y = 0; y < Nval; ++y) {
        pixel = emu -> memoryC8[emu -> I + y];
        for (int x = 0; x < 8; ++x) {
            if ((pixel & (0x80 >> x)) != 0) //Inverted
            {
                if (emu -> gfx[(Xval + x + ((Yval + y) * 64))] == 1) {
                    emu -> Vreg[15] = 1;
                }
                emu -> gfx[(Xval + x + ((Yval + y) * 64))% (64 * 32)] ^= 1;
            }
        }
    }
            //VF   not done
    //Drawflag
    emu -> draw = true;
    emu -> pc += 2;

};

void OP_CodeFunctionsEX9E(Chip8 * emu) {
       //printg("OP_CodeFunctionsEX9E\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    if ((emu -> keystate[emu -> Vreg[Xval]]) != 0) {
        emu -> pc += 2;
    }
    emu -> pc += 2;
};

void OP_CodeFunctionsEXA1(Chip8 * emu) {
       //printg("OP_CodeFunctionsEXA1\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    if ((emu -> keystate[emu -> Vreg[Xval]]) == 0) {
        emu -> pc += 2;
    }
    emu -> pc += 2;
};

void OP_CodeFunctionsFX07(Chip8 * emu) {
       //printg("OP_CodeFunctionsFX07\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    emu -> Vreg[Xval] = emu -> delay_timer;
    emu -> pc += 2;
};
void OP_CodeFunctionsFX0A(Chip8 * emu) {
       //printg("OP_CodeFunctionsFX0A\n");

    unsigned short Xval = (emu -> opcode & 0x0F00) >> 8;
    bool keypressed = false;

    for(int i = 0; i < 16; ++i)
					{
						if(emu->keystate[i] != 0)
						{
							emu->Vreg[Xval] = i;
                            keypressed = true;
						}
					}

    //Blocking other operation and retry 
    if(!keypressed) 
    {
        return ;
    }
    emu -> pc += 2;

};
void OP_CodeFunctionsFX15(Chip8 * emu) {
       //printg("OP_CodeFunctionsFX15\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    emu -> delay_timer = emu -> Vreg[Xval];
    emu -> pc += 2;

};
void OP_CodeFunctionsFX18(Chip8 * emu) {
   //printg("OP_CodeFunctionsFX18\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    emu -> sound_timer = emu -> Vreg[Xval];
    emu -> pc += 2;

};
void OP_CodeFunctionsFX1E(Chip8 * emu) {
       //printg("OP_CodeFunctionsFX1E\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
   /* if(Xval != 0x0F){
    emu -> I += emu -> Vreg[Xval];
    
    }*/ //Some oddity with commodore chip here
    //********
    if(emu->I + emu->Vreg[( emu->opcode & 0x0F00) >> 8] > 0xFFF){	
 						 emu->Vreg[0xF] = 1;}
					else{
						emu->Vreg[0xF] = 0;}
					emu->I += emu->Vreg[(emu->opcode & 0x0F00) >> 8];
 
 
     emu -> pc += 2;  
};

void OP_CodeFunctionsFX29(Chip8 * emu) {
    //Tuto
       //printg("OP_CodeFunctionsFX29\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    emu -> I = emu ->   Vreg[Xval]* 0x5;
    emu -> pc += 2;
};
void OP_CodeFunctionsFX33(Chip8 * emu) {
       //printg("OP_CodeFunctionsFX33\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    emu -> memoryC8[emu -> I] = emu -> Vreg[Xval] / 100;
    emu -> memoryC8[emu -> I + 1] = (emu -> Vreg[Xval] / 10) % 10;
    emu -> memoryC8[emu -> I + 2] = (emu -> Vreg[Xval] % 100) % 10;
    emu -> pc += 2;

};
void OP_CodeFunctionsFX55(Chip8 * emu) {
       //printg("OP_CodeFunctionsFX55\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    for (int i = 0; i <= Xval; ++i) {
        emu -> memoryC8[i + emu -> I] = emu -> Vreg[i];
    }
 
          emu -> pc += 2;

};
void OP_CodeFunctionsFX65(Chip8 * emu) {
       //printg("OP_CodeFunctionsFX65\n");

    unsigned int Xval = (emu -> opcode & 0x0F00) >> 8;
    for (int i = 0; i <= Xval; ++i) {
        emu -> Vreg[i] = emu -> memoryC8[i + emu -> I];
    }

 
    emu -> pc += 2;

};

void( * OP_CodeFunctions[])(Chip8 * emu) = {
    OP_CodeFunctions0NNN,
    OP_CodeFunctions00E0,
    OP_CodeFunctions00EE,
    OP_CodeFunctions1NNN,
    OP_CodeFunctions2NNN,
    OP_CodeFunctions3XNN,
    OP_CodeFunctions4XNN,
    OP_CodeFunctions5XY0,
    OP_CodeFunctions6XNN,
    OP_CodeFunctions7XNN,
    OP_CodeFunctions8XY0,
    OP_CodeFunctions8XY1,
    OP_CodeFunctions8XY2,
    OP_CodeFunctions8XY3,
    OP_CodeFunctions8XY4,
    OP_CodeFunctions8XY5,
    OP_CodeFunctions8XY6,
    OP_CodeFunctions8XY7,
    OP_CodeFunctions8XYE,
    OP_CodeFunctions9XY0,
    OP_CodeFunctionsANNN,
    OP_CodeFunctionsBNNN,
    OP_CodeFunctionsCXNN,
    OP_CodeFunctionsDXYN,
    OP_CodeFunctionsEX9E,
    OP_CodeFunctionsEXA1,
    OP_CodeFunctionsFX07,
    OP_CodeFunctionsFX0A,
    OP_CodeFunctionsFX15,
    OP_CodeFunctionsFX18,
    OP_CodeFunctionsFX1E,
    OP_CodeFunctionsFX29,
    OP_CodeFunctionsFX33,
    OP_CodeFunctionsFX55,
    OP_CodeFunctionsFX65
};
// Function CCCCCCCCCCCCCCCCCCCCCC

void emulateCycle(Chip8 * emu) {
    // Fetch Opcode
    emu -> opcode = emu -> memoryC8[emu -> pc] << 8 | emu -> memoryC8[emu -> pc + 1];
    // Decode Opcode
    switch (emu -> opcode & 0xF000) { //remove all but the first hex
    case 0x0000:
        switch (emu -> opcode & 0x00FF) { //remove all but the first hex
        case 0x00EE:
            OP_CodeFunctions[2](emu);
            break;
        case 0x00E0:
            OP_CodeFunctions[1](emu);
            break;
        case 0x0000:
            OP_CodeFunctions[0](emu);
            break;
        default: 
            break;
        }
        break;
    case 0xA000: //ANNN  set I to adress ANNN
        OP_CodeFunctions[20](emu);
        break;
    case 0xB000: //ANNN  jumps to adress NNN + V0
        OP_CodeFunctions[21](emu);
        break;
    case 0xC000:
        OP_CodeFunctions[22](emu);
        break;
    case 0xD000:
        // DXYYYNNNN
        OP_CodeFunctions[23](emu);
        break;
    case 0xE000:
        switch (emu -> opcode & 0x00FF) { //remove all but the first hex     
        case 0x009E:
            OP_CodeFunctions[24](emu);
            break;
        case 0x00A1:
            OP_CodeFunctions[25](emu);
            break;
        default:
            break;
        }
        break;
    case 0xF000:
        switch (emu -> opcode & 0xF0FF) { //remove all but the first hex

        case 0xF007:
            OP_CodeFunctions[26](emu);
            break;
        case 0xF00A:
            OP_CodeFunctions[27](emu);
            break;
        case 0xF015:
            OP_CodeFunctions[28](emu);
            break;
        case 0xF018:
            OP_CodeFunctions[29](emu);
            break;
        case 0xF01E:
            OP_CodeFunctions[30](emu);
            break;
        case 0xF029:
            OP_CodeFunctions[31](emu);
            break;
        case 0xF033:
            OP_CodeFunctions[32](emu);
            break;
        case 0xF055:
            OP_CodeFunctions[33](emu);
            break;
        case 0xF065:
            OP_CodeFunctions[34](emu);
            break;

        default:
            break;
        }
        break;

   case 0x1000:
        OP_CodeFunctions[3](emu);
        break;
    case 0x2000:
        OP_CodeFunctions[4](emu);
        break;
    case 0x3000:
        OP_CodeFunctions[5](emu);
        break;
    case 0x4000:
        OP_CodeFunctions[6](emu);
        break;
    case 0x5000:
        OP_CodeFunctions[7](emu);
        break;
    case 0x6000:
        OP_CodeFunctions[8](emu);
        break;
    case 0x7000:
        OP_CodeFunctions[9](emu);
        break; /**/
    case 0x8000:
        switch (emu -> opcode & 0xF00F) { //remove all but the first hex
        case 0x8000:
            OP_CodeFunctions[10](emu);
            break;
        case 0x8001:
            OP_CodeFunctions[11](emu);
            break;
        case 0x8002:
            OP_CodeFunctions[12](emu);
            break;
        case 0x8003:
            OP_CodeFunctions[13](emu);
            break;
        case 0x8004:
            OP_CodeFunctions[14](emu);
            break;
        case 0x8005:
            OP_CodeFunctions[15](emu);
            break;
        case 0x8006:
            OP_CodeFunctions[16](emu);
            break;
        case 0x8007:
            OP_CodeFunctions[17](emu);
            break;
        case 0x800E:
            OP_CodeFunctions[18](emu);
            break;
        default:
            break;
        }
        break;
    case 0x9000:
        OP_CodeFunctions[19](emu);
        break;
    default:
        break;
    }

    // Update timers
    if (emu -> delay_timer > 0)
        --emu -> delay_timer;

    if (emu -> sound_timer > 0) {
        if (emu -> sound_timer == 1)
            printf("BEEP!\n");
        --emu -> sound_timer;
    }

 }

void loadGame(Chip8 * emu, char * filename) {
    FILE * game = fopen(filename, "rb");
    if (game == NULL) {
        perror("Error opening file");
    }
    unsigned int file_size;
    // Determine file size in octets
    fseek(game, 0, SEEK_END);
    file_size = ftell(game);
    fseek(game, 0, SEEK_SET);

    unsigned char * buffer = (unsigned char * ) malloc(sizeof(unsigned char) *file_size);

    size_t bytes_read = fread(buffer, 1, file_size, game);

    fscanf(game, "%s", buffer);
    if((4096-512) > file_size)
	{
	
    for (int i = 0; i < file_size; ++i) {
        emu -> memoryC8[i + 512] = buffer[i];
    }
    }
    else
		{printf("Error: ROM too big for memory");
         }
	

    free(buffer);
    fclose(game);
}
