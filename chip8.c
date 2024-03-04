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

void OP_CodeFunctions1NNN(Chip8* emu) {
  
};
void OP_CodeFunctions2NNN(Chip8* emu) {

};
void OP_CodeFunctions3XNN(Chip8* emu) {

};
void OP_CodeFunctions4XNN(Chip8* emu) {

};

void OP_CodeFunctionsANNN(Chip8* emu) {
   emu.I = emu.opcode & 0x0FFF;
   emu.pc += 2;
   return 0;

};
void OP_CodeFunctionsBNNN(Chip8* emu) {
   emu.pc = register[0] +0x0FFF ;
   return 0;
};
void OP_CodeFunctionsCXNN(Chip8* emu) {
     //Get only X an then Shift 
    unsigned int Xval = (opcode & 0x0F00)>> 8
    register[Xval] = rand() & 0x00FF;
};
void OP_CodeFunctionsDXYN(Chip8* emu) {
     //Get only X an then Shift 
    unsigned int Xval = (opcode & 0x0F00)>> 8
     //Get only Y an then Shift 
    unsigned int Yval = (opcode & 0x00F0)>> 4
    register[Xval] = rand() & 0x00FF;
      
};
void (*grade_functions[])(Chip8* emu) =
{OP_CodeFunctions1NNN,OP_CodeFunctions2NNN, OP_CodeFunctions3XNN, OP_CodeFunctions4XNN
,OP_CodeFunctionsANNN,OP_CodeFunctionsBNNN,OP_CodeFunctionsCXNN,P_CodeFunctionsDXNN
,
,
,
,};
// Function CCCCCCCCCCCCCCCCCCCCCC


void emulateCycle(Chip8* emu){
  // Fetch Opcode
  opcode = memoryc8[pc] << 8 | memoryc8[pc + 1];

  // Decode Opcode
  switch(opcode & 0xF000 ) { //remove all but the first hex
    case 0xA000: //ANNN  set I to adress ANNN
      // Execute Opcode
        OP_CodeFunctions[index]();
       
          break;
     case 0xB000://ANNN  jumps to adress NNN + V0
      // Execute Opcode
        I = opcode & 0x0FFF;
          pc += 2;
          break;
     case 0xA000:
      // Execute Opcode
        I = opcode & 0x0FFF;
          pc += 2;
          break;
     case 0xA000:
      // Execute Opcode
        I = opcode & 0x0FFF;
          pc += 2;
          break;
    


     case:
     break;
  }
  // Update timers
  // Update timers
  if(delay_timer > 0)
    --delay_timer;
 
  if(sound_timer > 0)
  {
    if(sound_timer == 1)
      printf("BEEP!\n");
    --sound_timer;
}
}

 ;



void loadGame(Chip8* emu, char * filename, FILE* game){
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
