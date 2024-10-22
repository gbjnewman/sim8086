const fs = require('fs');
const buffer = fs.readFileSync('listing_0039_more_movs');

const instructions = {
  0b100010: 'mov',  //register/memory to/from register
  0b1100011: 'mov', //immediate to register/memory
  0b1011: 'mov',    //immediate to register
  0b1010000: 'mov', //memory to accumulator
  0b1010001: 'mov', //accumulator to memory
};

const regFieldw0 = {
  0b000: 'al',
  0b001: 'cl',
  0b010: 'dl',
  0b011: 'bl',
  0b100: 'ah',
  0b101: 'ch',
  0b110: 'dh',
  0b111: 'bh',
}

const regFieldw1 = {
  0b000: 'ax',
  0b001: 'cx',
  0b010: 'dx',
  0b011: 'bx',
  0b100: 'sp',
  0b101: 'bp',
  0b110: 'si',
  0b111: 'di',
}

const rmField = {
  0b000: 'bx + si',
  0b001: 'bx + di',
  0b010: 'bp + si',
  0b011: 'bp + di',
  0b100: 'si',
  0b101: 'di',
  0b110: 'bp',
  0b111: 'bx',
}

console.log('bits 16\n');

let i = 0;
while (i < buffer.length) {
  let firstByte = buffer[i];

  if (firstByte>>4===0b1011) {    //immediate to register move
    let secondByte = buffer[i+1];
    let instructionOutput = `${instructions[0b1111&(firstByte>>4)]}`;
    let reg = 0b111&(firstByte);
    let wSet = 0b1&(firstByte>>3);
    let regOutput = wSet===0b1 ? `${regFieldw1[reg]}` : `${regFieldw0[reg]}`;
    let immediateOutput = '';

    if (wSet === 0b0) {   // if 8bit
      immediateOutput = secondByte;
      i = i + 2;
    } else if (wSet === 0b1) {    //if 16bit
      let thirdByte = buffer[i+2];
      immediateOutput = (thirdByte<<8)|secondByte;
      i = i + 3;
    }

    console.log(`${instructionOutput} ${regOutput}, ${immediateOutput}`);

  } else if (firstByte>>2===0b100010) {   //  move register/memory to/from register
    let secondByte = buffer[i+1];
    let instructionOutput = `${instructions[0b111111&(firstByte>>2)]}`;
    let dSet = 0b1&(firstByte>>1);
    let wSet = 0b1&(firstByte);
    let mod = 0b11&(secondByte>>6);
    let reg = 0b111&(secondByte>>3);
    let regMem = 0b111&(secondByte);
    let regOutput = '';
    let rmOutput = '';

    // modes for register/memory to/from register
    if (mod===0b00 && regMem===0b110) {   // special case for mode 00
      console.log(' mod is 00 and regmem is 110')
      i = i + 4

    } else if (mod===0b00) {    // mode 00 memory mode, no displacement(spec case if rm110
      regOutput = wSet===0b1 ? `${regFieldw1[reg]}` : `${regFieldw0[reg]}`;
      rmOutput = `[${rmField[regMem]}]`;
      i = i + 2;

    } else if (mod===0b01) {    // mode 01 memory mode, 8 bit displacement
      let thirdByte = buffer[i+2];
      regOutput = wSet===0b1 ? `${regFieldw1[reg]}` : `${regFieldw0[reg]}`;
      rmOutput = (thirdByte===0b0) ? `[${rmField[regMem]}]` : `[${rmField[regMem]} + ${thirdByte}]`;
      i = i + 3;

    } else if (mod===0b10) {    // mode 10 memory mode, 16 bit displacement
      let thirdByte = buffer[i+2];
      let fourthByte = buffer[i+3];
      regOutput = wSet===0b1 ? `${regFieldw1[reg]}` : `${regFieldw0[reg]}`;
      if (thirdByte===0b0 && fourthByte===0b0) {
      rmOutput = `[${rmField[regMem]}]`;
      } else {
      addressLoc = (fourthByte<<8)|thirdByte
      rmOutput = `[${rmField[regMem]} + ${addressLoc}]`;
      }
      i = i +4;

    } else if (mod===0b11) {    // mode 11 register mode, no displacement
      regOutput = wSet===0b1 ? `${regFieldw1[reg]}` : `${regFieldw0[reg]}`;
      rmOutput = wSet===0b1 ? `${regFieldw1[regMem]}` : `${regFieldw0[regMem]}`;
      i = i + 2

    }

    console.log((dSet===0b1) ? `${instructionOutput} ${regOutput}, ${rmOutput}` : `${instructionOutput} ${rmOutput}, ${regOutput}`);

  } /*else if (firstByte>>1===0b1100011) {
  } else if (firstByte>>1===0b1010000) {
  } else if (firstByte>>1===0b1010001) {
  }*/
}
