#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

struct chip8 {
unsigned short opcode; // 35 opcode so need 2bytes in hex
unsigned char memoryc8[8192]; // memoryc8 chip8 locations of 1bytes * 2
// 512 first bytesare occupied 0x200 
//Up to 1FFF in our case with 8192 bytes
//Raise it to a var holding 8 bytes ?
// (0xF00-0xFFF)  Display refresh
// the 96 bytes below that (0xEA0-0xEFF) were reserved for callstackC8
/*

 These machines had 4096 (0x1000) memoryc8 locations, all of which are 8 bits (a byte) which is where the term CHIP-8 originated. 
First 512 bytes occupied.  From (0x200) and do not access any of the memoryc8 below the location 512 (0x200) 0100 0000 0000.
The uppermost 256 bytes (0xF00-0xFFF) are reserved for display refresh, and the 96 bytes below that (0xEA0-0xEFF) were reserved for the call stackC8, internal use, and other variables.

0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM*/
unsigned char Vreg[32]; //Data register V0to VF *2
unsigned short I; // Index
unsigned short pc; // Programm counter
unsigned short stackC8[32]; // stackC8
unsigned short* sp; // stackC8pointer ?

unsigned char gfx[64 * 32];// Graphic chips, array double
unsigned char gfx2[64][32];// Graphic chips, array double

unsigned char delay_timer; // Dekat timer
unsigned char sound_timer;// Sound timer

unsigned char keystate[32];
} 

typedef struct chip8 Chip8 ;

void initialize(Chip8* emu){
  pc     = 0x200;  // Program counter starts at 0x200
  opcode = 0;      // Reset current opcode	
  I      = 0;      // Reset index register
  sp     = 0;      // Reset stackC8 pointer

  // Clear display	
      memset(emu.gfx, 0, sizeof(emu.gfx));
  // Clear stackC8
      memset(emu.stackC8, 0, sizeof(emu.stackC8));
  // Clear registers V0-VF
      memset(emu.Vreg, 0, sizeof(emu.Vreg));
  // Clear memoryc8
      memset(emu.memoryc8, 0, sizeof(emu.memoryc8));

  // Load fontset
  for(int i = 0; i < 80; ++i){
  // (0x000to 0x04F)  79 adresslocations
     memoryc8[i] = chip8_fontset[i];
    }
}

// Function CCCCCCCCCCCCCCCCCCCCCC

void OP_CodeFunctions0NNN(Chip8* emu){ 
    pc +=2; //Useless only here for the count
};
void OP_CodeFunctions00EN(Chip8* emu){ 
  disp_clear();
    pc +=2; 
   return 0;
};
void OP_CodeFunctions00EE(Chip8* emu) { 
  pc +=2; //Useless only here for the count
  return 0; 
};

void OP_CodeFunctions1NNN(Chip8* emu) {
  emu.stack[emu.sp] = emu.pc;
  ++emu.sp;
  emu.pc = emu.opcode & 0x0FFF;
};
void OP_CodeFunctions2NNN(Chip8* emu) {
  emu.stack[sp] = pc;
  ++emu.sp;
  emu.pc = emu.opcode & 0x0FFF;
};
void OP_CodeFunctions3XNN(Chip8* emu) {
     //Get only X an then Shift 
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    if(emu.Vreg[Xval] == emu.opcode & 0x00FF; ) {
           emu.pc += 2;
    }
    emu.pc += 2;
};
void OP_CodeFunctions4XNN(Chip8* emu) {
     //Get only X an then Shift 
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    if(emu.Vreg[Xval] != emu.opcode & 0x00FF; ) {
           emu.pc += 2;
    }
     emu.pc += 2;
};

void OP_CodeFunctions5XY0(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
    if(emu.Vreg[Xval] ==  emu.Vreg[Yval]; ) {
           emu.pc += 2;
    }
     emu.pc += 2;
};

void OP_CodeFunctions6XNN(Chip8* emu) {
    emu.Vreg[Xval]= (emu.opcode & 0x00FF);   
     emu.pc += 2;
};

void OP_CodeFunctions7XNN(Chip8* emu) {
    emu.Vreg[Xval] += (emu.opcode & 0x00FF);   
    emu.pc += 2;
};

void OP_CodeFunctions8XY0(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
     emu.Vreg[Xval] =  emu.Vreg[Yval]; 
     emu.pc += 2;
};

void OP_CodeFunctions8XY1(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
     emu.Vreg[Xval] |=  emu.Vreg[Yval]; 
     emu.pc += 2;
};
 
