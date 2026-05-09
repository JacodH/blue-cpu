const fs = require('fs');

var asm_file_path = process.cwd()+'/programs/'+process.argv[2];

console.log(`Assembling: ${asm_file_path}`);

var OP_CODES = {
    // Memory
    "SET": "0x01",
    "MOV": "0x02",
    "CLR": "0x03",
    "GET": "0x04",
    "STR": "0x05",

    // Arithmetic
    "ADD": "0xa1",
    "SUB": "0xa2",
    "MUL": "0xa3",
    "DIV": "0xa4",
    "SMUL": "0xa5",
    "SDIV": "0xa6",
    "IADD": "0xa7",
    "ISUB": "0xa8",

    // Disk
    "DREAD": "0xd1",

    // Control
    "HLT": "0xc0",
    "NOP": "0xc1",

    // Emulation
    "OUT": "0xe1",
    "SOUT": "0xe2",
}

var REGISTERS = {
    "r0": "0x00",
    "r1": "0x01",
    "r2": "0x02",
    "r3": "0x03",
    "r4": "0x04",
    "r5": "0x05",
    "r6": "0x06",
    "r7": "0x07",
    "r8": "0x08",
    "r9": "0x09",
    "r10": "0x0A",
    "r11": "0x0B",
    "r12": "0x0C",
    "r13": "0x0D",
    "r14": "0x0E",
    "r15": "0x0F",
}

function int_to_hex(int) {
    let val = parseInt(int);
    return "0x"+val.toString(16).padStart(2, "0");
}

function operand_to_hex(operand, labels, i) {
    // Empty operands are just 0x00
    if (operand == undefined) {return "0x00"};

    // Registers
    if (operand.startsWith('r') == true) {
        return REGISTERS[operand];
    }

    // Hex immediate
    if (operand.startsWith('0x')) {
        if (operand.length == 6) {
            // operand is word
            let low = parseInt(operand, 16) & 0xFF;
            let high = (parseInt(operand, 16) >> 8) & 0xFF;

            low = int_to_hex(low);
            high = int_to_hex(high);

            return [low, high]
        }else {
            // operand is byte
            return operand;
        }
    }

    // Labels turn into their address
    if (operand.startsWith('$')) {
        operand = operand.slice(1);
        let addr = labels[operand];

        return int_to_hex(addr);
    }

    // Must be decimal immediate 
    let val = parseInt(operand);
    if (val < 0 || val > 255) {
        if (i < 3) {
            // treat as word 
            val = val & 0xFFFF;
            let low = int_to_hex(val & 0xFF);
            let high = int_to_hex((val >> 8) & 0xFF);
            return [low, high];
        }else {
            // treat as byte 
            val = val & 0xFF;
            return int_to_hex(val);
        }
    }
    return int_to_hex(operand);
}

fs.readFile(asm_file_path, 'utf8', (err, data) => {
    console.log(data);
    // 1. Clean lines
    var clean_lines = data.split("\r\n");
    for (let i = 0; i < clean_lines.length; i++) {
        clean_lines[i] = clean_lines[i].split(';')[0].trimStart();
        if (clean_lines[i] == "") {
            clean_lines.splice(i, 1);
            i--;
        }
    }
    
    console.log("\n1. Cleaned lines: ");
    // for (line of clean_lines) {console.log(line)};
    
    // 2. Store label addresses 
    var addr = 0;
    var labels = {};
    
    for (line of clean_lines) {
        if (line[line.length-1] == ":") {
            line = line.slice(0, -1);
            labels[line] = addr
        }else {
            addr += 4;
            console.log(int_to_hex(addr)+" | "+line);
        }
    }
    console.log("\n2. Found labels: ");
    console.log(labels);

    // 3. Final, convert everything to hex
    var hex = "";
    for (line of clean_lines) {
        // Skip over labels
        if (line[line.length-1] == ":") { continue };
        
        // For functions, we need to remove the indentation
        line = line.trimStart();
        
        let operands = line.split(" ");
        // console.log(operands);


        // Convert opcode to hex
        hex += OP_CODES[operands[0]]+", ";
        for (let i = 1; i < 4; i++) {
            // check 
            let output = operand_to_hex(operands[i], labels, i)
            if (Array.isArray(output)) {
                // We tried to parse a word, it returns 2 bytes
                hex += output[0]+", ";
                hex += output[1]+", ";
                i++;
            }else {
                hex += output+", ";
            }
        }
        hex += "\n";
    }

    console.log("\n3. Convert to hex: ");
    console.log(hex);

    console.log(`Size: ${addr}/512 Bytes, ${((addr/512)*100).toFixed()}%`);
    console.log(`Blocks: ${Math.ceil(addr/512)}`);
});
