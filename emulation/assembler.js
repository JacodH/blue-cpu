const fs = require('fs');

var asm_file_path = 'programs/asm/'+process.argv[2];
var out_file_path = 'programs/hex/'+process.argv[2].split('.')[0]+'.hex';

// Opcode lookup table
var opcode_lookup_table = {
    // memory
    "SET": {code: "0x01", operands: ["reg", "word"]},
    "MOV": {code: "0x02", operands: ["reg", "reg"]},
    "CLR": {code: "0x03", operands: ["reg"]},
    "GET": {code: "0x04", operands: ["reg", "reg", "byte"]},
    "STR": {code: "0x05", operands: ["reg", "reg", "byte"]},
    "GETB": {code: "0x06", operands: ["reg", "reg", "byte"]},
    "STRB": {code: "0x07", operands: ["reg", "reg", "byte"]},

    // disk
    "DREAD": {code: "0xd1", operands: ["reg", "reg"]},
    "DREAD": {code: "0xd1", operands: ["reg", "reg"]},

    // arithmetic 
    "ADD": {code: "0xa1", operands: ["reg", "reg", "reg"]},
    "SUB": {code: "0xa2", operands: ["reg", "reg", "reg"]},
    "MUL": {code: "0xa3", operands: ["reg", "reg", "reg"]},
    "DIV": {code: "0xa4", operands: ["reg", "reg", "reg"]},
    "SMUL": {code: "0xa5", operands: ["reg", "reg", "reg"]},
    "SDIV": {code: "0xa6", operands: ["reg", "reg", "reg"]},
    "IADD": {code: "0xa7", operands: ["reg", "reg", "byte"]},
    "ISUB": {code: "0xa8", operands: ["reg", "reg", "byte"]},

    // comparison 
    "EQ": {code: "0xb1", operands: ["reg", "reg", "reg"]},
    "NE": {code: "0xb2", operands: ["reg", "reg", "reg"]},
    "LT": {code: "0xb3", operands: ["reg", "reg", "reg"]},
    "GT": {code: "0xb4", operands: ["reg", "reg", "reg"]},
    "LTS": {code: "0xb5", operands: ["reg", "reg", "reg"]},
    "GTS": {code: "0xb6", operands: ["reg", "reg", "reg"]},

    // bitwise
    "AND": {code: "0x81", operands: ["reg", "reg", "reg"]},
    "XOR": {code: "0x82", operands: ["reg", "reg", "reg"]},
    "LSH": {code: "0x83", operands: ["reg", "reg", "reg"]},
    "RSH": {code: "0x84", operands: ["reg", "reg", "reg"]},

    // control 
    "HLT": {code: "0xc0", operands: []},
    "NOP": {code: "0xc1", operands: []},
    "JMP": {code: "0xc2", operands: ["reg"]},
    "IJMP": {code: "0xc3", operands: ["word"]},
    "JIT": {code: "0xc4", operands: ["reg", "reg"]},
    "JIF": {code: "0xc5", operands: ["reg", "reg"]},
    "IJIT": {code: "0xc6", operands: ["reg", "word"]},
    "IJIF": {code: "0xc7", operands: ["reg", "word"]},

    // emulator
    "OUT": {code: "0xe1", operands: ["reg"]},

    // stack
    "CALL": {code: "0xf3", operands: ["word"]},
    "RET": {code: "0xf4", operands: []},

    // "   ": "0x  ",
}

var register_lookup_table = {
    "r0":  "0x00",
    "r1":  "0x01",
    "r2":  "0x02",
    "r3":  "0x03",
    "r4":  "0x04",
    "r5":  "0x05",
    "r6":  "0x06",
    "r7":  "0x07",
    "r8":  "0x08",
    "r9":  "0x09",
    "r10": "0x0a",
    "r11": "0x0b",
    "r12": "0x0c",
    "r13": "0x0d",
    "r14": "0x0e",
    "r15": "0x0f",
}

console.log("");

console.log("Input: "+asm_file_path);
console.log("Output: "+out_file_path);

console.log("");