void OP_CodeFunctions8XY2(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
     emu.Vreg[Xval] &=  emu.Vreg[Yval]; 
     emu.pc += 2;
};
 
void OP_CodeFunctions8XY3(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
     emu.Vreg[Xval] ^=  emu.Vreg[Yval]; 
     emu.pc += 2;
};
 
void OP_CodeFunctions8XY4(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
    //VF setting for overflow  emu.Vreg[15]   done
     if( emu.Vreg[Yval] > (0xFF -  emu.Vreg[Xval])){
        emu.Vreg[0xF] = 1; // carry
       }
  else{
     emu.Vreg[0xF] = 0;
  }
    emu.Vreg[Xval] +=  emu.Vreg[Yval]; 
    emu.pc += 2;
    
   pc += 2;    
};
 
void OP_CodeFunctions8XY5(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4;
        //VF setting for underflow  emu.Vreg[15] not done
     emu.Vreg[Xval] -=  emu.Vreg[Yval]; 
     emu.pc += 2;
};
 
void OP_CodeFunctions8XY6(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
        //VF   not done
     emu.Vreg[Xval] >>= 1; 
     emu.pc += 2;
};

void OP_CodeFunctions8XY7(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
        //VF   not done
     emu.Vreg[Xval] = emu.Vreg[Yval]-emu.Vreg[Xval] ; 
     emu.pc += 2;
};
 
void OP_CodeFunctions8XYE(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
    //VF   not done
    emu.Vreg[Xval] <<= 1; 
    emu.pc += 2;

};
 

 
 void OP_CodeFunctions9XY0(Chip8* emu) {
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    unsigned int Yval = (emu.opcode & 0x00F0)>> 4; 
    if(emu.Vreg[Xval] !=  emu.Vreg[Yval]; ) {
           emu.pc += 2;
    }
     emu.pc += 2;

};
 

void OP_CodeFunctionsANNN(Chip8* emu) {
   emu.I = emu.opcode & 0x0FFF;
   emu.pc += 2;
   return 0;

};
void OP_CodeFunctionsBNNN(Chip8* emu) {
   emu.pc = emu.Vreg[0] +0x0FFF ;
    emu.pc += 2;
   return 0;
};
void OP_CodeFunctionsCXNN(Chip8* emu) {
     //Get only X an then Shift 
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    emu.Vreg[Xval] = rand() & 0x00FF;
    emu.pc += 2;

};

//REVOIR
void OP_CodeFunctionsDXYN(Chip8* emu) {
     //Get only X an then Shift 
    unsigned short  Xval = (emu.opcode & 0x0F00)>> 8;
     //Get only Y an then Shift 
    unsigned short  Yval = (emu.opcode & 0x00F0)>> 4;
    unsigned short  Nval = (emu.opcode & 0x000F);

    emu.Vreg[15] = 0;

       for(int y = 0; y < Nval; ++y){
        pixel = emu.memory[I + y];
          for(int x = 0; x < 8; ++x){
            if((pixel & (0x80 >> x)) != 0)//Inverted
       {
         if(emu.gfx[(Xval + x + ((Yval + y) * 64))] == 1){
          emu.Vreg[15] = 1;                        }         
       emu.gfx[(Xval + x + ((Yval + y) * 64))]  ^= 1;
      }
     }
    }
    emu.Vreg[31] = 1;
    emu.pc += 2;      
            //VF   not done

};

void OP_CodeFunctionsEX9E(Chip8* emu){
   unsigned short Xval = (emu.opcode & 0x0F00)>> 8;
  if(emu.keystate[V[Xval]] == 0){
         pc += 2;
  }
      pc += 2;
      return 0;
  
};
void OP_CodeFunctionsEXA1(Chip8* emu){
    unsigned short Xval = (emu.opcode & 0x0F00)>> 8;
  if(emu.keystate[V[Xval]] != 0){
         pc += 2;
  }
      pc += 2;
      return 0;
  
};
void OP_CodeFunctionsFX07(Chip8* emu){
    unsigned short Xval = (emu.opcode & 0x0F00)>> 8;
    emu.Vreg[Xval] = emu.delay_timer;  
    emu.pc += 2;  
};
void OP_CodeFunctionsFX0A(Chip8* emu){
    unsigned short Xval = (emu.opcode & 0x0F00)>> 8;
    emu.Vreg[Xval] = get_key(emu); //get_key M.I.A
    emu.pc += 2; 
  
};
void OP_CodeFunctionsFX15(Chip8* emu){
   unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    emu.delay_timer = emu.Vreg[Xval]  ;
    emu.pc += 2;
  
};
void OP_CodeFunctionsFX18(Chip8* emu){
   unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    emu.sound_timer = emu.Vreg[Xval]  ;
    emu.pc += 2;
  
};
void OP_CodeFunctionsFX1E(Chip8* emu){
     unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    emu.I += emu.Vreg[Xval]  ;
    emu.pc += 2;        //VF   not done

};

