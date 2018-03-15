#include "compute_node.hh"

ComputeNode::ComputeNode(std::string line, RegisterList *list, std::string prev) {	

std::string opcode;
	std::vector<std::string> parameters;
	int leftDelimeter, rightDelimeter, lastInLine;
    int returnChk = line.find(" = ");
	int last = 0;
	instruction.general.llvm_Line = line;
	int n = 1; 
	int dependencies = 0;
	int i = 10;
    // ////////////////////////////////////////////////////////////////////
   

    // Find the return register. If it exists, it is always the first component of the line
    if (returnChk > 0) {
    	// Create new pointer to the return register
        instruction.general.returnRegister = new Register(line.substr((line.find("%")+1), returnChk));
        list->addRegister(instruction.general.returnRegister);
        // In all instances where a return register is the first component, the next component is
        // the opcode, which is parsed and removed from line
        line = line.substr(returnChk+3);
    	instruction.general.opCode = line.substr(0,line.find(' ')-1);
    	line = line.substr(line.find(' '));
    }
    // If no return register is found then the first component must instead be the opcode
    // as of LLVM 3.4 instruction types
    else {
    	// Search for first none empty space which is were the opcode must begin
    	// Then store the opcode and remove the parsed information from the line
    	for(int i = 0; i < line.length(); i++){
    		if(line[0] != ' ') {
				instruction.general.opCode = line.substr(i, line.find(' ')-1);
    			line = line.substr(line.find(' '));
    			break;
    		}
    	}
    	// No return register needed
    	instruction.general.returnRegister = NULL;
    }
    // //////////////////////////////////////////////////////////////////////////////////


	// Loop to break apart each component of the LLVM line
	// Any brackets or braces that contain data are stored as under the parent set
	// Components are pushed back into the parameter vector in order they are read
	// This vector is initialized after the return register and opcode are found
	// and the line at this point already has those components removed if they
	// exist.

    	for (int i = 0; i < line.length(); i++) {
    		// Looks until the loop finds a non-space character
    		if (line[i] != ' ') {

    			// Each of these functions preform the same action using a different
    			// bracket or brace as the searchable character. Since the loop looks
    			// through the line sequentially no type of brace or bracket takes
    			// precendence over the others, it simply finds the first instance
    			// and matches the component to the correct closing character and stores
    			// the entire encapsulated component and its sub components if they exist
    			// as one entry in the struct


    			if (line[i] == '(') {
    				lastInLine = 0; //Returned as -1 when no character found in string
    				leftDelimeter = i + 1 + line.substr(i + 1).find('(');
    				rightDelimeter = i + line.substr(i).find(')');
    				lastInLine = rightDelimeter;
    				while ((leftDelimeter < rightDelimeter) && (lastInLine >= 0)) {
    					lastInLine = line.substr(leftDelimeter + 1).find('(');
    					leftDelimeter = leftDelimeter + 1 + line.substr(leftDelimeter + 1).find('(');
    					rightDelimeter = rightDelimeter + 1 + line.substr(rightDelimeter + 1).find(')');
    				}
    				parameters.push_back(line.substr(i, rightDelimeter - (i - 1)));
    				i += (rightDelimeter - i);
    			}

    			else if (line[i] == '[') {
    				lastInLine = 0;
    				leftDelimeter = i + 1 + line.substr(i + 1).find('[');
    				rightDelimeter = i + line.substr(i).find(']');
    				lastInLine = rightDelimeter;
    				while ((leftDelimeter < rightDelimeter) && (lastInLine >= 0)) {
    					lastInLine = line.substr(leftDelimeter + 1).find('[');
    					leftDelimeter = leftDelimeter + 1 + line.substr(leftDelimeter + 1).find('[');
    					rightDelimeter = rightDelimeter + 1 + line.substr(rightDelimeter + 1).find(']');
    				}
    				parameters.push_back(line.substr(i, rightDelimeter - (i - 1)));
    				i += (rightDelimeter - i);
    			}

    			else if (line[i] == '{') {
    				lastInLine = 0;
    				leftDelimeter = i + 1 + line.substr(i + 1).find('{');
    				rightDelimeter = i + line.substr(i).find('}');
    				lastInLine = rightDelimeter;
    				while ((leftDelimeter < rightDelimeter) && (lastInLine >= 0)) {
    					lastInLine = line.substr(leftDelimeter + 1).find('{');
    					leftDelimeter = leftDelimeter + 1 + line.substr(leftDelimeter + 1).find('{');
    					rightDelimeter = rightDelimeter + 1 + line.substr(rightDelimeter + 1).find('}');
    				}
    				parameters.push_back(line.substr(i, rightDelimeter - (i - 1)));
    				i += (rightDelimeter - i);
    			}
    			else if (line[i] == '<') {
    				lastInLine = 0;
    				leftDelimeter = i + 1 + line.substr(i + 1).find('<');
    				rightDelimeter = i + line.substr(i).find('>');
    				lastInLine = rightDelimeter;
    				while ((leftDelimeter < rightDelimeter) && (lastInLine >= 0)) {
    					lastInLine = line.substr(leftDelimeter + 1).find('<');
    					leftDelimeter = leftDelimeter + 1 + line.substr(leftDelimeter + 1).find('<');
    					rightDelimeter = rightDelimeter + 1 + line.substr(rightDelimeter + 1).find('>');
    				}
    				parameters.push_back(line.substr(i, rightDelimeter - (i - 1)));
    				i += (rightDelimeter - i);
    			}
    			// End of functions that search for encapulated components

    			// The else condition catches all other components of the function and stores them
    			// in the order found as parameters

    			else {
    				leftDelimeter = i;
    				if (line[i] == '=') {
    					//Ignore
    				}
    				else if (line[i] == ',') {
    					//Ignore
    				}
    				else if (line.substr(i + 1).find(" ") > line.substr(i + 1).find(",")) {
    					rightDelimeter = 1 + line.substr(i + 1).find(",");
    					parameters.push_back(line.substr(leftDelimeter, rightDelimeter));
    					i += rightDelimeter;

    				}
    				else if (line.substr(i + 1).find(" ") < line.substr(i + 1).find(",")) {
    					rightDelimeter = 1 + line.substr(i + 1).find(" ");
    					parameters.push_back(line.substr(leftDelimeter, rightDelimeter));
    					i += rightDelimeter;

    				}
    				else if (line.substr(i + 1).find(" ") == -1) {
    					parameters.push_back(line.substr(leftDelimeter));
    					i = (line.length()-1);
    					//This will always be the last object on the line

    				}
    			}
    		}
    	}

		instruction.general.fields = parameters.size();
		last = (parameters.size() - 1);
    	// Once all components have been found, navigate through and define each component
    	// and initialize the attributes struct values to match the line
		//Instruction Type
			// Terminator
			// Binary Operations
				// Integer
				// Floating Point
			// Bitwise Binary Operation
			// Vector Operations
			// Aggregate Operations
			// Memory Access and Addressing Oprations
			// Converstion Operation
			// Other Operations
			// Custom Operations

    		switch (s_opMap[opCode]) {

			// Terminator Instructions 

    		case IR_Ret: {
				// ret <type> <value>; Return a value from a non - void function
				// ret void; Return from void function
				instruction.general.terminator = true;
				instruction.general.flowControl = true;
				if (parameters[last].find("void")) {
					// If void is found then it must not have a return value
					instruction.terminator.type = "void";
				}
				else {
					// If void is not found then the last parameters must be the return value,
					// and preceding it the return type
					instruction.terminator.type = parameters[last - 1];
					if (parameters[last][0] == '%') {
						// Find if return value references a register
						instruction.terminator.ivalue = "void";
						instruction.terminator.intermediate = false;
						if (list->findRegister(parameters[last]) == NULL) {
							instruction.terminator.value = new Register(parameters[last]);
							list->addRegister(instruction.terminator.value);
							instruction.dependencies.registers[0] = instruction.terminator.value;
						}
						else {
							instruction.terminator.value = list->findRegister(parameters[last]);
							instruction.dependencies.registers[0] = instruction.terminator.value;
						}
					}
					else {
						instruction.terminator.intermediate = true;
						instruction.terminator.ivalue = parameters[last];
					}
				}
				break;
			}

			case IR_Br: {
				// br i1 <cond>, label <iftrue>, label <iffalse>
				// br label <dest>          ; Unconditional branch
				instruction.general.terminator = true;
				instruction.general.flowControl = true;
				if (parameters.size() == 2) {
					//Unconditional branch
					// Check if register already exists and create new one if not
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.terminator.dest = new Register(parameters[last]);
						list->addRegister(instruction.terminator.dest);
						instruction.dependencies.registers[0] = instruction.terminator.dest;
					}
					else {
						instruction.terminator.dest = list->findRegister(parameters[last]);
						instruction.dependencies.registers[0] = instruction.terminator.dest;
					}
				}
				else {
					// Conditional Branch
					// Check if register already exists and create new one if not
					instruction.terminator.type = parameters[0];
					if (list->findRegister(parameters[last-2]) == NULL) {
						instruction.terminator.iftrue = new Register(parameters[last-2]);
						list->addRegister(instruction.terminator.iftrue);
						instruction.dependencies.registers[1] = instruction.terminator.iftrue;
					}
					else {
						instruction.terminator.iftrue = list->findRegister(parameters[last - 2]);
						instruction.dependencies.registers[1] = instruction.terminator.iftrue;
					}
					// Check if register already exists and create new one if not
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.terminator.iffalse = new Register(parameters[last]);
						list->addRegister(instruction.terminator.iffalse);
						instruction.dependencies.registers[2] = instruction.terminator.iffalse;
					}
					else {
						instruction.terminator.iffalse = list->findRegister(parameters[last]);
						instruction.dependencies.registers[2] = instruction.terminator.iffalse;
					}
					// Check if register already exists and create new one if not
					if (list->findRegister(parameters[1]) == NULL) {
						instruction.terminator.cond = new Register(parameters[1]);
						list->addRegister(instruction.terminator.cond);
						instruction.dependencies.registers[0] = instruction.terminator.cond;
					}
					else {
						instruction.terminator.cond = list->findRegister(parameters[1]);
						instruction.dependencies.registers[0] = instruction.terminator.cond;
					}
				}
				break;
			}

			case IR_Switch: {
				// switch <intty> <value>, label <defaultdest> [ <intty> <val>, label <dest> ... ]
				// When using a switch statement the default case is within instruction.terminator
				// while each case statement exists within instruction.terminator.cases
				instruction.general.terminator = true;
				instruction.general.flowControl = true;
				instruction.terminator.intty = parameters[0];
				if (parameters.size() <= 5) {
					if (list->findRegister(parameters[1]) == NULL) {
						instruction.terminator.value = new Register(parameters[1]);
						list->addRegister(instruction.terminator.value);
						instruction.dependencies.registers[0] = instruction.terminator.value;
					}
					else {
						instruction.terminator.value = list->findRegister(parameters[1]);
						instruction.dependencies.registers[0] = instruction.terminator.value;
					}

					if (list->findRegister(parameters[3]) == NULL) {
						instruction.terminator.defaultdest = new Register(parameters[3]);
						list->addRegister(instruction.terminator.defaultdest);
						instruction.dependencies.registers[1] = instruction.terminator.defaultdest;
					}
					else {
						instruction.terminator.defaultdest = list->findRegister(parameters[3]);
						instruction.dependencies.registers[1] = instruction.terminator.defaultdest;
					}
				}
				
				else {
					// Since the switch statement will always be n*4 elements where n is the number of case statements (min 1 = default only)

					// NOTE *** BUG BUG *** NOTE //
					// Currently the parser will see the entire struct as a single element, that will need to be broken apart here
					// to make this section of the code functionable. 
					n = parameters.size() / 4;
					instruction.terminator.cases.statements = n;
					while (n >= 1) {
						instruction.terminator.cases.intty[n - 1] = parameters[((n - 1) * 4)];
						if (list->findRegister(parameters[((n - 1) * 4) + 1]) == NULL) {
							instruction.terminator.cases.value[n - 1] = new Register(parameters[((n - 1) * 4) + 1]);
							list->addRegister(instruction.terminator.cases.value[n - 1]);
							instruction.dependencies.registers[(n * 2) - 2] = instruction.terminator.cases.value[n - 1];
						}
						else {
							instruction.terminator.cases.value[n - 1] = list->findRegister(parameters[((n - 1) * 4) + 1]);
							instruction.dependencies.registers[(n * 2) - 2] = instruction.terminator.cases.value[n - 1];
						}

						if (list->findRegister(parameters[((n - 1) * 4) + 3]) == NULL) {
							instruction.terminator.cases.dest[n - 1] = new Register(parameters[((n - 1) * 4) + 3]);
							list->addRegister(instruction.terminator.cases.dest[n - 1]);
							instruction.dependencies.registers[((n * 2) - 1)] = instruction.terminator.cases.dest[n - 1];
						}
						else {
							instruction.terminator.cases.dest[n - 1] = list->findRegister(parameters[((n - 1) * 4) + 3]);
							instruction.dependencies.registers[(n * 2) - 1] = instruction.terminator.cases.dest[n - 1];
						}
						n--;
					}
				}			
				break;
			}

			case IR_IndirectBr: {
				// indirectbr <somety>* <address>, [ label <dest1>, label <dest2>, ... ]
				instruction.general.terminator = true;
				instruction.general.flowControl = true;
				if (parameters.size() <= 3) {
					instruction.terminator.somety = parameters[0];
					if (list->findRegister(parameters[1]) == NULL) {
						instruction.terminator.Addr = new Register(parameters[1]);
						list->addRegister(instruction.terminator.Addr);
						instruction.dependencies.registers[0] = instruction.terminator.Addr;
					}
					else {
						instruction.terminator.Addr = list->findRegister(parameters[1]);
						instruction.dependencies.registers[0] = instruction.terminator.Addr;
					}
				}
				else {
					// NOTE *** BUG BUG *** NOTE //
					// Currently the parser will see the entire struct as a single element, that will need to be broken apart here
					// to make this section of the code functionable. 

					n = parameters.size() / 2;
					instruction.terminator.cases.statements = n;

					while (n >= 1) {
						if (list->findRegister(parameters[((n - 1) * 2) + 1]) == NULL) {
							instruction.terminator.cases.dest[n - 1] = new Register(parameters[((n - 1) * 2) + 1]);
							list->addRegister(instruction.terminator.cases.dest[n - 1]);
							instruction.dependencies.registers[n - 1] = instruction.terminator.cases.dest[n - 1];
						}
						else {
							instruction.terminator.cases.dest[n - 1] = list->findRegister(parameters[((n - 1) * 2) + 1]);
							instruction.dependencies.registers[n - 1] = instruction.terminator.cases.dest[n - 1];
						}
						n--;
					}
				}

				break;
			}

			case IR_Invoke: {
				// <result> = invoke [cconv] [ret attrs] <ptr to function ty> <function ptr val>(<function args>) [fn attrs]
				//   to label <normal label> unwind label <exception label>
				instruction.general.terminator = true;
				instruction.general.flowControl = true;

				if (list->findRegister(parameters[last]) == NULL) {
					instruction.terminator.exception_label = new Register(parameters[last]);
					list->addRegister(instruction.terminator.exception_label);
					instruction.dependencies.registers[0] = instruction.terminator.exception_label;
				}
				else {
					instruction.terminator.exception_label = list->findRegister(parameters[last]);
					instruction.dependencies.registers[0] = instruction.terminator.exception_label;
				}

				if (list->findRegister(parameters[last-3]) == NULL) {
					instruction.terminator.normal_label = new Register(parameters[last-3]);
					list->addRegister(instruction.terminator.normal_label);
					instruction.dependencies.registers[1] = instruction.terminator.normal_label;
				}
				else {
					instruction.terminator.normal_label = list->findRegister(parameters[last-3]);
					instruction.dependencies.registers[1] = instruction.terminator.normal_label;
				}

				// Determine the calling convention, default to c if none specified
					if (parameters[0] == "ccc") instruction.attributes.cconv.ccc = true;
					else if (parameters[0] == "fastcc") instruction.attributes.cconv.fastcc = true;
					else if (parameters[0] == "coldcc") instruction.attributes.cconv.coldcc = true;
					else if (parameters[0] == "cc10") instruction.attributes.cconv.cc10 = true;
					else if (parameters[0] == "cc11") instruction.attributes.cconv.cc11 = true;
					else instruction.attributes.cconv.ccc = true;

				// Implementation incomplete

				break;
			}

			case IR_Resume: {
				// resume <type> <value>
				instruction.general.terminator = true;
				instruction.general.flowControl = true;

				instruction.terminator.type = parameters[0];

				if (list->findRegister(parameters[1]) == NULL) {
					instruction.terminator.value = new Register(parameters[1]);
					list->addRegister(instruction.terminator.value);
					instruction.dependencies.registers[0] = instruction.terminator.value;
				}
				else {
					instruction.terminator.value = list->findRegister(parameters[1]);
					instruction.dependencies.registers[0] = instruction.terminator.value;
				}
				break;

			}

			case IR_Unreachable: {
				// The unreachable instruction has no defined semantics.
				break;
			}

			// Binary Operations

			case IR_Add: {
				// <result> = add <ty> <op1>, <op2>          ; yields {ty}:result
				// <result> = add nuw <ty> <op1>, <op2>; yields{ ty }:result
				// <result> = add nsw <ty> <op1>, <op2>; yields{ ty }:result
				// <result> = add nuw nsw <ty> <op1>, <op2>; yields{ ty }:result

				dependencies = 0;

				for (int i = 0; i <= 1; i++) {
					if (parameters[i] == "nuw") instruction.flags.nuw = true;
					if (parameters[i] == "nsw") instruction.flags.nsw = true;
				}

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last-1][0] == '%') {
					if (list->findRegister(parameters[last-1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last-1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last-1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last-1];
				}



				break;
			}

			case IR_FAdd: {
				// <result> = fadd [fast-math flags]* <ty> <op1>, <op2>   ; yields {ty}:result

					for(int i = 0; i < parameters.size() - 3; i++){
					if (parameters.at(i) == "nnan") instruction.flags.nnan = true;
					else if (parameters.at(i) == "ninf") instruction.flags.ninf = true;
					else if (parameters.at(i) == "nsz") instruction.flags.nsz = true;
					else if (parameters.at(i) == "arcp") instruction.flags.arcp = true;
					else if (parameters.at(i) == "contract") instruction.flags.contract = true;
					else if (parameters.at(i) == "afn") instruction.flags.afn = true;
					else if (parameters.at(i) == "reassoc") instruction.flags.reassoc = true;
					else if (parameters.at(i) == "fast") instruction.flags.fast = true;
				}

				dependencies = 0;

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}

				break;
			}

			case IR_Sub: {
				// <result> = sub <ty> <op1>, <op2>          ; yields {ty}:result
				// <result> = sub nuw <ty> <op1>, <op2>; yields{ ty }:result
				// <result> = sub nsw <ty> <op1>, <op2>; yields{ ty }:result
				// <result> = sub nuw nsw <ty> <op1>, <op2>; yields{ ty }:result

				dependencies = 0;

				for (int i = 0; i <= 1; i++) {
					if (parameters[i] == "nuw") instruction.flags.nuw = true;
					if (parameters[i] == "nsw") instruction.flags.nsw = true;
				}

				instruction.binary.ty = parameters[last - 2];

					// Check if adding from register or immediate value
					if (parameters[last][0] == '%') {
						if (list->findRegister(parameters[last]) == NULL) {
							instruction.binary.op2 = new Register(parameters[last]);
							list->addRegister(instruction.binary.op2);
							instruction.dependencies.registers[dependencies] = instruction.binary.op2;
							dependencies++;
						}
						else {
							instruction.binary.op2 = list->findRegister(parameters[last]);
							instruction.dependencies.registers[dependencies] = instruction.binary.op2;
							dependencies++;
						}
					}
					else {
						instruction.binary.immediate2 = true;
						instruction.binary.iop2 = parameters[last];
					}

					// Check if value is from register or immediate value
					if (parameters[last - 1][0] == '%') {
						if (list->findRegister(parameters[last - 1]) == NULL) {
							instruction.binary.op1 = new Register(parameters[last - 1]);
							list->addRegister(instruction.binary.op1);
							instruction.dependencies.registers[dependencies] = instruction.binary.op1;
							dependencies++;
						}
						else {
							instruction.binary.op1 = list->findRegister(parameters[last - 1]);
							instruction.dependencies.registers[dependencies] = instruction.binary.op1;
							dependencies++;
						}
					}
					else {
						instruction.binary.immediate1 = true;
						instruction.binary.iop1 = parameters[last - 1];
					}




				break;
			}

			case IR_FSub: {
				// <result> = fsub [fast-math flags]* <ty> <op1>, <op2>   ; yields {ty}:result

				for (int i = 0; i < parameters.size() - 3; i++) {
					if (parameters.at(i) == "nnan") instruction.flags.nnan = true;
					else if (parameters.at(i) == "ninf") instruction.flags.ninf = true;
					else if (parameters.at(i) == "nsz") instruction.flags.nsz = true;
					else if (parameters.at(i) == "arcp") instruction.flags.arcp = true;
					else if (parameters.at(i) == "contract") instruction.flags.contract = true;
					else if (parameters.at(i) == "afn") instruction.flags.afn = true;
					else if (parameters.at(i) == "reassoc") instruction.flags.reassoc = true;
					else if (parameters.at(i) == "fast") instruction.flags.fast = true;
				}

				dependencies = 0;

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}

				break;
			}

			case IR_Mul: {
				// <result> = mul <ty> <op1>, <op2>          ; yields {ty}:result
				// <result> = mul nuw <ty> <op1>, <op2>; yields{ ty }:result
				// <result> = mul nsw <ty> <op1>, <op2>; yields{ ty }:result
				// <result> = mul nuw nsw <ty> <op1>, <op2>; yields{ ty }:result

				dependencies = 0;

				for (int i = 0; i <= 1; i++) {
					if (parameters[i] == "nuw") instruction.flags.nuw = true;
					if (parameters[i] == "nsw") instruction.flags.nsw = true;
				}

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}


				break;
			}

			case IR_FMul: {
				// <result> = fmul [fast-math flags]* <ty> <op1>, <op2>   ; yields {ty}:result


				for (int i = 0; i < parameters.size() - 3; i++) {
					if (parameters.at(i) == "nnan") instruction.flags.nnan = true;
					else if (parameters.at(i) == "ninf") instruction.flags.ninf = true;
					else if (parameters.at(i) == "nsz") instruction.flags.nsz = true;
					else if (parameters.at(i) == "arcp") instruction.flags.arcp = true;
					else if (parameters.at(i) == "contract") instruction.flags.contract = true;
					else if (parameters.at(i) == "afn") instruction.flags.afn = true;
					else if (parameters.at(i) == "reassoc") instruction.flags.reassoc = true;
					else if (parameters.at(i) == "fast") instruction.flags.fast = true;
				}

				dependencies = 0;

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}

				break;
			}

			case IR_UDiv: {
				// <result> = udiv <ty> <op1>, <op2>         ; yields {ty}:result
				// <result> = udiv exact <ty> <op1>, <op2>; yields{ ty }:result
				dependencies = 0;
			
				if (parameters[0] == "exact") instruction.flags.exact = true;				

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}


				break;
			}

			case IR_SDiv: {
				// <result> = sdiv <ty> <op1>, <op2>         ; yields {ty}:result
				// <result> = sdiv exact <ty> <op1>, <op2>; yields{ ty }:result

				dependencies = 0;

				if (parameters[0] == "exact") instruction.flags.exact = true;

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}

				break;
			}

			case IR_FDiv: {
				// <result> = fdiv [fast-math flags]* <ty> <op1>, <op2>   ; yields {ty}:result


				for (int i = 0; i < parameters.size() - 3; i++) {
					if (parameters.at(i) == "nnan") instruction.flags.nnan = true;
					else if (parameters.at(i) == "ninf") instruction.flags.ninf = true;
					else if (parameters.at(i) == "nsz") instruction.flags.nsz = true;
					else if (parameters.at(i) == "arcp") instruction.flags.arcp = true;
					else if (parameters.at(i) == "contract") instruction.flags.contract = true;
					else if (parameters.at(i) == "afn") instruction.flags.afn = true;
					else if (parameters.at(i) == "reassoc") instruction.flags.reassoc = true;
					else if (parameters.at(i) == "fast") instruction.flags.fast = true;
				}

				dependencies = 0;

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}

				break;
			}

			case IR_URem: {
				// <result> = urem <ty> <op1>, <op2>   ; yields {ty}:result

				dependencies = 0;

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}


				break;
			}

			case IR_SRem: {
				// <result> = srem <ty> <op1>, <op2>   ; yields {ty}:result

				dependencies = 0;

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}
			}

			case IR_FRem: {
				// <result> = frem [fast-math flags]* <ty> <op1>, <op2>   ; yields {ty}:result


				for (int i = 0; i < parameters.size() - 3; i++) {
					if (parameters.at(i) == "nnan") instruction.flags.nnan = true;
					else if (parameters.at(i) == "ninf") instruction.flags.ninf = true;
					else if (parameters.at(i) == "nsz") instruction.flags.nsz = true;
					else if (parameters.at(i) == "arcp") instruction.flags.arcp = true;
					else if (parameters.at(i) == "contract") instruction.flags.contract = true;
					else if (parameters.at(i) == "afn") instruction.flags.afn = true;
					else if (parameters.at(i) == "reassoc") instruction.flags.reassoc = true;
					else if (parameters.at(i) == "fast") instruction.flags.fast = true;
				}

				dependencies = 0;

				instruction.binary.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.binary.op2 = new Register(parameters[last]);
						list->addRegister(instruction.binary.op2);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
					else {
						instruction.binary.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op2;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate2 = true;
					instruction.binary.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.binary.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.binary.op1);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
					else {
						instruction.binary.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.binary.op1;
						dependencies++;
					}
				}
				else {
					instruction.binary.immediate1 = true;
					instruction.binary.iop1 = parameters[last - 1];
				}

				break;
			}

			// Bitwise Operations

			case IR_Shl: {
				// <result> = shl <ty> <op1>, <op2>           ; yields {ty}:result
				// <result> = shl nuw <ty> <op1>, <op2>; yields{ ty }:result
				// <result> = shl nsw <ty> <op1>, <op2>; yields{ ty }:result
				// <result> = shl nuw nsw <ty> <op1>, <op2>; yields{ ty }:result
				dependencies = 0;

				for (int i = 0; i <= 1; i++) {
					if (parameters[i] == "nuw") instruction.flags.nuw = true;
					if (parameters[i] == "nsw") instruction.flags.nsw = true;
				}

				instruction.bitwise.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.bitwise.op2 = new Register(parameters[last]);
						list->addRegister(instruction.bitwise.op2);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
					else {
						instruction.bitwise.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate2 = true;
					instruction.bitwise.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.bitwise.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.bitwise.op1);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
					else {
						instruction.bitwise.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate1 = true;
					instruction.bitwise.iop1 = parameters[last - 1];
				}

				break;
			}

			case IR_LShr: {
				// <result> = lshr <ty> <op1>, <op2>         ; yields {ty}:result
				// <result> = lshr exact <ty> <op1>, <op2>; yields{ ty }:result

				dependencies = 0;

				if (parameters[0] == "exact") instruction.flags.exact = true;

				instruction.bitwise.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.bitwise.op2 = new Register(parameters[last]);
						list->addRegister(instruction.bitwise.op2);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
					else {
						instruction.bitwise.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate2 = true;
					instruction.bitwise.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.bitwise.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.bitwise.op1);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
					else {
						instruction.bitwise.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate1 = true;
					instruction.bitwise.iop1 = parameters[last - 1];
				}

				break;
			}

			case IR_AShr: {
				// <result> = ashr <ty> <op1>, <op2>         ; yields {ty}:result
				// <result> = ashr exact <ty> <op1>, <op2>; yields{ ty }:result

				dependencies = 0;

				if (parameters[0] == "exact") instruction.flags.exact = true;

				instruction.bitwise.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.bitwise.op2 = new Register(parameters[last]);
						list->addRegister(instruction.bitwise.op2);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
					else {
						instruction.bitwise.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate2 = true;
					instruction.bitwise.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.bitwise.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.bitwise.op1);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
					else {
						instruction.bitwise.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate1 = true;
					instruction.bitwise.iop1 = parameters[last - 1];
				}
				break;
			}

			case IR_And: {
				// <result> = and <ty> <op1>, <op2>   ; yields {ty}:result
				dependencies = 0;

				instruction.bitwise.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.bitwise.op2 = new Register(parameters[last]);
						list->addRegister(instruction.bitwise.op2);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
					else {
						instruction.bitwise.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate2 = true;
					instruction.bitwise.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.bitwise.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.bitwise.op1);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
					else {
						instruction.bitwise.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate1 = true;
					instruction.bitwise.iop1 = parameters[last - 1];
				}
				break;
			}

			case IR_Or: {
				// <result> = or <ty> <op1>, <op2>   ; yields {ty}:result

				dependencies = 0;

				instruction.bitwise.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.bitwise.op2 = new Register(parameters[last]);
						list->addRegister(instruction.bitwise.op2);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
					else {
						instruction.bitwise.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate2 = true;
					instruction.bitwise.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.bitwise.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.bitwise.op1);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
					else {
						instruction.bitwise.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate1 = true;
					instruction.bitwise.iop1 = parameters[last - 1];
				}
				break;
			}

			case IR_Xor: {
				// <result> = xor <ty> <op1>, <op2>   ; yields {ty}:result

				dependencies = 0;

				instruction.bitwise.ty = parameters[last - 2];

				// Check if adding from register or immediate value
				if (parameters[last][0] == '%') {
					if (list->findRegister(parameters[last]) == NULL) {
						instruction.bitwise.op2 = new Register(parameters[last]);
						list->addRegister(instruction.bitwise.op2);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
					else {
						instruction.bitwise.op2 = list->findRegister(parameters[last]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op2;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate2 = true;
					instruction.bitwise.iop2 = parameters[last];
				}

				// Check if value is from register or immediate value
				if (parameters[last - 1][0] == '%') {
					if (list->findRegister(parameters[last - 1]) == NULL) {
						instruction.bitwise.op1 = new Register(parameters[last - 1]);
						list->addRegister(instruction.bitwise.op1);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
					else {
						instruction.bitwise.op1 = list->findRegister(parameters[last - 1]);
						instruction.dependencies.registers[dependencies] = instruction.bitwise.op1;
						dependencies++;
					}
				}
				else {
					instruction.bitwise.immediate1 = true;
					instruction.bitwise.iop1 = parameters[last - 1];
				}
				break;
			}

			// Vector Operations 

			case IR_Alloca: {
				// <result> = alloca <type>[, <ty> <NumElements>][, align <alignment>]     ; yields {type*}:result

				attributes.params.dataType = parameters[0];

				if (parameters[1] == "align") {
					attributes.params.align = parameters[2];
				}
				else if (parameters.size() == 5) {
					attributes.params.type2 = parameters[1];
					attributes.params.numElements = parameters[2];
					attributes.params.align = parameters[4];
				}
				else {
					attributes.params.type2 = parameters[1];
					attributes.params.numElements = parameters[2];
				}
				i = RESET;
				break;
			}
			case IR_Load: {
				//		    uint8_t* getCurData() { return curData; }
				// <result> = load [volatile] <ty>* <pointer>[, align <alignment>][, !nontemporal !<index>][, !invariant.load !<index>]
				// <result> = load atomic[volatile] <ty>* <pointer>[singlethread] <ordering>, align <alignment>
				//	!<index> = !{ i32 1 }
				break;
			}
			case IR_Store: {
				//			comm_interface.hh
				//		    int prepRead(Addr src, size_t length);
				//		    int prepWrite(Addr dst, uint8_t* value, size_t length);
				// store [volatile] <ty> <value>, <ty>* <pointer>[, align <alignment>][, !nontemporal !<index>]        ; yields {void}
				// store atomic[volatile] <ty> <value>, <ty>* <pointer>[singlethread] <ordering>, align <alignment>; yields{ void }

				break;
			}
			case IR_GetElementPtr: {
				// <result> = getelementptr <pty>* <ptrval>{, <ty> <idx>}*
				// <result> = getelementptr inbounds <pty>* <ptrval>{, <ty> <idx>}*
				// <result> = getelementptr <ptr vector> ptrval, <vector index type> idx


				break;
			}
			case IR_Fence: {
				// fence [singlethread] <ordering>                   ; yields {void}

				break;
			}
			case IR_AtomicCmpXchg: {
				// cmpxchg [volatile] <ty>* <pointer>, <ty> <cmp>, <ty> <new> [singlethread] <ordering>  ; yields {ty}

				break;
			}
			case IR_AtomicRMW: {
				// atomicrmw [volatile] <operation> <ty>* <pointer>, <ty> <value> [singlethread] <ordering>                   ; yields {ty}

				break;
			}
			case IR_Trunc: {
				// <result> = trunc <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_ZExt: {
				// <result> = zext <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_SExt: {
				// <result> = sext <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_FPToUI: {
				// <result> = fptoui <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_FPToSI: {
				// <result> = fptosi <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_UIToFP: {
				// <result> = uitofp <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_SIToFP: {
				// <result> = sitofp <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_FPTrunc: {
				// <result> = fptrunc <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_FPExt: {
				// <result> = fpext <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_PtrToInt: {
				// <result> = ptrtoint <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_IntToPtr: {
				// <result> = inttoptr <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_BitCast: {
				// <result> = bitcast <ty> <value> to <ty2>             ; yields ty2

				break;
			}
			case IR_AddrSpaceCast: {
				// <result> = addrspacecast <pty> <ptrval> to <pty2>       ; yields pty2

				break;
			}
			case IR_ICmp: {
				// <result> = icmp <cond> <ty> <op1>, <op2>   ; yields {i1} or {<N x i1>}:result

				attributes.params.operand2 = parameters[i];
				attributes.params.operand1 = parameters[i - 1];
				attributes.params.dataType = parameters[i - 2];
				attributes.condition.cond = parameters[i - 3];

				if (parameters[i - 3] == "eq") attributes.condition.eq = true;
				else if (parameters[i - 3] == "ne") attributes.condition.ne = true;
				else if (parameters[i - 3] == "ugt") attributes.condition.ugt = true;
				else if (parameters[i - 3] == "uge") attributes.condition.uge = true;
				else if (parameters[i - 3] == "ult") attributes.condition.ult = true;
				else if (parameters[i - 3] == "ule") attributes.condition.ule = true;
				else if (parameters[i - 3] == "sgt") attributes.condition.sgt = true;
				else if (parameters[i - 3] == "sge") attributes.condition.sge = true;
				else if (parameters[i - 3] == "slt") attributes.condition.slt = true;
				else if (parameters[i - 3] == "sle") attributes.condition.sle = true;

				break;
			}
			case IR_FCmp: {
				// <result> = fcmp <cond> <ty> <op1>, <op2>     ; yields {i1} or {<N x i1>}:result

				attributes.params.operand2 = parameters[i];
				attributes.params.operand1 = parameters[i - 1];
				attributes.params.dataType = parameters[i - 2];
				attributes.condition.cond = parameters[i - 3];


				if (parameters[i - 3] == "false") attributes.condition.condFalse = true;
				else if (parameters[i - 3] == "oeq") attributes.condition.oeq = true;
				else if (parameters[i - 3] == "ogt") attributes.condition.ogt = true;
				else if (parameters[i - 3] == "oge") attributes.condition.oge = true;
				else if (parameters[i - 3] == "olt") attributes.condition.olt = true;
				else if (parameters[i - 3] == "ole") attributes.condition.ole = true;
				else if (parameters[i - 3] == "one") attributes.condition.one = true;
				else if (parameters[i - 3] == "ord") attributes.condition.ord = true;
				else if (parameters[i - 3] == "ueq") attributes.condition.ueq = true;
				else if (parameters[i - 3] == "ugt") attributes.condition.ugt = true;
				else if (parameters[i - 3] == "uge") attributes.condition.uge = true;
				else if (parameters[i - 3] == "ult") attributes.condition.ult = true;
				else if (parameters[i - 3] == "ule") attributes.condition.ule = true;
				else if (parameters[i - 3] == "une") attributes.condition.une = true;
				else if (parameters[i - 3] == "uno") attributes.condition.uno = true;
				else if (parameters[i - 3] == "true") attributes.condition.condTrue = true;

				break;
			}
			case IR_PHI: {
				// <result> = phi <ty> [ <val0>, <label0>], ...

				//Improvement Opportunity - Dynamically allocated string to eliminate PHIPATHMAX
				attributes.params.dataType = parameters[0];
				for (int i = 1; i <= PHIPATHMAX; i++) {
					if (!(parameters[i].empty())) {
						attributes.phi.paths[i - 1] = parameters[i];
					}
				}
				break;
			}
			case IR_Call: {
				// <result> = [tail] call[cconv][ret attrs] <ty>[<fnty>*] <fnptrval>(<function args>)[fn attrs]

				break;
			}
			case IR_Select: {
				// <result> = select selty <cond>, <ty> <val1>, <ty> <val2>             ; yields ty
				// selty is either i1 or {<N x i1>}


				break;
			}
			case IR_VAArg: {
				// <resultval> = va_arg <va_list*> <arglist>, <argty>

				break;
			}
			case IR_ExtractElement: {
				// <result> = extractelement <n x <ty>> <val>, i32 <idx>    ; yields <ty>

				break;
			}
			case IR_InsertElement: {
				// <result> = insertelement <n x <ty>> <val>, <ty> <elt>, i32 <idx>    ; yields <n x <ty>>

				break;
			}
			case IR_ShuffleVector: {
				// <result> = shufflevector <n x <ty>> <v1>, <n x <ty>> <v2>, <m x i32> <mask>    ; yields <m x <ty>>

				break;
			}
			case IR_ExtractValue: {
				// <result> = extractvalue <aggregate type> <val>, <idx>{, <idx>}*

				break;
			}
			case IR_InsertValue: {
				// <result> = insertvalue <aggregate type> <val>, <ty> <elt>, <idx>{ , <idx> }*; yields <aggregate type>

				break;
			}
			case IR_LandingPad: {
				// <resultval> = landingpad <resultty> personality <type> <pers_fn> <clause>+
				// <resultval> = landingpad <resultty> personality <type> <pers_fn> cleanup <clause>*

				// <clause> : = catch <type> <value>
				// <clause> : = filter <array constant type> <array constant>

				break;
			}
			case IR_DMAFence: {

				break;
			}
			case IR_DMAStore: {

				break;
			}
			case IR_DMALoad: {

				break;
			}
			case IR_IndexAdd: {

				break;
			}
			case IR_SilentStore: {

				break;
			}
			case IR_Sine: {

				break;
			}
			case IR_Cosine: {

				break;
			}
			case IR_Move: {

				break;
			}
    		default: {

    			break;
    		}
    		}
    	}


void
ComputeNode::compute() {
	
	switch (s_opMap[opCode]) {
	case IR_Move: {
	
		break;
	}
	case IR_Ret: {
		Operations::llvm_ret(attributes);
		break;
	}
	case IR_Br: {
		Operations::llvm_br(attributes);
		break;
	}
	case IR_Switch: {
		Operations::llvm_switch(attributes);
		break;
	}
	case IR_IndirectBr: {
		Operations::llvm_indirectbr(attributes);
		break;
	}
	case IR_Invoke: {
		Operations::llvm_invoke(attributes);
		break;
	}
	case IR_Resume: {
		Operations::llvm_resume(attributes);
		break;
	}
	case IR_Unreachable: {
		Operations::llvm_unreachable(attributes);
		break;
	}
	case IR_Add: {
		Operations::llvm_add(attributes);
		break;
	}
	case IR_FAdd: {
		Operations::llvm_fadd(attributes);
		break;
	}
	case IR_Sub: {
		Operations::llvm_sub(attributes);
		break;
	}
	case IR_FSub: {
		Operations::llvm_fsub(attributes);
		break;
	}
	case IR_Mul: {
		Operations::llvm_mul(attributes);
		break;
	}
	case IR_FMul: {
		Operations::llvm_fmul(attributes);
		break;
	}
	case IR_UDiv: {
		Operations::llvm_udiv(attributes);
		break;
	}
	case IR_SDiv: {
		Operations::llvm_sdiv(attributes);
		break;
	}
	case IR_FDiv: {
		Operations::llvm_fdiv(attributes);
		break;
	}
	case IR_URem: {
		Operations::llvm_urem(attributes);
		break;
	}
	case IR_SRem: {
		Operations::llvm_srem(attributes);
		break;
	}
	case IR_FRem: {
		Operations::llvm_frem(attributes);
		break;
	}
	case IR_Shl: {
		Operations::llvm_shl(attributes);
		break;
	}
	case IR_LShr: {
		Operations::llvm_lshr(attributes);
		break;
	}
	case IR_AShr: {
		Operations::llvm_ashr(attributes);
		break;
	}
	case IR_And: {
		Operations::llvm_and(attributes);
		break;
	}
	case IR_Or: {
		Operations::llvm_or(attributes);
		break;
	}
	case IR_Xor: {
		Operations::llvm_xor(attributes);
		break;
	}

	case IR_Alloca: {
		Operations::llvm_alloca(attributes);
		break;
	}
	case IR_Load: {
		Operations::llvm_load(attributes);
		break;
	}
	case IR_Store: {
		Operations::llvm_store(attributes);
		break;
	}
	case IR_GetElementPtr: {
		Operations::llvm_getelementptr(attributes);
		break;
	}
	case IR_Fence: {
		Operations::llvm_fence(attributes);
		break;
	}
	case IR_AtomicCmpXchg: {
		Operations::llvm_cmpxchg(attributes);
		break;
	}
	case IR_AtomicRMW: {
		Operations::llvm_atomicrmw(attributes);
		break;
	}
	case IR_Trunc: {
		Operations::llvm_trunc(attributes);
		break;
	}
	case IR_ZExt: {
		Operations::llvm_zext(attributes);
		break;
	}
	case IR_SExt: {
		Operations::llvm_sext(attributes);
		break;
	}
	case IR_FPToUI: {
		Operations::llvm_fptoui(attributes);
		break;
	}
	case IR_FPToSI: {
		Operations::llvm_fptosi(attributes);
		break;
	}
	case IR_UIToFP: {
		Operations::llvm_uitofp(attributes);
		break;
	}
	case IR_SIToFP: {
		Operations::llvm_sitofp(attributes);
		break;
	}
	case IR_FPTrunc: {
		Operations::llvm_fptrunc(attributes);
		break;
	}
	case IR_FPExt: {
		Operations::llvm_fpext(attributes);
		break;
	}
	case IR_PtrToInt: {
		Operations::llvm_ptrtoint(attributes);
		break;
	}
	case IR_IntToPtr: {
		Operations::llvm_inttoptr(attributes);
		break;
	}
	case IR_BitCast: {
		Operations::llvm_bitcast(attributes);
		break;
	}
	case IR_AddrSpaceCast: {
		Operations::llvm_addrspacecast(attributes);
		break;
	}
	case IR_ICmp: {
		Operations::llvm_icmp(attributes);
		break;
	}
	case IR_FCmp: {
		Operations::llvm_fcmp(attributes);
		break;
	}
	case IR_PHI: {
		Operations::llvm_phi(attributes, prevBB);
		break;
	}
	case IR_Call: {
		Operations::llvm_call(attributes);
		break;
	}
	case IR_Select: {
		Operations::llvm_select(attributes);
		break;
	}
	case IR_VAArg: {
		Operations::llvm_vaarg(attributes);
		break;
	}
	case IR_ExtractElement: {
		Operations::llvm_extractelement(attributes);
		break;
	}
	case IR_InsertElement: {
		Operations::llvm_insertelement(attributes);
		break;
	}
	case IR_ShuffleVector: {
		Operations::llvm_shufflevector (attributes);
		break;
	}
	case IR_ExtractValue: {
		Operations::llvm_extractvalue(attributes);
		break;
	}
	case IR_InsertValue: {
		Operations::llvm_insertvalue(attributes);
		break;
	}
	case IR_LandingPad: {
		Operations::llvm_landingpad(attributes);
		break;
	}
	case IR_DMAFence: {
		Operations::llvm_dmafence(attributes);
		break;
	}
	case IR_DMAStore: {
		Operations::llvm_dmastore(attributes);
		break;
	}
	case IR_DMALoad: {
		Operations::llvm_dmaload(attributes);
		break;
	}
	case IR_IndexAdd: {
		Operations::llvm_indexadd(attributes);
		break;
	}
	case IR_SilentStore: {
		Operations::llvm_silentstore(attributes);
		break;
	}
	case IR_Sine: {
		Operations::llvm_sine(attributes);
		break;
	}
	case IR_Cosine: {
		Operations::llvm_cosine(attributes);
		break;
	}

	default: {

		break;
	}
	}


}

bool ComputeNode::commit(){
	if(instruction.general.returnRegister != NULL){
		instruction.general.returnRegister->commit();
		instruction.cycle.current++;
		if(instruction.cycle.current >= instruction.cycle.max){
			instruction.general.returnRegister->commit();
			return true;
		}
	}
	return false;
}

bool ComputeNode::checkDependency(){
	int size = instruction.dependencies.registers.size();
	bool hot = true;
	for(int i = 0; i < size; i++){
		if(instruction.dependencies.registers[i] != NULL){
			if(instruction.dependencies.registers[i].hot == false) hot = false;
			else hot = true;
		}
	}
	return hot;
}

void ComputeNode::reset(){
	if(instruction.general.returnRegister != NULL){
		instruction.general.returnRegister->reset();
		instruction.cycle.current = 0;
	}
}