fs.readFile(asm_file_path, 'utf8', (err, data) => {
    if (err) {console.error(err)};

    // this array will be filled with the final hex values of the asm to be sent to the output file
    var output_hex = [];

    // So apparently windows has both a \r and \n terminator at the end of each line
    // Linux only uses \n
    // Mac typically uses 

    // BUG: This might cause issues when this code is ran on a linux / mac machine. 
    var lines = data.split("\r\n"); 

    // Remove white space and comments
    console.log("Removing white space and comments.");
    for (let i = 0; i < lines.length; i++) {
        // remove white space
        lines[i] = lines[i].trim();

        // if line has a comment
        if (lines[i].includes(";")) {
            // remove the content after the comment start
            lines[i] = lines[i].split(";")[0];
        }
    }

    // Remove all "" strings from lines array
    lines = lines.filter((line) => {
        return line;
    });
    console.log(lines);

    // Now we have a clean array where each index is a clean line. 
    // This array still has the . section starters.
    // It also has labels for functions and data

    // For text labels, I really just need to find the address the point to and store that in an array. 
    // For data labels, I really just need to know how many bytes belong to each label? 

    console.log("");
    console.log("Finding labels and corresponding addresses...");

    var current_directive = "None";
    var address = 0;

    var function_labels = [];
    var data_labels = [];

    // this array will be filled with all of the bytes in the .text section
    var text_hex = [];
    // this array will be filled with all of the bytes in the .data section
    var data_hex = [];
    // this array will be their sums, text then data sections
    var final_hex = [];

    for (let i = 0; i < lines.length; i++) {
        // this line is a section directive 
        if (lines[i][0] == ".") {
            current_directive = lines[i];
            lines.splice(i, 1);
            i--;
            continue; // skip to next line
        }

        // this line is a label
        if (lines[i].includes(":")) {
            if (current_directive == ".text") {
                // function label
                lines[i] = lines[i].replace(":", "");
                console.log(`Found function label '${lines[i]}'`);
                function_labels.push([lines[i], immediateToHex(address)]);
            }

            if (current_directive == ".data") {
                // data label
                lines[i] = lines[i].replace(":", "");

                // see if this has a multiplier (*)
                if (lines[i].includes("*")) {
                    // line has a multiplier 
                    let split = lines[i].split(' ');
                    
                    // find multiplier value 
                    let multiplier_value = split[split.indexOf("*")+1]
                    let multiplied = split[split.indexOf("*")-1]
                    
                    console.log(`Found RAM pointer data label '${lines[i].split(' ')[0]}' pointing to ${multiplier_value} bytes`);

                    for (let i = 0; i < multiplier_value; i++) {
                        data_hex.push(multiplied);
                    }

                    data_labels.push([lines[i].split(' ')[0], parseInt(multiplier_value)]);
                }else {
                    console.log(`Found RAM pointer data label '${lines[i].split(' ')[0]}' pointing to ${lines[i].split(' ').length-1} bytes`);
    
                    console.log([lines[i].split(' ')[0], lines[i].split(' ').length-1]);
                    data_labels.push([lines[i].split(' ')[0], lines[i].split(' ').length-1]);
    
                    console.log(...lines[i].split(' ').splice(1, lines[i].split(' ').length-1));
                    data_hex.push(...lines[i].split(' ').splice(1, lines[i].split(' ').length-1));
                }
                
            }

            lines.splice(i, 1);
            i--;

            continue; // skip to next line
        }

        address += 4;
    }

    // Calculate data label addresses 
    let offset = address;
    for (let i = 0; i < data_labels.length; i++) {
        offset += data_labels[i][1]
        data_labels[i][1] = immediateToHex(offset-data_labels[i][1]);
    }
    
    console.log("");
    
    
    console.log("Function labels: ");
    console.log(function_labels);
    console.log("");
    
    console.log("Data labels: ");
    console.log(data_labels);
    console.log("");
    
    console.log("Cleaned code: ");
    for (let i = 0; i < lines.length; i++) {
        console.log(`${printLine(i*4, i)} ${lines[i]}`);
    }
    console.log("");
    console.log("Data hex: ");
    for (let i = 1; i < data_hex.length+1; i++) {
        process.stdout.write(data_hex[i-1]+" ");
        if (i % 4 == 0){process.stdout.write("\n")};
    }
    console.log("");
    console.log("");
    
    // Replace opcodes with hex
    console.log("Replacing opcodes, operands and labels with hex...");

    for (let i = 0; i < lines.length; i++) {
        lines[i] = lines[i].split(" ");
        
        // Opcode data
        let opcode_data = opcode_lookup_table[lines[i][0]];

        // debug printout for matching expected types
        for (let j = 0; j < 4; j++) {
            if (lines[i][j] == undefined) {continue;};
            process.stdout.write(`${lines[i][j].padEnd(4, " ")} `)
            if (j > 0) {
                process.stdout.write(`[${opcode_data.operands[j-1]}] `)
            }
        }

        // Replace opcode
        lines[i][0] = opcode_data.code;
        
        // replace operands 
        // by looping through them
        // and by looking at our table at the top 
        // to see what type if data the operand takes
        for (let j = 1; j < 4; j++) {
            let operand_type = opcode_data.operands[j-1];

            if (operand_type == "reg") {
                // simple replace if the operand is register 
                lines[i][j] = register_lookup_table[lines[i][j]];
            }else if (operand_type == "byte") {
                // bytes only store immediate values
                
                // check if its a hex immediate
                if (lines[i][j].includes("0x")) {
                    // Do literally nothing. its stored exactly as we want it to be
                    continue;
                }

                // it must be a integer immediate 
                lines[i][j] = "0x"+toHex(parseFloat(lines[i][j]), 2);
            }else if (operand_type == "word") {
                // words can either be labels or immediate values

                // check if its a label
                if (lines[i][j].includes("@")) {
                    // includes a pointer to a data label

                    // cleaning
                    lines[i][j] = lines[i][j].replace("@", "");
                    
                    // search through data labels to replace with corresponding RAM address
                    for (let k = 0; k < data_labels.length; k++) {
                        let label = data_labels[k];

                        // check 
                        if (label[0].includes(lines[i][j])) {
                            // replace label requester to pointer

                            lines[i][j] = label[1][0];    // low
                            lines[i][j+1] = label[1][1];  // high
                            break;
                        }
                    }
                    continue;
                }else if (lines[i][j].includes("$")) {
                    // includes a pointer to a function label
                    
                    // cleaning
                    lines[i][j] = lines[i][j].replace("$", "");
                    
                    // search through data labels to replace with corresponding RAM address
                    for (let k = 0; k < function_labels.length; k++) {
                        let label = function_labels[k];

                        // check 
                        if (label[0].includes(lines[i][j])) {
                            // replace label requester to pointer
                            lines[i][j] = label[1][0];    // low
                            lines[i][j+1] = label[1][1];  // high
                            break;
                        }
                    }
                } else if (lines[i][j].includes("0x")) {
                    let low_byte = toHex(lines[i][j] & 0xFF, 2);
                    let high_byte = toHex(lines[i][j] >> 8, 2);

                    lines[i][j] = "0x"+low_byte;
                    lines[i][j+1] = "0x"+high_byte;
                    continue;
                }

                // it must be a integer immediate 
                // but this is a word not a byte
                // so we need to separate it between a low byte and high byte
                let low_byte = toHex(lines[i][j] & 0xFF, 2);
                let high_byte = toHex(lines[i][j] >> 8, 2);

                // console.log("FOUND FOUND FOUND!!");
                // console.log(`low byte: ${low_byte}\nhigh byte: ${high_byte}`);
                
                lines[i][j] = "0x"+low_byte;
                lines[i][j+1] = "0x"+high_byte;
                continue;
            }
        }

        console.log("");
    }
    console.log("");

    // Fill in any empty bytes with 0x00
    for (let i = 0; i < lines.length; i++) {
        for (let k = 0; k < 4; k++) {
            if (lines[i][k] == undefined) {
                lines[i][k] = "0x00";
            }
            text_hex.push(lines[i][k]);
        }

        // lines[i] = lines[i].join(" ");
    }


    console.log(".text hex: ");
    console.log(text_hex);
    console.log(".data hex: ");
    console.log(data_hex);

    console.log("");
    console.log("Full hex: ");
    final_hex.push(...text_hex);
    final_hex.push(...data_hex);


    // Print out final hex + data hex
    for (let i = 1; i < final_hex.length+1; i++) {
        process.stdout.write(final_hex[i-1]+" ");
        if (i % 4 == 0) {process.stdout.write("\n");}
    }
    console.log("");
    console.log("");

    // Write to output file

    // remove all the 0x identifiers and turn to real bytes
    for (let i = 0; i < final_hex.length; i++) {
        final_hex[i] = parseInt(final_hex[i], 16);
    }

    var buffer = Buffer.from(final_hex);

    fs.writeFile(out_file_path, buffer, (err) => {
        if (err) throw err;
        console.log(`Saved ${final_hex.length} bytes successfully to ${out_file_path}`);
    });

    // console.log(lines);
})

// utility 
function toHex(num, padding = 4) {
    return num.toString(16).padStart(padding, '0')
}

function printLine(address, line) {
    return `[0x${toHex(address)} | ${line.toString().padStart(3, ' ')}]`
}

function immediateToHex(num) {
    // immediates are all 16 bits
    // thus 2 bytes

    // so we need to store the low byte first 
    // then the high byte

    // well return the 2 bytes as an array, low first 

    let low = num & 0xFF;
    let high = (num >> 8) & 0xFF;
    return ["0x"+toHex(low, 2), "0x"+toHex(high, 2)];
}