void OP_CodeFunctionsFX29(Chip8* emu){
  unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
    emu.I += sprite_addr[emu, Xval]  ;
    emu.pc += 2;
};
void OP_CodeFunctionsFX33(Chip8* emu){
  unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
   emu.memory[I]     = emu.Vreg[Xval] / 100;
  emu.memory[I + 1] = (emu.Vreg[Xval] / 10) % 10;
  emu.memory[I + 2] = (emu.Vreg[Xval] % 100) % 10;
  pc += 2;

};
void OP_CodeFunctionsFX55(Chip8* emu){
     unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
   for(int i = 0; i <= Xval; ++i){
        emu.memory[i + emu.I] =  emu.Vreg[i]  ;
    }
};
void OP_CodeFunctionsFX65(Chip8* emu){
    unsigned int Xval = (emu.opcode & 0x0F00)>> 8;
   for(int i = 0; i <= Xval; ++i){
      emu.Vreg[i] = emu.memory[i + emu.I]   ;
    }
};

void (*grade_functions[])(Chip8* emu) =
{OP_CodeFunctions0NNN,OP_CodeFunctions00EN,OP_CodeFunctions00EE,
OP_CodeFunctions1NNN,OP_CodeFunctions2NNN,OP_CodeFunctions3XNN,OP_CodeFunctions4XNN,
OP_CodeFunctions5XY0,OP_CodeFunctions6XNN,OP_CodeFunctions7XNN,
OP_CodeFunctions8XY0,OP_CodeFunctions8XY1,OP_CodeFunctions8XY2,OP_CodeFunctions8XY3,OP_CodeFunctions8XY4,
OP_CodeFunctions8XY5,OP_CodeFunctions8XY6,OP_CodeFunctions8XY7,OP_CodeFunctions8XYE,OP_CodeFunctions9XY0,
OP_CodeFunctionsANNN,OP_CodeFunctionsBNNN,OP_CodeFunctionsCXNN,OP_CodeFunctionsDXYN,
OP_CodeFunctionsEX9E,OP_CodeFunctionsEXA1,
OP_CodeFunctionsFX07,OP_CodeFunctionsFX0A,OP_CodeFunctionsFX15,OP_CodeFunctionsFX18,OP_CodeFunctionsFX1E,
OP_CodeFunctionsFX29,OP_CodeFunctionsFX33,OP_CodeFunctionsFX55,OP_CodeFunctionsFX65
};
// Function CCCCCCCCCCCCCCCCCCCCCC

 
void emulateCycle(Chip8 * emu) {
    // Fetch Opcode
    opcode = memoryc8[pc] << 8 | memoryc8[pc + 1];

    // Decode Opcode
    switch (opcode & 0xF000) { //remove all but the first hex
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
        break;
     case 0x9000:
        OP_CodeFunctions[19](emu);
        break;
    default:
      break;
    }

    switch (opcode & 0xF0FF) { //remove all but the first hex
     case 0x00EE:  
             OP_CodeFunctions[0](emu);
        break;
      case 0x00E0:  
              OP_CodeFunctions[1](emu);
        break;    
    case 0x0000:
            OP_CodeFunctions[2](emu);
        break;
        //KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKEYYSS
     case 0xE09E:
            OP_CodeFunctions[24](emu);
        break;
     case 0xE0A1:
            OP_CodeFunctions[25](emu);
        break;
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



    
     switch (opcode & 0xF00F) { //remove all but the first hex
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

    // Update timers
    if (emu.delay_timer > 0)
        --delay_timer;

    if (emu.sound_timer > 0) {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

 



void loadGame(Chip8* emu, char* filename, FILE* game){
    game = fopen(filename, "rb");
   if (game == NULL) {
        perror("Error opening file");
        return 1;
    }
    unsigned int = file_size;
    // Determine file size in octets
    fseek(game, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(game, 0, SEEK_SET);

    unsigned char* buffer = (unsigned char *)malloc(file_size);

    size_t bytes_read = fread(buffer, 1, file_size, game);

    fscanf(fp, "%s", buffer); 
    for(int i = 0; i < bufferSize; ++i){
      emu.memory[i + 512] = buffer[i];
    }

    free(buffer);


}
fclose(fp); // Close the file

